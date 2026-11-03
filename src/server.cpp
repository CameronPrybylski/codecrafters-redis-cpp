#include <server.h>
#include <storage_engine.h>

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>


Server::Server()
{
    storageEngine = std::make_shared<StorageEngine>();
}

bool Server::Create()
{
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0)
    {
        return false;
    }
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        std::cerr << "setsockopt failed\n";
        return false;
    }
    return true;
}

bool Server::Bind()
{
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(6379);

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        std::cerr << "Failed to bind to port 6379\n";
        return false;
    }

    return true;
}

bool Server::ListenForConnections()
{
    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        std::cerr << "listen failed\n";
        return false;
    }
    return true;
}

int Server::AcceptClient()
{
  /*
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  std::cout << "Waiting for a client to connect...\n";

  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  // Uncomment the code below to pass the first stage
  int client = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
  */
  int client = accept(server_fd, nullptr, nullptr);
  return client;
}

void Server::Run()
{
    if(!(Create() && Bind() && ListenForConnections()))
    {
        return;
    }
    while(true)
    {
        int client = AcceptClient();
        if(client > -1)
        {
            std::thread(&Server::HandleClient, this, client).detach();
        }
    }
}

void Server::Close()
{
    close(server_fd);
}

void Server::HandleClient(int client_fd)
{
  std::string messageStream;
  std::vector<std::string> respArr;
  int numOfElements = 0;
  while(true)
  {
    char buffer[64] = {};
    ssize_t bytesReceived = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if(bytesReceived <= 0)
    {
      break;
    }
    std::string msg(buffer);
    messageStream += msg;
    //HandleMessageStream(messageStream, numOfElements, respArr);    
    std::string response = HandleMessageStream(messageStream, numOfElements, respArr, client_fd);
    if(!response.empty())
    {
        HandleResponse(client_fd, response);  
    }
  }
  close(client_fd);
}

std::string Server::HandleMessageStream(std::string& messageStream, int& numOfElements, std::vector<std::string>& respArr, int client_fd)
{
    std::string response = "";
    if(messageStream.empty())
    {
        return response;
    }
    size_t start = messageStream.find("*");
    if(start == 0 && numOfElements == 0)
    {
        numOfElements = NumberOfElements(messageStream);
    }
    if(respArr.size() < numOfElements)
    {
        AddToRespArr(messageStream, respArr);
    }
    if(respArr.size() == numOfElements)
    {
        response = HandleRespArr(respArr, client_fd);
        respArr.clear();
        numOfElements = 0;
    }
    return response;
}

int Server::NumberOfElements(std::string& messageStream)
{
  size_t end = messageStream.find("\r\n");
  std::string strNum = "";
  for(int i = 1; i < end; ++i)
  {
    strNum += messageStream[i];
  }
  messageStream = messageStream.substr(end + 2, messageStream.length());
  return stoi(strNum);
}

void Server::AddToRespArr(std::string& messageStream, std::vector<std::string>& respArr)
{

  size_t end = messageStream.find("\r\n");
  while(end != std::string::npos)
  {
    if(end == std::string::npos)
    {
      break;
    }

    std::string element = messageStream.substr(0, end);

    if(element.find("*") != std::string::npos)
    {

    }
    else if(element.find("$") != std::string::npos)
    {

    }
    else if(element.find(":") != std::string::npos)
    {

    }
    else
    {
      respArr.push_back(element);
    }
    messageStream = messageStream.substr(element.length() + 2, messageStream.length());
    end = messageStream.find("\r\n");
  }
}

std::string Server::HandleRespArr(std::vector<std::string>& respArr, int client_fd)
{
  std::string cmd = respArr[0];
  std::string response = "";
  if(cmd == "ECHO")
  {
    return Echo(respArr);
  }
  else if(cmd == "SET")
  {
    return Set(respArr);
  }
  else if(cmd == "GET")
  {
    return Get(respArr);
  }
  else if(cmd == "RPUSH")
  {
    return RPush(respArr);
  }
  else if(cmd == "LPUSH")
  {
    return LPush(respArr);
  }
  else if(cmd == "LRANGE")
  {
    return LRange(respArr);
  }
  else if(cmd == "LLEN")
  {
    return LLen(respArr);
  }
  else if(cmd == "LPOP")
  {
    return LPop(respArr);
  }
  else if(cmd == "BLPOP")
  {
    return BLPop(respArr);
  }
  else
  {
    return "+PONG\r\n";
  }
}

std::string Server::Echo(std::vector<std::string> &respArr)
{
    return "$" + std::to_string(respArr[1].size()) + "\r\n" + respArr[1] + "\r\n";
}

