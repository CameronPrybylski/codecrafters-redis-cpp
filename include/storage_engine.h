#pragma once

#include <map>
#include <deque>
#include <chrono>

class StorageEngine {

public:
    StorageEngine(){}

    void AddEntry(std::string key, std::string val, bool expires, int timeExpiresMs = 0);
    std::string GetValue(std::string key);

    bool Contains(std::string key);
    bool ContainsList(std::string listKey);

    void AddToList(std::string listKey, std::string entry, bool right);
    void AddEntriesToList(std::string listKey, std::vector<std::string>& entries, bool right);
    std::string RemoveFromList(std::string listKey, bool right);
    std::vector<std::string> RemoveEntriesFromList(std::string listKey, int numOfEntries, bool right);

    int ElementsInList(std::string listKey);
    std::deque<std::string> GetList(std::string listKey);

    void HandleNegativeIndex(int& index, int elementsInList);

    std::vector<std::string> ElementsInRange(std::string listKey, int start, int end);

    bool CheckExpired(std::string key);

private:
    std::map<std::string, std::string> keyVals;
    std::map<std::string, int> keyExpires;
    std::map<std::string, std::chrono::system_clock::time_point> keyTimes;

    std::map<std::string, std::deque<std::string>> keyLists;
};