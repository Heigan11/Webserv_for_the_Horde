#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <poll.h>

#define PORT    5555
#define BUFLEN  4096

int readFromClientHTTP(int fd, char *buf){
    int nbytes;

    nbytes = read(fd, buf, BUFLEN);
    if (nbytes <= 0)
        return -1;
    else
        std::cout << "Server got msg: " << buf << std::endl;
    return 0;
}

int writeToClientHTTP(int fd, char *buf){
    int nbytes;

    for (size_t i = 0; buf[i] != '\0'; i++) {
        buf[i] += 1;
    }
    nbytes = write(fd, buf, strlen(buf) + 1);
    if (nbytes < 0){
        std::cerr << "Server: write failure" << std::endl;
    }
    std::cout << "Write back: " << buf << std::endl;

    return 0;
}

int writeToClientHTTPForm(int fd, char *buf){
    return 0;
}

int main(){

    int sock;
    int newsock;
    char buf[BUFLEN];
    socklen_t size;
    int err;

    struct sockaddr_in addr;
    struct sockaddr_in client;
    fd_set active_set, read_set;
 
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    sock = socket(PF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

    bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(sock, 3);

    FD_ZERO(&active_set);
    FD_SET(sock, &active_set);

    while (1)
    {
        read_set = active_set;
        select(FD_SETSIZE, &read_set, NULL, NULL, NULL);
        for (size_t i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &read_set))
            {
                if (i == sock)
                {
                    size = sizeof(client);
                    newsock = accept(sock,(struct sockaddr*)&client, &size);
                    std::cout << "Connect from host " << inet_ntoa(client.sin_addr)
                    << ", port" << ntohs(client.sin_port) << std::endl;
                    FD_SET(newsock, &active_set);
                } 
                else
                {
                    readFromClientHTTP(i, buf);
                    if (strstr(buf, "stop")) {
                        close(i);
                        FD_CLR(i, &active_set);
                    } else {
                        writeToClientHTTP(i, buf);
                    }
                }
            }
        }     
    }
    
    close(sock);
    return 0;
}

//g++ select_server.cpp -o server
