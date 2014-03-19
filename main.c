#include "bfun.h"

int main(int argc, char* argv[]) {
  char* filename;
  if (argc > 1) filename = argv[1];
  else filename = "foo";
  char* out_file = "out.txt";

  int vs = 4;

  /*
  cube* plain = new_cube(vs);
  cube* one_false = new_cube(vs);
  set_false(one_false, 1);
  */
  cube* one_true = new_cube(vs);
  set_true(one_true, 1);
  cube* one_each = new_cube(vs);
  set_false(one_each, 1);
  set_true(one_each, 2);

  cube_list* test1 = new_cube_list(vs);
  add_cube(test1, one_each);
  add_cube(test1, one_true);

  printf("\nComplementing...\n");
  print_bfun(test1);

  bfun* result = complement(test1);
  printf("\n************************************\n");
  printf("\nResult is:\n");
  print_bfun(result);
  printf("\n************************************\n");

  del_bfun(test1);
  del_bfun(result);
  /*
  del_cube(plain, vs);
  del_cube(one_true, vs);
  del_cube(one_false, vs);
  del_cube(one_each, vs);
  */

  bfun* b = read_file(filename);
  bfun* cb = complement(b);
  
  printf("\n************************************\n");
  printf("\nResult is:\n");
  print_bfun(cb);
  printf("\n************************************\n");

  write_file(out_file, cb);

  del_bfun(b);
  del_bfun(cb);
}

