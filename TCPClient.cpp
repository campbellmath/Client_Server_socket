#include "TCPClient.h"
#include <cstring>

/*===========================================================================*/
TCPClient::TCPClient(const std::string & hostname, int port) :
    m_errMsg(""),
    m_errCode(0),
    m_hostname(hostname),
    m_port(port),
    m_socket(INVALID_SOCKET),
    m_address(new struct sockaddr_in)
{

}
/*===========================================================================*/
TCPClient::~TCPClient()
{
    delete this->m_address;
}
/*===========================================================================*/
TCPClient::TCPClient(const TCPClient & rhs) :
    m_errMsg(rhs.m_errMsg),
    m_errCode(rhs.m_errCode),
    m_hostname(rhs.m_hostname),
    m_port(rhs.m_port),
    m_socket(rhs.m_socket),
    m_address(new struct sockaddr_in)
{
    memcpy(this->m_address, rhs.m_address, sizeof(struct sockaddr_in));
}
/*===========================================================================*/
TCPClient & TCPClient::operator = (const TCPClient & rhs)
{
    if (this != &rhs) {
        m_errMsg=rhs.m_errMsg;
        m_errCode=rhs.m_errCode;
        m_hostname=rhs.m_hostname;
        m_port=rhs.m_port;
        m_socket=rhs.m_socket;
        memcpy(this->m_address, rhs.m_address, sizeof(struct sockaddr_in));
    }

    return *this;
}
/*===========================================================================*/
TCPClient & TCPClient::open()
{
#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */

    struct addrinfo * result = 0;
    struct addrinfo hints;
    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2,2), &(this->m_wsaData));
    if (iResult != 0) {
        this->m_errMsg = "WSAStartup failed";
        this->m_errCode = iResult;
        return *this;
    }

    ZeroMemory(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    char port_buf[10];
    _itoa_s(this->m_port, port_buf, 10);
    iResult = getaddrinfo(this->m_hostname.c_str(), port_buf, &hints, &result);
    if ( iResult != 0 ) {
        this->m_errMsg = "getaddrinfo failed  failed";
        this->m_errCode = iResult;
        WSACleanup();
        return *this;
    }

    // Attempt to connect to an address until one succeeds
    for(struct addrinfo *ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
        // Create a SOCKET for connecting to server
        this->m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (this->m_socket == INVALID_SOCKET) {
            this->m_errMsg = "socket failed";
            this->m_errCode = WSAGetLastError();
            WSACleanup();
            return *this;
        }

        // Connect to server.
        iResult = connect(this->m_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(this->m_socket);
            this->m_socket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);
    result = 0;

    if (this->m_socket == INVALID_SOCKET) {
        this->m_errMsg = "Unable to connect to server";
        this->m_errCode = WSAGetLastError();
        WSACleanup();
        return *this;
    }

#else
    /* make a socket */
    this->m_socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    if(this->m_socket== SOCKET_ERROR) {
        this->m_errMsg = "create socket error";
        this->m_errCode = SOCKET_ERROR;
        return *this;;
    }

    long nHostAddress;
    struct hostent* pHostInfo;   /* holds info about a machine */

    /* get IP address from name */
    pHostInfo=gethostbyname(this->m_hostname.c_str());
    /* copy address into long */
    memcpy(&nHostAddress,pHostInfo->h_addr,pHostInfo->h_length);

    /* fill address struct */
    this->m_address->sin_addr.s_addr=nHostAddress;
    this->m_address->sin_port=htons(this->m_port);
    this->m_address->sin_family=AF_INET;

    /* connect to host */
    if(connect(this->m_socket, (struct sockaddr *) (this->m_address), sizeof(struct sockaddr)) == SOCKET_ERROR) {
        this->m_errMsg = "connect error";
        this->m_errCode = SOCKET_ERROR;
        return *this;;
    }
#endif

    return *this;
}
/*===========================================================================*/
TCPClient & TCPClient::send(std::vector<unsigned char>  buf)
{
    size_t num_success_write = 0;;
#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */
    num_success_write = ::send( this->m_socket, (const char *) (&buf[0]), buf.size(), 0);
#else
    num_success_write = write(this->m_socket, &buf[0], buf.size());
#endif
    if (num_success_write!=buf.size()) {
        this->m_errMsg = "sned error";
        this->m_errCode = SOCKET_ERROR;
        return *this;
    }

#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */
    ::shutdown(this->m_socket, SD_SEND);
#else
    ::shutdown(this->m_socket, SHUT_WR);
#endif

    return *this;
}
/*===========================================================================*/
#define BUFFER_SIZE (1024)
TCPClient & TCPClient::recv(std::vector<unsigned char> &buf)
{
    size_t num_success_read = 0;;
    unsigned char read_buf[BUFFER_SIZE+1];
    buf.clear();
    /* read from socket into buffer */
    while (true) {
#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */
        num_success_read = ::recv(this->m_socket, (char *)&read_buf[0], BUFFER_SIZE, 0);
#else
        num_success_read = read(this->m_socket, read_buf, BUFFER_SIZE);
#endif
        if (num_success_read > 0) {
            for (size_t idx = 0; idx < num_success_read; idx++) {
                buf.push_back(read_buf[idx]);
            }
        } else if (num_success_read == 0){
            break;
        } else {
            this->m_errMsg = "recv error";
            this->m_errCode = SOCKET_ERROR;
            break;
        }
    }

    return *this;
}
#undef BUFFER_SIZE
/*===========================================================================*/
TCPClient & TCPClient::close()
{
#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */
    closesocket(this->m_socket);
    WSACleanup();
    this->m_socket = INVALID_SOCKET;
#else
    if(::close(this->m_socket) == SOCKET_ERROR) {
        this->m_errMsg = "close socketerror";
        this->m_errCode = SOCKET_ERROR;
    }

    this->m_socket = 0;
#endif
    return *this;
}
/*===========================================================================*/
