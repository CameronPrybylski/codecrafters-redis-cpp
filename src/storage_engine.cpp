#include <storage_engine.h>
#include <iostream>
#include <climits>


void StorageEngine::AddEntry(std::string key, std::string value, bool expires, int timeExpiresMs)
{
    keyVals[key] = value;
    if(expires)
    {
        keyExpires[key] = timeExpiresMs;
        keyTimes[key] = std::chrono::system_clock::now();
    }
}

std::string StorageEngine::GetValue(std::string key)
{
    if(keyVals.contains(key))
    {
        return keyVals[key];
    }
    return "";
}

bool StorageEngine::Contains(std::string key)
{
    return keyVals.contains(key);
}

bool StorageEngine::ContainsList(std::string listKey)
{
    return keyLists.contains(listKey);
}

void StorageEngine::AddToList(std::string listKey, std::string entry, bool right)
{
    if(keyLists.contains(listKey))
    {
        if(right)
            keyLists[listKey].push_back(entry);
        else
            keyLists[listKey].push_front(entry);
    }
    else
    {
        keyLists[listKey] = {entry};
    }
}

void StorageEngine::AddEntriesToList(std::string listKey, std::vector<std::string>& entries, bool right)
{
    for(int i = 0; i < entries.size(); ++i)
    {
        AddToList(listKey, entries[i], right);
    }
}

std::string StorageEngine::RemoveFromList(std::string listKey, bool right)
{
    std::string entry = "";
    if(keyLists.contains(listKey) && !(keyLists[listKey].empty()))
    {
        if(right)
        {
            entry = keyLists[listKey].back();
            keyLists[listKey].pop_back();
        }
        else
        {
            entry = keyLists[listKey].front();
            keyLists[listKey].pop_front();
        }
        if(keyLists[listKey].empty())
        {
            keyLists.erase(listKey);
        }
    }
    return entry;
}

std::vector<std::string> StorageEngine::RemoveEntriesFromList(std::string listKey, int numOfEntries, bool right)
{
    if(keyLists.contains(listKey))
    {
        numOfEntries = std::min(numOfEntries, ElementsInList(listKey));
        std::vector<std::string> entries;
        std::string entry;
        for(int i = 0; i < numOfEntries; ++i)
        {
            entry = RemoveFromList(listKey, right);
            if(entry.empty())
            {
                break;
            }
            entries.push_back(entry);
        }
        return entries;
    }
    return std::vector<std::string>();
}

int StorageEngine::ElementsInList(std::string listKey)
{
    if(keyLists.contains(listKey))
    {
        return keyLists[listKey].size();
    }
    return 0;
}

std::deque<std::string> StorageEngine::GetList(std::string listKey)
{
    if(keyLists.contains(listKey))
    {
        return keyLists[listKey];
    }
    return std::deque<std::string>();
}

void StorageEngine::HandleNegativeIndex(int& index, int elementsInList)
{
    if(index < 0)
    {
        index = elementsInList + index;
        index = std::max(0, index);
    }
}

std::vector<std::string> StorageEngine::ElementsInRange(std::string listKey, int start, int end)
{
    int elementsInList = ElementsInList(listKey);
    HandleNegativeIndex(start, elementsInList);
    HandleNegativeIndex(end, elementsInList);
    if(elementsInList > 0 && start < elementsInList && start <= end )
    {
        //Make end exclusive incase end is greater than elements in list
        end += 1;
        end = std::min(end, elementsInList);
        std::vector<std::string> result(end - start);
        std::deque<std::string> keyList = GetList(listKey);
        int index = 0;
        for(int i = start; i < end; ++i)
        {
            result[index] = keyList[i];
            ++index;
        }
        return result;
    }
    return std::vector<std::string>();
}

bool StorageEngine::CheckExpired(std::string key)
{
    if(keyTimes.contains(key))
    {
        auto time = keyTimes[key];
        auto now = std::chrono::system_clock::now();
        int timeToExpire = keyExpires[key];
        std::chrono::duration<double, std::milli> ms_double = now - time;
        if(ms_double > std::chrono::milliseconds(timeToExpire))
        {
            keyVals.erase(key);
            keyExpires.erase(key);
            keyTimes.erase(key);
            return true;
        }
    }
    return false;
}

std::string StorageEngine::GetType(std::string key)
{
    if(keyVals.contains(key))
    {
        return "string";
    }
    if(keyLists.contains(key))
    {
        return "list";
    }
    if(streams.contains(key))
    {
        return "stream";
    }
    else
    {
        return "none";
    }
}

