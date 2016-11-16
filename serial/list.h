#include <stdio.h>
#include <stdlib.h>

#include "city_tour.h"

#ifndef list_h
#define list_h

typedef struct {
    int *list;
    int size;
} stack;

stack * init_stack(int num_cities);
void print_stack(stack *s);
int empty(stack *s);
void destroy_stack(stack *s);
void push(stack *s, int num_cities, int c);
int pop(stack *s);

#endif
