#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>      // For exit() and EXIT_FAILURE
#include <iostream>     // For cout
#include <unistd.h>     // For read
#include <thread>
#include <map>
#include <cstring>
#include <netdb.h> /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "HtmlRequest.h"
#include "HtmlResponse.h"
#include "Cache.h"
#include "Socket.h"
#include "Sender.h"
#include "Runner.h"
#include "easylogging++.h"
#include <mutex>

using namespace std;

INITIALIZE_EASYLOGGINGPP

Cache cache;
std::mutex queue_mutex;

void run(int connection, int id)
{
  std::cout << "connected" << std::endl;

  char buffer[10000];
  std::cout << "bout to read: "
            << "\r\n";

  int res = recv(connection, buffer, 10000, 0);
  std::cout << res << "\r\n";

  if (res == 0)
  {
    std::cout << "connection closed by client"
              << "\r\n";
    return; // connection closed by client
  }

  // auto bytesRead = read(connection, buffer, 10000);

  std::cout << "req: "
            << "\r\n";
  std::cout << buffer << "\r\n";
  HtmlRequest req(buffer);

  std::cout << req.printRequest() << std::endl;

  std::time_t t = std::time(0); // get time now
  std::tm *now = std::localtime(&t);
  char timeBuffer[300];
  strftime(timeBuffer, 300, "%a, %d %b %Y %H:%M:%S %Z", now);
  LOG(INFO) << std::to_string(id) << ": \"" << req.requestLine << "\" from " << req.host << " @ " << timeBuffer;

  if (req.method == "CONNECT")
  {

    std::cout << "connect request recieved" << std::endl;
    Socket sendSocket(req.host, req.port);
    std::cout << "socket created" << std::endl;

    std::string resp = "HTTP/1.1 200 OK\r\n\r\n";
    LOG(INFO) << std::to_string(id) << ": "
              << "Responding \"HTTP/1.1 200 OK\"";
    int ret = send(connection, resp.c_str(), strlen(resp.c_str()), 0);

    if (ret <= 0)
    {
      std::cout << "failed to establish connection with client, client unresponsive" << std::endl;
      std::cout << errno << std::endl;
      return;
    }

    std::cout << "sent " << resp << std::endl;

    fd_set readfds;
    fd_set writefds;

    while (true)
    {
      FD_ZERO(&readfds);
      FD_ZERO(&writefds);

      // add master socket to set
      FD_SET(sendSocket.sockfd, &readfds);
      FD_SET(sendSocket.sockfd, &writefds);

      FD_SET(connection, &readfds);
      FD_SET(connection, &writefds);

      int max_sd = sendSocket.sockfd + 1;

      if (sendSocket.sockfd < connection)
      {
        max_sd = connection + 1;
      }

      std::cout << "blocking here" << std::endl;
      int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
      std::cout << "past it" << std::endl;
      if (FD_ISSET(sendSocket.sockfd, &readfds))
      {

        char buffer[10000];
        int res = recv(sendSocket.sockfd, buffer, 10000, 0);
        std::cout << "send sock " << res << std::endl;
        if (res <= 0)
        {
          close(sendSocket.sockfd);
          close(connection);
          break;
        }
        if (FD_ISSET(connection, &writefds))
        {
          int ret = send(connection, buffer, 10000, 0);
          if (ret <= 0)
          {
            close(sendSocket.sockfd);
            close(connection);
            break;
          }
        }
      }
      else if (FD_ISSET(connection, &readfds))
      {
        char buffer[10000];
        int res = recv(connection, buffer, 10000, 0);
        std::cout << "conn " << res << std::endl;
        if (res <= 0)
        {

          close(sendSocket.sockfd);
          close(connection);
          LOG(INFO) << std::to_string(id) << ": \""
                << "Tunnel Closed";
          break;
        }
        if (FD_ISSET(sendSocket.sockfd, &writefds))
        {
          int ret = send(sendSocket.sockfd, buffer, 10000, 0);
          if (ret <= 0)
          {
            close(sendSocket.sockfd);
            close(connection);
            LOG(INFO) << std::to_string(id) << ": \""
                << "Tunnel Closed";
            break;
          }
        }
      }
    }
    
    return;
  }
  std::unique_lock<std::mutex> lock(queue_mutex);
  std::pair<int, bool> cacheResponse = cache.containsRequest(req);
  int cacheLocation = cacheResponse.first;
  bool needsValidation = cacheResponse.second;

  if (cacheLocation != -1)
  {

    HtmlResponse resp = cache.cache[cacheLocation].second;
    lock.unlock();
    if (needsValidation)
    {
      LOG(INFO) << std::to_string(id) << ": \""
                << "in cache, requires validation";
      std::string etag = "";
      if (resp.checkIfInHeaders("ETag"))
      {
        etag = resp.getHeader("ETag");
      }

      Socket sendSocket(req.host, req.port);

      Sender sender;

      LOG(INFO) << std::to_string(id) << ": "
                                         "Requesting "
                << req.requestLine << " from " << req.host;

      HtmlResponse validationResponse = sender.send(req.printConditionalRequest(resp.getHeader("Date"), etag), sendSocket);

      LOG(INFO) << std::to_string(id) << ": "
                                         "Recieved "
                << validationResponse.firstLine << " from " << req.host;

      close(sendSocket.sockfd);

      std::cout << "validation response:" << std::endl
                << validationResponse.printResponse() << std::endl;

      if (validationResponse.resp_type == "304")
      {
        send(connection, resp.printResponse().c_str(), strlen(resp.printResponse().c_str()), 0);
        send(connection, &resp.body.data()[0], resp.body.size(), 0);
        LOG(INFO) << std::to_string(id) << ": "
                  << "Responding " << resp.firstLine;
        close(connection);
      }
      else
      {
        lock.lock();
        cache.storeRequestInCache(req, validationResponse, id);
        lock.unlock();

        send(connection, validationResponse.printResponse().c_str(), strlen(validationResponse.printResponse().c_str()), 0);
        send(connection, &validationResponse.body.data()[0], validationResponse.body.size(), 0);
        LOG(INFO) << std::to_string(id) << ": "
                  << "Responding " << validationResponse.firstLine;
        close(connection);
      }

      return;
    }
    else
    {
      LOG(INFO) << std::to_string(id) << ": "
                << "in cache, valid";
      send(connection, resp.printResponse().c_str(), strlen(resp.printResponse().c_str()), 0);
      send(connection, &resp.body.data()[0], resp.body.size(), 0);
      LOG(INFO) << std::to_string(id) << ": "
                << "Responding " << resp.firstLine;
      close(connection);
      return;
    }
  }
  else
  {
    lock.unlock();
    if (req.method == "GET")
    {
      LOG(INFO) << std::to_string(id) << ": "
                << "not in cache" << std::endl;
    }
  }

  Socket sendSocket(req.host, req.port);

  Sender sender;

  HtmlResponse resp = sender.send(req.printRequest(), sendSocket);

  close(sendSocket.sockfd);
  std::cout << "sending" << std::endl;
  send(connection, resp.printResponse().c_str(), strlen(resp.printResponse().c_str()), 0);
  std::cout << "sending1" << std::endl;
  send(connection, &resp.body.data()[0], resp.body.size(), 0);
  std::cout << "sending2" << std::endl;
  lock.lock();
  cache.storeRequestInCache(req, resp, id);
  lock.unlock();
  std::cout << "Cache size: " << cache.cache.size() << std::endl;

  close(connection);
}

