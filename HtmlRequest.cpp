#include "HtmlRequest.h"
#include <string>

// #include <sys/socket.h> // For socket functions
// #include <netinet/in.h> // For sockaddr_in
// #include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
// #include <unistd.h> // For read
#include <map>
#include <iterator>
#include <sstream>

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

    std::cout << "request: "
              << "\r\n";

    std::cout << request << "\r\n";
    pos = s.find(delimiter); // original get request
    request = s.substr(0, pos);

    requestLine = request;

    s.erase(0, pos + delimiter.length());

    headersAndBody = s;

    std::cout << "headers parsse: "
              << "\r\n";

    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        token = s.substr(0, pos);

        size_t kv_break = token.find(": ");

        if (kv_break == -1)
        { // message body
            // body = token;
            HtmlRequest::parseBody(s);
            break;
        }

        header_key = token.substr(0, kv_break);
        header_val = token.substr(kv_break + 2, token.length());

        m[header_key] = header_val;

        s.erase(0, pos + delimiter.length());
    }

    

    std::cout << "body parsed"
              << "\r\n";
    delimiter = " ";

    pos = request.find(delimiter); // original get request

    method = request.substr(0, pos);
    request.erase(0, pos + delimiter.length());

    pos = request.find(delimiter);
    url = request.substr(0, pos);
    request.erase(0, pos + delimiter.length());

    protocol = request.substr(0, pos);
    request.erase(0, pos + delimiter.length());

    delimiter = "http://";

    std::cout << "url parsing: "
              << "\r\n";

    bool parsed = HtmlRequest::parseUrl("http://");
    if (!parsed)
    {
        parsed = HtmlRequest::parseUrl("https://");
    }
    if (!parsed)
    {
        parsed = HtmlRequest::parseUrl("");
    }

    modifiedRequestLine = method + " " + url + " " + protocol;
}

bool HtmlRequest::parseUrl(std::string delimiter)
{

    if (delimiter == "")
    {
        delimiter = '/';
        int pos = url.find(delimiter);
        host = url.substr(0, pos);

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

        std::cout << "host: "
                  << "\r\n";
        std::cout << host << "\r\n";

        std::cout << "pos: "
                  << "\r\n";
        std::cout << pos << "\r\n";

        if (pos != -1)
        {
            port = host.substr(pos + delimiter.length(), host.length());
            host = host.substr(0, pos);
        }
        else
        {
            port = "80";
        }

        std::cout << "port: "
                  << "\r\n";

        std::cout << port << "\r\n";
        return true;
    }

    int pos;
    if ((pos = url.find(delimiter)) != std::string::npos)
    {
        std::string scheme = url.substr(0, pos);
        url.erase(0, pos + delimiter.length());

        delimiter = '/';
        pos = url.find(delimiter);
        host = url.substr(0, pos);

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

        std::cout << "host: "
                  << "\r\n";
        std::cout << host << "\r\n";

        if (pos != -1)
        {
            port = host.substr(pos + delimiter.length(), host.length());
            host = host.substr(0, pos);
        }
        else
        {
            port = "80";
        }

        std::cout << "port: "
                  << "\r\n";

        std::cout << port << "\r\n";
        return true;
    }
    return false;
}

std::string HtmlRequest::printHeaders()
{

    std::stringstream ss;
    for (std::map<std::string, std::string>::iterator it = m.begin(); it != m.end(); it++)
    {
        ss << it->first // string (key)
           << ':'
           << it->second // string's value
           << "\r\n";
    }
    return ss.str();
}

bool HtmlRequest::checkIfInHeaders(std::string s) {
    std::map<std::string, std::string>::iterator iter = m.find(s);
    if (iter != m.end())
    {
        return true;
    }
    return false;
}

std::string HtmlRequest::getHeader(std::string s) { //should always be called after calling checkIfInHeaders, will crash otherwise
    std::map<std::string, std::string>::iterator iter = m.find(s);
    return iter->second;
}



void HtmlRequest::parseBody(std::string s) // s should contain the body
{
    std::stringstream ss;

    if (checkIfInHeaders("Content-Length"))
    {
        body = s.substr(2, 2+atoi(getHeader("Content-Length").c_str())); // 2 to offset the /r/n
    }
    else
    {
        body = "";
    }
}

std::string HtmlRequest::printRequest()
{
    return method + " " + url + " " + protocol + "\r\n" + HtmlRequest::printHeaders() + "\r\n" + body;
}

std::string HtmlRequest::printConditionalRequest(std::string date, std::string etag){
    
    std::string s = method + " " + url + " " + protocol + "\r\n" + "Host: " + host + "\r\n" + "If-Modified-Since: " + date;

    if (etag != ""){
        return s + "\r\n" + "If-None-Match: " + etag + "\r\n\r\n";
    }
    return s + "\r\n\r\n";
}