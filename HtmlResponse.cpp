#include "HtmlResponse.h"
#include <string>

// #include <sys/socket.h> // For socket functions
// #include <netinet/in.h> // For sockaddr_in
// #include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
// #include <unistd.h> // For read
#include <map>
#include <iterator>
#include <sstream>

HtmlResponse::HtmlResponse(std::vector<char> buffer)
{
    std::string s(buffer.begin(), buffer.end());
    std::string delimiter = "\r\n";

    size_t pos = 0;
    std::string token;

    // std::map<std::string, std::string> m;

    std::string header_key;
    std::string header_val;

    std::string request;
    start_len = 0;

    pos = s.find(delimiter); // original get request
    request = s.substr(0, pos);

    firstLine = request;

    s.erase(0, pos + delimiter.length());
    start_len += pos + delimiter.length();

    headersAndBody = s;

    std::cout << "headers parse: "
              << "\r\n";

    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        token = s.substr(0, pos);

        size_t kv_break = token.find(": ");

        if (kv_break == -1)
        { // message body
            // body = token;
            HtmlResponse::parseBody(buffer);
            break;
        }

        header_key = token.substr(0, kv_break);
        header_val = token.substr(kv_break + 2, token.length());

        m[header_key] = header_val;

        s.erase(0, pos + delimiter.length());
        start_len += pos + delimiter.length();
    }

    std::cout << "body parsed: "
              << "\r\n";
    delimiter = " ";

    pos = request.find(delimiter); // original get request

    protocol = request.substr(0, pos);
    request.erase(0, pos + delimiter.length());

    pos = request.find(delimiter);
    resp_type = request.substr(0, pos);
    request.erase(0, pos + delimiter.length());

    message = request.substr(0, pos);
    request.erase(0, pos + delimiter.length());

    delimiter = "http://";

    std::cout << "url parsing: "
              << "\r\n";
}

std::string HtmlResponse::printHeaders()
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

bool HtmlResponse::checkIfInHeaders(std::string s)
{
    std::map<std::string, std::string>::iterator iter = m.find(s);
    if (iter != m.end())
    {
        return true;
    }
    return false;
}

std::string HtmlResponse::getHeader(std::string s)
{ // should always be called after calling checkIfInHeaders, will crash otherwise
    std::map<std::string, std::string>::iterator iter = m.find(s);
    return iter->second;
}

void HtmlResponse::parseBody(std::vector<char> v) // s should contain the body
{
    std::stringstream ss;

    if (checkIfInHeaders("Content-Length"))
    {
        // body = s.substr(2, 2+ atoi(getHeader("Content-Length").c_str()));// 2 to offset the /r/n
        body = std::vector<char>(2 + v.begin() + start_len, 2 + v.begin() + start_len + atoi(getHeader("Content-Length").c_str()));
    }
    if (checkIfInHeaders("Transfer-Encoding"))
    {
        if (getHeader("Transfer-Encoding") == "chunked")
        {
            body = std::vector<char>(0);
        }
    }
}

std::string HtmlResponse::printResponse()
{
    return protocol + " " + resp_type + " " + message + "\r\n" + HtmlResponse::printHeaders() + "\r\n";
}