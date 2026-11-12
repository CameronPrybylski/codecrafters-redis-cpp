#include <multi.h>
#include <iostream>

Multi::Multi(int client_fd) : client_fd(client_fd), executing(false)
{

}

void Multi::AddCommand(std::vector<std::string> &command)
{
    this->commands.push(command);
}

std::vector<std::string> Multi::GetCommand()
{
    std::vector<std::string> command;
    if(!commands.empty())
    {
        command = commands.front();
        commands.pop();
    }
    return command;
}

bool MultiManager::Contains(int client_fd)
{
    return multis.contains(client_fd);
}

/*
bool MultiManager::MultiEmpty(int client_fd)
{
    if(multis.contains(client_fd) && multis[client_fd]->Empty())
    {
        return true;
    }
    return false;
}
*/
bool MultiManager::AddNewMulti(int client_fd)
{
    if(multis.contains(client_fd))
    {
        return false;
    }
    multis[client_fd] = std::make_shared<Multi>(client_fd);
    return true;
}

bool MultiManager::AddToMulti(int client_fd, std::vector<std::string>& command)
{
    if(multis.contains(client_fd))
    {
        multis[client_fd]->AddCommand(command);
        return true;
    }
    return false;
}

int MultiManager::MultiCommandCount(int client_fd)
{
    if(multis.contains(client_fd))
    {
        return multis[client_fd]->Count();
    }
    return 0;
}

std::vector<std::string> MultiManager::Execute(int client_fd)
{
    std::vector<std::string> command;
    if(multis.contains(client_fd))
    {
        if(!multis[client_fd]->IsExecuting())
            multis[client_fd]->SetExecuting(true);

        command = multis[client_fd]->GetCommand();
    }
    if(command.empty())
    {
        multis.erase(client_fd);
    }
    return command;
}

bool MultiManager::IsExecuting(int client_fd)
{
    if(multis.contains(client_fd))
    {
        return multis[client_fd]->IsExecuting();
    }
    return false;
}
