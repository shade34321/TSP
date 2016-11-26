#include "tsp_mpi.h"


int main(int argc, char* argv[]) {
   FILE* digraph_file;
   double start, finish;
   int local_ok = 1, one_msg_sz;
   char usage[MAX_STRING];
   char* ret_buf;

   MPI_Init(&argc, &argv);
   comm = MPI_COMM_WORLD;
   MPI_Comm_size(comm, &comm_sz);
   MPI_Comm_rank(comm, &my_rank);
   sprintf(usage, "usage: mpiexec -n <procs> %s <digraph file>\n",
         argv[0]);

   if (my_rank == 0 && argc != 2) local_ok = 0;
   Check_for_error(local_ok, usage, comm);
   if (my_rank == 0) {
      digraph_file = fopen(argv[1], "r");
      if (digraph_file == NULL) local_ok = 0;
   }
   Check_for_error(local_ok, "Can't open digraph file", comm);
   Read_digraph(digraph_file);
   if (my_rank == 0) fclose(digraph_file);

   loc_best_tour = Alloc_tour(NULL);
   Init_tour(loc_best_tour, 1000000);
   best_tour_cost = 1000000;

   MPI_Type_contiguous(n+1, MPI_INT, &tour_arr_mpi_t);
   MPI_Type_commit(&tour_arr_mpi_t);

   MPI_Pack_size(1, MPI_INT, comm, &one_msg_sz);
   mpi_buffer = 
      malloc(100*comm_sz*(one_msg_sz + MPI_BSEND_OVERHEAD)*sizeof(char));
   MPI_Buffer_attach(mpi_buffer,
         100*comm_sz*(one_msg_sz + MPI_BSEND_OVERHEAD));

   start = MPI_Wtime();
   Par_tree_search();
   finish = MPI_Wtime();
   Cleanup_msg_queue();
   MPI_Barrier(comm);
   MPI_Buffer_detach(&ret_buf, &one_msg_sz);
   
   if (my_rank == 0) {
      Print_tour(loc_best_tour, "Best tour");
      printf("Cost = %d\n", loc_best_tour->cost);
      printf("Elapsed time = %e seconds\n", finish-start);
   }

   MPI_Type_free(&tour_arr_mpi_t);
   free(loc_best_tour->cities);
   free(loc_best_tour);
   free(digraph);

   MPI_Finalize();
   return 0;
}

void print_usage(int numarguments, char *args[]) {
    printf("To use this, please pass in the filename of the cost matrix and how many cities therea are.\n");
    printf("%s, ${COST_MATRIX_FILENAME} ${NUM_CITIES}\n", args[0]);
}

void Init_tour(tour_t tour, cost_t cost) {
   int i;

   tour->cities[0] = 0;
   for (i = 1; i <= n; i++) {
      tour->cities[i] = -1;
   }
   tour->cost = cost;
   tour->count = 1;
}


void Read_digraph(FILE* digraph_file) {
   int i, j, local_ok = 1;

   if (my_rank == 0) fscanf(digraph_file, "%d", &n);
   MPI_Bcast(&n, 1, MPI_INT, 0, comm);
   if (n <= 0) local_ok = 0;
   Check_for_error(local_ok, "Number of vertices must be positive", comm);

   digraph = malloc(n*n*sizeof(cost_t));

   if (my_rank == 0) {
      for (i = 0; i < n; i++)
         for (j = 0; j < n; j++) {
            fscanf(digraph_file, "%d", &digraph[i*n + j]);
            if (i == j && digraph[i*n + j] != 0) {
               fprintf(stderr, "Diagonal entries must be zero\n");
               local_ok = 0;;
            } else if (i != j && digraph[i*n + j] <= 0) {
               fprintf(stderr, "Off-diagonal entries must be positive\n");
               fprintf(stderr, "diagraph[%d,%d] = %d\n", i, j, digraph[i*n+j]);
               local_ok = 0;
            }
         }
   }
   Check_for_error(local_ok, "Error in digraph file", comm);
   MPI_Bcast(digraph, n*n, MPI_INT, 0, comm);
}


