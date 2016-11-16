#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef city_h
#define city_h

extern int *digraph;
extern int num_cities;

typedef struct {
    int *cities;
    int count;
    int cost;
} city_tour;

int cost(int c1, int c2);

city_tour * init_tour(int cost, int start_city);
void destroy_tour(city_tour *tour);
void print_tour(city_tour *tour);
void copy_tour(city_tour *t1, city_tour *t2);
void add_city(city_tour *tour, int city);
void remove_last_city(city_tour *tour);
int visited(city_tour *tour, int city);

int feasible(city_tour *tour, city_tour *best_tour, int city);

#endif
