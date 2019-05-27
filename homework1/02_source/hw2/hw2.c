#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define TNUM 3 // �ڽĽ������� ���� ����
#define TRUE 1 // ����� ����
#define FALSE 0 // ����ȵ� ����
int turn = 0; // ȣ��� ������ ���� ī��Ʈ
int status[TNUM] = { FALSE }; // �ʱ⿡�� ��� �����尡 ���� �� �� ����

/* �Լ����� */
void* minimum(void* arg);

/* �������� */
int arr[10]; // �����迭
int part1[2] = { 0, 2 }; // ù��° ����
int part2[2] = { 3, 5 }; // �ι�° ����
int part3[2] = { 6, 9 }; // ����° ����

int main(int argc, char *argv[])
{
	pthread_t id_t[TNUM];
	void* t_return[TNUM]; // �ڽ� �����尡 �����ϴ� ���� �����ϱ� ���� ����
	int fin_result; // ���� ����� ������ ����
	int i; // �����
	int wait_count = 0;

	/* input.txt ���� �б�ó�� */
	FILE *fp = NULL;
	fp = fopen("input.txt", "r");
	for (i = 0; i < 10; i++)
		fscanf(fp, "%d", &arr[i]);

	/* �ڽĽ����� ���� */
	pthread_create(&id_t[0], NULL, minimum, (void*)part1);
	pthread_create(&id_t[1], NULL, minimum, (void*)part2);
	pthread_create(&id_t[2], NULL, minimum, (void*)part3);

	/* �ڽĽ����� �ŵα� */
	pthread_join(id_t[0], &t_return[0]);
	pthread_join(id_t[1], &t_return[1]);
	pthread_join(id_t[2], &t_return[2]);

	do // ���ɶ� ��Ŀ������ ���� 
	{
		if (status[0] && status[1] && status[2]) // �� �ڽ� �������� ������ ��� �Ϸ�Ǿ�߸� ���԰���
		{
			/* the process to get minimum value from threads */
			fin_result = (int)t_return[0]; // �ڽĽ����忡�� �޾ƿ� ������ ������ �ʱ�ȭ
			for (i = 0; i < 3; i++)
				if (fin_result >(int)t_return[i]) // ���� �ʱ�ȭ�� ���� �ڽĽ������� ���ϰ����� ������,
					fin_result = (int)t_return[i]; // fin_result�� ���� �ش� ������ ���ŵ� 
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
	int result; // �� �ڽĽ������� ������� �����ϴ� ����
	int start = ((int*)arg)[0]; // ���� �ε��� ��ȣ
	int end = ((int*)arg)[1]; // ���� �ε��� ��ȣ

	// �����迭�κ��� �ּڰ��� ���ϴ� ó��
	result = arr[start]; // �����迭�� ������ ������ �ʱ�ȭ
	int i; // �����
	for (i = start; i <= end; i++)
	{
		if (result > arr[i])
			result = arr[i];
		printf("tid # %lx : the minimum number is %d\n", pthread_self(), result);
	}
	status[turn] = TRUE; // �ڽĽ������� ó���� �������� �˸�
	turn++; // ������ ��ȣ�� ����

	pthread_exit((void*)result);
}