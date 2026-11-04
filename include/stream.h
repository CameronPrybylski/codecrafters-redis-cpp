#pragma once

#include <string>
#include <vector>
#include <utility>

struct StreamEntry {
    std::string id;
    std::vector<std::pair<std::string, std::string>> keyValPairs;
    void AddKeyValPair(std::pair<std::string, std::string> keyValPair)
    {
        keyValPairs.push_back(keyValPair);
    }
};

struct Stream {
    std::vector<StreamEntry> entries;
};