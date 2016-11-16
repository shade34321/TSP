#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

#ifndef tsp_h
#define tsp_h

int convert_2D_subscript(int rows, int r, int c);
void read_costs(char *filename);
void validate_cost_matrix();
void print_cost_matrix();
void print_usage(int numarguments, char *args[]);
void stack_DFS(city_tour *best_tour, int starting_city);
void rec_DFS(city_tour *best_tour, city_tour *current_tour, int starting_city);

#endif