int main(int argc, char const *argv[])
{
  std::cout << "Hello Docker container!" << std::endl;

  Cache cache;

  Socket rcvSocket(AF_INET, SOCK_STREAM, 0);

  std::cout << "socket" << std::endl;

  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;
  sockaddr.sin_port = htons(12345); // htons is necessary to convert a number to
                                    // network byte order
  if (bind(rcvSocket.sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
  {
    std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  int len_out_fds = 10;

  std::cout << "bound" << std::endl;

  // Start listening. Hold at most 10 connections in the queue

  if (listen(rcvSocket.sockfd, 10) < 0)
  {
    std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "listened" << std::endl;

  std::string filepath = "proxy.log";
  // Logger::startLog(filepath);

  el::Configurations conf("logger.conf");
  // Reconfigure single logger
  el::Loggers::reconfigureLogger("default", conf);
  // Actually reconfigure all loggers instead
  el::Loggers::reconfigureAllLoggers(conf);
  // Now all the loggers will use configuration from file
  LOG(INFO) << "My first info log using default logger";
  LOG(INFO) << "My sec info log using default logger";
  LOG(INFO) << "My third info log using default logger";
  // ThreadPool threadPool(cache);
  std::cout << "accepting" << std::endl;
  int id = 1;
  while (1)
  {
    // Grab a connection from the queue
    auto addrlen = sizeof(sockaddr);
    int connection = accept(rcvSocket.sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
    if (connection < 0)
    {
      std::cout << "Failed to grab connection. error: " << errno << std::endl;
      exit(EXIT_FAILURE);
    }

    thread t1(run, connection, id);
    t1.detach();
    id++;
  }
  close(rcvSocket.sockfd);

  // Close the connections

  return 0;
}
