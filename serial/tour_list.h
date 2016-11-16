#include <stdio.h>
#include <stdlib.h>

#include "city_tour.h"

#ifndef tour_list_h
#define tour_list_h

typedef struct {
    city_tour *list;
    int size;
} stack;

stack * init_stack(int num_cities);
void print_stack(stack *s);
int empty(stack *s);
void destroy_stack(stack *s);
void push(stack *s, int num_cities, city_tour *tour);
city_tour * pop(stack *s);

#endif
