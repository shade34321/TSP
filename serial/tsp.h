#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef tsp_h
#define tsp_h

int convert_2D_subscript(int rows, int r, int c);
void read_costs(char *filename, int num_cities, int *digraph);
void validate_cost_matrix(int num_cities, int *digraph);
void print_cost_matrix(int num_cities, int *digraph);
void print_usage(int numarguments, char *args[]);
void stack_DFS(int starting_city, int numCities, int *digraph);

// Don't have the list stuff done yet
//int feasible(stack *s, int city);

void test();

#endif