void Print_digraph(void) {
   int i, j;

   printf("Order = %d\n", n);
   printf("Matrix = \n");
   for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++)
         printf("%2d ", digraph[i*n+j]);
      printf("\n");
   }
   printf("\n");
}

void Par_tree_search(void) {
   city_t nbr;
   my_stack_t stack;  // Stack for searching
   my_stack_t avail;  // Stack for unused tours
   tour_t curr_tour;

   avail = Init_stack();
   stack = Init_stack();
   Partition_tree(stack);

   while (!Empty_stack(stack)) {
      curr_tour = Pop(stack);
      if (City_count(curr_tour) == n) {
         if (Best_tour(curr_tour)) {
            Update_best_tour(curr_tour);
         }
      } else {
         for (nbr = n-1; nbr >= 1; nbr--) 
            if (Feasible(curr_tour, nbr)) {
               Add_city(curr_tour, nbr);
               Push_copy(stack, curr_tour, avail);
               Remove_last_city(curr_tour);
            }
      }
      Free_tour(curr_tour, avail);
   }
   Free_stack(stack);
   Free_stack(avail);
   MPI_Barrier(comm);
   Get_global_best_tour();

}

void Get_global_best_tour(void) {
   struct {
      int cost;
      int rank;
   } loc_data, global_data;
   loc_data.cost = Tour_cost(loc_best_tour);
   loc_data.rank = my_rank;

   /* Both 0 and the owner of the best tour need global_data */
   MPI_Allreduce(&loc_data, &global_data, 1, MPI_2INT, MPI_MINLOC, comm);
   if (global_data.rank == 0) return;
   if (my_rank == 0) {
      MPI_Recv(loc_best_tour->cities, n+1, MPI_INT, global_data.rank,
            0, comm, MPI_STATUS_IGNORE);
      loc_best_tour->cost = global_data.cost;
      loc_best_tour->count = n+1;
   } else if (my_rank == global_data.rank) {
      MPI_Send(loc_best_tour->cities, n+1, MPI_INT, 0, 0, comm);
   } 
}

void Partition_tree(my_stack_t stack) {
   int my_count, local_ok = 1;
   int queue_size, init_tour_count;
   city_t *queue_list = NULL;
   city_t *tour_list;
   int counts[comm_sz];  /* For scatter */
   int displacements[comm_sz];  /* For scatter */

   if (my_rank == 0) {
      queue_size = Get_upper_bd_queue_sz();
      if (queue_size == 0) local_ok = 0;
   }
   Check_for_error(local_ok, "Too many processes", comm);

   if (my_rank == 0) 
      Build_initial_queue(&queue_list, queue_size, &init_tour_count);
   MPI_Bcast(&init_tour_count, 1, MPI_INT, 0, comm);

   Set_init_tours(init_tour_count, counts, displacements, 
         &my_count, &tour_list);

   MPI_Scatterv(queue_list, counts, displacements, tour_arr_mpi_t,
         tour_list, my_count, tour_arr_mpi_t, 0, comm);
   
   Build_init_stack(stack, tour_list, my_count);

   if (my_rank == 0) free(queue_list);
   free(tour_list);

}

void Build_init_stack(my_stack_t stack, city_t tour_list[], int my_count) {
   int i;
   tour_t tour = Alloc_tour(NULL);

   for (i = my_count-1; i >= 0; i--) {
      Create_tour_fr_list(tour_list + i*(n+1), tour);
      Push_copy(stack, tour, NULL);
   }
   Free_tour(tour, NULL);
}

void Create_tour_fr_list(city_t list[], tour_t tour) {
   int count = 1, cost = 0;
   city_t city1, city2;

   memcpy(tour->cities, list, (n+1)*sizeof(city_t));

   city1 = 0;
   while (count <= n && list[count] != -1) {
      city2 = list[count];
      count++;
      cost += Cost(city1, city2);
      city1 = city2;
   }
   tour->count = count;
   tour->cost = cost;
}

