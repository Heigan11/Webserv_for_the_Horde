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
#include <fstream>
#include <sys/wait.h>

#define PORT    8080
#define BUFLEN  4096

#define STDIN		0
#define STDOUT		1
#define STDERR		2

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

// char **getCgiEnv(){
//     char **env = 
// }

void cgiCall(char **env, int fd, char *body)
{
    pid_t	pid;
    int     status;
    int     reqFd;
    char    *argv[10] = { "/bin/sh", "cgi.sh", NULL };
 
    pid = fork();

    if (pid < 0)
        exit(-1);

    else if (pid == 0){

        std::ofstream out("./html/req_body_tmp.txt");
        reqFd = open("./html/req_body_tmp.txt", O_RDWR, O_CREAT, O_TRUNC);
        write(reqFd, body, strlen(body));  

        dup2(reqFd, STDIN);
        dup2(fd, STDOUT);
        if (execve("/bin/sh", argv, env) < 0){
            std::cerr << "execute error" << std::endl;
            exit(-1);
        }   
        close(reqFd);   
        exit(0);
    }
    else
        waitpid(pid, &status, 0);
}

void send_to_fd(int fd){
    std::stringstream http;

    http << "HTTP/1.1 200 OK\r\n";
    http << "Connection: keep-alive\r\n";
    http << "Content-type: text/html\r\n";

    std::ifstream file ("lion.txt");
    if (!file) {
        std::cerr << "file was not open" << std::endl; 
        return;
    }
    std::string line;
	std::string res;
	while (std::getline(file, line))
		res += line;

    http << "Content-length: " << res.length() << "\r\n";
    http << "\r\n";
    http << res;

    send(fd, http.str().c_str(), http.str().length() + 1, 0);
}

int writeToClientHTTP(int fd, char *buf, char **env){
    int nbytes;
    int ret;

    std::stringstream http;
    std::string path;
    std::string body;

    char *p = strstr(buf, "index");
    if (strstr(buf, "horoscope")){
        std::cerr << "FORK" << std::endl;
        if (strstr(buf, "POST")){
            std::cerr << ">>>>>>>>>>>buf>>>>>>>>>>>>>" << std::endl;
            std::cerr << buf << std::endl;
            std::cerr << "<<<<<<<<<<<buf<<<<<<<<<<<<<" << std::endl;
            body = strrchr(buf, '\n') + 1;
            if (body.empty()){
                *strrchr(buf, '\n') = ' ';
                body = strrchr(buf, '\n') + 1;
            }
            std::cerr << ">>>>>>>>>>>body>>>>>>>>>>>>>" << std::endl;
            std::cerr << body << std::endl;
            std::cerr << "<<<<<<<<<<<body<<<<<<<<<<<<<" << std::endl;
        }
        *strchr(buf, '\n') = '\0';
        std::string req_1 = buf;
        std::string req = "REQUEST=" + req_1;
        char *cstr = new char[req.length() + 1];
        strcpy(cstr, req.c_str());
        env[0] = cstr;

        if (!body.empty()){
            char *cstr_2 = new char[body.length() + 1];
            strcpy(cstr_2, body.c_str());
            cgiCall(env, fd, cstr_2);
            delete [] cstr_2;
        } else {
            cgiCall(env, fd, "");
        }
        delete [] cstr;
        return -1;
    }

    if (p && p - buf < 20){

        http << "HTTP/1.1 200 OK\r\n";
        http << "Connection: keep-alive\r\n";
        http << "Content-type: text/html\r\n";

        path = "index.txt";
        std::ifstream file (path);
        if (!file) {
            std::cerr << "file was not open" << std::endl; 
            return -1;
        }
        std::string line;
	    std::string res;
	    while (std::getline(file, line))
		    res += line;

        http << "Content-length: " << res.length() << "\r\n";
        http << "\r\n";
        http << res;

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
    std::cout << "Write nbytes = " << nbytes << std::endl;
    return ret;
}

int writeToClientHTTPForm(int fd, char *buf){
    return 0;
}

int main(int ac, char **av, char **env){
    setlocale(LC_ALL, "rus");

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
                    if (writeToClientHTTP(i, buf, env) < 0){
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
