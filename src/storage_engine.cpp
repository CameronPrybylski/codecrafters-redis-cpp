#include <storage_engine.h>
#include <iostream>


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

void StorageEngine::AddToStream(std::string streamKey, StreamEntry entry)
{
    if(streams.contains(streamKey))
    {
        streams[streamKey].entries.push_back(entry);
    }
    else
    {
        streams[streamKey].entries = {entry};
    }
}

void StorageEngine::AddEntryToStream(std::string streamKey, std::vector<std::string> keyValuePairs, std::string entryID)
{
    StreamEntry entry;
    entry.id = entryID;
    for(int i = 0; i < keyValuePairs.size(); i += 2)
    {
        std::pair<std::string, std::string> entryKeyVal = std::make_pair(keyValuePairs[i], keyValuePairs[i+1]);
        entry.AddKeyValPair(entryKeyVal);
    }
    AddToStream(streamKey, entry);
}
