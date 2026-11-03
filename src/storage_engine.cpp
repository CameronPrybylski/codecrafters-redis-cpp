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
