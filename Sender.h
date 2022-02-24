#ifndef __SEND_MY_H__
#define __SEND_MY_H__
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>      // For exit() and EXIT_FAILURE
#include <iostream>     // For cout
#include <unistd.h>     // For read
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "HtmlResponse.h"
#include "Socket.h"
#include <string>

class Sender
{
public:
    Sender(){};

    HtmlResponse send(std::string request, Socket sendSocket)
    {
        char response[100000];

        int total = strlen(request.c_str());
        int sent = 0;
        int bytes;
        do
        {
            bytes = write(sendSocket.sockfd, request.c_str() + sent, total - sent);
            if (bytes < 0)
                std::cout << "ERROR writing message to socket" << std::endl;
            if (bytes == 0)
                break;
            sent += bytes;
        } while (sent < total);

        std::cout << "sent" << std::endl;

        std::cout << request.c_str() << std::endl;

        /* receive the response */
        memset(response, 0, sizeof(response));
        total = sizeof(response) - 1;

        std::cout << "sent1" << std::endl;
        std::cout << request << std::endl;
        int received = 0;

        std::vector<char> v(1000000);
        bytes = recv(sendSocket.sockfd, &v.data()[received], total - received, 0);

        received += bytes;

        HtmlResponse resp(v);

        // resp.body =
        std::cout << "got 2 here: " << std::endl;
        if (resp.checkIfInHeaders("Content-Length"))
        {
            int body_len = atoi(resp.getHeader("Content-Length").c_str());
            std::cout << "body length: " << body_len << " " << resp.body.size() << std::endl;
            while (resp.body.size() < body_len)
            {
                bytes = recv(sendSocket.sockfd, &v.data()[received], total - received, 0);

                received += bytes;

                HtmlResponse resp(v);
                std::cout << "body length: " << body_len << " " << resp.body.size() << std::endl;
            }
        }
        // if (resp.checkIfInHeaders("Transfer-Encoding"))
        // {
        //     std::cout << "got 3 here: " << std::endl;
        //     if (resp.getHeader("Transfer-Encoding") == "chunked")
        //     {
        //         std::cout << "got 4 here: " << resp.start_len << std::endl;
        //         std::vector<char>::iterator start = 2 + v.begin() + resp.start_len;
        //         std::vector<char>::iterator next = 2 + v.begin() + resp.start_len;
        //         while (true)
        //         {
                    
        //             std::string match(next, next + 2);
        //             // std::cout << match << std::endl;
        //             next += 1;
        //             if (match == "\r\n")
        //             {
        //                 std::string hex(start, next-1);
        //                 std::cout << "hex:" + hex + "hex:"<< std::endl;
        //                 std::cout << "hexlength:" << hex.length() << "hex:"<< std::endl;
        //                 // std::string hex1(start, next);
        //                 //                         std::cout << "hex2:" + hex1 + "hex2:"<< std::endl;
        //                 // std::cout << "hexlength2:" << hex1.length() << "hex2:"<< std::endl;
                        
        //                 unsigned long x = std::stoul(hex, nullptr, 16);
        //                 // int x = 1;
        //                 std::cout << "x: " << x  << std::endl;
        //                 start = next + 2;
        //                 next = next + 2;
        //                 if (x == 0)
        //                 {
        //                     break;
        //                 }
        //                 else if (x > v.end() - next - 20)
        //                 {
        //                     int total = x - std::distance(next, v.end()) + 20; // guarantees that we get the digits of the next hex
        //                     bytes = recv(sendSocket.sockfd, &v.data()[received], total, 0);
        //                     if (bytes == 0)
        //                     {
        //                         break;
        //                     }
        //                     received += bytes;
        //                     start = next + x + 0;
        //                     next = next + x + 0;
        //                 }
        //                 else
        //                 {
        //                     start = next + x + 0;
        //                     next = next + x + 0;
        //                 }
        //             }
        //         }
        //     }
        // }

        std::cout << "response :ooo" << std::endl;
        std::cout << resp.printResponse() << std::endl;
        std::cout << "response :pppp" << std::endl;
        return resp;
    };
};
#endif