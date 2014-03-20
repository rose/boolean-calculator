#ifndef _CUBELIST_H
#define _CUBELIST_H

#include "utils.h"
#include "cube.h"

typedef struct cube_list_t {
  cube* begin;
  cube* end;
  int cube_count;
  int var_count;
} cube_list;

typedef cube_list bfun; // use for non list specific functions

// cube_list functions
cube_list* new_cube_list(int var_count);
void del_cube_list(cube_list* cl);
void add_cube(cube_list* cl, cube* c);
void add_cube_no_dup(cube_list* cl, cube* c);
cube_list* negate_cube(cube* c, int var_count);

#ifdef TEST_CUBELIST
int main(int argc, char* argv[]) {
  bfun* foo = readFile(argv[1]);
  print_bfun(foo);
  del_bfun(foo);
}
#endif // TEST_CUBELIST

#endif // _CUBELIST_H
