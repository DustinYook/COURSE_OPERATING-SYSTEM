#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#define PNUM 3 // 프로세스 개수
#define T_SLOT 2 // time_slot의 길이 (문제에서 2로 주어짐)	

struct pcb
{
	int pid; // 프로세스 아이디
	int priority; // 프로세스 우선순위
	int arrival; // 프로세스 도착순서
	int tot_burst; // 프로세스 전체실행시간
	int pcnt; // 프로세스 실행횟수
};

int main(int argc, char* argv[])
{
	/* 변수선언 영역 */
	// 1) 제 1 영역
	struct pcb p[PNUM]; 
	int i, j, k; // 제어변수
	// 2) 제 2 영역
	int id = 0; // 프로세스 제어를 위한 변수
	int runtime = 0; // 프로세스가 실행한 시간을 계산하는 변수
	int tot_time = 0; // 프로세스 전체 실행시간
	int tick; // 스케줄러의 논리적시간을 제어하는 변수	

	/* 파일처리 읽어들이는 영역 */
	int** input = (int*)malloc(3);
	for (i = 0; i < 3; i++)
		input[i] = (int*)malloc(3);
	FILE *fp = fopen("test.txt", "r");
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			fscanf(fp, "%d", &input[i][j]);

	/* 읽어들인 내용울 출력하는 영역 */
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
			printf("%d ", input[i][j]);
		printf("\n");
	}

	/* 읽어들인 내용을 정렬하는 영역 */
	struct pcb p_tmp;
	for (i = 1; i < 3; i++)
	{
		p_tmp.priority = input[i][0];
		p_tmp.arrival = input[i][1];
		p_tmp.tot_burst = input[i][2];
		for (j = i; j > 0; j--) 
		{
			if (input[j - 1][0] > p_tmp.priority)
			{
				for (k = 0; k < 3; k++)
					input[j][k] = input[j - 1][k];
				k = 0;
					
				if (j == 1)
				{
					input[j - 1][0] = p_tmp.priority;
					input[j - 1][1] = p_tmp.arrival;
					input[j - 1][2] = p_tmp.pcnt;
					break;
				}
			}
			else if (input[j - 1][0] <= p_tmp.priority)
			{
				input[j][0] = p_tmp.priority;
				input[j][1] = p_tmp.arrival;
				input[j][2] = p_tmp.tot_burst;
				break;
			}
			else
				printf("ERROR!\n");
		}
	}

	/* 읽어들인 내용울 출력하는 영역 */
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
			printf("%d ", input[i][j]);
		printf("\n");
	}

	/* 읽어온 데이터를 PCB에 할당하는 영역 */
	for (i = 0; i < PNUM; i++)
	{
		p[i].pid = i;
		p[i].priority = input[i][0]; // 프로세스에 우선순위 부여
		p[i].arrival = input[i][1]; // 프로세스에 도착순서 부여
		p[i].tot_burst = input[i][2]; // 프로세스에 전체실행시간 부여		
		tot_time += p[i].tot_burst; // 각 프로세스의 전체실행시간을 총 실행시간에 더해줌
	}

	for (tick = 0; tick < tot_time; tick++)
	{
	LABEL:
		// 1) 잔여 burst_time이 있는 경우 && time_slot이 끝나지 않은 경우
		if (p[id].tot_burst != 0 && runtime != T_SLOT)
		{
			printf("%2d%2d ", p[id].priority, p[id].arrival);
			p[id].tot_burst--;
			runtime++; // 실행한 시간 증가
		}
		// 2) 잔여 burst_time이 없는 경우 || time_slot이 끝난 경우
		else
		{
			// 잔여 burst_time이 있으나 할당 time_slot 종료된 경우
			if (p[id].tot_burst != 0 && runtime == T_SLOT)
				p[id].pcnt++; // 프로세스 실행횟수 증가	
			id++; // 다른 프로세스로 이동

			if (id == PNUM) // 마지막 프로세스까지 실행한 경우 
				id = PNUM - 1; // 바로 이전 프로세스로 이동

			if (p[id].priority != p[id - 1].priority)
				if (p[id - 1].tot_burst != 0 || p[id - 2].tot_burst != 0)
					id = 0; // 원형큐를 이용하여 처음으로 보냄
				
			runtime = 0; // 실행한 시간 초기화
			goto LABEL; // 여기서는 사실 시간이 경과되지 않으므로 무효화 처리를 위해 레이블 이용
		}
	}
	return 0;
}