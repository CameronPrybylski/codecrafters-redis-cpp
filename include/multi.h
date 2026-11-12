#pragma once

#include <queue>
#include <map>
#include <memory>


class Multi {

public:
    Multi(int client_fd);
    int GetClient(){return this->client_fd;}

    void AddCommand(std::vector<std::string>& command);
    std::vector<std::string> GetCommand();

    bool Empty(){return commands.empty();}
    bool IsExecuting(){return this->executing;}
    void SetExecuting(bool executing){this->executing = executing;}

    int Count(){return commands.size();}

private:
    int client_fd;
    std::queue<std::vector<std::string>> commands;
    bool executing;
};

class MultiManager {
public:
    MultiManager(){}

    bool Contains(int client_fd);

    //bool MultiEmpty(int client_fd);

    bool AddNewMulti(int client_fd);
    bool AddToMulti(int client_fd, std::vector<std::string>& command);

    int MultiCommandCount(int client_fd);

    std::vector<std::string> Execute(int client_fd);
    bool IsExecuting(int client_fd);

private:
    std::map<int, std::shared_ptr<Multi>> multis;
};

