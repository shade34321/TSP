#include "city.h"

/*
 * Inititializes a tour for us so we can use it.
 */

city_tour * init_tour(int cost, int start_city) {
	city_tour *tmp  = (city_tour *)malloc(sizeof(city_tour));

	if(!tmp) {
		printf("Unable to allocate memory for tour!\n");
		exit(1);
	}

	tmp->cities = (int *)malloc(sizeof(int) * num_cities + 1);

	if(!tmp->cities) {
		printf("Unable to allocate memory for cities within tour.\n");
		exit(1);
	}

	// Sets up our city tour for us.
	for(int i = 0; i < num_cities; i++) {
		if(i == start_city) {
			tmp->cities[i] = 0;
		} else {
			tmp->cities[i] = -1;
		}
	}
	
	tmp->cost = cost;
	tmp->count = 1;	

	return tmp;
}

void destroy_tour(city_tour *tour) {
	free(tour->cities);
	free(tour);
}

void print_tour(city_tour * tour) {
	for(int i = 0; i < tour->count; i++) {
		printf("%d", tour->cities[i]);

		if( i < tour->count - 1) {
			printf(" -> ");
		}
		
		//Line Wrap
		if(i != 0 && i % 20 == 0) { 
			printf("\n");
		}
	}

	printf("\nCost: %d\nCities in Tour: %d\n", tour->cost, tour->count);
}

void copy_tour(city_tour *t1, city_tour *t2) {
	//Last one copied it backwards. Figured this was easier:)
	memcpy(t2->cities, t1->cities, ((num_cities+1)*sizeof(int)));
	t2->cost = t1->cost;
	t2->count = t1->count;
}

int cost(int c1, int c2) {
	return digraph[c1 * num_cities + c2];
}

void add_city(city_tour *tour, int city) {
	if (tour->count > 0) {
		tour->cost += cost(tour->cities[(tour->count)-1], city);
	}

	tour->cities[tour->count] = city;	
	//Make sure you put tour->count in parenthesis otherwise it works out of order.
	(tour->count)++;
}

int visited(city_tour *tour, int city) {
	for(int i = 0; i < tour->count; i++) {
		if(tour->cities[i] == city) {
			return 1;
		}
	}

	return 0;
}

void remove_last_city(city_tour *tour){
	//Make sure we account for the cost first.
	//Similar as to how we did it in add_city
	if(tour->count > 0) {
		tour->cost -= cost(tour->cities[(tour->count)-2], tour->cities[(tour->count)-1]);
	} else {
		tour->cost = 0;
	}

	tour->cities[(tour->count)-1] = -1;
	(tour->count)--;
}

int feasible(city_tour *tour, city_tour *best_tour, int city) {
    int t1 = tour->cost + cost(tour->cities[(tour->count)-1], city);
    int btc = best_tour->cost;
	if (t1 < btc) {
		return 1;
	}

	return 0;
}
