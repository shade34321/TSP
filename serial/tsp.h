#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

#ifndef tsp_h
#define tsp_h

typedef struct {
    int *cities;
    int count;
    int cost;
} city_tour;

int convert_2D_subscript(int rows, int r, int c);
void read_costs(char *filename);
void validate_cost_matrix();
void print_cost_matrix();
void print_usage(int numarguments, char *args[]);
void stack_DFS(city_tour *best_tour, int starting_city);

int cost(int c1, int c2);

city_tour * init_tour(int cost, int start_city);
void destroy_tour(city_tour *tour);
void print_tour(city_tour *tour);
void copy_tour(city_tour *t1, city_tour *t2);
void add_city(city_tour *tour, int city);
void remove_last_city(city_tour *tour);
int visited(city_tour *tour, int city);

int feasible(city_tour *tour, city_tour *best_tour, int city);

void test();

#endif