std::string StorageEngine::AddToStream(std::string streamKey, StreamEntry entry)
{
    std::string lessThanErr = "ERR The ID specified in XADD is equal or smaller than the target stream top item";
    std::string isZeroErr = "ERR The ID specified in XADD must be greater than 0-0";
    if(entry.id == "*")
    {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto currTimeMilis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        std::string strCurrTimeMilis = std::to_string(currTimeMilis);
        entry.id = strCurrTimeMilis + "-0";
    }
    if(entry.ZeroID())
    {
        return isZeroErr;
    }
    if(entry.WildCardSequence() && entry.idPart1() == "0")
    {
        entry.id = "0-1";
    }
    if(streams.contains(streamKey))
    {
        StreamEntry lastEntry = streams[streamKey].entries.back();
        
        if(entry.WildCardSequence())
        {
            entry.id = entry.idPart1() + "-" + streams[streamKey].NextEntryID(entry.idPart1());
        }
        if(entry < lastEntry)
        {
            return lessThanErr;
        }
        streams[streamKey].entries.push_back(entry);
    }
    else
    {
        if(entry.WildCardSequence())
        {
            entry.id = entry.idPart1() + "-1";
        }
        streams[streamKey].entries = {entry};
    }

    return entry.id;
}

std::string StorageEngine::AddEntryToStream(std::string streamKey, std::vector<std::string> keyValuePairs, std::string entryID)
{
    StreamEntry entry;
    entry.id = entryID;
    for(int i = 0; i < keyValuePairs.size(); i += 2)
    {
        entry.AddKeyValPair(keyValuePairs[i], keyValuePairs[i+1]);
    }
    return AddToStream(streamKey, entry);
}

std::map<std::string, std::vector<std::string>> StorageEngine::StreamEntriesInRange(std::string streamKey, std::string start, std::string end)
{
    if(streams.contains(streamKey))
    {
        std::vector<StreamEntry> entries = streams[streamKey].entries;
        std::map<std::string, std::vector<StreamEntry>> entryMap;
        std::map<std::string, std::vector<std::string>> ans;
        std::vector<long> range = GetStreamRange(start, end);
        for(int i = 0; i < entries.size(); ++i)
        {
            long timestamp = stol(entries[i].idPart1());
            int sequenceNum = stoi(entries[i].idPart2());
            if(timestamp >= range[0] && timestamp <= range[2] && sequenceNum >= range[1] && sequenceNum <= range[3])
            {
                entryMap[entries[i].id].push_back(entries[i]);
            }
        }
        for(auto entry : entryMap)
        {
            for(int i = 0; i < entry.second.size(); ++i)
            {
                ans[entry.first].push_back(entry.second[i].ToString());
            }
        }
        return ans;
    }
    return std::map<std::string, std::vector<std::string>>();
}

std::vector<long> StorageEngine::GetStreamRange(std::string start, std::string end)
{
    std::vector<long> range(4);
    long lStartPt1= 0;
    long lEndPt1 = INT_MAX;
    long lStartPt2 = 0;
    long lEndPt2 = INT_MAX;
    if(start == "-")
    {
        std::string strEndPt1 = end.substr(0, end.find("-"));
        std::string strEndPt2 = end.substr(end.find("-") + 1, end.length());
        lEndPt1 = stol(strEndPt1);
        lEndPt2 = stol(strEndPt2);
    }
    else if(end == "+")
    {
        std::string strStartPt1 = start.substr(0, start.find("-"));
        std::string strStartPt2 = start.substr(start.find("-") + 1, start.length());
        lStartPt1 = stol(strStartPt1);
        lStartPt2 = stol(strStartPt2);
    }
    else if(end == "<>")
    {
        std::string strStartPt1 = start.substr(0, start.find("-"));
        std::string strStartPt2 = start.substr(start.find("-") + 1, start.length());
        lStartPt1 = stol(strStartPt1);
        lStartPt2 = stol(strStartPt2);
        lStartPt2 += 1;
    }
    else
    {
        std::string strStartPt1 = start.substr(0, start.find("-"));
        std::string strEndPt1 = end.substr(0, end.find("-"));
        std::string strStartPt2 = start.substr(start.find("-") + 1, start.length());
        std::string strEndPt2 = end.substr(end.find("-") + 1, end.length());
        lStartPt1 = stol(strStartPt1);
        lEndPt1 = stol(strEndPt1);
        lStartPt2 = stol(strStartPt2);
        lEndPt2 = stol(strEndPt2);
    }
    range = {lStartPt1, lStartPt2, lEndPt1, lEndPt2};
    return range;
}

std::string StorageEngine::MaxStreamID(std::string streamKey)
{
    if(streams.contains(streamKey))
    {
        Stream stream = streams.at(streamKey);
        StreamEntry lastEntry = stream.GetLastEntry();
        if(!lastEntry.id.empty())
        {
            return lastEntry.id;
        }
    }
    return std::string();
}
