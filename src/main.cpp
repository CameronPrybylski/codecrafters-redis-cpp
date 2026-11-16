#include <iostream>

#include <server.h>


int main(int argc, char* argv[]) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  

  int port = -1;
  for(int i = 0; i < argc; ++i)
  {
    std::string arg = argv[i];
    if(arg == "--port")
    {
      std::string strPort = argv[i+1];
      port = stoi(strPort);
    }
  }

  Server server;
  if(port > 0)
    server.SetPort(port);

  server.Run();

  server.Close();

  return 0;
}
