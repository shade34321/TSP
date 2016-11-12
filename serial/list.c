#include "list.h"

void init_stack(stack *s) {
    s = (stack *)malloc(sizeof(stack));

    if (!s) {
        printf("Unable to allocate enough memory to initialize stack.\n");
        exit(1);
    }
    s->list = malloc(
    s->size = 0;
}
void print_city(city *c);
void print_stack(stack *s);
void empty(stack *s);
void destroy_stack(stack *s);
void push(stack *s, city *c);
void copy_list(stack *s, stack *copy);
city * pop(list *s);
