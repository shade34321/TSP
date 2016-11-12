#include "tsp.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage(argc, argv);
        exit(1);
    }

    //Get toal number of cities from command line
    int num_cities = atoi(argv[2]);
    int *digraph = malloc(sizeof(int) * num_cities * num_cities);

    if (!digraph) {
        printf("Unable to allocate memory for the digraph.\n");
        exit(1);
    }

    read_costs(argv[1], num_cities, digraph);
    print_cost_matrix(num_cities, digraph);
    validate_cost_matrix(num_cities, digraph);
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
void read_costs(char *filename, int num_cities, int *digraph) {
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
void validate_cost_matrix(int num_cities, int *digraph) {
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
void print_cost_matrix(int num_cities, int *digraph) {
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
 * Stack based DFW
 */
void stack_DFS(int starting_city, int numCities, int *digraph) {
    return;
}

/*
 * Method to test other methods to ensure stuff is working correctly.
 */
void test() {
    return;
}
