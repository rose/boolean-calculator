#include <stdio.h>

int main(int argc, char* argv[]) {
  char* filename;
  if (argc > 1) filename = argv[1];
  else filename = "foo";

  printf(filename);
}
