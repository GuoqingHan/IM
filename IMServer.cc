#include "IMServer.hpp"

int main()
{
  signal(SIGPIPE, SIG_IGN);
  IMServer *im = new IMServer();
  im->InitServer();
  im->Run();
  return 0;
}