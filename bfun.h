#ifndef _BFUN_H
#define _BFUN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include "utils.h"
#include "cubelist.h"


// single bfun* functions
bfun* complement(bfun* b);
bfun* try_simplify(bfun* b);
bool has_all_dc(bfun* b);

// combining bfun*s
void append_cubes(bfun* b, bfun* g);
bfun* or(bfun* b, bfun* g);
bfun* and(bfun* b, bfun* g);

// bfun*s & variables
void and_var(bfun* b, int var);
int best_split(bfun* b);
bfun* pos_co(bfun* b, int var);
bfun* neg_co(bfun* b, int var);

#endif // _BFUN_H

