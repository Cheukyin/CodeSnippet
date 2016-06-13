/*
 * Test harness for pointer.c
 *
 * By: Dustin Lundquist <dol@cs.washington.edu>
 */

#include <stdio.h>
#include "pointer.h"

typedef struct test_s {
    char *function_name;
    int (*function_under_test)();
    int expected_result;
} test;

typedef struct withinSameBlock_args_s {
    char * function_name;
    int * ptr1;
    int * ptr2;
    int expected_result;
} withinSameBlock_args;

typedef struct withinArray_args_s {
    char * function_name;
    int * base;
    int size;
    int * ptr;
    int expected_result;
} withinArray_args;

typedef struct invert_args_s {
    char * function_name;
    int x;
    int p;
    int n;
    int expected_result;
} invert_args;

static test tests[] = {
    { "intSize()",      intSize,        sizeof(int) },
    { "doubleSize()",   doubleSize,     sizeof(double) },
    { "pointerSize()",  pointerSize,    sizeof(void *) },
    { "changeValue()",  changeValue,    351 },
    { NULL, NULL, 0 }
};

static withinSameBlock_args box_args[] = {
  {"withinSameBlock", (int *) 0x1,        (int *) 72,          0},
  {"withinSameBlock", (int *) 0x1,        (int *) 4,           1},
  {"withinSameBlock", (int *) 0x12345678, (int *) 0x1,         0},
  {"withinSameBlock", (int *) 0x12345678, (int *) 0x12345658,  1},
  {"withinSameBlock", (int *) 0x12345678, (int *) 0x12345686,  0},
  {NULL, NULL, NULL, 0}
};

static withinArray_args arr_args[] = {
  {"withinArray", (int *) 0x1, 4, (int *) 0xd, 1}, 
  {"withinArray", (int *) 0x1, 4, (int *) 0x11, 0}, 
  {"withinArray", (int *) 0x14, 4, (int *) 0xd, 0}, 
  {NULL, NULL, 0, NULL, 0}
};

static invert_args inverts[] = {
    {"invert", 142,    3,  3, 182},
    {"invert", 1645,   2,  4, 1617},
    {"invert", 123456, 12, 1, 127552},
    {NULL,     0,      0,  0, 0}
};

int main(int argc, char **argv) {
    int result;
    int score = 0;
    test * t;

    for (t = tests; t->function_name; t++) {
        result = t->function_under_test();

        printf("%s = %d", t->function_name, result);

        if (result == t->expected_result) {
            printf("\t[ OK ]\n");
            score ++;
        } else {
            printf("\t[ fail ] (expected %d)\n", t->expected_result);
        }
    }

    withinSameBlock_args * args;

    for (args = box_args; args->function_name != NULL; args++) {
        result = withinSameBlock(args->ptr1, args->ptr2);

        printf("%s(%p, %p) = %d", args->function_name, args->ptr1, args->ptr2, result);
        if (result == args->expected_result) {
          printf("\t[ OK ]\n");
        } else {
          printf("\t[ fail ] (expected %d)\n", args->expected_result);
        }
    }

    withinArray_args * args2;

    for (args2 = arr_args; args2->function_name != NULL; args2++) {
      result = withinArray((args2->base), args2->size, args2->ptr);
      printf("%s(%p, %d, %p) = %d", args2->function_name, args2->base, args2->size, args2->ptr, result);
      if (result == args2->expected_result) {
        printf("\t[ OK ]\n");
      } else {
        printf("\t[ fail ] (expected %d)\n", args2->expected_result);
      }
    }
    
    invert_args * args3;
    for (args3 = inverts; args3->function_name != NULL; args3++) {
      result = invert(args3->x, args3->p, args3->n);
      printf("%s(%d, %d, %d) = %d", args3->function_name, args3->x, args3->p, args3->n, result);
      if (result == args3->expected_result) {
        printf("\t[ OK ]\n");
      } else {
        printf("\t[ fail ] (expected %d)\n", args3->expected_result);
      }
    }

    return 0;
}
