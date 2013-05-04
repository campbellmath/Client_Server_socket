#include <iostream>
#include <vector>
#include "TCPServer.h"
#include <cstdio>
#include <cstdlib>

/*===========================================================================*/
int main(int argc, const char *argv[])
{
    if (argc < 2) {
        std::cout<<"usage:   ./server port"<<std::endl;
        std::cout<<"example: ./server 12345"<<std::endl;
        exit(-1);
    }

    int port = atoi(argv[1]);

    TCPServer server (port);

    std::vector<unsigned char> send_data;
    std::vector<unsigned char> recv_data;

    for (int i = 0; i < 26; i++) {
        char c = i+'A';
        send_data.push_back(c);
    }
    int cout = 0;

    server.open().bind().listen();

    while (true) {
        server.accept().recv(recv_data);
        std::cout<<"recv : "<<std::endl;
        for (size_t idx = 0; idx < recv_data.size(); idx++) {
            std::cout<<recv_data[idx];
        }
        std::cout<<std::endl;

        server.send(send_data).closeClient();
        std::cout<<"send: "<<std::endl;
        for (size_t idx = 0; idx < send_data.size(); idx++) {
            std::cout<<send_data[idx];
        }
        std::cout<<"\nOK "<<++cout<<std::endl;
    }

    return 0;
}
/*===========================================================================*/
