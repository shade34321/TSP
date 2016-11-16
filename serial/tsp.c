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
//    print_cost_matrix();
//    validate_cost_matrix();

    //city_tour *t = init_tour(0, 0);	
    //print_tour(t);
    
    city_tour *best_tour = NULL; 

    printf("Recursive DFS\n");
    best_tour = init_tour(100000000, starting_city);
    city_tour *ct = init_tour(0, starting_city);
    double rec_starttime = get_timestamp();
    rec_DFS(best_tour, ct, starting_city);
    double rec_stoptime = get_timestamp();

    
    printf("Recursive Best Tour: \n");
    print_tour(best_tour);
    destroy_tour(best_tour);
    printf("Recursive time: %f\n", rec_stoptime - rec_starttime);
    
    printf("Iterative 1\n");
    best_tour = init_tour(100000000, starting_city);
    double iter_1_starttime = get_timestamp();
    stack_DFS(best_tour, starting_city);
    double iter_1_endtime = get_timestamp();
    
    printf("Iterative 1 Best Tour: \n");
    print_tour(best_tour);
    printf("Recursive time: %f\n", iter_1_endtime - iter_1_starttime);
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

double get_timestamp() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_usec + (double)now.tv_sec * 1000000;
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

    //printf("The stack before we process cities looks like...\n");
    //print_stack(s);

	while(!empty(s)) {
		city = pop(s);
	        //printf("Processing city %d\n", city);	
		if (city == -1) { //Sentinel value used to determine end of branch
		        //printf("Removing city %d because it's a sentinel value.\n", city);
			remove_last_city(current_tour);
		        //print_tour(current_tour);
		} else if (feasible(current_tour, best_tour, city)) {
		        //printf("Adding city %d to the current tour\n", city);
			add_city(current_tour, city);
		        //print_tour(current_tour);

			if(current_tour->count == num_cities) {
				//printf("We've reached the max amount of cities\n");
				//Do we have a better tour?
				//if(best_tour->cost > ( current_tour->cost + cost(current_tour->cities[current_tour->count-1]))) {
				if(feasible(current_tour, best_tour, starting_city)) {
					//printf("We have the best tour!\n");
					copy_tour(current_tour, best_tour);
					add_city(best_tour, starting_city);
				    	//print_tour(best_tour);

				} else {
					//printf("We don't have the best tour!\n");
				}
			        //printf("Removing last city from tour\n");			
				remove_last_city(current_tour);
				//print_tour(current_tour);
			} else {
				//Lets add the rest of the cities to this branch
				push(s, num_cities, -1);
				for(int i = num_cities-1; i >= 0; i--) {
						if ( i != starting_city && !(visited(current_tour, i))) {
							push(s, num_cities, i); //Pushing on the stack backwards so we process the stack in order
						}
				}
			//printf("Current stack looks like...\n");
			//print_stack(s);
			}
		} else {
			//printf("Current path doesn't look good. Moving on.\n");
		}
	}

	//Make sure we don't have memory leaks
	destroy_stack(s);
	destroy_tour(current_tour);	
}

void rec_DFS(city_tour *best_tour, city_tour *current_tour, int starting_city) {
	int curr_city;

	if(current_tour->count == num_cities) {
		if(feasible(current_tour, best_tour, starting_city)) {
			copy_tour(current_tour, best_tour);
			add_city(best_tour, starting_city); // Add the starting city to the tour
		}
	} else {
		for(curr_city = 0; curr_city < num_cities; curr_city++) {
			//Does it lead to a better path, have we visited it yet? , Are we not the starting city?
			if(feasible(current_tour, best_tour, curr_city) && !visited(current_tour, curr_city) && curr_city != starting_city) {
				add_city(current_tour, curr_city);
				rec_DFS(best_tour, current_tour, starting_city);
				remove_last_city(current_tour);
			}
		}
	}
}
