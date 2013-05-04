#include "TCPServer.h"
#include <cstring>

/*===========================================================================*/
TCPServer::TCPServer(int port) :
    m_errMsg(""),
    m_errCode(0),
    m_clientSocket(0),
    m_serverSocket(0),
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
    /* make a socket */
    this->m_serverSocket=socket(AF_INET, SOCK_STREAM, 0);

    if(this->m_serverSocket == SOCKET_ERROR) {
        this->m_errMsg = "create socket error";
        this->m_errCode = SOCKET_ERROR;
        return *this;
    }

    return *this;
}
/*===========================================================================*/
TCPServer & TCPServer::bind()
{
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

    return *this;
}
/*===========================================================================*/
#define QUEUE_SIZE (1024)
TCPServer & TCPServer::listen()
{
    /* establish listen queue */
    if(::listen(this->m_serverSocket, QUEUE_SIZE) == SOCKET_ERROR) {
        this->m_errMsg = "listen error";
        this->m_errCode = SOCKET_ERROR;
        return *this;
    }

    return *this;
}
#undef QUEUE_SIZE
/*===========================================================================*/
TCPServer & TCPServer::accept()
{
    const int nAddressSize=sizeof(struct sockaddr_in);
    this->m_clientSocket = ::accept(this->m_serverSocket, (struct sockaddr*) (this->m_address), (socklen_t *) &nAddressSize);

    return *this;
}
/*===========================================================================*/
TCPServer & TCPServer::send(std::vector<unsigned char>  buf)
{
    size_t num_success_write = 0;;
    num_success_write = write(this->m_clientSocket , &buf[0], buf.size());
    if (num_success_write!=buf.size()) {
        this->m_errMsg = "sned error";
        this->m_errCode = SOCKET_ERROR;
    }

    ::shutdown(this->m_clientSocket, SHUT_WR);

    return *this;
}
/*===========================================================================*/
#define BUFFER_SIZE (1024)
TCPServer & TCPServer::recv(std::vector<unsigned char> &buf)
{
    size_t num_success_read = 0;;
    unsigned char read_buf[BUFFER_SIZE+1];
    buf.clear();
    /* read from socket into buffer */

    while (true) {
        num_success_read = read(this->m_clientSocket, read_buf, BUFFER_SIZE);
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
TCPServer & TCPServer::closeClient()
{
    if(::close(this->m_clientSocket) == SOCKET_ERROR) {
        this->m_errMsg  = "close client socketerror";
        this->m_errCode = SOCKET_ERROR;
        return *this;
    }

    this->m_clientSocket = 0;

    return *this;
}
/*===========================================================================*/
