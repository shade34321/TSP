#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct city
{
    int x, y, weight; //where in the matrix it's located and it's weight which is the information we need the most.
    struct city *next; // So we can chain cities together to print out the path afterwards
} city;

/*
 * Doubly linked list implementation.
 */
typedef struct list {
    city *head;
} list;

list *best_path;
int best_tour = 65535; // set to a high number so we can set it on the first go round.

/*
 * Prints the city info
 */
void print_city(city *c) {
    printf("City \n\tx: %d\n\ty: %d\n\tweight: %d\n", c->x, c->y, c->weight);
}

/*
 * Prints the list of the known good path
 */
void print_list(list *singly){
    city *temp = singly->head;

    if(!temp) {
        printf("list is empty.");
		return;
    }

	
    while(temp) {
        printf("%d", temp->weight);
        temp = temp->next;
		if(temp) {
			printf("-->");
		}
    }

    printf("\n");
}

/*
 * Checks to see if the stack is empty
 * Returns 1 for empty
 * Returns 0 for not empty
 */
int empty(list *singly) {
    if (singly->head) {
        return 0;
    }

    return 1;
}

/*
 * Deletes the list
 *
 */
void destroy(list *singly) {
    city *temp = singly->head;


    while (temp) {
        temp = singly->head->next;
        free(singly->head);
        singly->head = temp;
    }
}

/*
 * Initializes are doubly linked list so we can use it.
 */
void init_list( list *singly) {
    singly->head = NULL;
}

/*
 * Stack push Implementation
 */
void push(list *stack, int temp_x, int temp_y, int temp_weight) {
    city *temp = (city *)malloc(sizeof(city));
    if (!temp) {
        printf("Unable to allocate memory!");
        exit(1);
    }

    temp->x = temp_x;
    temp->y = temp_y;
    temp->weight = temp_weight;
    temp->next = NULL;
    
    // If our list is currently empty
    if(empty(stack)) {
        stack->head = temp;
    } else {
        temp->next = stack->head;
        stack->head = temp;
    }
}

void push_city(list *stack, city *new_city) {
    if (empty(stack)) {
        stack->head = new_city;
		stack->head->next = NULL;
    } else {
        new_city->next = stack->head;
        stack->head = new_city;
    }
}

/*
 * Copies the list from one to another
 * Provides a deep copy of the list
 */
void copy_list(list *singly, list *copy) {
    city *temp, *temp1;
    temp = singly->head;
    
    while(temp){
        temp1 = (city *)malloc(sizeof(city));

        if(!temp1) {
            printf("Unable to allocate memory!");
            exit(1);
        }

        temp1->x = temp->x;
        temp1->y = temp->y;
        temp1->weight = temp->weight;

        push_city(copy, temp1);
        temp = temp->next;
    }
    
}

/*
 * Stack pop implementation
 * Returns the popped value
 */
city * pop(list *stack) {
    if (empty(stack)) {
        printf("We entered a werid state and the stack is empty!");
    }

    city *temp = stack->head;
    stack->head = temp->next;
    return temp;
}

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

    if (fp == NULL) {
        printf("Error in opening file! Terminating.....\n");
        exit(1);
    }

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
 * Verifies the cost matrix that was inputted is in the correct format. 
 * I dont' trust my copy and paste skills since initially there were a lot of
 * formatting issues.
 */
void validate_cost_matrix(int numCities, int *cost) {
    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities;j ++) {
            //Make sure we have 0 cost for going to itself
            if (i == j && cost[numCities * i + j] != 0) {
               printf("Error in cost Matrix! Column %d Row %d is not 0...\n", i, j);
               printf("Terminating....\n");
               exit(1);
            } else if (cost[numCities * i + j] != cost[numCities * j + i]) {
                printf("Warning!\n");
                printf("Column %d Row %d: %d\n", i, j, cost[numCities * i + j]);
                printf("Column %d Row %d: %d\n", j, i, cost[numCities * j + i]);
            }
        }
    }
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

/*
 * Gets the weight of the current path
 * returns -1 if there it is empty.
 */
int get_current_weight(list *path) {
    int weight = -1;
    city *tmp = path->head;

    while(tmp) {
        weight += tmp->weight;
        tmp = tmp->next;
    }

    if(weight >= 0){
        weight+=1;
    }

    return weight;
}

/*
 * Gets the number of cities in the current tour
 */
int num_cities(list *path) {
    int city_count = -1;
    city *tmp = path->head;

    while(tmp) {
        city_count+=1;
        tmp = tmp->next;
    }

    if (city_count >= 0) {
        city_count+=1;
    }

    return city_count;
}

/*
 * Removes a city from the list. 
 * This allows me not to have to worry about freeing the data;
 */
