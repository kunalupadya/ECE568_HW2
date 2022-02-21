#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read
#include <thread>
#include <map>
#include <cstring>
#include <netdb.h> /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "HtmlRequest.h"

using namespace std;
class Geeks
{
    public:
    int id;
     
    //Default Constructor
     
    //Parameterized Constructor
    Geeks(int x)
    {
        cout << "Parameterized Constructor called" << endl;
        id=x;
    }
};


void task1(std::string msg)
{
    sleep(2);
    std::cout << "task1 says: " << msg;
}

int main(int argc, char const *argv[])
{
  std::cout << "Hello Docker container!" << std::endl;



  // Create a socket (IPv4, TCP)
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    std::cout << "Failed to create socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "socket" << std::endl;

  // Listen to port 9999 on any address
  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;
  sockaddr.sin_port = htons(9999); // htons is necessary to convert a number to
                                   // network byte order
  if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
    std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

    int len_out_fds = 10;

    int outsockfds[len_out_fds];

    memset( outsockfds, 0, len_out_fds*sizeof(int) );

    // Create a socket (IPv4, TCP)
    // for (size_t i = 0; i < len_out_fds; i++)
    // {
    //     outsockfds[i] = socket(AF_INET, SOCK_STREAM, 0);
    //     if (sockfd == -1) {
    //         std::cout << "Failed to create socket. errno: " << errno << std::endl;
    //         exit(EXIT_FAILURE);
    //     }
    //     std::cout << "socket" << std::endl;

    //     // Listen to port 9999 on any address
    //     sockaddr_in sockaddr;
    //     sockaddr.sin_family = AF_INET;
    //     sockaddr.sin_addr.s_addr = INADDR_ANY;
    //     sockaddr.sin_port = htons(10000+i); // htons is necessary to convert a number to
    //                                     // network byte order
    //     if (bind(outsockfds[i], (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
    //         std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
    //         exit(EXIT_FAILURE);
    //     }
    // }
    


  std::cout << "bound" << std::endl;

  // Start listening. Hold at most 10 connections in the queue
  if (listen(sockfd, 10) < 0) {
    std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "listened" << std::endl;

  while (1)
  {
      // Grab a connection from the queue
  auto addrlen = sizeof(sockaddr);
  int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
  if (connection < 0) {
    std::cout << "Failed to grab connection. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }


  std::cout << "connected" << std::endl;

//   std::thread t1(task1, "Hello");
//   std::thread t2(task1, "yeet");

//   std::thread t3(task1, "meet");

  // Read from the connection
  char buffer[10000];
  auto bytesRead = read(connection, buffer, 10000);

  std::string s = buffer;

  std::cout << s << std::endl;

  HtmlRequest req (buffer);

  std::cout << req.method << std::endl;
  std::cout << req.url << std::endl;
  std::cout << req.requestLine << std::endl;
  std::cout << req.protocol << std::endl;
  std::cout << req.host << std::endl;
  std::cout << "port: "<<req.port << std::endl;
  std::cout << req.headersAndBody << std::endl;

  std::cout << "done wid it" << std::endl;


  std::cout << req.printRequest() << std::endl;


std::cout << "done wid it1" << std::endl;


    
    struct addrinfo *addr;
        int x;
    ptrdiff_t fd;
std::cout << "done wid it2" << std::endl;


    // hostname = "google.com";

    bzero(&addr, sizeof(addr));
    std::cout << "done wid it3" << std::endl;

    std::string port = req.port;
std::cout << req.host.c_str() << std::endl;
    x =  getaddrinfo(req.host.c_str(), "80", NULL, &addr);
    fd = socket(addr->ai_family, SOCK_STREAM, 0);

        std::cout << addr->ai_addr << std::endl;
    std::cout << port << std::endl;
    
    x = connect(fd, addr->ai_addr, (int)addr->ai_addrlen);

    std::cout << x << std::endl;
    
        if (x < 0){
        std::cout << "ERROR CONNECTING1 :(" << std::endl;
        
    }

    std::cout << "CONNECTED!!!!!1 :)))" << std::endl;


    // struct hostent *server = gethostbyname("www.google.com");

        

    // if (server == NULL) error("ERROR, no such host");

//     int i = 0;
//      outsockfds[i] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//         if (sockfd == -1) {
//             std::cout << "Failed to create socket. errno: " << errno << std::endl;
//             exit(EXIT_FAILURE);
//         }
//         std::cout << "socket" << std::endl;


//         // host = gethostbyname(url.c_str());
        

//         // Listen to port 9999 on any address
//         sockaddr_in sockaddr;
//         sockaddr.sin_family = AF_INET;
//         sockaddr.sin_addr.s_addr = *((unsigned long*) server->h_addr);//inet_addr(host.c_str());

//         // memcpy(&sockaddr.sin_addr.s_addr,server->h_addr,server->h_length);
//         sockaddr.sin_port = htons(8888+i); // htons is necessary to convert a number to
//                                         // network byte order
// std::cout << host << std::endl;
// std::cout << server->h_addr << std::endl;

//     if (connect(sockfd,(struct sockaddr *)&sockaddr,sizeof(sockaddr)) < 0){
//         std::cout << "ERROR CONNECTING" << std::endl;
        
//     }

    size_t total, received, sent, bytes;

    char response[100000];


    total = strlen(req.printRequest().c_str());
    sent = 0;
    do {
        bytes = write(fd,req.printRequest().c_str()+sent,total-sent);
        if (bytes < 0)
            std::cout << "ERROR writing message to socket" << std::endl;
        if (bytes == 0)
            break;
        sent+=bytes;
    } while (sent < total);

    std::cout << "sent" << std::endl;

    std::cout << req.printRequest().c_str() << std::endl;

    /* receive the response */
    memset(response,0,sizeof(response));
    total = sizeof(response)-1;
    

    std::cout << "sent1" << std::endl;

    bytes = read(fd,response+received,total-received);

    received = bytes;

    // send(connection, response+received, strlen(response), 0);

    // if chunked
    // std::string delimiter = "\r\n";
    // do { 
    //     bytes = read(fd,response+received,total-received);
    //     if (bytes < 0)
    //         std::cout << "ERROR reading response from socket" << std::endl;
    //     if (bytes == 0)
    //         break;
    //     received+=bytes;

    //     std::string recent = response+received;
    //     std::cout << recent << std::endl;
    //     // int pos = recent.find(delimiter);

        

    //     if (received >3){
    //         // std::cout << "rec" << std::endl;;
    //         // std::cout << response + received - 4 << std::endl;
    //         std::string lastFour =  response + received - 4;
    //         if (lastFour == "\r\n\r\n"){ // not technically reading the bytes in a chunked manner, but it's ok
    //             std::cout << "response complete" << std::endl;
    //             break;
    //         }
    //     }
    //     // std::cout << bytes << std::endl;
        
    // } while (received < total);

    std::cout << "recieved" << std::endl;

    std::cout << bytes << std::endl;

    std::cout << response << std::endl;

    std::cout << received << std::endl;

    // std::cout << "recieved" << std::endl;

    // if (connect(outsockfds[i], (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == -1) {
    //     perror("connect");
    //     exit(EXIT_FAILURE);
    // }
    // std::cout << "Connected " << errno << std::endl;

    // t3.join();
  // Send a message to the connection
//   std::string response = "Good talking to you\n";
  send(connection, response, strlen(response), 0);




    close(connection); 




  

  }
  close(sockfd);
  

  // Close the connections


  return 0;
}