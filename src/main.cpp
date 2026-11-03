#include <iostream>

#include <server.h>


int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  

  Server server;

  server.Run();

  server.Close();

  return 0;
}
