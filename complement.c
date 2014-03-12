#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"
#include "cubelist.h"


// bfun* constructors
bfun* bf_new_var (int v);
bfun* bool_to_bf (bfun* f, bool b);
bfun* empty_bf(bfun* f);

// single bfun* functions
bfun* complement(bfun* f);
bool try_simplify(bfun* f);

// combining bfun*s
bfun* and(bfun* f, bfun* g);
bfun* or(bfun* f, bfun* g);
bfun* not(bfun* f);

// bfun*s & variables
int most_binate(bfun* f);
bfun* pos_co(bfun* f, int x);
bfun* neg_co(bfun* f, int x);


int main(int argc, char* argv[]) {
  char* filename;
  if (argc > 1) filename = argv[1];
  else filename = "foo";

  bfun* cl = readFile(filename);
}


bfun* complement (bfun* f) {
  if (try_simplify(f)) {
    return f;
  } else {
    int x = most_binate(f);
    bfun* p = complement( pos_co(f, x) );
    bfun* n = complement( neg_co(f, x) );
    free(f);
    p = and(bf_new_var(x), p);
    n = and(not(bf_new_var(x)), n);
    return or(p, n);
  }
}


bool try_simplify(bfun* f) {
  // empty cubelist is never true -> change to true bfun
  if (f->cube_count == 0) {
    bool_to_bf(f, true);
    return true;
  } 

  // cubelist with don't care cube is always true -> change to false bfun
  for (int i = 0; i < f->cube_count; i++) {
    if (f->pos[i] == NULL && f->neg[i] == NULL) {
      bool_to_bf(f, false);
      return true;
    }
  }

  // cubelist with just one cube -> negate manually
  if (f->cube_count == 1) {
    // 
  }

  return false;
}

