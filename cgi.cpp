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

#define BUFLEN  4096

#define STDIN		0
#define STDOUT		1
#define STDERR		2

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

std::string find_request(char **env){
    int i = 0;
    char *buf;

    while (env[i]){
        if (env[i][0] == 'R' && env[i][1] == 'E' && env[i][2] == 'Q'){
            buf = strdup(strchr(env[i], '=') + 1);
            if (strchr(buf, '=')) {
            buf = strdup(strchr(buf, '=') + 1);
            *strchr(buf, ' ') = '\0';
            }
            break ;
        }
        i++;
    }
    return (buf);
}

int readFromClientHTTP(char **env){
//     int fd = find_fd(env);
//     std::cerr << "FD in fork = " << fd << std::endl;
    int fd = 4;
//    listen(fd, 3);
    char buf[BUFLEN];
    int nbytes;

    std::cerr << "READING DATA FORK" << std::endl;
    nbytes = recv(fd, buf, BUFLEN, 0);
    std::cerr << "READING DATA FORK CANCELLED" << std::endl;
    if (nbytes < 0){
        std::cerr << "reading error" << std::endl;
        return -1;
    }
    else if (nbytes == 0){
        std::cerr << "reading no data" << std::endl;
        return -1;
    }
    else {
        std::cerr << "Содержимое http-запроса FORK:" << std::endl;
        std::cerr << "========================" << std::endl << std::endl; 
        for (size_t i = 0; i < nbytes; i++){
            std::cerr << buf[i];
        }
        std::cerr << "========================" << std::endl;        
    }
    return 0;
}

std::string get_form(){

    std::stringstream http;
    std::string path;
    std::string result;

    http << "HTTP/1.1 200 OK\r\n";
    http << "Connection: keep-alive\r\n";
    http << "Content-type: text/html\r\n";

    path = "form.txt";
    std::ifstream file (path);
    if (!file) {
        std::cerr << "file was not open" << std::endl; 
        return nullptr;
    }
    std::string line;
	std::string res;
	while (std::getline(file, line))
		res += line;

    http << "Content-length: " << res.length() << "\r\n";
    http << "\r\n";
    http << res;

    return http.str();
}

std::string get_lion(){

    std::stringstream http;
    std::stringstream html;

    std::string msg = "Lion's msg";

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

    return http.str();
}

int main(int ac, char **av, char **env){

    int nbytes;
    int ret;
    int fd = find_fd(env);
    std::string request = find_request(env);
    std::cerr << "REQUEST = " << request << std::endl;

    std::string msg;

    msg = get_form();
    if (request.compare("Lion") == 0)
        msg = get_lion();

    send(1, msg.c_str(), msg.length() + 1, 0);

    //readFromClientHTTP(env);

    std::cerr << "FORK OVER" << std::endl;
}

//g++ cgi.cpp -o cgi

/*    
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
*/