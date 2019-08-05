#include "IMServer.hpp"

int main()
{
  IMServer *im = new IMServer();
  im->InitServer();
  im->Run();
  return 0;
}