#include <stdio.h>
#include <stdlib.h>

#include "city_tour.h"

#ifndef list_h
#define list_h

typedef void (*print)(stack *s);

typedef struct {
    void *list;
    int size;
    size_t data_size;
    void * init_value;
    print p;
} stack;

stack * init_stack(int num_cities, size_t ds, void * iv, print * print_function);
void print_stack(stack *s);
void print_int(stack *s);
int empty(stack *s);
void destroy_stack(stack *s);
void push(stack *s, int num_cities, void * c);
void * pop(stack *s);

#endif