std::string Server::Set(std::vector<std::string> &respArr)
{
    int time = 0;
    bool expires = false;

    if(respArr.size() > 3)
    {
        time = stoi(respArr[4]);
        expires = true;
        if(respArr[3] == "EX")
        {
            time *= 1000;
        }
    }

    storageEngine->AddEntry(respArr[1], respArr[2], expires, time);

    return "+OK\r\n";
}

std::string Server::Get(std::vector<std::string> &respArr)
{
    std::string key = respArr[1];
    if(storageEngine->Contains(key) && !(storageEngine->CheckExpired(key)))
    {
        std::string value = storageEngine->GetValue(key);
        std::string resp = "$" + std::to_string(value.length()) + "\r\n" + value + "\r\n";
        return resp;
    }
    else
    {
        return "$-1\r\n";
    }
}

std::string Server::RPush(std::vector<std::string> &respArr)
{
   return Push(respArr, true);
}

std::string Server::LPush(std::vector<std::string> &respArr)
{
    return Push(respArr, false);
}

std::string Server::Push(std::vector<std::string> &respArr, bool right)
{
    std::string listKey = respArr[1];
    std::vector<std::string> entries;
    entries.assign(respArr.begin() + 2, respArr.end());

    storageEngine->AddEntriesToList(listKey, entries, right);

    int elementsInList = storageEngine->ElementsInList(listKey);

    return ":" + std::to_string(elementsInList) + "\r\n";
}

std::string Server::LRange(std::vector<std::string> &respArr)
{
    std::string listKey = respArr[1];
    int start = stoi(respArr[2]);
    int end = stoi(respArr[3]);
    std::vector<std::string> result = storageEngine->ElementsInRange(listKey, start, end);
    std::string strResult = "*" + std::to_string(result.size()) + "\r\n";
    
    for(int i = 0; i < result.size(); ++i)
    {
        strResult +=  "$" + std::to_string(result[i].length()) + "\r\n" + result[i] + "\r\n";
    }

    return strResult;
}

std::string Server::LLen(std::vector<std::string> &respArr)
{
    std::string listKey = respArr[1];
    int length = storageEngine->ElementsInList(listKey);
    return ":"  + std::to_string(length) + "\r\n";
}

std::string Server::LPop(std::vector<std::string> &respArr)
{
    return Pop(respArr, false);
}

std::string Server::RPop(std::vector<std::string> &respArr)
{
    return Pop(respArr, true);
}

std::string Server::BLPop(std::vector<std::string> &respArr)
{
    return BPop(respArr, false);
}

std::string Server::BRPop(std::vector<std::string> &respArr)
{
    return BPop(respArr, true);
}

std::string Server::BPop(std::vector<std::string> &respArr, bool right)
{
    int timeout = stoi(respArr[2]);
    auto start = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();
    auto diff = now - start;
    std::string listKey = respArr[1];
    std::string result;
    std::vector<std::string> bpopArr = {respArr[0], respArr[1]};
    storageEngine->IncrementBPop(listKey);
    //diff < std::chrono::seconds(timeout) ||
    while(timeout == 0)
    {
        //result = storageEngine->BPop(listKey, right);
        result = Pop(bpopArr, right);
        if(result != "$-1\r\n")
        {
            //std::cout << result << std::endl;
            //result = "$" + std::to_string(result.length()) + "\r\n" + result + "\r\n";
            break;
        }
        now = std::chrono::system_clock::now();
        diff = now - start;
    }
    std::string arrLength = "*2\r\n";
    std::string elements = "$" + std::to_string(listKey.length()) + "\r\n" + listKey + "\r\n" + result;
    return arrLength + elements;
}

std::string Server::Pop(std::vector<std::string> &respArr, bool right)
{
    int numOfEntries = 1;
    std::string result = "";
    if(respArr.size() > 2)
    {
        numOfEntries = stoi(respArr[2]);
    }
    std::vector<std::string> entries = storageEngine->RemoveEntriesFromList(respArr[1], numOfEntries, right);
    if(entries.empty())
    {
        result = "$-1\r\n";
    }
    else if(entries.size() > 1)
    {
        result = "*" + std::to_string(entries.size()) + "\r\n";
    }
    for(int i = 0; i < entries.size(); ++i)
    {
        result += "$" + std::to_string(entries[i].length()) + "\r\n" + entries[i] + "\r\n";
    }
    return result;
}

void Server::HandleResponse(int client_fd, std::string response)
{
    const char* chResp = response.c_str();
    send(client_fd, chResp, strlen(chResp), 0);
}