#ifndef __HTMLRES_H__
#define __HTMLRES_H__

#include <map>
#include <string>
#include <vector>

class HtmlResponse
{
    
public:
std::map<std::string, std::string> m;
    std::string message;
    std::string resp_type;
    std::string protocol;
    std::string headersAndBody;
    std::string host;
    std::string port;
    std::string firstLine;
    std::vector<char> body;
    int start_len;

    HtmlResponse(std::vector<char>buffer);
    bool parseUrl(std::string delimiter);
    std::string printHeaders();
    void parseBody(std::vector<char> v);
    bool checkIfInHeaders(std::string s);
    std::string getHeader(std::string s);

    std::string printResponse();
};

#endif