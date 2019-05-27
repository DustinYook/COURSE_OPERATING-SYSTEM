// Objective : To simulate the steps involved in translating from logical address to physical address
#include <stdio.h>
#include <stdlib.h>

/* �ڷᱸ�� ����*/
int pageTable[256]; // ������ ���̺� -> [(������ �ѹ�) | ������ �ѹ�]
int frameTable[256][2]; // ������ ���̺� -> [(������ �ѹ�) | ��뿩�� | ������ �ѹ�]
int TLB_table[32][2]; // TLB ���ο� ����� ���̺� -> [(�迭 �ε���) | ������ �ѹ� | ������ �ѹ�]
int FIFO_queue[32]; // victim�� �����ϱ� ���� �ڷᱸ�� -> [(�迭 �ε���) | ������ �ѹ�]

/* �������� */
int frameNum = 0; // ������ �ѹ�
int physical_addr = 0; // �����ּ�
int hit; // TLB hit�� ī��Ʈ
int FIFO_count; // FIFO queue�� �� �� ī��Ʈ
int totalCount = 0; // ��ü �о���� Ƚ�� ī��Ʈ
int i, j; // �����

/* �Լ����� */
void paging(int pnum); // ������ ���̺� ���� ��� �޸𸮷� �ø��� ó��
int TLB_lookUp(int pnum); // TLB ���� ���̺��� ã�ƺ��� ó��

int main(void)
{
	/* �����ּ� �������� �б� */
	FILE* flog = fopen("addresses.txt", "r"); // ���ּҸ� �о����

	/* ������� ���� */
	FILE* fphy = fopen("physical_address.txt", "w"); // �����ּҿ� ���� �����ۼ�
	FILE* fframe = fopen("frame_table.txt", "w"); // ������ ���̺� ���� �����ۼ� 
	FILE* ftlb = fopen("final_TLB.txt", "w"); // ���������� TLB ���ο� ����� ���̺� ���� �����ۼ� 

	/* ������ ���̺� �ʱ�ȭ */
	for (i = 0; i < 256; i++)
		pageTable[i] = -1; // Demand Paging�̹Ƿ� ó������ �������

	/* ������ ���̺� �ʱ�ȭ */
	for (i = 0; i < 256; i++)
	{
		frameTable[i][0] = 0; // FREE���� ǥ��
		frameTable[i][1] = 0; // ��������� ǥ��
	}
	
	/* TLB ���� ���̺� �ʱ�ȭ */
	for (i = 0; i < 32; i++)
		for (j = 0; j < 2; j++)
			TLB_table[i][j] = -1; // ��������� ǥ��

	/* victim�� ������ �� �̿��� ť�� �ʱ�ȭ */
	for (i = 0; i < 32; i++)
		FIFO_queue[i] = -1; // ��������� ǥ��

	/* �ּҺ�ȯ */
	while (!feof(flog))
	{
		/* ���Ϸκ��� ���ּ� �б� */
		int logical_addr; // ���ּҸ� ������ ����
		fscanf(flog, "%d\n", &logical_addr); // ���Ϸκ��� ���ּҸ� �Է¹���

		// 1) ���ּҸ� �ɰ��� ������ �ѹ��� �������� ����
		int pageNum = logical_addr / 256; // ���ּ��� ���� 8��Ʈ -> 256���� ���� ��
		int offset = logical_addr % 256; // ���ּ��� ���� 8��Ʈ -> 256���� ���� ������
		paging(pageNum); // ����¡ ó��

		// 2) TLB ���� ���̺��� ã�ƺ��� �Լ� ȣ��
		if (TLB_lookUp(pageNum) == 0)
			paging(pageNum);

		// 3) �����ּҸ� ���������� ���ϴ� ó��
		physical_addr = (frameNum * 256) + offset; // ���� ����� �����ּҸ� ����
		fprintf(fphy, "%d ", physical_addr);

		totalCount++; // ��ü �о���� Ƚ�� ī��Ʈ
	}

	// 4) TLB hit ratio ���ϴ� ó��
	printf("TLB hit ratio : %.2lf\n", (double)hit / totalCount);

	// 5) ������ ���̺��� ���Ͽ� �ۼ��ϴ� ó��
	for (i = 0; i < 256; i++)
		fprintf(fframe, "%3d %3d %3d\r\n", i, frameTable[i][0], frameTable[i][1]);

	// 6) ���� TLB ���� ���̺��� ���Ͽ� �ۼ��ϴ� ó��
	for (i = 0; i < 32; i++)
		fprintf(ftlb, "%3d %3d\r\n", TLB_table[i][0], TLB_table[i][1]);

	return 0;
}

