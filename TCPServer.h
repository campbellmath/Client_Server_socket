#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */

#   undef UNICODE
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include <winsock2.h>
#   include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")

#else

#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <netdb.h>
#   include <unistd.h>

#endif

#include <vector>
#include <string>

#define SOCKET_ERROR (-1)

#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */
typedef SOCKET tcpsocket;
#else
typedef int tcpsocket;
#define INVALID_SOCKET (0)
#endif

class TCPServer {
    public:
        TCPServer(int port = 0);
        virtual ~TCPServer();
        // copy constructor
        TCPServer(const TCPServer & rhs);
        // assignment operator
        TCPServer & operator = (const TCPServer & rhs);

        TCPServer & open();
        TCPServer & bind();
        TCPServer & listen();
        TCPServer & accept();
        TCPServer & send(const std::vector<unsigned char>  buf);
        TCPServer & recv(std::vector<unsigned char> &buf);
        TCPServer & closeClient();

    public:
        inline const std::string & errMsg() const { return this->m_errMsg; }
        inline const int errCode() const { return this->m_errCode; }
    private:
        std::string m_errMsg;
        int         m_errCode;
    private:
#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */
        WSADATA m_wsaData;
        struct addrinfo * m_result;
#endif
        tcpsocket m_clientSocket, m_serverSocket; /* handle to socket */
        int m_port;
        struct sockaddr_in * m_address; /* Internet socket address stuct */
}; /* end of class TCPServer */

#endif /* end of __TCPSERVER_H__ */
