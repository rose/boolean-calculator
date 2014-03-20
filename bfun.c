#include "bfun.h"

bfun* complement (bfun* b_initial) {
  // does not free argument.  

  bfun* b;
  if (!(b = complement_simplify(b_initial))) {
    return b;
  } else {
    int x = best_split(b);

    bfun* pc = pos_co(b,x);
    bfun* nc = neg_co(b,x);

    bfun* p = complement(pc);
    bfun* n = complement(nc);

    del_bfun(pc);
    del_bfun(nc);

    // and_var modifies the passed cube_list
    and_var(p, x);
    and_var(n, x * -1);

    // or allocates a new cube & copies the cubelists over
    bfun* result = or(p,n);
    del_bfun(p);
    del_bfun(n);
    
    return result;
  }
}


bfun* pos_co (bfun* b, int var) {
  // reuse cubes?  No, because future splits may be on different variables
  // using no_dup makes the runtime quadratic!  But may substantially reduce
  // the number of cubes in each sublist.  TODO time this

  bfun* result = new_bfun(b->var_count);

  for (cube* c = b->begin; c != NULL; c = c->next) {
    cube* co_cube = NULL;

    switch (c->values[v]) {
      case dc:
        co_cube = copy(c, b->var_count);
        add_cube_no_dup(result, co_cube);
        break;
      case t:
        co_cube = copy(c, b->var_count);
        set_dc(co_cube, v);
        add_cube_no_dup(result, co_cube);
        break;
    }
  }

  return result;
}


bfun* neg_co (bfun* b, int var) {
  bfun* result = new_bfun(b->var_count);

  for (cube* c = b->begin; c != NULL; c = c->next) {
    cube* co_cube = NULL;

    switch (c->values[v]) {
      case dc:
        co_cube = copy(c, b->var_count);
        add_cube_no_dup(result, co_cube);
        break;
      case f:
        co_cube = copy(c, b->var_count);
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


cube_list* negate_cube(cube* c, int var_count) {
  cube_list* result = new_cube_list(var_count);

  for (int i = 1; i <= var; i++) {
    val value = c->values[i];
    cube* cn = NULL;

    if (value == t || value == f) cn = new_cube(var);
    if      (value == t)  set_false(cn, i);
    else if (value == f)   set_true(cn, i);
    if(cn) add_cube(result, cn);
  }

  return result;
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
      int sign;
      if      (c->values[i] == t) sign =  1;
      else if (c->values[i] == f) sign = -1;
      if(diff[i] * sign < 0) {
        is_binate[i] = true;
        found_binate = true;
      }
      if (c->values[i] == t || c->values[i] == f) {
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


