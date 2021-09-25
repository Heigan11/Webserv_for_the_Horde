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
#define BUFLEN  512

int readFromClient(int fd, char *buf){
    int nbytes;

    nbytes = read(fd, buf, BUFLEN);
    if (nbytes <= 0)
        return -1;
    else
        std::cout << "Server got msg: " << buf << std::endl;
    return 0;
}

int writeToClient(int fd, char *buf){
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

int main(){

    int sock;
    int newsock;
    char buf[BUFLEN];
    socklen_t size;
    int err;

    struct sockaddr_in addr;
    struct sockaddr_in client;
 
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

    bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(sock, 3);

    pollfd act_set[100];
    act_set[0].fd = sock;
    act_set[0].events = POLLIN;
    act_set[0].revents = 0;
    int num_set = 1;

    while(1){
        int ret = poll(act_set, num_set, -1);
        if (ret > 0){
            for (size_t i = 0; i < num_set; i++)
            {
                if (act_set[i].revents & POLLIN)
                {
                    std::cout << "Get POLLIN at fd " << act_set[i].fd << std::endl;
                    act_set[i].revents &= ~POLLIN;
                    if (i == 0)
                    {
                        size = sizeof(client);
                        newsock = accept(sock, (struct sockaddr*)&client, &size); 
                        std::cout << "new client at port " << ntohs(client.sin_port) << std::endl;
                        if (num_set < 100) 
                        {
                            act_set[num_set].fd = newsock;
                            act_set[num_set].events = POLLIN;
                            act_set[num_set].revents = 0;
                            num_set++;
                        }                       
                    } 
                    else 
                    {
                        err = readFromClient(act_set[i].fd, buf);
                        if (err < 0 || strstr(buf, "stop")){
                            std::cout << "get stop" << std::endl;
                            close(act_set[i].fd);
                            if (i < num_set - 1)
                            {
                                act_set[i] = act_set[num_set - 1];
                                num_set--;
                                i--;
                            }
                        }
                        else
                            writeToClient(act_set[i].fd, buf);
                    }
                }
            }            
        }
    }

    close(sock);
    return 0;
}

//g++ poll_server.cpp -o server
