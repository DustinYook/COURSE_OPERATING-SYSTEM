#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define TNUM 9
#define GSIZE 3
void* maximum(void* arg);
int** share;

typedef struct{int** in;} buf;
typedef struct{int** m;} min;
typedef struct{int** e0;} slice0;
typedef struct{int** e1;} slice1;
typedef struct{int** e2;} slice2;
typedef struct{int** e3;} slice3;
typedef struct{int** e4;} slice4;
typedef struct{int** e5;} slice5;
typedef struct{int** e6;} slice6;
typedef struct{int** e7;} slice7;
typedef struct{int** e8;} slice8;
typedef struct{int** element;}slice;

int main(int argc, char* argv[])
{
	pthread_t id_t[TNUM]; // 자식 스레드의 ID
	void* t_return[TNUM]; // 자식 스레드로부터 결과를 리턴받아 저장하는 변수
	int fin_result; // 최종 결과를 저장할 변수
	int i, j; // 제어변수

	/* 파일 읽어들이는 처리 */
	FILE *fp = NULL;
	fp = fopen("input.txt", "r");
	int** input = (int **)malloc(sizeof(int *) * TNUM);
	for (i = 0; i < TNUM; i++)
		input[i] = (int *)malloc(sizeof(int) * TNUM);
	share = (int **)malloc(sizeof(int *) * TNUM);
	for (i = 0; i < TNUM; i++)
		share[i] = (int *)malloc(sizeof(int) * TNUM);
	for (i = 0; i < TNUM; i++)
	{
		for (j = 0; j < TNUM; j++)
		{
			fscanf(fp, "%d ", &input[i][j]);
			share[i][j] = 0;
		}
	}
	buf* bf = (buf*)malloc(sizeof(buf));
	bf->in = input;

	/* 자식 스레드에 인자로 전달할 Grid 공간 생성 */
	slice0* s0 = (slice0*)malloc(sizeof(slice0));
	slice1* s1 = (slice1*)malloc(sizeof(slice1));
	slice2* s2 = (slice2*)malloc(sizeof(slice2));
	slice3* s3 = (slice3*)malloc(sizeof(slice3));
	slice4* s4 = (slice4*)malloc(sizeof(slice4));
	slice5* s5 = (slice5*)malloc(sizeof(slice5));
	slice6* s6 = (slice6*)malloc(sizeof(slice6));
	slice7* s7 = (slice7*)malloc(sizeof(slice7));
	slice8* s8 = (slice8*)malloc(sizeof(slice8));
	int** a0 = (int **)malloc(sizeof(int *) * GSIZE);
	int** a1 = (int **)malloc(sizeof(int *) * GSIZE);
	int** a2 = (int **)malloc(sizeof(int *) * GSIZE);
	int** a3 = (int **)malloc(sizeof(int *) * GSIZE);
	int** a4 = (int **)malloc(sizeof(int *) * GSIZE);
	int** a5 = (int **)malloc(sizeof(int *) * GSIZE);
	int** a6 = (int **)malloc(sizeof(int *) * GSIZE);
	int** a7 = (int **)malloc(sizeof(int *) * GSIZE);
	int** a8 = (int **)malloc(sizeof(int *) * GSIZE);
	for (i = 0; i < GSIZE; i++)
	{
		a0[i] = (int *)malloc(sizeof(int) * GSIZE);
		a1[i] = (int *)malloc(sizeof(int) * GSIZE);
		a2[i] = (int *)malloc(sizeof(int) * GSIZE);
		a3[i] = (int *)malloc(sizeof(int) * GSIZE);
		a4[i] = (int *)malloc(sizeof(int) * GSIZE);
		a5[i] = (int *)malloc(sizeof(int) * GSIZE);
		a6[i] = (int *)malloc(sizeof(int) * GSIZE);
		a7[i] = (int *)malloc(sizeof(int) * GSIZE);
		a8[i] = (int *)malloc(sizeof(int) * GSIZE);
	}
	s0->e0 = a0;
	s1->e1 = a1;
	s2->e2 = a2;
	s3->e3 = a3;
	s4->e4 = a4;
	s5->e5 = a5;
	s6->e6 = a6;
	s7->e7 = a7;
	s8->e8 = a8;

	/* 읽어온 자료를 각 스레드에게 Grid로 할당하기 위해 slicing하는 작업 */
	for (i = 0; i < GSIZE; i++)
	{
		for (j = 0; j < GSIZE; j++)
		{
			a0[i][j] = input[i][j];
			a1[i][j] = input[i][j + 3];
			a2[i][j] = input[i][j + 6];
			a3[i][j] = input[i + 3][j];
			a4[i][j] = input[i + 3][j + 3];
			a5[i][j] = input[i + 3][j + 6];
			a6[i][j] = input[i + 6][j];
			a7[i][j] = input[i + 6][j + 3];
			a8[i][j] = input[i + 6][j + 6];
		}
	}
	
	/* 데이터를 제대로 읽어 들여왔는지 출력 */
	printf("These are the data from input.txt : \n\n");
	for (i = 0; i < TNUM; i++)
	{
		for (j = 0; j < TNUM; j++)
			printf("%3d ", input[i][j]);
		printf("\n");
	} printf("\n");

	/* 자식 스레드의 생성 */
	pthread_create(&id_t[0], NULL, maximum, (void *)s0);
	pthread_create(&id_t[1], NULL, maximum, (void *)s1);
	pthread_create(&id_t[2], NULL, maximum, (void *)s2);
	pthread_create(&id_t[3], NULL, maximum, (void *)s3);
	pthread_create(&id_t[4], NULL, maximum, (void *)s4);
	pthread_create(&id_t[5], NULL, maximum, (void *)s5);
	pthread_create(&id_t[6], NULL, maximum, (void *)s6);
	pthread_create(&id_t[7], NULL, maximum, (void *)s7);
	pthread_create(&id_t[8], NULL, maximum, (void *)s8);

	/* 자식스레드를 reap하여 최종 selection을 하는 처리 */
	min* mini = (min*)malloc(sizeof(min));
	mini->m = share;
	for (i = 0; i < 10; i++)
		pthread_join(id_t[i], &t_return[i]); // 자식 스레드로부터 reaping
	fin_result = (int)t_return[0];
	for (i = 0; i < TNUM; i++)
	{
		if (fin_result < t_return[i])
			fin_result = t_return[i];
	} printf("\nSelection thread # %lx's maximum : %d\n", pthread_self(), fin_result);

	return EXIT_SUCCESS;
}

void* maximum(void *arg)
{
	int result; // 스레드별 결과를 저장하는 변수
	int i, j; // 제어변수
	int tn = 0; // 스레드의 번호
	slice sl = *(slice*)arg;
	for (i = 0; i < GSIZE; i++)
		for (j = 0; j < GSIZE; j++)
			if (result < sl.element[i][j])
				result = sl.element[i][j];
	share[0][tn++] = result;
	printf("Child thread # %lx's maximum : %d\n", pthread_self(), result);
	pthread_exit((void*)result);
}