void Set_init_tours(int init_tour_count, int counts[], int displacements[],
      int* my_count_p, city_t** tour_list_p) {
   int quotient, remainder, i;

   quotient = init_tour_count/comm_sz;
   remainder = init_tour_count % comm_sz;
   for (i = 0; i < remainder; i++) 
      counts[i] = quotient+1;
   for (i = remainder; i  < comm_sz; i++)
      counts[i] = quotient;
   *my_count_p = counts[my_rank];
   displacements[0] = 0;
   for (i = 1; i < comm_sz; i++)
      displacements[i] = displacements[i-1] + counts[i-1];

   *tour_list_p = malloc((*my_count_p)*(n+1)*sizeof(int));
}


void Build_initial_queue(city_t** queue_list_p, int queue_size, 
      int* init_tour_count_p) {
   my_queue_t queue;
   int curr_sz = 0, i;
   city_t nbr;
   tour_t tour = Alloc_tour(NULL);
   city_t* queue_list;

   Init_tour(tour, 0);
   queue = Init_queue(2*queue_size);

   /* Breadth-first search */
   Enqueue(queue, tour);  // Enqueues a copy
   Free_tour(tour, NULL);
   curr_sz++;
   while (curr_sz < comm_sz) {
      tour = Dequeue(queue);
      curr_sz--;
      for (nbr = 1; nbr < n; nbr++)
         if (!Visited(tour, nbr)) {
            Add_city(tour, nbr);
            Enqueue(queue, tour);
            curr_sz++;
            Remove_last_city(tour);
         }
      Free_tour(tour, NULL);
   }  

   *init_tour_count_p = curr_sz; 


   /* Copy the city lists from queue into queue_list */
   queue_list = malloc((*init_tour_count_p)*(n+1)*sizeof(int));
   for (i = 0; i < *init_tour_count_p; i++)
      memcpy(queue_list + i*(n+1), Queue_elt(queue,i)->cities, 
            (n+1)*sizeof(int));
   *queue_list_p = queue_list;
   Free_queue(queue);
}

int Best_tour(tour_t tour) {
   cost_t cost_so_far = Tour_cost(tour);
   city_t last_city = Last_city(tour);

   Look_for_best_tours();

   if (cost_so_far + Cost(last_city, home_town) < best_tour_cost)
      return 1;
   else
      return 0;
}

void Look_for_best_tours(void) {
   int done = 0, msg_avail, tour_cost;
   MPI_Status status;

   while(!done) {
      MPI_Iprobe(MPI_ANY_SOURCE, TOUR_TAG, comm, &msg_avail, 
            &status);
      if (msg_avail) {
         MPI_Recv(&tour_cost, 1, MPI_INT, status.MPI_SOURCE, TOUR_TAG,
               comm, MPI_STATUS_IGNORE);
         if (tour_cost < best_tour_cost) best_tour_cost = tour_cost;
      } else {
         done = 1;
      }
   }
}


void Update_best_tour(tour_t tour) {
   Copy_tour(tour, loc_best_tour);
   Add_city(loc_best_tour, home_town);
   best_tour_cost = Tour_cost(loc_best_tour);
   Bcast_tour_cost(best_tour_cost);
}

void Bcast_tour_cost(int tour_cost) {
   int dest;

   for (dest = 0; dest < comm_sz; dest++)
      if (dest != my_rank)
         MPI_Bsend(&tour_cost, 1, MPI_INT, dest, TOUR_TAG, comm);
}


void Copy_tour(tour_t tour1, tour_t tour2) {

   memcpy(tour2->cities, tour1->cities, (n+1)*sizeof(city_t));
   tour2->count = tour1->count;
   tour2->cost = tour1->cost;
}

void Add_city(tour_t tour, city_t new_city) {
   city_t old_last_city = Last_city(tour);
   tour->cities[tour->count] = new_city;
   (tour->count)++;
   tour->cost += Cost(old_last_city,new_city);
}

