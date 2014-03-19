#include <stdio.h>

#include "utils.h"
#include "bfun.h"

void run_file(char* filename);
void do_quit();
void do_and(int dest, int m, int n);
void do_or(int dest, int m, int n);
void do_not(int dest, int n);
void do_write(int n);
void do_read(int n);
