#include "project/tmp.hpp"

int main(int argc, char* argv[])
{
  (void) argc;
  (void) argv;

  if(tmp::add(2, 2) != 4)
    return -1;
  return 0;
}
