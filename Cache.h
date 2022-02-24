#ifndef __CACHE_H__
#define __CACHE_H__
#include <map>
#include <string>
#include "HtmlRequest.h"
#include "HtmlResponse.h"
#include <vector>
#include <utility>


class Cache {
    public:
    std::vector<std::pair<HtmlRequest, HtmlResponse> > cache;
    
    Cache();

    std::pair<int, bool> containsRequest(HtmlRequest req); // if you plan on using the response cached, need to lock until you do so as otherwise the int may change

    bool storeRequestInCache(HtmlRequest req, HtmlResponse resp, int id);
    std::vector<std::string> splitHeader(std::string);
    
};
#endif