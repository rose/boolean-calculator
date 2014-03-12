#include "cubelist.h"


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

void del_cube(cube* c) {
  free(c->values);
  free(c);
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

bool all_dc (cube* c, int var_count) {
  return c->dc_count == var_count;
}

void print_cube(cube* c, int var_count) {
  printf("  cube at %p has %d dc variables\n    ", c, c->dc_count);

  for (int i = 0; i <= var_count; i++) {
    printf("%d ", (int) c->values[i]);
  }

  printf("\n");
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
  cube* c = cl->begin;

  while (c != NULL) {
    cube* next = c->next;
    del_cube(c);
    c = next;
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
      // add_term(foo, bar);
    } else if (c->values[i] == f) {
      cube* bar = new_cube(var_count);
      set_true(bar, i);
      // add_term(foo, bar);
    }
  }
  return foo;
}

bfun* readFile(char* filename) {
  FILE *fp = fopen(filename, "r");
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

void print_bfun(bfun* foo) {
  printf("bfun:\n  start: %p\n  end: %p\n", foo->begin, foo->end);
  printf("  cube_count: %d\n  var_count: %d\n", foo->cube_count, foo->var_count);
  
  cube* cursor = foo->begin;
  while (cursor != NULL) {
    print_cube(cursor, foo->var_count);
    cursor = cursor->next;
  }
}

