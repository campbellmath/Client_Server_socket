#ifndef __TCPCLIENT_H__
#define __TCPCLIENT_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <vector>
#include <string>

#define SOCKET_ERROR (-1)

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
        std::string m_hostname;
        int m_port;
        int m_socket;                     /* handle to socket */
        struct sockaddr_in * m_address;   /* Internet socket address stuct */
}; /* end of class TCPClient */

#endif /* end of __TCPCLIENT_H__ */
