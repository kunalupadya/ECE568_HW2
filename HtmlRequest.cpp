#include "HtmlRequest.h"
#include <string>

// #include <sys/socket.h> // For socket functions
// #include <netinet/in.h> // For sockaddr_in
// #include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
// #include <unistd.h> // For read
#include <map>

HtmlRequest::HtmlRequest(char *buffer)
{
    std::string s = buffer;
    std::string delimiter = "\r\n";

    size_t pos = 0;
    std::string token;

    // std::map<std::string, std::string> m;

    std::string header_key;
    std::string header_val;

    std::string request;

    pos = s.find(delimiter); // original get request
    request = s.substr(0, pos);

    requestLine = request;
    std::cout << request << std::endl;
    s.erase(0, pos + delimiter.length());

    headersAndBody = s;

    std::cout << "headersAndBody:\n" << headersAndBody << std::endl;

    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        std::cout << ":o" << std::endl;
        token = s.substr(0, pos);

        size_t kv_break = token.find(": ");

        if (kv_break == -1){ //message body
            body = token;
            break;
        }


        header_key = token.substr(0, kv_break);
        header_val = token.substr(kv_break + 2, token.length());

        std::cout << header_key << std::endl;
        std::cout << header_val << std::endl;

        m[header_key] = header_val;

        std::cout << ":p" << std::endl;
        s.erase(0, pos + delimiter.length());
    }

std::cout << "done" << std::endl;
    delimiter = " ";

    std::cout << "request: " << request << std::endl;
    std::cout << "delimiter: " << delimiter << std::endl;
    pos = request.find(delimiter); // original get request

    // request =
    method = request.substr(0, pos);
    std::cout << "method: " << method << std::endl;
    request.erase(0, pos + delimiter.length());

    pos = request.find(delimiter);
    url = request.substr(0, pos);
    std::cout << "url: " << url << std::endl;
    request.erase(0, pos + delimiter.length());

    protocol = request.substr(0, pos);
    std::cout << "protocol: " << protocol << std::endl;
    request.erase(0, pos + delimiter.length());

    delimiter = "http://";

    bool parsed = HtmlRequest::parseUrl("http://");
    if (!parsed)
    {
        parsed = HtmlRequest::parseUrl("https://");
    }
    if (!parsed)
    {
        parsed = HtmlRequest::parseUrl("");
    }
    std::cout << "parsed: " << parsed << std::endl;
}

bool HtmlRequest::parseUrl(std::string delimiter)
{

    if (delimiter == "")
    {
        HtmlRequest::parseUrl("https://");
    }

    int pos;
    if ((pos = url.find(delimiter)) != std::string::npos)
    {
        std::string scheme = url.substr(0, pos);
        url.erase(0, pos + delimiter.length());
        std::cout << "url1: " << url << std::endl;

        delimiter = '/';
        pos = url.find(delimiter);
        std::cout << "pos: " << pos << std::endl;
        std::cout << "pos: " << url.length() << std::endl;
        host = url.substr(0, pos);
        std::cout << "host: " << host << std::endl;

        if (pos == -1)
        {
            url = "/";
        }
        else
        {
            url = "/" + url.substr(pos + delimiter.length());
        }

        delimiter = ':';
        pos = host.find(delimiter);

        if (pos != -1)
        {
            port = host.substr(pos + delimiter.length());
            std::cout << "port: " << port << std::endl;
        }
        else
        {
            port = "80";
        }
        return true;
    }
    return false;
}

std::string HtmlRequest::printRequest(){
    return method + " " + url + " " + protocol + "\r\n" + headersAndBody;
}