#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <thread>

#include <vector>

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
  */
  int client = accept(server_fd, nullptr, nullptr);
  return client;
}

void HandleClient(int client_fd)
{
  while(true)
  {
    char buffer[64] = {};
    ssize_t bytesReceived = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if(bytesReceived <= 0)
    {
      break;
    }
    char pong[] = "+PONG\r\n";
    send(client_fd, pong, strlen(pong), 0);
  }
  close(client_fd);
}


int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  
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

  return 0;
}
