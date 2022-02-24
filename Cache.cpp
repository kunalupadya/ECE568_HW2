#include "Cache.h"
#include "HtmlRequest.h"
#include <string>
#include <array>
#include <vector>
#include <utility>
#include <chrono>
#include <iostream> // For cout
#include "easylogging++.h"

#define MAX_SIZE = 1000

Cache::Cache() {}
std::pair<int, bool> Cache::containsRequest(HtmlRequest req)
{
    if (req.method != "GET")
    {
        std::pair<int, bool> ret(-1, false);
        return ret;
    }

    int max_stale = 0;
    int min_fresh = 0;
    int maxage = 0;

    bool mustValidate = false;
    bool requiredValidate = false;
    std::cout << "19"
              << "\r\n";
    if (req.checkIfInHeaders("Cache-Control"))
    {
        std::vector<std::string> cacheControls = splitHeader(req.getHeader("Cache-Control"));
        std::cout << "23"
                  << "\r\n";
        for (size_t i = 0; i < cacheControls.size(); i++)
        {
            std::cout << "26"
                      << "\r\n";
            if (cacheControls[i] == "private")
            {
                std::pair<int, bool> ret(-1, false);
                return ret;
            }
            if (cacheControls[i] == "no-store")
            {
                std::pair<int, bool> ret(-1, false);
                return ret;
            }
            if (cacheControls[i] == "no-cache")
            {
                requiredValidate = true;
            }
            int pos;
            std::string delimiter = "=";
            if ((pos = cacheControls[i].find(delimiter)) != std::string::npos)
            {
                std::string key = cacheControls[i].substr(0, pos);
                int val = atoi(cacheControls[i].substr(pos + delimiter.length(), cacheControls[i].length()).c_str());
                if (key == "max-stale")
                {
                    max_stale = val;
                }
                if (key == "min-fresh")
                {
                    min_fresh = val;
                }
                if (key == "max-age" || key == "s-maxage")
                {
                    if (val > maxage)
                    {
                        maxage = val;
                    }
                }
            }
        }
    }

    std::cout << "65"
              << "\r\n";
    int lastInd = -1;
    int j = cache.size();

    for (size_t i = 0; i < j; i++)
    {

        if (req.host == cache[i].first.host && req.url == cache[i].first.url)
        {
            if (lastInd != -1)
            {
                cache.erase(cache.begin() + lastInd);
                j = j - 1;
                i = i - 1;
            }
            lastInd = i;
        }
    }
    if (lastInd == -1)
    {
        std::pair<int, bool> ret(-1, false);
        return ret;
    }

    int ageSince = 0;

    HtmlResponse resp = cache[lastInd].second;

    std::tm tm = {};

    strptime(resp.getHeader("Date").c_str(), "%a, %d %b %Y %H:%M:%S %Z", &tm);

    std::time_t t = std::time(0); // get time now
    std::tm *now = std::localtime(&t);

    std::cout << resp.getHeader("Date") << "\r\n";
    std::cout << now->tm_sec << "\r\n";
    std::cout << tm.tm_sec << "\r\n";

    std::time_t date_time = std::mktime(&tm);
    std::time_t now_time = std::mktime(now);

    ageSince = now_time - date_time;

    std::cout << ageSince << "\r\n";

    if (resp.checkIfInHeaders("Cache-Control"))
    {
        std::vector<std::string> cacheControls = splitHeader(resp.getHeader("Cache-Control"));

        bool potentiallyValidate;

        for (size_t i = 0; i < cacheControls.size(); i++)
        {
            int pos;
            std::string delimiter = "=";
            if ((pos = cacheControls[i].find(delimiter)) != std::string::npos)
            {
                std::string key = cacheControls[i].substr(0, pos);

                if (key == "max-age" || key == "s-maxage")
                {
                    int val = atoi(cacheControls[i].substr(pos + delimiter.length(), cacheControls[i].length()).c_str());

                    if (val > maxage)
                    {
                        maxage = val;
                    }
                }
            }

            if (cacheControls[i] == "no-cache")
            {
                requiredValidate = true;
            }
            if (cacheControls[i] == "must-revalidate")
            {
                mustValidate = true;
            }
            if (cacheControls[i] == "proxy-revalidate")
            {
                mustValidate = true;
            }
        }
    }

    if (requiredValidate)
    {
        std::cout << "required validate" << std::endl;
        std::pair<int, bool> ret(lastInd, true);
        return ret;
    }

    if (mustValidate && ageSince + min_fresh > maxage)
    {
        std::cout << "must" << std::endl;
        std::pair<int, bool> ret(lastInd, true);
        return ret;
    }

    if (ageSince + min_fresh - max_stale > maxage)
    {
        std::cout << "gen validate" << std::endl;
        std::cout << ageSince << " " << min_fresh << " " << max_stale << " " << maxage << " " << std::endl;

        std::pair<int, bool> ret(lastInd, true);
        return ret;
    }

    std::pair<int, bool> ret(lastInd, false);

    return ret;
}

