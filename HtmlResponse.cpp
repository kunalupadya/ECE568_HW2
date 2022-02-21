#include "HtmlResponse.h"
#include <string>

// #include <sys/socket.h> // For socket functions
// #include <netinet/in.h> // For sockaddr_in
// #include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
// #include <unistd.h> // For read
#include <map>

HtmlResponse::HtmlResponse(char *buffer)
{
    std::string s = buffer;
    std::string delimiter = "\r\n";

    size_t pos = 0;
    std::string token;

    // std::map<std::string, std::string> m;

    std::string header_key;
    std::string header_val;

    std::string response;

    pos = s.find(delimiter); // original response
    response = s.substr(0, pos);

    requestLine = response;
    std::cout << response << std::endl;
    s.erase(0, pos + delimiter.length());

    headersAndBody = s;

    std::cout << "headersAndBody:\n"
              << headersAndBody << std::endl;

    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        std::cout << ":o" << std::endl;
        token = s.substr(0, pos);

        size_t kv_break = token.find(": ");

        if (kv_break == -1)
        { // message body
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

    std::cout << "request: " << response << std::endl;
    std::cout << "delimiter: " << delimiter << std::endl;
    pos = response.find(delimiter); // original  response

    // request =
    method = response.substr(0, pos);
    std::cout << "method: " << method << std::endl;
    response.erase(0, pos + delimiter.length());

    pos = response.find(delimiter);
    url = response.substr(0, pos);
    std::cout << "url: " << url << std::endl;
    response.erase(0, pos + delimiter.length());

    protocol = response.substr(0, pos);
    std::cout << "protocol: " << protocol << std::endl;
    response.erase(0, pos + delimiter.length());

    delimiter = "http://";

    bool parsed = HtmlResponse::parseUrl("http://");
    if (!parsed)
    {
        parsed = HtmlResponse::parseUrl("https://");
    }
    if (!parsed)
    {
        parsed = HtmlResponse::parseUrl("");
    }
    std::cout << "parsed: " << parsed << std::endl;
}

bool HtmlResponse::parseUrl(std::string delimiter)
{

    if (delimiter == "")
    {
        HtmlResponse::parseUrl("https://");
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

std::string HtmlResponse::printResponse()
{
    return method + " " + url + " " + protocol + "\r\n" + headersAndBody;
}