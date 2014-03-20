#include "bfun.h"

bfun* complement (bfun* b_initial) {
  // does not free argument.  

  bfun* b;
  if (!(b = complement_simplify(b_initial))) {
    return b;
  } else {
    int split_var = best_split(b);

    bfun* p_co = pos_cofactor(b,split_var);
    bfun* n_co = neg_cofactor(b,split_var);

    bfun* inv_p = complement(p_co);
    bfun* inv_n = complement(n_co);

    del_bfun(p_co);
    del_bfun(n_co);

    // and_var modifies the passed cube_list
    and_var(inv_p,  split_var);
    and_var(inv_n, -split_var);

    // or allocates a new cube & copies the cubelists over
    bfun* inv = or(inv_p,inv_n);
    del_bfun(inv_p);
    del_bfun(inv_n);
    
    return inv;
  }
}


bfun* pos_cofactor (bfun* b, int var) {
  // reuse cubes?  No, because future splits may be on different variables
  // using no_dup makes the runtime quadratic!  But may substantially reduce
  // the number of cubes in each sublist.  TODO time this

  bfun* result = new_bfun(b->var_count);

  for (cube* c = b->begin; c != NULL; c = c->next) {
    cube* co_cube = NULL;

    switch (c->values[var]) {
      case dc:
        co_cube = copy(c, b->var_count);
        add_cube_no_dup(result, co_cube);
        break;
      case t:
        co_cube = copy(c, b->var_count);
        set_dc(co_cube, var);
        add_cube_no_dup(result, co_cube);
        break;
    }
  }

  return result;
}


bfun* neg_cofactor (bfun* b, int var) {
  bfun* result = new_bfun(b->var_count);

  for (cube* c = b->begin; c != NULL; c = c->next) {
    cube* co_cube = NULL;

    switch (c->values[var]) {
      case dc:
        co_cube = copy(c, b->var_count);
        add_cube_no_dup(result, co_cube);
        break;
      case f:
        co_cube = copy(c, b->var_count);
        set_dc(co_cube, var);
        add_cube_no_dup(result, co_cube);
        break;
    }
  }
  
  return result;
}


bool has_all_dc(bfun* b) {
  for (cube* c = b->begin; c != NULL; c = c->next) {
    if (all_dc(c, b->var_count)) return true;
  }
  return false;
}


bfun* complement_simplify(bfun* b) {
  bfun* result = NULL;

  if (b->cube_count == 0) { // empty cubelist is never true -> change to true bfun
    result = new_cube_list(b->var_count);
    add_cube(result, new_cube(b->var_count));
  } else if (has_all_dc(b)) { // always true -> change to empty (false) bfun
    result = new_cube_list(b->var_count); 
  } else if (b->cube_count == 1) { // just one cube -> negate manually
    result = negate_cube(b->begin, b->var_count);
  }

  return result;
}


void and_var(bfun* b, int var) {
  for (cube* c = b->begin; c != NULL; c = c->next) {
    insert_var (c, var);
  }
}


bfun* or(bfun* b, bfun* g) {
  bfun* result = new_bfun(b->var_count);
  append_cubes(result, b);
  append_cubes(result, g);
  return result;
}


bfun* and(bfun* b, bfun* g) {
  bfun* m_not = complement(b);
  bfun* n_not = complement(g);
  bfun* mn_or = or(m_not, n_not);
  bfun* to_return = complement(mn_or);
  del_bfun(m_not);
  del_bfun(n_not);
  del_bfun(mn_or);
  return to_return;
}


void append_cubes(bfun* b, bfun* g) {
  for (cube* c = g->begin; c != NULL; c = c->next) {
    add_cube(b, copy(c, b->var_count));
  }
}


bool var_stats(bfun* b, int* count, int* diff, int* is_binate) {
  // TODO this data should be stored in bfun rather than recalculated each time

  bool found_binate = false;

  for ( int i = 0; i <= b->var_count; i++ ) {
    is_binate[i] = false;
    count[i] = 0;
    diff[i] = 0;
  }

  // yuck
  for(cube* c = b->begin; c != NULL; c = c->next) {
    for (int i = 1; i <= b->var_count; i++) {
      int sign, value_i = c->values[i];
      if      (value_i == t) sign =  1;
      else if (value_i == f) sign = -1;
      if(diff[i] * sign < 0) {
        is_binate[i] = true;
        found_binate = true;
      }
      if (value_i == t || value_i == f) {
        count[i]++;
        diff[i]+=sign;
      }
      
    }
  }

  return found_binate;
}


int best_split(bfun* b) {
  // select most binate variable (one that appears in most cubes)
  // break tie by most even t-f split (diff)
  // break remaining tie by lowest index
  // no binate variable?  Unate with most cubes & first index
  // TODO surely this can be unyuckified a bit

  // remember that 0 is not a valid variable
  int* count = Malloc(sizeof(int) * (b->var_count + 1));
  int* diff = Malloc(sizeof(int) * (b->var_count + 1));
  int* is_binate = Malloc(sizeof(int) * (b->var_count + 1));

  // fill arrays with information from b
  bool binate_exists = var_stats(b, count, diff, is_binate);

  int best_index = 0; 
  int best_count = 0;
  int best_diff = INT_MAX;

  for (int i = 1; i <= b->var_count; i++) {
    bool worthy = !binate_exists || is_binate[i];
    bool highest_count = count[i] > best_count;
    bool tied_count_better_diff = count[i] == best_count && diff[i] < best_diff;

    if ( worthy && ( highest_count || tied_count_better_diff) ) {
      best_index = i;
      best_count = count[i];
      best_diff = diff[i];
    }
  }

  free(count);
  free(diff);
  free(is_binate);

  // breathe sigh of relief
  return best_index;
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

