#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <map>
#include <queue>

class StorageEngine;
class MultiManager;

class Server {

public:
    Server();

    bool Create();
    bool Bind();
    bool ListenForConnections();
    int AcceptClient();

    void Run();
    void Close();

    void HandleClient(int client_fd);
    
    std::string HandleMessageStream(std::string& messageStream, int& numOfElements, std::vector<std::string>& respArr, int client_fd);
    
    int NumberOfElements(std::string& messageStream);
    
    void AddToRespArr(std::string& messageStream, std::vector<std::string>& respArr);

    std::string HandleRespArr(std::vector<std::string>& respArr, int client_fd);
    std::string Echo(std::vector<std::string>& respArr);
    std::string Set(std::vector<std::string>& respArr);
    std::string Get(std::vector<std::string>& respArr);
    std::string RPush(std::vector<std::string>& respArr);
    std::string LPush(std::vector<std::string>& respArr);
    std::string Push(std::vector<std::string>& respArr, bool right);
    std::string LRange(std::vector<std::string>& respArr);
    std::string LLen(std::vector<std::string>& respArr);
    std::string LPop(std::vector<std::string>& respArr);
    std::string RPop(std::vector<std::string>& respArr);
    std::string BLPop(std::vector<std::string>& respArr);
    std::string BRPop(std::vector<std::string>& respArr);

    std::string BPop(std::vector<std::string>& respArr, bool right);
    std::string Pop(std::vector<std::string>& respArr, bool right);

    std::string Type(std::vector<std::string>& respArr);

    std::string XAdd(std::vector<std::string>& respArr);

    std::string XRange(std::vector<std::string>& respArr);
    std::string XRead(std::vector<std::string>& respArr);
    std::string XReadBlock(std::vector<std::string>& respArr);
    std::string ParseRangeEntries(std::map<std::string, std::vector<std::string>>& entriesInRange, std::string streamKey);

    std::string Incr(std::vector<std::string>& respArr);

    std::string Multi(std::vector<std::string>& respArr, int client_fd);
    std::string Exec(int client_fd);
    std::string Discard(int client_fd);

    std::string Watch(std::vector<std::string>& respArr, int client_fd);


    void HandleResponse(int client_fd, std::string response);

private:
    int server_fd;

    std::shared_ptr<StorageEngine> storageEngine;
    std::shared_ptr<MultiManager> multiManager;

    //std::map<std::string

    std::map<std::string, std::function<std::string(std::vector<std::string>&)>> commands;
};