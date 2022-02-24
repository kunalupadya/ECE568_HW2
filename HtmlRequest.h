#ifndef __HTMLREQ_H__
#define __HTMLREQ_H__

#include <map>
#include <string>
#include "HtmlResponse.h"

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
    std::string modifiedRequestLine;
    std::string body;


    HtmlRequest(char *buffer);
    bool parseUrl(std::string delimiter);
    std::string printHeaders();
    void parseBody(std::string s);
    bool checkIfInHeaders(std::string s);
    std::string getHeader(std::string s);

    std::string printRequest();

    std::string printConditionalRequest(std::string date, std::string etag);
};

#endif