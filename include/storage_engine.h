#pragma once

#include <map>
#include <deque>
#include <chrono>

#include <stream.h>

struct Stream;
struct StreamEntry;

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

    std::string GetType(std::string key);

    std::string AddToStream(std::string streamKey, StreamEntry entry);
    std::string AddEntryToStream(std::string streamKey, std::vector<std::string> keyValuePairs, std::string entryID = "");

    std::map<std::string, std::vector<std::string>> StreamEntriesInRange(std::string streamKey, std::string start, std::string end);

private:
    std::map<std::string, std::string> keyVals;
    std::map<std::string, int> keyExpires;
    std::map<std::string, std::chrono::system_clock::time_point> keyTimes;

    std::map<std::string, std::deque<std::string>> keyLists;
    std::map<std::string, Stream> streams;
};