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
    std::string idPart1() const
    {
        return id.substr(0, id.find("-"));
    }
    std::string idPart2() const
    {
        if(id == "*")
        {
            return id;
        }
        return id.substr(id.find("-") + 1, id.length());
    }

    bool WildCardSequence()
    {
        return idPart2() == "*";
    }

    bool WildCardTimeStamp()
    {
        return id == "*";
    }

    bool ZeroID()
    {
        return idPart1() == "0" && idPart2() == "0";
    }

    bool operator<(const StreamEntry& other)
    {
        long thisIDPt1 = stol(this->idPart1());
        long otherIDPt1 = stol(other.idPart1());
        int thisIDPt2 = stoi(this->idPart2());
        int otherIDPt2 = stoi(other.idPart2());
        if(thisIDPt1 > otherIDPt1)
        {
            return false;
        }
        else if(thisIDPt1 == otherIDPt1 && thisIDPt2 > otherIDPt2)
        {
            return false;
        }
        return true;
    }
};

struct Stream {
    std::vector<StreamEntry> entries;

    std::string NextEntryID(std::string idPt1)
    {
        std::string idPt2 = "0";
        for(auto entry : entries)
        {
            if(entry.idPart1() == idPt1)
            {
                idPt2 = std::to_string(stoi(entry.idPart2()) + 1);
            }
        }
        return idPt2;
    }
};