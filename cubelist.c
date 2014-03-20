#include "cubelist.h"


void* Malloc(int size) {
  void* foo = malloc(size);
  if (foo == NULL) {
    printf("Unable to allocate %d bytes of memory!  Crashing now.\n", size);
    exit(2);
  }
  return foo;
}


// cube_list functions

cube_list* new_cube_list(int var_count) {
  cube_list* cl = malloc(sizeof(cube_list));
  cl->begin = NULL;
  cl->end = NULL;
  cl->cube_count = 0;
  cl->var_count = var_count;

  return cl;
}

void del_cube_list(cube_list* cl) {
#ifdef DEBUG
  printf("Deleting cube_list at %p\n", cl);
  print_bfun(cl);
#endif

  cube* c = cl->begin;

  int i = 0;
  while (c != NULL) {
#ifdef DEBUG
    printf("  %d:", i);
#endif
    cube* next = c->next;
    del_cube(c, cl->var_count);
    c = next;
    i++;
  }

  free(cl);
}

void add_cube(cube_list* cl, cube* c) {
  cl->cube_count++;

  if (cl->begin == NULL) {
    cl->begin = c;
    cl->end = c;
    return;
  }
  cl->end->next = c;
  cl->end = c;
}


void add_cube_no_dup(cube_list* cl, cube* c) {
  for (cube* this = cl->begin; this != NULL; this = this->next) {
    if (same_cube(this, c, cl->var_count)) return;
  }
  add_cube(cl, c);
}

cube_list* negate_cube(cube* c, int var_count) {
  cube_list* result = new_cube_list(var_count);

  for (int i = 1; i <= var_count; i++) {
    val value = c->values[i];
    cube* cn = NULL;

    if (value == t || value == f) cn = new_cube(var_count);
    if      (value == t)  set_false(cn, i);
    else if (value == f)   set_true(cn, i);
    if(cn) add_cube(result, cn);
  }

  return result;
}

void del_bfun_leave_cubes(bfun* b) {
  // for now bfuns are just cube_lists, so there's nothing to do here
}
