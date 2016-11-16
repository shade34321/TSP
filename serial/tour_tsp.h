#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "tour_list.h"

#ifndef tour_tsp_h
#define tour_tsp_h

int convert_2D_subscript(int rows, int r, int c);
void read_costs(char *filename);
void validate_cost_matrix();
void print_cost_matrix();
void print_usage(int numarguments, char *args[]);
double get_timestamp();
void iter_DFS(city_tour *best_tour, int starting_city);

#endif
