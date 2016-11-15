#include "list.h"

stack * init_stack(int num_cities, size_t ds, void * iv, print * pf) {
    s->data_size = ds;
    s->init_value = iv;
    s->print = &pf;
    stack *s = (stack *)malloc(sizeof(stack));

    if (!s) {
        printf("Unable to allocate enough memory to initialize stack.\n");
        exit(1);
    }
    s->list = malloc(sizeof(data_size) * (num_cities * num_cities));

	if (!s->list) {
		printf("Unable to allocate enough memory for city list within the stack.\n");
		exit(1);
	}    

	for (int i = 0; i < (num_cities * num_cities); i++) {
        memcpy(&(s->list[i*(s->data_size)]), s->init_value, s->data_size);
	}

	s->size = 0;

    return s;
}

void print_stack(stack *s) {
	for(int i = 0; i < s->size; i+=s->data_size) {
        s->print(i); 
		
		if( i < s->size - 1) {
			printf(" -> ");
		}
	
		if ( i != 0 && i % 20 == 0) {
			printf("\n");
		}
	}

	printf("\nStack Size: %d\n", s->size);
}

void print_int(void * i) {
    printf("%d", *((int *)i)); 
}

int empty(stack *s) {
	return (s->size == 0 ? 1 : 0);
}
void destroy_stack(stack *s) {
	free(s->list);
	free(s);
}
void push(stack *s, int num_cities, void * c) {
	if ( s->size == num_cities * num_cities) {
		printf("Shit done broke! You're trying to add more onto the stack than we allow.\n");
		exit(1);
	}

    memcpy(&(s->list[ (s->size) * (s->data_size) ]), c, s->data_size);
	(s->size)++;
}

void * pop(stack *s) {
	if (s->size == 0) {
		printf("Stack is empty! We done goofed.\n");
		exit(1);
	}

	void * temp = s->list[s->size-1];
    memcpy(&(s->list[ (s->size-1) * (s->data_size) ]), s->init_value, s->data_size);
	(s->size)--;

	return temp;
}
