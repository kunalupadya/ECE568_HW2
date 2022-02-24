#ifndef __SOC_H__
#define __SOC_H__
#include <sys/socket.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream> // For cout
#include <string>
// #include <netdb.h> /* struct hostent, gethostbyname */
// #include <arpa/inet.h>

class Socket
{
public:
    int sockfd;
    struct addrinfo *addr; //may or may not be initialized depending on which constructor is used, it's up to the user to ensure that it is used correctly

    Socket(int a, int b, int c)
    {
        sockfd = socket(a, b, c);

        if (sockfd == -1)
        {
            std::cout << "Failed to create socket. errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }
        int optval = 1;
        if ((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int))) == -1)
        {
            close(sockfd);
            // TODO throw error socket not reusable
        }
    }

    Socket(std::string host, std::string port)
    {
        int x;

        bzero(&addr, sizeof(addr));
        std::cout << "done wid it3" << std::endl;

        std::cout << host.c_str() << std::endl;
        x = getaddrinfo(host.c_str(), port.c_str(), NULL, &addr);

        sockfd = socket(addr->ai_family, SOCK_STREAM, 0);

        if (sockfd == -1)
        {
            std::cout << "Failed to create socket. errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }
        int optval = 1;
        if ((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int))) == -1)
        {
            close(sockfd);
            // TODO throw error socket not reusable
        }

        x = connect(sockfd, addr->ai_addr, (int)addr->ai_addrlen);
        std::cout << x << std::endl;

        if (x < 0)
        {
        std::cout << "ERROR CONNECTING1 :(" << std::endl;
        // TODO ERROR, close out the socket
        }
    }
    ~Socket()
    {
        std::cout << "destructor\n";
        close(sockfd);
    };
};
#endif