void remove_city(list *stack) {
    city *temp = pop(stack);

    free(temp);
}

/*
 * Checks to see if we can add the city to the stack of cities to visit. This
 * ensures the city isn't already in our stack.
 * Returns 1 if you can add it and 0 if you can't.
 */
int feasible(list *stack, int starting_city, int current_city) {
    city *tmp = stack->head;
    int can_add = 1;

    while(tmp) {
        if (tmp->x == starting_city && tmp->y == current_city) {
            can_add = 0;
        }

        tmp = tmp->next;
    }

    return can_add;
}

void tsp(int starting_city, int numCities, int *cost) {
    list tmp_cities, tmp_path;
    init_list(&tmp_cities);
    init_list(&tmp_path);

    // We know the first and last city in the best path will the starting city
    // so we will add it now.
    push(&tmp_path, starting_city, starting_city, 0);

    // Allows us to start from any city
    for (int i = numCities - 1; i >= 0; i--) {
        // Only add the city that we need.
        if (i != starting_city) {
            push(&tmp_cities, starting_city, i, cost[numCities * starting_city + i]);
        }
    }

    while (!empty(&tmp_cities)) {
        city *tmp = pop(&tmp_cities);

        //If we've gone through all children
        if (tmp->x == -1) {
            printf("Removing sentinel city.\n");
            remove_city(&tmp_path);
        } else {
            push_city(&tmp_path, tmp);
            if (num_cities(&tmp_path) == numCities) {
                if( best_tour > get_current_weight(&tmp_path)) {
                    //We have a new best path
                    printf("Num Cities in tmp_path list: %d\n", num_cities(&tmp_path));
                    print_list(&tmp_path);
                    best_tour = get_current_weight(&tmp_path);
                    printf("Assigned best_tour\n");
                    if(best_path) {
                        destroy(best_path);
                    }
                    printf("Destroyed best_path\n");
                    copy_list(&tmp_path, best_path);
                    printf("Copying list\n");
                }             
                remove_city(&tmp_path);
            } else {
                //Sentinel value so we know when to stop
                push(&tmp_cities, -1, -1, 0);

                for (int i = numCities -1; i >= 0; i--) {
                    if(feasible(&tmp_path, starting_city, i)){
                        push(&tmp_cities, starting_city, i, cost[numCities * starting_city + i]);
                    }
                }
            }
        }
    }

}

void test_methods() {
    validate_cost_matrix(numCities, cost);
    print_cost_matrix(numCities, cost);
    
    printf("best_path should be empty\n");
    if(empty(best_path)) {
        printf("best_path is empty\n");
    } else {
        printf("best_path is not empty\n");
    }
    
    printf("Testing push\n");
    push(best_path, 0,0,1);
    push(best_path, 0,1,2);
    push(best_path, 0,2,3);
    city *temp = (city *)malloc(sizeof(city));
    
    if(!temp) {
        printf("Unable to allocate enough memory!\n");
        //EXIT_FAILURE;
        exit(1);
    }
    
    temp->x = 0;
    temp->y = 3;
    temp->weight = 4;
    
    printf("testing print_city\n");
    print_city(temp);
    
    printf("Testing push_city\n");
    push_city(best_path, temp);
    
    printf("best_path should not be empty\n");
    if(empty(best_path)) {
        printf("best_path is empty\n");
    } else {
        printf("best_path is not empty\n");
    }
    
    print_list(best_path);
    
    printf("There should be 4 cities in the list: %d\n", num_cities(best_path));
    printf("Testing remove city");
    remove_city(best_path);
    print_list(best_path);
    
    printf("Testing if we can add (0,1) into the list: %d\n", feasible(best_path, 0, 1));
    
    list *best_copy;
    best_copy = (list *)malloc(sizeof(list));
    
    if(!best_copy) {
        printf("Unable to allocate memory\n");
        exit(1);
    }
    
    init_list(best_copy);
    
    printf("Testing copy_list\n");
    copy_list(best_path, best_copy);
    print_list(best_path);
    print_list(best_copy);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage(argc, argv);
        //EXIT_FAILURE;
        exit(1);
    }

    //Get total number of cities from command line
    int numCities = atoi(argv[2]);
    int *cost = malloc(sizeof(int) * numCities * numCities);

    if(!cost) {
        printf("Unable to allocate enough memory!\n");
        //EXIT_FAILURE;
        exit(1);
    }

    best_path = (list *)malloc(sizeof(list));

    if(!best_path) {
        printf("Unable to allocate enough memory!\n");
        //EXIT_FAILURE;
        exit(1);
    }

    init_list(best_path);

    read_costs(argv[1], numCities, cost);
    
    
    
    
    tsp(0, numCities, cost);

    print_list(best_path);

    free(cost);
}
