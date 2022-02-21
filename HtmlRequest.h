#ifndef __HTMLREQ_H__
#define __HTMLREQ_H__

#include <map>
#include <string>

class HtmlRequest
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

    HtmlRequest(char *buffer);
    bool parseUrl(std::string delimiter);

    std::string printRequest();
};

#endif