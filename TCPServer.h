#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <vector>
#include <string>

#define SOCKET_ERROR (-1)

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
        TCPServer & send(std::vector<unsigned char>  buf);
        TCPServer & recv(std::vector<unsigned char> &buf);
        TCPServer & closeClient();

    public:
        inline const std::string & errMsg() const { return this->m_errMsg; } 
        inline const int errCode() const { return this->m_errCode; }
    private:
        std::string m_errMsg;
        int         m_errCode;
    private:
        int m_clientSocket, m_serverSocket;  /* handle to socket */
        int m_port;
        struct sockaddr_in * m_address;      /* Internet socket address stuct */
}; /* end of class TCPServer */

#endif /* end of __TCPSERVER_H__ */
