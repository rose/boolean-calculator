#ifndef _CUBELIST_H
#define _CUBELIST_H

#include "utils.h"

typedef struct cube_t {
  int dc_count;
  val* values;
  // next is for cube_lists, should not be allocated or freed by cube functions
  struct cube_t* next; 
} cube;

typedef struct cube_list_t {
  cube* begin;
  cube* end;
  int cube_count;
  int var_count;
} cube_list;

typedef cube_list bfun; // use for non list specific functions

void* Malloc(int size);

// cube functions
cube* new_cube(int vars);
void del_cube(cube* c, int vars);
cube* copy(cube* c, int vars);
void remove_var (cube* c, int var);
void insert_var (cube* c, int var);
void set_false (cube* c, int var);
void set_true (cube* c, int var);
void set_dc (cube* c, int var);
bool all_dc (cube* c, int var_count);
void print_cube(cube* c, int var_count);

// cube_list functions
cube_list* new_cube_list(int var_count);
void del_cube_list(cube_list* cl);
void add_cube(cube_list* cl, cube* c);
void add_cube_no_dup(cube_list* cl, cube* c);
bool same_cube(cube* c1, cube* c2, int var_count);

// bfun functions
bfun* new_bfun(int vars);
void del_bfun(bfun* b);
void del_bfun_leave_cubes(bfun* b);
void add_term(bfun* foo, cube* c);
bfun* invert_cube(cube* c, int var_count);
bfun* readFile(char* filename);
void print_bfun(bfun* foo);

#ifdef TEST_CUBELIST
int main(int argc, char* argv[]) {
  bfun* foo = readFile(argv[1]);
  print_bfun(foo);
  del_bfun(foo);
}
#endif // TEST_CUBELIST

#endif // _CUBELIST_H
