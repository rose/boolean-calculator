#include "parser.h"

int main() {
  run_file("BooleanCalculatorEngine/cmd4.txt");
}

void run_file(char* filename) {
  FILE* fp = fopen(filename, "r");
  char c;
  int dest, op, op2;
  fscanf(fp, "%c", &c);

  while (c != 'q') {
    fscanf(fp, "%d", &dest);

    switch (c) {
      case 'r':
        do_read(dest); break;
      case 'p':
        do_write(dest); break;
      case '!':
        fscanf(fp, "%d", &op);
        do_not(dest, op); break;
      case '+':
        fscanf(fp, "%d", &op);
        fscanf(fp, "%d", &op2);
        do_or(dest, op, op2); break;
      case '&':
        fscanf(fp, "%d", &op);
        fscanf(fp, "%d", &op2);
        do_and(dest, op, op2); break;
      default:
        printf("Unknown command %c, dying now!\n", c);
        exit(2);
    }

    fscanf(fp, "%c", &c); // consume new line after args
    fscanf(fp, "%c", &c);

    do_quit();
  }
}

void do_read(int n) {
  printf("do_read %d\n", n);
}

void do_write(int n) {
  printf("do_write %d\n", n);
}

void do_not(int dest, int n) {
  printf("do_not %d\n", n);
}

void do_or(int dest, int m, int n) {
  printf("do_or %d %d\n", m, n);
}

void do_and(int dest, int m, int n) {
  printf("do_and %d %d\n", m, n);
}

void do_quit() {
  printf("do_quit\n");
}