void paging(int pnum)
{
	static int pagingCount = 0; // ����¡�� Ƚ�� ī��Ʈ
	if (pageTable[pnum] == -1) // �ش� ������ �ѹ��� ����ִ� ���
	{
		frameNum = pagingCount;
		pageTable[pnum] = frameNum; // ������ ���̺� ������ �ѹ��� ���
		frameTable[pagingCount][0] = 1; // ������ ���̺��� ��뿩�θ� ��������� ����
		frameTable[pagingCount][1] = pnum; // ������ ���̺� ������ �ѹ� ���
		pagingCount++;
	}
	else
	{
		pagingCount--;
		frameNum = pagingCount;
		pageTable[pnum] = pagingCount;
		frameTable[pagingCount][0] = 1; // ������ ���̺��� ��뿩�θ� ��������� ����
		frameTable[pagingCount][1] = pnum; // ������ ���̺� ������ �ѹ� ���
		pagingCount++;
	}
}

int TLB_lookUp(int pnum)
{
	int flag = 0; // TLB ���� ���̺� �����ϴ��� �Ǵ��ϴ� �÷���

	// TLB ���� ���̺��� ã�ƺ��� ó��
	for (i = 0; i < 32; i++)
	{
		// �������ѹ��� TLB ���� ���̺� �ִ� ���
		if (TLB_table[i][0] == pnum)
		{
			printf("TLB hit\n");
			flag = 1; // �����Ѵٰ� �÷��� ����
			frameNum = TLB_table[i][1]; // ������ �ѹ��� set
			hit++; // TLB hit count ���� 
			break; // Ž������
		}
	}
	if (flag == 0) // TLB ���� ���̺� ���� ���
	{
		printf("TLB miss\n");
		// 1_�� ���� �ִ� ���
		if (FIFO_count < 32)
		{
			TLB_table[FIFO_count][0] = pnum; // TLB ���� ���̺� ������ �ѹ� ����
			TLB_table[FIFO_count][1] = pageTable[pnum]; // TLB ���� ���̺� ������ �ѹ� ����

			FIFO_queue[FIFO_count] = pnum; // FIFO queue�� �������� ������ �ѹ� ���
			FIFO_count++; // enqueue�� �Ǿ����Ƿ� ī��Ʈ ����

			frameNum = TLB_table[FIFO_count][1]; // ������ �ѹ��� set
		}
		// 2_�� ���� ���� ���
		else
		{
			// FIFO queue���� ���� �۾��� ����
			for (i = 0; i < 31; i++)
				FIFO_queue[i] = FIFO_queue[i + 1]; // FIFO queue�� ���Ҹ� �� ĭ�� ������ ���
			FIFO_queue[31] = -1; // ������ ���� ����ִ� ǥ��
			FIFO_count--; // dequeue�� �Ǿ����Ƿ� ī��Ʈ ����

			// TLB ���� ���̺����� ���� �۾��� ����
			for (i = 0; i < 31; i++)
			{
				TLB_table[i][0] = TLB_table[i + 1][0]; // TLB ���� ������ �ѹ��� �� ĭ�� ������ ���
				TLB_table[i][1] = TLB_table[i + 1][1]; // TLB ���� ������ �ѹ��� �� ĭ�� ������ ���
			}

			/* �� ������ ���� �ִ� �۾� ���� */
			TLB_table[31][0] = pnum; // TLB ���� ���̺� ������ �ѹ� ����
			TLB_table[31][1] = pageTable[pnum]; // TLB ���� ���̺� ������ �ѹ� ����
			FIFO_queue[31] = pnum; // FIFO queue�� �������� ������ �ѹ� ���
			FIFO_count++; // enqueue�� �Ǿ����Ƿ� ī��Ʈ ����

			frameNum = TLB_table[31][1]; // ������ �ѹ��� set
		}
	}
	return flag;
}