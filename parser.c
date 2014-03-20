#include "parser.h"


bfun** functions;
char str_buf[10];
int vars;

void alloc_functions();
void free_functions();
void scanint(char* str, int* n);
void check_bounds(int n);


void parse() {
  char c;
  int dest, op, op2;
  scanf("%c", &c);

  alloc_functions();

  while (c != 'q') {
    scanint("%d", &dest);

    switch (c) {
      case 'r':
        do_read(dest); break;
      case 'p':
        do_write(dest); break;
      case '!':
        scanint("%d", &op);
        do_not(dest, op); break;
      case '+':
        scanint("%d", &op);
        scanint(" %d", &op2);
        do_or(dest, op, op2); break;
      case '&':
        scanint("%d", &op);
        scanint(" %d", &op2);
        do_and(dest, op, op2); break;
      default:
        printf("Unknown command %c, dying now!\n", c);
        do_quit();
        exit(2);
    }

    scanf("%c", &c); // consume new line after args
    scanf("%c", &c);
  }

  free_functions();
}


void scanint(char* str, int* n_ptr) {
  // printf("entering scanint with n_ptr = %p, n = %d, str = %s\n", n_ptr, *n_ptr, str);
  scanf(str, n_ptr);
  check_bounds(*n_ptr);
}


void alloc_functions() {
  printf("Allocating space for %d functions.  ", MAX_FUNCTIONS);
  printf("Please only use function ids between 0 and %d.\n", MAX_FUNCTIONS - 1);

  functions = Malloc(sizeof(bfun*) * MAX_FUNCTIONS);
  for (int i = 0; i < MAX_FUNCTIONS; i++) {
    functions[i] = NULL;
  }
}


void maybe_free(int n) {
  if (functions[n] != NULL) {
    printf("Deleting function %d\n", n);
    del_bfun(functions[n]);
    functions[n] = NULL;
  }
}


void free_functions() {
  for (int i = 0; i < MAX_FUNCTIONS; i++) {
    maybe_free(i);
  }
  free(functions);
}


void check_bounds(int n) {
  if (n < 0 || n >= MAX_FUNCTIONS) {
    printf("Cannot access function %d.  Function must be between 0 and %d.\n", n, MAX_FUNCTIONS - 1);
    printf("Crashing now.\n");
    free_functions();
    exit(2);
  }
}


bool function_exists(int n) {
  if (functions[n] == NULL) {
    printf("Function %d has not been initialized!  Ignoring this command.", n);
    return false;
  }
  return true;
}


void do_read(int n) {
  printf("do_read %d\n", n);
  maybe_free(n);

  sprintf(str_buf, "%d.pcn", n);
  functions[n] = read_file(str_buf);
}


void do_write(int n) {
  printf("do_write %d\n", n);
  sprintf(str_buf, "%d.pcn", n);
  if (function_exists(n)) {
    write_file(str_buf, functions[n]);
  }
}


void do_not(int dest, int n) {
  printf("do_not %d\n", n);
  if (function_exists(n)) {
    maybe_free(dest);
    functions[dest] = complement(functions[n]);
  }
}


void do_or(int dest, int m, int n) {
  printf("do_or %d %d\n", m, n);
  if (function_exists(m) && function_exists(n)) {
    maybe_free(dest);
    functions[dest] = or(functions[m], functions[n]);
  }
}


void do_and(int dest, int m, int n) {
  printf("do_and %d %d\n", m, n);
  if (function_exists(m) && function_exists(n)) {
    maybe_free (dest);
    functions[dest] = and(functions[m], functions[n]);
  }
}


void do_quit() {
  printf("do_quit\n");
  printf("This was really a special session.  I'll never forget you.\n");
  free_functions();
  printf("Memory deleted.  Have a nice day!\n");
}

