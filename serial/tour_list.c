#include "tour_list.h"

stack * init_stack(int num_cities) {
    stack *s = (stack *)malloc(sizeof(stack));

    if (!s) {
        printf("Unable to allocate enough memory to initialize stack.\n");
        exit(1);
    }
    s->list = (city_tour *)malloc(sizeof(city_tour) * (num_cities * num_cities));

	if (!s->list) {
		printf("Unable to allocate enough memory for city list within the stack.\n");
		exit(1);
	}    

    city_tour *tmp = alloc_tour();
    init_tour(tmp, -1, -1);

	for (int i = 0; i < (num_cities * num_cities); i++) {
		memcpy(&(s->list[i]), tmp, sizeof(city_tour));
	}

	s->size = 0;

    return s;
}

void print_stack(stack *s) {
	printf("\nStack Size: %d\n", s->size);

	for(int i = 0; i < s->size; i++) {
        printf("\t");
	    print_tour(&(s->list[i]));
        printf("\n");
	}

}

int empty(stack *s) {
	return (s->size == 0 ? 1 : 0);
}
void destroy_stack(stack *s) {
    /*
    for(int i = 0; i < s->size; i++) {
        destroy_tour(&(s->list[i]));
    }*/
	free(s->list);
	free(s);
}
void push(stack *s, int num_cities, city_tour *c) {
	if ( s->size == num_cities * num_cities) {
		printf("Shit done broke! You're trying to add more onto the stack than we allow.\n");
		exit(1);
	}

	copy_tour(c, &(s->list[s->size]));
    
	(s->size)++;
}

city_tour * pop(stack *s) {
	if (s->size == 0) {
		printf("Stack is empty! We done goofed.\n");
		exit(1);
	}

    //Take care of it similarly to how it's done with push
	city_tour *tmp = alloc_tour();
    copy_tour( &s->list[s->size-1], tmp); 
	null_tour(&(s->list[s->size-1]));
	(s->size)--;

	return tmp;
}

