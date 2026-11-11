#include <stream.h>

void StreamEntry::AddKeyValPair(std::string key, std::string value)
{
    std::pair<std::string, std::string> entryKeyVal = std::make_pair(key, value);
    keyValPairs.push_back(entryKeyVal);
}

std::string StreamEntry::idPart1() const
{
    return id.substr(0, id.find("-"));
}

std::string StreamEntry::idPart2() const
{
    if(id == "*")
    {
        return id;
    }
    return id.substr(id.find("-") + 1, id.length());
}

bool StreamEntry::WildCardSequence()
{
    return idPart2() == "*";
}

bool StreamEntry::WildCardTimeStamp()
{
    return id == "*";
}

bool StreamEntry::ZeroID()
{
    return idPart1() == "0" && idPart2() == "0";
}

bool StreamEntry::operator<(const StreamEntry& other)
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

std::string StreamEntry::ToString()
{
    std::string entry = "";
    for(int i = 0; i < keyValPairs.size(); ++i)
    {
        entry += keyValPairs[i].first + " " + keyValPairs[i].second + " ";
    }
    entry = entry.substr(0, entry.length() - 1);
    return entry;
}

std::string Stream::NextEntryID(std::string idPt1)
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