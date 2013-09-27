#ifndef __TCPCLIENT_H__
#define __TCPCLIENT_H__

#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */

#define WIN32_LEAN_AND_MEAN

#   include <windows.h>
#   include <winsock2.h>
#   include <ws2tcpip.h>
#   include <cstdlib>
#   include <cstdio>
#   pragma comment (lib, "Ws2_32.lib")
#   pragma comment (lib, "Mswsock.lib")
#   pragma comment (lib, "AdvApi32.lib")

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

class TCPClient {
    public:
        TCPClient(const std::string & hostname = "localhost", int port = 0);
        virtual ~TCPClient();
        // copy constructor
        TCPClient(const TCPClient & rhs);
        // assignment operator
        TCPClient & operator = (const TCPClient & rhs);

        TCPClient & open();
        TCPClient & send(std::vector<unsigned char>  buf);
        TCPClient & recv(std::vector<unsigned char> &buf);
        TCPClient & close();

    public:
        inline const std::string & errMsg() const { return this->m_errMsg; }
        inline const int errCode() const { return this->m_errCode; }
    private:
        std::string m_errMsg;
        int         m_errCode;
    private:
#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */
        WSADATA m_wsaData;
#endif
        std::string m_hostname;
        int m_port;
        tcpsocket m_socket; /* handle to socket */
        struct sockaddr_in * m_address; /* Internet socket address stuct */
}; /* end of class TCPClient */

#endif /* end of __TCPCLIENT_H__ */
