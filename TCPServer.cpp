#include "TCPServer.h"
#include <cstring>

/*===========================================================================*/
TCPServer::TCPServer(int port) :
    m_errMsg(""),
    m_errCode(0),
    m_clientSocket(INVALID_SOCKET),
    m_serverSocket(INVALID_SOCKET),
    m_port(port),
    m_address(new struct sockaddr_in)
{

}
/*===========================================================================*/
TCPServer::~TCPServer()
{
    delete this->m_address;
}
/*===========================================================================*/
TCPServer::TCPServer(const TCPServer & rhs) :
    m_errMsg(rhs.m_errMsg),
    m_errCode(rhs.m_errCode),
    m_clientSocket(rhs.m_clientSocket),
    m_serverSocket(rhs.m_serverSocket),
    m_port(rhs.m_port),
    m_address(new struct sockaddr_in)
{
    memcpy(this->m_address, rhs.m_address, sizeof(struct sockaddr_in));
}
/*===========================================================================*/
TCPServer & TCPServer::operator = (const TCPServer & rhs)
{
    if (this != &rhs) {
        this->m_errMsg=rhs.m_errMsg;
        this->m_errCode=rhs.m_errCode;
        this->m_clientSocket=rhs.m_clientSocket;
        this->m_serverSocket=rhs.m_serverSocket;
        this->m_port=rhs.m_port;
        memcpy(this->m_address, rhs.m_address, sizeof(struct sockaddr_in));
    }

    return *this;
}
/*===========================================================================*/
TCPServer & TCPServer::open()
{
#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */
    this->m_result = 0;

    int iResult = WSAStartup(MAKEWORD(2,2), &(this->m_wsaData));

    if (iResult != 0) {
        this->m_errMsg = "WSAStartup failed";
        this->m_errCode = iResult;
        return *this;
    }

    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(struct addrinfo));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags    = AI_PASSIVE;

    // Resolve the server address and port
    char port_buf[10];
    _itoa_s(this->m_port, port_buf, 10);
    iResult = getaddrinfo(NULL, port_buf, &hints, &(this->m_result));
    if ( iResult != 0 ) {
        printf("getaddrinfo failed");
        this->m_errCode = iResult;

        WSACleanup();
        return *this;
    }

    // Create a SOCKET for connecting to server
    this->m_serverSocket = socket(this->m_result->ai_family, this->m_result->ai_socktype, this->m_result->ai_protocol);

    if (this->m_serverSocket == INVALID_SOCKET) {
        this->m_errMsg = "create socket error";
        this->m_errCode = WSAGetLastError();
        return *this;
    }

#else
    /* make a socket */
    this->m_serverSocket=socket(AF_INET, SOCK_STREAM, 0);

    if(this->m_serverSocket == SOCKET_ERROR) {
        this->m_errMsg = "create socket error";
        this->m_errCode = SOCKET_ERROR;
        return *this;
    }
#endif

    return *this;
}
/*===========================================================================*/
TCPServer & TCPServer::bind()
{
#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */

    // Setup the TCP listening socket
    int iResult = ::bind( this->m_serverSocket, this->m_result->ai_addr, (int)(this->m_result->ai_addrlen));
    if (iResult == SOCKET_ERROR) {
        this->m_errMsg = "bind error";
        this->m_errCode = WSAGetLastError();
        closesocket(this->m_serverSocket);
        WSACleanup();
        return *this;
    }

    freeaddrinfo(this->m_result);
    this->m_result = 0;

#else
    const int nAddressSize=sizeof(struct sockaddr_in);
    /* fill address struct */
    this->m_address->sin_addr.s_addr = INADDR_ANY;
    this->m_address->sin_port        = htons(this->m_port);
    this->m_address->sin_family      = AF_INET;

    /* bind to a port */
    if(::bind(this->m_serverSocket, (struct sockaddr *) (this->m_address), sizeof(struct sockaddr)) == SOCKET_ERROR) {
        this->m_errMsg = "bind error";
        this->m_errCode = SOCKET_ERROR;
        return *this;
    }
    /*  get port number */
    if (getsockname( this->m_serverSocket, (struct sockaddr *) (this->m_address), (socklen_t *) &nAddressSize)==SOCKET_ERROR) {
        this->m_errMsg = "getsockname error";
        this->m_errCode = SOCKET_ERROR;
        return *this;
    }

#endif
    return *this;
}
/*===========================================================================*/
#define QUEUE_SIZE (1024)
TCPServer & TCPServer::listen()
{
#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */
    if (::listen(this->m_serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        this->m_errMsg = "listen error";
        this->m_errCode = WSAGetLastError();
        closesocket(this->m_serverSocket);
        WSACleanup();
        return *this;
    }
#else
    /* establish listen queue */
    if(::listen(this->m_serverSocket, QUEUE_SIZE) == SOCKET_ERROR) {
        this->m_errMsg = "listen error";
        this->m_errCode = SOCKET_ERROR;
        return *this;
    }
#endif

    return *this;
}
#undef QUEUE_SIZE
/*===========================================================================*/
TCPServer & TCPServer::accept()
{
#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */
    this->m_clientSocket = ::accept(this->m_serverSocket, NULL, NULL);
    if (this->m_clientSocket == INVALID_SOCKET) {
        this->m_errMsg = "accept error";
        this->m_errCode = WSAGetLastError();
        closesocket(this->m_serverSocket);
        WSACleanup();

        return *this;
    }
#else
    const int nAddressSize=sizeof(struct sockaddr_in);
    this->m_clientSocket = ::accept(this->m_serverSocket, (struct sockaddr*) (this->m_address), (socklen_t *) &nAddressSize);
#endif

    return *this;
}
/*===========================================================================*/
TCPServer & TCPServer::send(const std::vector<unsigned char> buf)
{

    size_t num_success_write = 0;;
#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */
    num_success_write = ::send( this->m_clientSocket, (const char *) (&buf[0]), buf.size(), 0);
#else
    num_success_write = write(this->m_clientSocket , &buf[0], buf.size());
#endif
    if (num_success_write!=buf.size()) {
        this->m_errMsg = "sned error";
        this->m_errCode = SOCKET_ERROR;
    }
#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */
    ::shutdown(this->m_clientSocket, SD_SEND);
#else
    ::shutdown(this->m_clientSocket, SHUT_WR);
#endif

    return *this;
}
/*===========================================================================*/
#define BUFFER_SIZE (1024)
TCPServer & TCPServer::recv(std::vector<unsigned char> &buf)
{

    size_t num_success_read = 0;;
    unsigned char *read_buf = new unsigned char[BUFFER_SIZE+1];
    buf.clear();
    /* read from socket into buffer */

    while (true) {
#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */
        num_success_read = ::recv(this->m_clientSocket, (char *) read_buf, BUFFER_SIZE, 0);
#else
        num_success_read = read(this->m_clientSocket, read_buf, BUFFER_SIZE);
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

    delete [] read_buf;

    return *this;
}
#undef BUFFER_SIZE
/*===========================================================================*/
TCPServer & TCPServer::closeClient()
{
#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */
    closesocket(this->m_clientSocket);
    this->m_clientSocket= INVALID_SOCKET;
#else
    if(::close(this->m_clientSocket) == SOCKET_ERROR) {
        this->m_errMsg  = "close client socketerror";
        this->m_errCode = SOCKET_ERROR;
        return *this;
    }

    this->m_clientSocket = INVALID_SOCKET;
#endif

    return *this;
}
/*===========================================================================*/
