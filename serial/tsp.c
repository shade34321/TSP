#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Converts a 2D array coordinate to a 1D array coordinate so we can work with
 * 2D arrays.
 * Not used but documented for future use.
 */
int convert_2d_location_to_1d(int rows, int i, int j) {
    return rows * i + j;
}

/*
 * Reads in the cost matrix from a file. 
 */
void read_costs(char *filename, int numCities, int *cost) {
    FILE *fp = fopen(filename, "r");

    // Grabbing the cost matrix from the file.
    // We don't do any error checking We could later on check to ensure the
    // cost matrix is right.
    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            // Apparently C doesn't support passing multi dimension arrays easily?
            fscanf(fp, "%d", &cost[numCities * i + j]);
        }
    }

    fclose(fp);
}

/*
 * Prints out the cost matrix
 */
void print_cost_matrix(int numCities, int *cost) {
    for (int i = 0; i < numCities; i++) {
        printf("%d", cost[numCities * i]);
        for (int j = 1; j < numCities; j++) {
            printf(" %d", cost[numCities * i + j]);
        }
        printf("\n"); //Add a new line after every row.
    }
}

/*
 * Prints out the usage.
 */
void print_usage(int numarguments, char *args[]) {
    printf("To use this please pass in the filename of the cost matrix and how many cities there are.\n");
    printf("%s ${FILENAME} ${NUM_CITIES}\n", args[0]);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage(argc, argv);
        //EXIT_FAILURE;
        return 1;
    }

    //Get total number of cities from command line
    int numCities = atoi(argv[2]);
    int *cost = malloc(sizeof(int) * numCities * numCities);

    if(!cost) {
        printf("Unable to allocate enough memory!\n");
        //EXIT_FAILURE;
        return 1;
    }

    read_costs(argv[1], numCities, cost);
    print_cost_matrix(numCities, cost);

    free(cost);
}
