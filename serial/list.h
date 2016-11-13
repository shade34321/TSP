#include <stdio.h>
#include <stdlib.h>

#ifndef list_h
#define list_h

typedef struct {
    int *list;
    int size;
} stack;

void init_stack(stack *s, int num_cities);
void print_stack(stack *s);
int empty(stack *s);
void destroy_stack(stack *s);
void push(stack *s, int num_cities, int c);
int pop(stack *s);

void test_stack();
#endif