bool Cache::storeRequestInCache(HtmlRequest req, HtmlResponse resp, int id)
{
    // always look in the cache for the
    if (resp.resp_type != "200")
    {
        return false;
    }

    if (req.checkIfInHeaders("Authorization"))
    {
        LOG(INFO) << std::to_string(id) << ": \"" << "not cacheable because authorization header is present in a public cache";
        return false;
    }
    bool validation = false;

    if (req.checkIfInHeaders("Cache-Control"))
    {
        std::vector<std::string> cacheControls = splitHeader(req.getHeader("Cache-Control"));

        for (size_t i = 0; i < cacheControls.size(); i++)
        {
            if (cacheControls[i] == "private")
            {
                LOG(INFO) << std::to_string(id) << ": \"" << "not cacheable because cache-control is private";

                return false;
            }
            if (cacheControls[i] == "no-store")
            {
                LOG(INFO) << std::to_string(id) << ": \"" << "not cacheable because cache-control is no-store";

                return false;
            }
        }
    }

    if (resp.checkIfInHeaders("Cache-Control"))
    {
        std::vector<std::string> cacheControls = splitHeader(req.getHeader("Cache-Control"));

        for (size_t i = 0; i < cacheControls.size(); i++)
        {
            if (cacheControls[i] == "private")
            {
                LOG(INFO) << std::to_string(id) << ": \"" << "not cacheable because cache-control is private";

                return false;
            }
            if (cacheControls[i] == "no-store")
            {
                LOG(INFO) << std::to_string(id) << ": \"" << "not cacheable because cache-control is no-store";

                return false;
            }
            if (cacheControls[i] == "must-revalidate")
            {
                validation = true;
            }
            if (cacheControls[i] == "proxy-revalidate")
            {

                validation = true;
            }
        }
    }
    if (validation)
    {
        LOG(INFO) << std::to_string(id) << ": \"" << "cached, but requires re-validation";
    }
    else
    {
        std::tm tm = {};
        strptime(resp.getHeader("Date").c_str(), "%a, %d %b %Y %H:%M:%S %Z", &tm);

        int maxage = -1;
        if (resp.checkIfInHeaders("Cache-Control"))
        {
            std::vector<std::string> cacheControls = splitHeader(req.getHeader("Cache-Control"));

            for (size_t i = 0; i < cacheControls.size(); i++)
            {
                int pos;
                std::string delimiter = "=";
                if ((pos = cacheControls[i].find(delimiter)) != std::string::npos)
                {
                    std::string key = cacheControls[i].substr(0, pos);
                    int val = atoi(cacheControls[i].substr(pos + delimiter.length(), cacheControls[i].length()).c_str());
                    if (key == "max-age" || key == "s-maxage")
                    {
                        if (val > maxage)
                        {
                            maxage = val;
                        }
                    }
                }
            }
        }
        if (maxage == 0)
        {
            LOG(INFO) << std::to_string(id) << ": \"" << "cached, but requires re-validation";
        }
        if (maxage == -1)
        {
            maxage = 1000000;

            if (!resp.checkIfInHeaders("Cache-Control"))
            {
                resp.m["Cache-Control"] = "max-age=" + std::to_string(maxage);
            }
        }
        std::time_t date_time = std::mktime(&tm);
        int expiry_time = date_time + maxage;



        LOG(INFO) << std::to_string(id) << ": \"" << "cached, expires at " << std::to_string(expiry_time);
    }
    std::pair<HtmlRequest, HtmlResponse> p(req, resp);
    cache.push_back(p);
    
    if (cache.size()>MAX_SIZE){
        cache.erase(cache.begin());
    }
    return true;
}

std::vector<std::string> Cache::splitHeader(std::string s)
{
    std::vector<std::string> ret;

    std::string tmp = s;
    std::string delimiter = ", ";
    int pos;
    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        ret.push_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
    }

    ret.push_back(s);
    // std::cout << ret << "\r\n";

    return ret;
}