void Remove_last_city(tour_t tour) {
   city_t old_last_city = Last_city(tour);
   city_t new_last_city;
   
   tour->cities[tour->count-1] = -1;
   (tour->count)--;
   new_last_city = Last_city(tour);
   tour->cost -= Cost(new_last_city,old_last_city);
}

int Feasible(tour_t tour, city_t city) {
   city_t last_city = Last_city(tour);

   if (!Visited(tour, city) && 
        Tour_cost(tour) + Cost(last_city,city) < best_tour_cost)
      return 1;
   else
      return 0;
}


int Visited(tour_t tour, city_t city) {
   int i;

   for (i = 0; i < City_count(tour); i++)
      if ( Tour_city(tour,i) == city ) return 1;
   return 0;
}


void Print_tour(tour_t tour, char* title) {
   int i;
   char string[MAX_STRING];

   if (my_rank >= 0)
      sprintf(string, "Proc %d > %s %p: ", my_rank, title, tour);
   else
      sprintf(string, "%s: ", title);
   for (i = 0; i < City_count(tour); i++)
      sprintf(string + strlen(string), "%d ", Tour_city(tour,i));
   printf("%s\n\n", string);
}

tour_t Alloc_tour(my_stack_t avail) {
   tour_t tmp;

   if (avail == NULL || Empty_stack(avail)) {
      tmp = malloc(sizeof(tour_struct));
      tmp->cities = malloc((n+1)*sizeof(city_t));
      return tmp;
   } else {
      return Pop(avail);
   }
}

void Free_tour(tour_t tour, my_stack_t avail) {
   if (avail == NULL) {
      free(tour->cities);
      free(tour);
   } else {
      Push(avail, tour);
   }
}

my_stack_t Init_stack(void) {
   int i;

   my_stack_t stack = malloc(sizeof(stack_struct));
   stack->list = malloc(n*n*sizeof(tour_t));
   for (i = 0; i < n*n; i++)
      stack->list[i] = NULL;
   stack->list_sz = 0;
   stack->list_alloc = n*n;

   return stack;
}


void Push(my_stack_t stack, tour_t tour) {
   if (stack->list_sz == stack->list_alloc) {
      fprintf(stderr, "Stack overflow in Push!\n");
      free(tour->cities);
      free(tour);
   } else {
      stack->list[stack->list_sz] = tour;
      (stack->list_sz)++;
   }
}

void Push_copy(my_stack_t stack, tour_t tour, my_stack_t avail) {
   tour_t tmp;

   if (stack->list_sz == stack->list_alloc) {
      fprintf(stderr, "Stack overflow!\n");
      exit(-1);
   }
   tmp = Alloc_tour(avail);
   Copy_tour(tour, tmp);
   stack->list[stack->list_sz] = tmp;
   (stack->list_sz)++;
}


tour_t Pop(my_stack_t stack) {
   tour_t tmp;

   if (stack->list_sz == 0) {
      fprintf(stderr, "Trying to pop empty stack!\n");
      exit(-1);
   }
   tmp = stack->list[stack->list_sz-1];
   stack->list[stack->list_sz-1] = NULL;
   (stack->list_sz)--;
   return tmp;
}


int  Empty_stack(my_stack_t stack) {
   if (stack->list_sz == 0)
      return 1;
   else
      return 0;
}


void Free_stack(my_stack_t stack) {
   int i;

   for (i = 0; i < stack->list_sz; i++) {
      free(stack->list[i]->cities);
      free(stack->list[i]);
   }
   free(stack->list);
   free(stack);
}

void Print_stack(my_stack_t stack, char title[]) {
   char string[MAX_STRING];
   int i, j;

   printf("Proc %d > %s\n", my_rank, title);
   for (i = 0; i < stack->list_sz; i++) {
      sprintf(string, "Proc %d > ", my_rank);
      for (j = 0; j < stack->list[i]->count; j++)
         sprintf(string + strlen(string), "%d ", stack->list[i]->cities[j]);
      printf("%s\n", string);
   }

}

