#pragma once

#include <map>
#include <chrono>

class StorageEngine {

public:
    StorageEngine(){}

    void AddEntry(std::string key, std::string val, bool expires, int timeExpiresMs = 0);
    bool Contains(std::string key){return keyVals.contains(key);}
    std::string GetValue(std::string key);

    void AddToList(std::string listKey, std::string entry);
    int ElementsInList(std::string listKey);

    bool CheckExpired(std::string key);

private:
    std::map<std::string, std::string> keyVals;
    std::map<std::string, int> keyExpires;
    std::map<std::string, std::chrono::system_clock::time_point> keyTimes;

    std::map<std::string, std::vector<std::string>> keyLists;
};