#pragma once

#include <string>
#include <vector>
#include <utility>

struct StreamEntry {
    std::string id;
    std::vector<std::pair<std::string, std::string>> keyValPairs;

    void AddKeyValPair(std::pair<std::string, std::string> keyValPair);
    std::string idPart1() const;
    std::string idPart2() const;
    bool WildCardSequence();
    bool WildCardTimeStamp();
    bool ZeroID();
    bool operator<(const StreamEntry& other);
    std::string ToString();
};

struct Stream {
    std::vector<StreamEntry> entries;

    std::string NextEntryID(std::string idPt1);
};