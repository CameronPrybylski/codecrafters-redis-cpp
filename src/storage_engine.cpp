#include <storage_engine.h>


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

void StorageEngine::AddToList(std::string listKey, std::string entry)
{
    if(keyLists.contains(listKey))
    {
        keyLists[listKey].push_back(entry);
    }
    else
    {
        keyLists[listKey] = {entry};
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
