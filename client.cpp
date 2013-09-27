#include <iostream>
#include <vector>
#include "TCPClient.h"
#include <cstdio>
#include <cstdlib>

/*===========================================================================*/
void readfile(const char * filename, std::vector<unsigned char> & send_data)
{
#if 1

    FILE *fp = NULL;
#ifdef _WIN32 /* Defined for applications for Win32 and Win64. */
    fopen_s(&fp, filename, "r");
    if (fp != NULL) {
#else
    if ((fp = fopen(filename, "r")) != NULL) {
#endif
        char c = 0;
        while (!feof(fp)) {
            fread(&c, sizeof(unsigned char), 1, fp);
            send_data.push_back(c);
        }
    } else {
        fprintf(stderr, "can't open file ");
        perror("makef");
        exit(-1);
    }
    fclose(fp);

    fp = NULL;
#else
    for (unsigned char c= 'a'; c <= 'z'; c++) {
            send_data.push_back(c);
    }
#endif

    return;
}
/*===========================================================================*/
int main(int argc, const char *argv[])
{
    if (argc < 4) {
        std::cout<<"usage:   ./client hostname port filename"<<std::endl;
        std::cout<<"example: ./client localhost 12345 client.cpp"<<std::endl;
        exit(-1);
    }

    const char *hostname = argv[1];
    int port             = atoi(argv[2]);
    const char *filename = argv[3];

    std::vector<unsigned char> send_data;
    std::vector<unsigned char> recv_data;

    readfile(filename, send_data);

    TCPClient client(hostname, port);

    client.open();

    client.send(send_data);
    std::cout<<"send : "<<std::endl;
    for (size_t idx = 0; idx < send_data.size(); idx++) {
        std::cout<<send_data[idx];
    }

    client.recv(recv_data).close();
    std::cout<<"recv : "<<std::endl;
    for (size_t idx = 0; idx < recv_data.size(); idx++) {
        std::cout<<recv_data[idx];
    }

    return 0;
}
/*===========================================================================*/
