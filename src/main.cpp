#include <iostream>

#include <server.h>

/*
bool IsClientAvailable(int clientSocket)
{
  struct pollfd pfd;
  pfd.fd = clientSocket;       // The socket you want to check
  pfd.events = POLLIN;      // Alert me when data is available to read
  pfd.revents = 0;          // Clear returned events

  // poll returns: > 0 if data is ready, 0 on timeout, -1 on error
  int timeout_ms = 1;
  int ret = poll(&pfd, 1, timeout_ms);
  
  if (ret > 0 && (pfd.revents & POLLIN)) {
      return true; 
  }
  return false;
}

int AcceptClient(int server_fd)
{
  /*
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  std::cout << "Waiting for a client to connect...\n";

  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  // Uncomment the code below to pass the first stage
  int client = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
  
  int client = accept(server_fd, nullptr, nullptr);
  return client;
}

std::string HandleRespArr(std::vector<std::string>& respArr, int client_fd)
{
  std::string cmd = respArr[0];
  std::string response = "";
  if(cmd == "ECHO")
  {
    return "$" + std::to_string(respArr[1].size()) + "\r\n" + respArr[1] + "\r\n";
  }
  else
  {
    return "+PONG\r\n";
  }
}

void AddToRespArr(std::string& messageStream, std::vector<std::string>& respArr)
{

  size_t end = messageStream.find("\r\n");
  while(end != std::string::npos)
  {
    if(end == std::string::npos)
    {
      return;
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
      //messageStream = messageStream.substr(element.length(), messageStream.length());
    }
    messageStream = messageStream.substr(element.length() + 2, messageStream.length());
    end = messageStream.find("\r\n");
  }
}

int NumberOfElements(std::string& messageStream)
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

std::string HandleMessageStream(std::string& messageStream, int& numOfElements, std::vector<std::string>& respArr, int client_fd)
{
  if(messageStream.empty())
  {
    return "";
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
    return HandleRespArr(respArr, client_fd);
  }
  return "";
}

void HandleClient(int client_fd)
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
      const char* chResp = response.c_str();
      send(client_fd, chResp, strlen(chResp), 0);
    }
    //char pong[] = "+PONG\r\n";
    //send(client_fd, pong, strlen(pong), 0);
  }
  close(client_fd);
}

*/
int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  
  /*
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
   std::cerr << "Failed to create server socket\n";
   return 1;
  }
  
  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(6379);
  
  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
    std::cerr << "Failed to bind to port 6379\n";
    return 1;
  }
  
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }

  while(true)
  {
    int client = AcceptClient(server_fd);
    if(client > -1)
    {
      std::thread(HandleClient, client).detach();
    }
  }

  close(server_fd);
  */

  Server server;

  server.Run();

  server.Close();

  return 0;
}
