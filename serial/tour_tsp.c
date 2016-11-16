#include "tour_tsp.h"

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

    
    city_tour *best_tour = alloc_tour(); 

    printf("Second Iterative \n");
    init_tour(best_tour, 100000000, starting_city);
    double iter_2_starttime = get_timestamp();
    iter_DFS(best_tour, starting_city);
    double iter_2_endtime = get_timestamp();
    
    printf("Iterative 2 Best Tour: \n");
    print_tour(best_tour);
    printf("Second iterative time: %f\n", iter_2_endtime - iter_2_starttime);
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

void iter_DFS(city_tour *best_tour, int starting_city) {
	int city;
	stack *s = NULL;

	city_tour *temp_tour = alloc_tour();
	init_tour(temp_tour, 0, starting_city);
//	city_tour *temp_tour;

	s = init_stack(num_cities);
	push(s, num_cities, temp_tour); //Pushing a tour onto the stack. Here we go! 

	while(!empty(s)) {
		temp_tour = pop(s);

		//Do we hae enough cities in this tour?
		if( temp_tour->count == num_cities) {
			if(feasible(temp_tour, best_tour, starting_city)) {
				copy_tour(temp_tour, best_tour);
				add_city(best_tour, starting_city);
			}
		} else {
			for (city = num_cities-1; city >= 0; city--) {
				if( feasible(temp_tour, best_tour, city) && !visited(temp_tour, city) && city != starting_city ) {
					add_city(temp_tour, city);
					push(s, num_cities, temp_tour);
					remove_last_city(temp_tour);
				}
			}
		}
	}

	//Make sure we don't have memory leaks
	destroy_stack(s);
	destroy_tour(temp_tour);	
}
