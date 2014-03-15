#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include "utils.h"
#include "cubelist.h"


// bfun* constructors
bfun* bf_new_var (int v);
bfun* bool_to_bf (bfun* b, bool bl);
bfun* empty_bf(bfun* b);

// single bfun* functions
bfun* complement(bfun* b);
bfun* try_simplify(bfun* b);
bool has_all_dc(bfun* b);

// combining bfun*s
void append_cubes(bfun* b, bfun* g);
bfun* or(bfun* b, bfun* g);

// bfun*s & variables
void and_var(bfun* b, int var);
int best_split(bfun* b);
bfun* pos_co(bfun* b, int x);
bfun* neg_co(bfun* b, int x);


int main(int argc, char* argv[]) {
  char* filename;
  if (argc > 1) filename = argv[1];
  else filename = "foo";

  int vs = 4;

  /*
  cube* plain = new_cube(vs);
  cube* one_true = new_cube(vs);
  set_true(one_true, 1);
  cube* one_false = new_cube(vs);
  set_false(one_false, 1);
  */
  cube* one_each = new_cube(vs);
  set_false(one_each, 1);
  set_true(one_each, 2);

  cube_list* test1 = new_cube_list(vs);
  add_cube(test1, one_each);

  printf("\nComplementing...\n");
  print_bfun(test1);

  bfun* result = complement(test1);
  printf("\nResult is:\n");
  print_bfun(result);

  del_bfun(result);
  /*
  del_cube(plain, vs);
  del_cube(one_true, vs);
  del_cube(one_false, vs);
  del_cube(one_each, vs);

  bfun* b = readFile(filename);
  bfun* cb = complement(b);
  
  del_bfun(cb);
  */
}


bfun* complement (bfun* b_initial) {
  printf("Complement %p\n", b_initial);
  print_bfun(b_initial);

  bfun* b = try_simplify(b_initial);
  if (b_initial != b) {
    printf("Complement returning simplified:\n");
    print_bfun(b);
    return b;
  } else {
    int x = best_split(b);

    bfun* pc = pos_co(b,x);
    bfun* nc = neg_co(b,x);

    bfun* p = complement(pc);
    bfun* n = complement(nc);

    del_bfun(pc);
    del_bfun(nc);

    // and_var modifies the passed cube
    and_var(p, x);
    and_var(n, x * -1);

    // or allocates a new cube & copies the cubelists over
    bfun* result = or(p,n);
    printf("Complement returning merged:\n");
    print_bfun(result);
    del_bfun(p);
    del_bfun(n);
    
    return result;
  }
}


bfun* pos_co (bfun* b, int v) {
  // reuse cubes?  No, because future splits may be on different variables
  // using no_dup makes the runtime quadratic!  But may substantially reduce
  // the number of cubes in each sublist.  TODO time this

  bfun* result = new_bfun(b->var_count);

  for (cube* c = b->begin; c != NULL; c = c->next) {
    cube* co_cube = NULL;

    switch (c->values[v]) {
      case dc:
        co_cube = copy(c);
        add_cube_no_dup(result, co_cube);
        break;
      case t:
        co_cube = copy(c);
        set_dc(co_cube, v);
        add_cube_no_dup(result, co_cube);
        break;
    }
  }

  return result;
}


bfun* neg_co (bfun* b, int v) {
  bfun* result = new_bfun(b->var_count);

  for (cube* c = b->begin; c != NULL; c = c->next) {
    cube* co_cube = NULL;

    switch (c->values[v]) {
      case dc:
        co_cube = copy(c);
        add_cube_no_dup(result, co_cube);
        break;
      case f:
        co_cube = copy(c);
        set_dc(co_cube, v);
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


cube_list* negate_cube(cube* c, int var) {
  cube_list* result = new_cube_list(var);

  for (int i = 0; i < var; i++) {
    val value = c->values[i];

    if (value == t) {
      cube* cn = new_cube(var);
      set_false(cn, i);
      add_cube(result, cn);
    } else if (value == f) {
      cube* cn = new_cube(var);
      set_true(cn, i);
      add_cube(result, cn);
    }
  }

  return result;
}


bfun* try_simplify(bfun* b) {
  bfun* result = NULL;

  if (b->cube_count == 0) { // empty cubelist is never true -> change to true bfun
    printf("Simplify returning empty cube_list \n");
    result = new_cube_list(b->var_count);
    add_cube(result, new_cube(b->var_count));
    print_bfun(result);
  } else if (has_all_dc(b)) { // always true -> change to empty (false) bfun
    printf("Simplify returning true cube_list \n");
    result = new_cube_list(b->var_count); 
    print_bfun(result);
  } else if (b->cube_count == 1) { // just one cube -> negate manually
    printf("Simplify returning single element cube_list \n");
    result = negate_cube(b->begin, b->var_count);
    print_bfun(result);
  }

  if (result == NULL) return b;
  del_bfun(b);
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


void append_cubes(bfun* b, bfun* g) {
  for (cube* c = g->begin; c != NULL; c = c->next) {
    add_cube(b, copy(c));
  }
}


bool var_stats(bfun* b, int* count, int* diff, int* is_binate) {
  // TODO this data should be stored in bfun rather than recalculated each time

  bool found_binate = false;

  for ( int i = 1; i < b->var_count; i++ ) {
    is_binate[i] = false;
    count[i] = 0;
    diff[i] = 0;
  }

  // yuck
  for(cube* c = b->begin; c != NULL; c = c->next) {
    for (int i = 1; i <= b->var_count; i++) {
      if (c->values[i] == t) {
        count[i]++;
        if (diff[i] < 0) {
          is_binate[i] = true;
          found_binate = true;
        }
        diff[i] ++;
      }
      if (c->values[i] == f) {
        count[i]++;
        if (diff[i] > 0) {
          is_binate[i] = true;
          found_binate = true;
        }
        diff[i]--;
      }
    }
  }
}


int best_split(bfun* b) {
  // select most binate variable (one that appears in most cubes)
  // break tie by most even t-f split (diff)
  // break remaining tie by lowest index
  // no binate variable?  Unate with most cubes & first index
  // TODO surely this can be unyuckified a bit

  // remember that 0 is not a valid variable
  int* count = Malloc(sizeof(int) * b->var_count + 1);
  int* diff = Malloc(sizeof(int) * b->var_count + 1);
  int* is_binate = Malloc(sizeof(int) * b->var_count + 1);

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


