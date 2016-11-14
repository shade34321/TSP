#include "tsp.h"

//Got tired of passing these around and they are usually just read only
int * digraph;
int num_cities;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage(argc, argv);
        exit(1);
    }

	int starting_city = 0;//Want to change this later to accept different starting cities.
    //Get toal number of cities from command line
    num_cities = atoi(argv[2]);
    digraph = malloc(sizeof(int) * num_cities * num_cities);

    if (!digraph) {
        printf("Unable to allocate memory for the digraph.\n");
        exit(1);
    }

    read_costs(argv[1]);
    print_cost_matrix();
    validate_cost_matrix();

	city_tour *t = init_tour(0, 0);	
	print_tour(t);

	city_tour *best_tour = init_tour(100000000, starting_city);
	stack_DFS(best_tour, starting_city);
}

/*
 * 2D arrays are weird in C. Better to treat it as 1D array.
 * Converts a 2D subscript, [i][j], into a 1D subscript
 * Not used but kept for documentation and/or future use.
 */
int convert_2D_subscript(int rows, int r, int c) {
    return rows * r + c;
}

/*
 * Reads in a digraph from a file. 
 */
void read_costs(char *filename) {
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Error in opening file! Terminating......\n");
        exit(1);
    }

    // Need to use C99 for this:/
    for (int i = 0 ; i < num_cities; i++) {
        for (int j = 0; j < num_cities; j++) {
            fscanf(fp, "%d", (digraph + (num_cities * i + j)));
        }
    }

    fclose(fp);
}

/*
 * Verifies the cost matrix/digraph has 0 costs for a city going to itself.
 *
 * Also most cost matrices are symetric. This identies whether or not we are using a
 * symetric matrix. If we're not using a symetric matrix then we tell the user
 * just in case there is a typo.
 *
 */
void validate_cost_matrix() {
    for (int i = 0; i < num_cities; i++) {
        for (int j = 0; j < num_cities; j++) {
            //Make sure we have 0 costs for going to itself
            //We exit if it's not 
            if (i == j && digraph[num_cities * i + j] != 0) {
                printf("Error in cost Matrix! Column %d at row %d is not 0...\n", i, j);
                printf("Terminating...\n");
                exit(1);
            } else if(digraph[num_cities * i + j] != digraph[num_cities * j + i]) {
                printf("Warning!\n");
                printf("Column %d at row %d [%d] does not match column %d at row %d [%d]\n", i, j, digraph[num_cities * i + j], j, i, digraph[num_cities * j + i]);
            }
        }
    }
}

/*
 * Prints out the digraph for our viewing pleasure.
 */
void print_cost_matrix() {
    for (int i = 0; i < num_cities; i++) {
        //Prints the first number correctly
        printf("%d", digraph[num_cities * i]);
        for (int j = 0; j < num_cities; j++) {
            printf(" %d", digraph[num_cities * i + j]);
        }
        printf("\n"); //Adds a new line after every row
    }
}

/*
 * Prints the usage for when the users tries to run the program incorrectly.
 */
void print_usage(int numarguments, char *args[]) {
    printf("To use this, please pass in the filename of the cost matrix and how many cities there are.\n");
    printf("%s, ${COST_MATRIX_FILENAME} ${NUM_CITIES}\n", args[0]);
}

/*
 * Stack based DFS
 */
void stack_DFS(city_tour *best_tour, int starting_city) {
	int city;
	stack *s = NULL;
	city_tour *current_tour = init_tour(0, starting_city);

	s = init_stack(num_cities);
	push(s, num_cities, -1); //Pushing a sentinel to mark the end of the stack
	//We are going to reuse our city variable. 
	for (city  = num_cities - 1; city >= 0; city--) {
		if ( city != starting_city ) {
			push(s, num_cities, city); //Pushing on the stack backwards so we process the stack in order
		}
	}

    printf("The stack before we process cities looks like...\n");
    print_stack(s);

	while(!empty(s)) {
		city = pop(s);
	    printf("Processing city %d\n", city);	
		if (city == -1) { //Sentinel value used to determine end of branch
            printf("Removing city %d because it's a sentinel value.\n", city);
			remove_last_city(current_tour);
            print_tour(current_tour);
		} else if (feasible(current_tour, best_tour, city)) {
            printf("Adding city %d to the current tour\n", city);
			add_city(current_tour, city);
            print_tour(current_tour);

			if(current_tour->count == num_cities) {
                printf("We'v reached the max amount of cities\n");
				//Do we have a better tour?
				//if(best_tour->cost > ( current_tour->cost + cost(current_tour->cities[current_tour->count-1]))) {
				if(feasible(current_tour, best_tour, starting_city)) {
                    printf("We have the best tour!\n");
					copy_tour(current_tour, best_tour);
					add_city(best_tour, starting_city);
                    print_tour(best_tour);
				}
	            printf("Removing last city from tour\n");			
				remove_last_city(current_tour);
                print_tour(current_tour);
			} else {
				//Lets add the rest of the cities to this branch
				push(s, num_cities, -1);
				for(int i = num_cities-1; i >= 0; i--) {
						if ( i != starting_city && !(visited(current_tour, i))) {
							push(s, num_cities, i); //Pushing on the stack backwards so we process the stack in order
						}
				}
                printf("Current stack looks like...\n");
                print_stack(s);
			}
		}
	}

	//Make sure we don't have memory leaks
	destroy_stack(s);
	destroy_tour(current_tour);	
}

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
	for(int i = 0; i < num_cities; i++) {
		printf("%d", tour->cities[i]);

		if( i < num_cities - 1) {
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

/*
 * Method to test other methods to ensure stuff is working correctly.
 */
void test() {
    return;
}
