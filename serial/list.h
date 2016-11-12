#include <stdio.h>
#include <stdlib.h>

#ifndef list_h
#define list_h

typedef struct {
    int *cities;
    int count;
    int cost;
} city_tour;

typedef struct {
    city *list;
    int size;
} stack;

void init_stack(stack *s);
void print_city(city *c);
void print_stack(stack *s);
void empty(stack *s);
void destroy_stack(stack *s);
void push(stack *s, city *c);
void copy_list(stack *s, stack *copy);
city * pop(list *s);

#endif
