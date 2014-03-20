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
bfun* complement_simplify(bfun* b);
bool has_all_dc(bfun* b);

// combining bfun*s
void append_cubes(bfun* b, bfun* g);
bfun* or(bfun* b, bfun* g);
bfun* and(bfun* b, bfun* g);

// bfun*s & variables
void and_var(bfun* b, int var);
int best_split(bfun* b);
// cofactor allocates and returns new bfun
bfun* cofactor(bfun* b, int var, val side);

// bfun functions
bfun* new_bfun(int vars);
void del_bfun(bfun* b);
void del_bfun_leave_cubes(bfun* b);
void add_term(bfun* foo, cube* c);
bfun* invert_cube(cube* c, int var_count);
bfun* read_file(char* filename);
void write_file(char* out_file, bfun* b);
void print_bfun(bfun* foo);

#endif // _BFUN_H
