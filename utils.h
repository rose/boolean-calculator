#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


void* Malloc(int size) {
  void* foo = malloc(size);
  if (foo == NULL) {
    printf("Unable to allocate %d bytes of memory!  Crashing now.\n");
    exit(2);
  }
  return foo;
}

typedef enum {
  bad, t, f, dc
} val;


#endif // _UTILS_H
