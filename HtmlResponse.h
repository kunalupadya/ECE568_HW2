#ifndef __HTMLREQ_H__
#define __HTMLREQ_H__

#include <map>
#include <string>

class HtmlResponse
{
    
public:
std::map<std::string, std::string> m;
    std::string method;
    std::string url;
    std::string protocol;
    std::string headersAndBody;
    std::string host;
    std::string port;
    std::string requestLine;
    std::string body;

    HtmlResponse(char *buffer);
    bool parseUrl(std::string delimiter);

    std::string printResponse();
};

#endif