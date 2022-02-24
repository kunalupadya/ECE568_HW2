// #include "Sender.h"
// #include <sys/socket.h> // For socket functions
// #include <netinet/in.h> // For sockaddr_in
// #include <cstdlib>      // For exit() and EXIT_FAILURE
// #include <iostream>     // For cout
// #include <unistd.h>     // For read
// #include <netdb.h>      /* struct hostent, gethostbyname */
// #include <arpa/inet.h>
// #include "HtmlResponse.h"
// #include "Socket.h"
// #include <string>

// HtmlResponse Sender::send(std::string request, Socket sendSocket)
// {
//     char response[100000];

//     int total = strlen(request.c_str());
//     int sent = 0;
//     int bytes;
//     do
//     {
//         bytes = write(sendSocket.sockfd, request.c_str() + sent, total - sent);
//         if (bytes < 0)
//             std::cout << "ERROR writing message to socket" << std::endl;
//         if (bytes == 0)
//             break;
//         sent += bytes;
//     } while (sent < total);

//     std::cout << "sent" << std::endl;

//     std::cout << request.c_str() << std::endl;

//     /* receive the response */
//     memset(response, 0, sizeof(response));
//     total = sizeof(response) - 1;

//     std::cout << "sent1" << std::endl;
//     int received = 0;
//     bytes = read(sendSocket.sockfd, response + received, total - received);

//     received = bytes;

//     HtmlResponse resp(response);

//     std::cout << "response :ooo" << std::endl;
//     std::cout << resp.printResponse() << std::endl;
//     std::cout << "response :pppp" << std::endl;
//     return resp;
// };