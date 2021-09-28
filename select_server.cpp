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

#define PORT    5555
#define BUFLEN  4096

int readFromClientHTTP(int fd, char *buf){
    int nbytes;

    nbytes = read(fd, buf, BUFLEN);
    if (nbytes <= 0)
        return -1;
    else
        std::cout << "Server got http request: " << buf << std::endl;
    return 0;
}

int writeToClientHTTP(int fd, char *buf){
    int nbytes;
    int ret;

    std::stringstream http;
    std::stringstream html;

    char *p = strstr(buf, "index.html");

    if (p && p - buf < 20){
        html << "<!DOCTYPE html>\r\n";
        html << "<html>\r\n";
        html << "<head>\r\n";
        html << "<meta charset = \"cp1251\">\r\n";
        html << "<title>Ecole server</title>\r\n";
        html << "<head>\r\n";
        html << "<body>\r\n";
        html << "<h2>For The Horde!</h2>\r\n";
        html << "<body>\r\n";
        html << "<html>\r\n";

        http << "HTTP/1.1 200 OK\r\n";
        http << "Connection: keep-alive\r\n";
        http << "Content-type: text/html\r\n";
        http << "Content-length: " << html.str().length() << "\r\n";
        http << "\r\n";
        http << html.str();
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
    // if (nbytes < 0)
    //     ret = -1;
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
                    if (err < 0){
                        close(i);
                        FD_CLR(i, &active_set);
                        std::cout << "Close socket = " << i << std::endl;
                    }

                    std::cout << "Write to client" << std::endl;
                    writeToClientHTTP(i, buf);
                }
            }
        }     
    } 
    // for (size_t i = 0; i < FD_SETSIZE; i++){
    //     if (FD_ISSET(i, &read_set))
    //         close(i);
    // }
    // return 0;
}

//g++ select_server.cpp -o server
