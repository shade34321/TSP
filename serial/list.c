#include "list.h"

stack * init_stack(int num_cities) {
    stack *s = (stack *)malloc(sizeof(stack));

    if (!s) {
        printf("Unable to allocate enough memory to initialize stack.\n");
        exit(1);
    }
    s->list = (int *)malloc(sizeof(int) * (num_cities * num_cities));

	if (!s->list) {
		printf("Unable to allocate enough memory for city list within the stack.\n");
		exit(1);
	}    

	for (int i = 0; i < (num_cities * num_cities); i++) {
		s->list[i] = -1;
	}

	s->size = 0;

    return s;
}

void print_stack(stack *s) {
	for(int i = 0; i < s->size; i++) {
		printf("%d", s->list[i]);
		
		if( i < s->size - 1) {
			printf(" -> ");
		}
	
		if ( i != 0 && i % 20 == 0) {
			printf("\n");
		}
	}

	printf("\nStack Size: %d\n", s->size);
}

int empty(stack *s) {
	return (s->size == 0 ? 1 : 0);
}
void destroy_stack(stack *s) {
	free(s->list);
	free(s);
}
void push(stack *s, int num_cities, int c) {
	if ( s->size == num_cities * num_cities) {
		printf("Shit done broke! You're trying to add more onto the stack than we allow.\n");
		exit(1);
	}

	s->list[s->size] = c;
	(s->size)++;
}

int pop(stack *s) {
	if (s->size == 0) {
		printf("Stack is empty! We done goofed.\n");
		exit(1);
	}

	int temp = s->list[s->size-1];
	s->list[s->size-1] = -1;
	(s->size)--;

	return temp;
}

