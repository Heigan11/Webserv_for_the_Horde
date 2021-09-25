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

#define SERVER_PORT 5555
#define BUFLEN      512

int writeToServer(int fd){
    char buf[BUFLEN];

    std::cout << "send to server > ";
    fgets(buf, BUFLEN, stdin);
    buf[strlen(buf) - 1] = '\0';
    write(fd, buf, strlen(buf) + 1);
    if (strstr(buf, "stop"))
        return -1;
    return 0;
}
int readFromServer(int fd){
    char buf[BUFLEN];
    int nbytes;

    nbytes = read(fd, buf, BUFLEN);
    if (nbytes == 0)
        std::cout << "Client: no msg" << std::endl;
    else
        std::cout << "Server's replay: " << buf << std::endl;
    return 0;
}

int main(){

    int sock;
    int isconnect;

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)); 

    isconnect = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (isconnect < 0){
        std::cerr << "Client: connection failure" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Connection is ready" << std::endl;

    while (1)
    {
        if (writeToServer(sock) < 0)
            break;
        if (readFromServer(sock) < 0)
            break;
    }
    std::cout << "The end" << std::endl;
    
    close(sock);
    return 0;
}

//g++ poll_client.cpp -o client
