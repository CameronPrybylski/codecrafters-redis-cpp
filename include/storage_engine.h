#pragma once

#include <map>
#include <chrono>

class StorageEngine {

public:
    StorageEngine(){}

    void AddEntry(std::string key, std::string val, bool expires, int timeExpiresMs = 0);
    std::string GetValue(std::string key);

    bool Contains(std::string key);
    bool ContainsList(std::string listKey);

    void AddToList(std::string listKey, std::string entry);
    int ElementsInList(std::string listKey);
    std::vector<std::string> GetList(std::string listKey);
    std::vector<std::string> ElementsInRange(std::string listKey, int start, int end);

    bool CheckExpired(std::string key);

private:
    std::map<std::string, std::string> keyVals;
    std::map<std::string, int> keyExpires;
    std::map<std::string, std::chrono::system_clock::time_point> keyTimes;

    std::map<std::string, std::vector<std::string>> keyLists;
};