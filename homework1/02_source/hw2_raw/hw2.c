#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define TNUM 3 // define the number of child threads

/* function prototype */
void* minimum(void* arg);

/* global variable */
int arr[10]; // global array (forms the critical section)
int part1[2] = { 0, 2 }; // the first segment
int part2[2] = { 3, 5 }; // the second segment
int part3[2] = { 6, 9 }; // the third segment

/* busy wating implementation */
#define TRUE 1
#define FALSE 0
int turn = 0; // the order of call
int status[3] = { FALSE }; // intial stats is all opened 

/* main thread */
int main(int argc, char *argv[])
{
	pthread_t id_t[TNUM];
	void* t_return[TNUM];
	int fin_result; // the array where the final result will be saved
	int i; // control variable
	int wait_count = 0;

	/* read data from input.txt file */
	FILE *fp = NULL;
	fp = fopen("input.txt", "r");
	for (i = 0; i < 10; i++)
		fscanf(fp, "%d", &arr[i]);

	/* the creation of child threads */
	pthread_create(&id_t[0], NULL, minimum, (void*)part1);
	pthread_create(&id_t[1], NULL, minimum, (void*)part2);
	pthread_create(&id_t[2], NULL, minimum, (void*)part3);

	/* reaping child threads */
	pthread_join(id_t[0], &t_return[0]);
	pthread_join(id_t[1], &t_return[1]);
	pthread_join(id_t[2], &t_return[2]);

	/* spin lock implementation */
	do 
	{
		if (status[0] && status[1] && status[2])
		{
			/* the process to get minimum value from threads */
			fin_result = (int)t_return[0]; // initialize random number from the thread
			for (i = 0; i < 3; i++)
			{
				if (fin_result >(int)t_return[i]) // if the initialzed value is bigger than thread result,
					fin_result = (int)t_return[i]; // then replaces the fin_result value with it. 
			}
			printf("\nChild thread # %lx's minimum : %d\n", id_t[0], (int)t_return[0]);
			printf("Child thread # %lx's minimum : %d\n", id_t[1], (int)t_return[1]);
			printf("Child thread # %lx's minimum : %d\n", id_t[2], (int)t_return[2]);
			printf("\nParent thread # %lx's minimum : %d\n", pthread_self(), fin_result);
			break;
		} wait_count++;
		printf("waiting count : %d\n", wait_count);
	} while (TRUE);
	
	return EXIT_SUCCESS;
}

/* child thread */
void* minimum(void* arg)
{
	int result; // the variable space where every thread saves the result
	int start = ((int*)arg)[0]; // the start index
	int end = ((int*)arg)[1]; // the end index

	/* the process to get minimum value from the global array */
	result = arr[0]; // initialize the result with the first element of global array
	int i; // control variable
	for (i = start; i <= end; i++)
	{
		if (result > arr[i])
			result = arr[i];
		printf("tid # %lx : the minimum number is %d\n", pthread_self(), result);
	}
	status[turn] = TRUE; // it means it is already completed
	turn++; // increase index number which is used by another threads

	pthread_exit((void*)result);
}