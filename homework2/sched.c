#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#define PNUM 3 // ���μ��� ����
#define T_SLOT 2 // time_slot�� ���� (�������� 2�� �־���)	

struct pcb
{
	int pid; // ���μ��� ���̵�
	int priority; // ���μ��� �켱����
	int arrival; // ���μ��� ��������
	int tot_burst; // ���μ��� ��ü����ð�
	int pcnt; // ���μ��� ����Ƚ��
};

int main(int argc, char* argv[])
{
	/* �������� ���� */
	// 1) �� 1 ����
	struct pcb p[PNUM]; 
	int i, j, k; // �����
	// 2) �� 2 ����
	int id = 0; // ���μ��� ��� ���� ����
	int runtime = 0; // ���μ����� ������ �ð��� ����ϴ� ����
	int tot_time = 0; // ���μ��� ��ü ����ð�
	int tick; // �����ٷ��� �����ð��� �����ϴ� ����	

	/* ����ó�� �о���̴� ���� */
	int** input = (int*)malloc(3);
	for (i = 0; i < 3; i++)
		input[i] = (int*)malloc(3);
	FILE *fp = fopen("test.txt", "r");
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			fscanf(fp, "%d", &input[i][j]);

	/* �о���� ����� ����ϴ� ���� */
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
			printf("%d ", input[i][j]);
		printf("\n");
	}

	/* �о���� ������ �����ϴ� ���� */
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

	/* �о���� ����� ����ϴ� ���� */
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
			printf("%d ", input[i][j]);
		printf("\n");
	}

	/* �о�� �����͸� PCB�� �Ҵ��ϴ� ���� */
	for (i = 0; i < PNUM; i++)
	{
		p[i].pid = i;
		p[i].priority = input[i][0]; // ���μ����� �켱���� �ο�
		p[i].arrival = input[i][1]; // ���μ����� �������� �ο�
		p[i].tot_burst = input[i][2]; // ���μ����� ��ü����ð� �ο�		
		tot_time += p[i].tot_burst; // �� ���μ����� ��ü����ð��� �� ����ð��� ������
	}

	for (tick = 0; tick < tot_time; tick++)
	{
	LABEL:
		// 1) �ܿ� burst_time�� �ִ� ��� && time_slot�� ������ ���� ���
		if (p[id].tot_burst != 0 && runtime != T_SLOT)
		{
			printf("%2d%2d ", p[id].priority, p[id].arrival);
			p[id].tot_burst--;
			runtime++; // ������ �ð� ����
		}
		// 2) �ܿ� burst_time�� ���� ��� || time_slot�� ���� ���
		else
		{
			// �ܿ� burst_time�� ������ �Ҵ� time_slot ����� ���
			if (p[id].tot_burst != 0 && runtime == T_SLOT)
				p[id].pcnt++; // ���μ��� ����Ƚ�� ����	
			id++; // �ٸ� ���μ����� �̵�

			if (id == PNUM) // ������ ���μ������� ������ ��� 
				id = PNUM - 1; // �ٷ� ���� ���μ����� �̵�

			if (p[id].priority != p[id - 1].priority)
				if (p[id - 1].tot_burst != 0 || p[id - 2].tot_burst != 0)
					id = 0; // ����ť�� �̿��Ͽ� ó������ ����
				
			runtime = 0; // ������ �ð� �ʱ�ȭ
			goto LABEL; // ���⼭�� ��� �ð��� ������� �����Ƿ� ��ȿȭ ó���� ���� ���̺� �̿�
		}
	}
	return 0;
}