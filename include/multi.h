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

private:
    int client_fd;
    std::queue<std::vector<std::string>> commands;
};

class MultiManager {
public:
    MultiManager(){}

    bool Contains(int client_fd);

    bool AddNewMulti(int client_fd);
    bool AddToMulti(int client_fd, std::vector<std::string>& command);

private:
    std::map<int, std::shared_ptr<Multi>> multis;
};

