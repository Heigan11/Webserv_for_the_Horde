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
#include <sstream>

#define PORT    8080
#define BUFLEN  4096

int readFromClientHTTP(int fd, char *buf){
    int nbytes;

    nbytes = recv(fd, buf, BUFLEN, 0);
    if (nbytes < 0){
        std::cout << "reading error" << std::endl;
        return -1;
    }
    else if (nbytes == 0){
        std::cout << "reading no data" << std::endl;
        return -1;
    }
    else {
        std::cout << "Содержимое http-запроса:" << std::endl;
        std::cout << "========================" << std::endl << std::endl; 
        for (size_t i = 0; i < nbytes; i++){
            std::cout << buf[i];
        }
        std::cout << "========================" << std::endl;        
    }
    return 0;
}

int writeToClientHTTP(int fd, char *buf){
    int nbytes;
    int ret;

    std::stringstream http;

    char *p = strstr(buf, "index.html");

    if (p && p - buf < 20){

        http << "HTTP/1.1 200 OK\r\n";
        http << "Connection: keep-alive\r\n";
        http << "Content-type: text/html\r\n";

        size_t lenght;
        std::stringstream buffer;
        char buf1[BUFLEN];
        int fd1 = open("./response.txt", O_RDONLY);
        std::cout << "fd1 = " << fd1 << std::endl;
        while (lenght = read(fd1, buf1, BUFLEN) > 0){
            buffer << buf1;
        }
        close(fd1);

        http << "Content-length: " << buffer.str().length() << "\r\n";
        http << "\r\n";
        http << buffer.str();

        ret = 0;
    } else {
        http << "HTTP/1.1 404 Not found\r\n";
        http << "Connection: close\r\n";
        http << "Content-type: image/png\r\n";
        http << "Content-length: 0\r\n";
        http << "\r\n";
        ret = -1;
    }

    nbytes = send(fd, http.str().c_str(), http.str().length() + 1, 0);
    if (nbytes < 0)
        ret = -1;
    std::cout << "Write ret = " << ret << std::endl;
    return ret;
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

    std::cout << "Server ready" << std::endl;

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
                    std::cout << "New socket = " << newsock << std::endl;
                } 
                else
                {
                    std::cout << "Socket = " << i << std::endl;
                    err = readFromClientHTTP(i, buf);
                    std::cout << "read ret  = " << err << std::endl;
                    if (err < 0){
                        close(i);
                        FD_CLR(i, &active_set);
                        std::cout << "Close socket read = " << i << std::endl;
                    }

                    std::cout << "Write to client" << std::endl;
                    if (writeToClientHTTP(i, buf) < 0){
                        close(i);
                        FD_CLR(i, &active_set);
                        std::cout << "Close socket write = " << i << std::endl;
                    };
                }
            }
        }     
    } 
    for (size_t i = 0; i < FD_SETSIZE; i++){
        if (FD_ISSET(i, &read_set))
            close(i);
    }
    return 0;
}

//g++ select_server.cpp -o server
