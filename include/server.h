#pragma once

#include <vector>
#include <map>
#include <string>

class Server {

public:
    Server(){}

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

private:
    int server_fd;
    std::map<std::string, std::string> keyVals;
};