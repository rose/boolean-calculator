#include "cubelist.h"


void* Malloc(int size) {
  void* foo = malloc(size);
  if (foo == NULL) {
    printf("Unable to allocate %d bytes of memory!  Crashing now.\n", size);
    exit(2);
  }
  return foo;
}


// cube functions

cube* new_cube(int vars) {
  cube* c = Malloc(sizeof(cube));
  c->values = Malloc((vars + 1) * sizeof(val));
  c->values[0] = bad; // variables numbered from 1, don't want to recalculate indices everywhere
  for (int i = 1; i <= vars; i++) {
    c->values[i] = dc;
  }
  c->dc_count = vars;
  c->next = NULL;

  return c;
}

void del_cube(cube* c, int vars) {
#ifdef DEBUG
  printf("  deleting cube %p ", c);
  print_cube(c, vars);
  printf("\n");
#endif
  free(c->values);
  free(c);
}

cube* copy(cube* c, int v) {
  cube* new_c = new_cube(v);

  for (int i = 0; i <= v; i++) {
    new_c->values[i] = c->values[i];
  }
  new_c->dc_count = c->dc_count;
  new_c->next = NULL;

  return new_c;
}

void remove_var (cube* c, int var) {
  if (c->values[var] != dc) {
    c->values[var] = dc;
    c->dc_count++;
  }
}

void set_false (cube* c, int var) {
  if (c->values[var] == dc) {
    c->dc_count--;
  }
  c->values[var] = f;
}

void set_true (cube* c, int var) {
  if (c->values[var] == dc) {
    c->dc_count--;
  }
  c->values[var] = t;
}

void insert_var (cube* c, int var) {
  if (var < 0) {
    set_false(c, var * -1);
  } else {
    set_true(c, var);
  }
}

void set_dc (cube* c, int var) {
  if (c->values[var] != dc) {
    c->dc_count++;
  }
  c->values[var] = dc;
}

bool all_dc (cube* c, int var_count) {
  return c->dc_count == var_count;
}

void print_cube(cube* c, int var_count) {
  // printf("  cube at %p has %d dc variables\n    ", c, c->dc_count);

  printf("[");
  for (int i = 1; i <= var_count; i++) {
    printf("%d ", (int) c->values[i]);
  }
  printf("] ");

  // printf("\n");
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

bool same_cube(cube* c1, cube* c2, int var_count) {
  if (c1->dc_count != c2->dc_count) {
    return false;
  }

  for (int i = 0; i < var_count; i++) {
    if (c1->values[i] != c2->values[i]) {
      return false;
    }
  }

  return true;
}


// bfun functions

bfun* new_bfun(int vars) {
  return (bfun*)new_cube_list(vars);
}

void del_bfun(bfun* b) {
  del_cube_list((cube_list*) b);
}

bfun* invert_cube(cube* c, int var_count) { // yuck
  bfun* foo = new_bfun(var_count);
  for (int i = 0; i < var_count; i++) {
    if (c->values[i] == t) {
      cube* bar = new_cube(var_count);
      set_false(bar, i);
      add_cube(foo, bar);
    } else if (c->values[i] == f) {
      cube* bar = new_cube(var_count);
      set_true(bar, i);
      add_cube(foo, bar);
    }
  }
  return foo;
}

bfun* read_file(char* filename) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Error opening file %s\n", filename);
    exit(1);
  }

  int var_count = 0;
  int cube_count = 0;

  fscanf(fp, "%d", &var_count);
  fscanf(fp, "%d", &cube_count);

  cube_list* cl = new_cube_list(var_count);

  int vars_not_dc;
  int var;
  val value;

  for (int i = 0; i < cube_count; i++) {

# ifdef DEBUG
    printf("Reading line %d... ", i);
#endif

    fscanf(fp, "%d", &vars_not_dc); 
    cube* c = new_cube(var_count);

# ifdef DEBUG
    printf("creating cube...\n");
#endif

    for (int j = 0; j < vars_not_dc; j++) {
      fscanf(fp, "%d", &var);
      if (var > 0) {
        set_true (c, var);
      } else {
        set_false (c, var * -1);
      }
    }

    add_cube(cl, c);

# ifdef DEBUG
    print_cube(c, var_count);
    if (c->dc_count != var_count - vars_not_dc) {
      printf("Error reading cube %d in file %s!\n", i, filename);
    }
#endif 
  }

  fclose(fp);
  return (bfun*) cl;
}


void write_file(char* name, bfun* b) {
  FILE *fp = fopen(name, "w");

  fprintf(fp, "%d\n%d\n", b->var_count, b->cube_count);
  for (cube* c = b->begin; c != NULL; c = c->next) {
    fprintf(fp, "%d ", b->var_count - c->dc_count);
    for (int i = 1; i <= b->var_count; i++) {
      switch (c->values[i]) {
        case t:
          fprintf(fp, "%d ", i);
          break;
        case f:
          fprintf(fp, "-%d ", i);
          break;
      }
    }
    fprintf(fp, "\n");
  }
}


void print_bfun(bfun* foo) {
  printf("  %p: ", foo);
  cube* cursor = foo->begin;
  while (cursor != NULL) {
    print_cube(cursor, foo->var_count);
    cursor = cursor->next;
  }
  printf("\n");
}

void del_bfun_leave_cubes(bfun* b) {
  // for now bfuns are just cube_lists, so there's nothing to do here
}