my_queue_t Init_queue(int size) {
   my_queue_t new_queue = malloc(sizeof(queue_struct));
   new_queue->list = malloc(size*sizeof(tour_t));
   new_queue->list_alloc = size;
   new_queue->head = new_queue->tail = new_queue->full = 0;

   return new_queue;
}


tour_t Dequeue(my_queue_t queue) {
   tour_t tmp;

   if (Empty_queue(queue)) {
      fprintf(stderr, "Attempting to dequeue from empty queue\n");
      exit(-1);
   }
   tmp = queue->list[queue->head];
   queue->head = (queue->head + 1) % queue->list_alloc;
   return tmp;
}

void Enqueue(my_queue_t queue, tour_t tour) {
   tour_t tmp;

   if (queue->full == 1) {
      fprintf(stderr, "Attempting to enqueue a full queue\n");
      fprintf(stderr, "list_alloc = %d, head = %d, tail = %d\n",
            queue->list_alloc, queue->head, queue->tail);
      exit(-1);
   }
   tmp = Alloc_tour(NULL);
   Copy_tour(tour, tmp);
   queue->list[queue->tail] = tmp;
   queue->tail = (queue->tail + 1) % queue->list_alloc; 
   if (queue->tail == queue->head)
      queue->full = 1;

}

int Empty_queue(my_queue_t queue) {
   if (queue->full == 1)
      return 0;
   else if (queue->head != queue->tail)
      return 0;
   else
      return 1;
}

void Free_queue(my_queue_t queue) {
   free(queue->list);
   free(queue);
}

void Print_queue(my_queue_t queue, char title[]) {
   char string[MAX_STRING];
   int i, j;

   printf("Proc %d > %s\n", my_rank, title);
   for (i = queue->head; i != queue->tail; i = (i+1) % queue->list_alloc) {
      sprintf(string, "Proc %d > %p = ", my_rank, queue->list[i]);
      for (j = 0; j < queue->list[i]->count; j++)
         sprintf(string + strlen(string), "%d ", queue->list[i]->cities[j]);
      printf("%s\n", string);
   }

}

int Get_upper_bd_queue_sz(void) {
   int fact = n-1;
   int size = n-1;

   while (size < comm_sz) {
      fact++;
      size *= fact;
   }

   if (size > Fact(n-1)) {
      fprintf(stderr, "You really shouldn't use so many threads for");
      fprintf(stderr, "such a small problem\n");
      size = 0;
   }
   return size;
}

long long Fact(int k) {
   long long tmp = 1;
   int i;

   for (i = 2; i <= k; i++)
      tmp *= i;
   return tmp;
}


void Cleanup_msg_queue(void) {
   int msg_recd;
   MPI_Status status;
   char string1[MAX_STRING];
   int counts[2] = {0,0};
   char work_buf[100000];

   MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &msg_recd, &status);
   while (msg_recd) {
      /* Just receive the message . . . */
      MPI_Recv(work_buf, 100000, MPI_BYTE, status.MPI_SOURCE, 
            status.MPI_TAG, comm, MPI_STATUS_IGNORE);
      if (status.MPI_TAG == TOUR_TAG)
         counts[1]++;
      else  // Unknown
         counts[0]++;
      MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &msg_recd, &status);
   }
   sprintf(string1, "Messages not received:  unknown = %d, tour = %d", 
         counts[0], counts[1]);
}

void Check_for_error( int local_ok, char message[], MPI_Comm  comm) {
   int ok;

   MPI_Allreduce(&local_ok, &ok, 1, MPI_INT, MPI_MIN, comm);
   if (ok == 0) {
      int my_rank;
      MPI_Comm_rank(comm, &my_rank);
      if (my_rank == 0) {
         fprintf(stderr, "Proc %d > %s\n", my_rank, message);
         fflush(stderr);
      }
      MPI_Finalize();
      exit(-1);
   }
}
