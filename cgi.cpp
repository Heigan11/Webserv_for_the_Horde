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

int find_fd(char **env){
    int i = 0;
    char *buf;

    while (env[i]){
        if (env[i][0] == 'F' && env[i][1] == 'D' && env[i][2] == '='){
            buf = strdup(strchr(env[i], '=') + 1);
            break ;
        }
        i++;
    }
    return atoi(buf);
}

int main(int ac, char **av, char **env){

    int nbytes;
    int ret;
    int fd = find_fd(env);

    std::stringstream http;
    std::stringstream html;
    std::string msg = "HEllo!";

    html << "<!DOCTYPE html>\r\n";
    html << "<html>\r\n";
    html << "<head>\r\n";
    html << "<meta charset = \"cp1251\">\r\n";
    html << "<title>Ecole server</title>\r\n";
    html << "<head>\r\n";
    html << "<body>\r\n";
    html << "<h2>" << msg << "</h2>\r\n";
    html << "<body>\r\n";
    html << "<html>\r\n";

    http << "HTTP/1.1 200 OK\r\n";
    http << "Connection: keep-alive\r\n";
    http << "Content-type: text/html\r\n";
    http << "Content-length: " << html.str().length() << "\r\n";
    http << "\r\n";
    http << html.str();

    send(fd, http.str().c_str(), http.str().length() + 1, 0);
}

//g++ cgi.cpp -o cgi