#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define TNUM 3 // 자식스레드의 수를 정의
#define TRUE 1 // 실행된 상태
#define FALSE 0 // 실행안된 상태
int turn = 0; // 호출된 스레드 개수 카운트
int status[TNUM] = { FALSE }; // 초기에는 모든 스레드가 실행 안 된 상태

/* 함수원형 */
void* minimum(void* arg);

/* 전역변수 */
int arr[10]; // 전역배열
int part1[2] = { 0, 2 }; // 첫번째 조각
int part2[2] = { 3, 5 }; // 두번째 조각
int part3[2] = { 6, 9 }; // 세번째 조각

int main(int argc, char *argv[])
{
	pthread_t id_t[TNUM];
	void* t_return[TNUM]; // 자식 스레드가 리턴하는 값을 저장하기 위한 공간
	int fin_result; // 최종 결과를 저장할 공간
	int i; // 제어변수
	int wait_count = 0;

	/* input.txt 파일 읽기처리 */
	FILE *fp = NULL;
	fp = fopen("input.txt", "r");
	for (i = 0; i < 10; i++)
		fscanf(fp, "%d", &arr[i]);

	/* 자식스레드 생성 */
	pthread_create(&id_t[0], NULL, minimum, (void*)part1);
	pthread_create(&id_t[1], NULL, minimum, (void*)part2);
	pthread_create(&id_t[2], NULL, minimum, (void*)part3);

	/* 자식스레드 거두기 */
	pthread_join(id_t[0], &t_return[0]);
	pthread_join(id_t[1], &t_return[1]);
	pthread_join(id_t[2], &t_return[2]);

	do // 스핀락 메커니즘의 구현 
	{
		if (status[0] && status[1] && status[2]) // 세 자식 스레드의 수행이 모두 완료되어야만 진입가능
		{
			/* the process to get minimum value from threads */
			fin_result = (int)t_return[0]; // 자식스레드에서 받아온 임의의 값으로 초기화
			for (i = 0; i < 3; i++)
				if (fin_result >(int)t_return[i]) // 만약 초기화된 값이 자식스레드의 리턴값보다 작으면,
					fin_result = (int)t_return[i]; // fin_result의 값이 해당 값으로 갱신됨 
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

void* minimum(void* arg)
{
	int result; // 각 자식스레드의 결과값을 저장하는 변수
	int start = ((int*)arg)[0]; // 시작 인덱스 번호
	int end = ((int*)arg)[1]; // 종료 인덱스 번호

	// 전역배열로부터 최솟값을 구하는 처리
	result = arr[start]; // 전역배열의 임의의 값으로 초기화
	int i; // 제어변수
	for (i = start; i <= end; i++)
	{
		if (result > arr[i])
			result = arr[i];
		printf("tid # %lx : the minimum number is %d\n", pthread_self(), result);
	}
	status[turn] = TRUE; // 자식스레드의 처리가 끝났음을 알림
	turn++; // 스레드 번호를 갱신

	pthread_exit((void*)result);
}