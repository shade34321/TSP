#include <stdio.h>
#include <stdlib.h>

#include "city_tour.h"

#ifndef list_h
#define list_h

typedef struct {
    void *list;
    int size;
    size_t data_size;
    void * init_value;
} stack;

typedef struct {
    int data;
    void (*print_int)(struct city);
} city;

stack * init_stack(int num_cities, size_t ds, void * iv,rint * print_function);
void print_stack(stack *s);
void print_city(city *c);
int empty(stack *s);
void destroy_stack(stack *s);
void push(stack *s, int num_cities, void * c);
void * pop(stack *s);

#endif
