// Objective : To simulate the steps involved in translating from logical address to physical address
#include <stdio.h>
#include <stdlib.h>

/* 자료구조 선언*/
int pageTable[256]; // 페이지 테이블 -> [(페이지 넘버) | 프레임 넘버]
int frameTable[256][2]; // 프레임 테이블 -> [(프레임 넘버) | 사용여부 | 페이지 넘버]
int TLB_table[32][2]; // TLB 내부에 저장된 테이블 -> [(배열 인덱스) | 페이지 넘버 | 프레임 넘버]
int FIFO_queue[32]; // victim을 결정하기 위한 자료구조 -> [(배열 인덱스) | 페이지 넘버]

/* 변수선언 */
int frameNum = 0; // 프레임 넘버
int physical_addr = 0; // 물리주소
int hit; // TLB hit를 카운트
int FIFO_count; // FIFO queue에 들어간 수 카운트
int totalCount = 0; // 전체 읽어들인 횟수 카운트
int i, j; // 제어변수

/* 함수선언 */
void paging(int pnum); // 페이지 테이블에 없는 경우 메모리로 올리는 처리
int TLB_lookUp(int pnum); // TLB 내부 테이블을 찾아보는 처리

int main(void)
{
	/* 논리적주소 저장파일 읽기 */
	FILE* flog = fopen("addresses.txt", "r"); // 논리주소를 읽어들임

	/* 결과파일 생성 */
	FILE* fphy = fopen("physical_address.txt", "w"); // 물리주소에 대한 파일작성
	FILE* fframe = fopen("frame_table.txt", "w"); // 프레임 테이블에 대한 파일작성 
	FILE* ftlb = fopen("final_TLB.txt", "w"); // 최종적으로 TLB 내부에 저장된 테이블에 대한 파일작성 

	/* 페이지 테이블 초기화 */
	for (i = 0; i < 256; i++)
		pageTable[i] = -1; // Demand Paging이므로 처음에는 비어있음

	/* 프레임 테이블 초기화 */
	for (i = 0; i < 256; i++)
	{
		frameTable[i][0] = 0; // FREE임을 표시
		frameTable[i][1] = 0; // 비어있음을 표시
	}
	
	/* TLB 내부 테이블 초기화 */
	for (i = 0; i < 32; i++)
		for (j = 0; j < 2; j++)
			TLB_table[i][j] = -1; // 비어있음을 표시

	/* victim을 결정할 때 이용할 큐를 초기화 */
	for (i = 0; i < 32; i++)
		FIFO_queue[i] = -1; // 비어있음을 표시

	/* 주소변환 */
	while (!feof(flog))
	{
		/* 파일로부터 논리주소 읽기 */
		int logical_addr; // 논리주소를 저장할 변수
		fscanf(flog, "%d\n", &logical_addr); // 파일로부터 논리주소를 입력받음

		// 1) 논리주소를 쪼개서 페이지 넘버와 오프셋을 구함
		int pageNum = logical_addr / 256; // 논리주소의 앞쪽 8비트 -> 256으로 나눈 몫
		int offset = logical_addr % 256; // 논리주소의 뒤쪽 8비트 -> 256으로 나눈 나머지
		paging(pageNum); // 페이징 처리

		// 2) TLB 내부 테이블을 찾아보는 함수 호출
		if (TLB_lookUp(pageNum) == 0)
			paging(pageNum);

		// 3) 물리주소를 최종적으로 구하는 처리
		physical_addr = (frameNum * 256) + offset; // 실제 저장된 물리주소를 구함
		fprintf(fphy, "%d ", physical_addr);

		totalCount++; // 전체 읽어오는 횟수 카운트
	}

	// 4) TLB hit ratio 구하는 처리
	printf("TLB hit ratio : %.2lf\n", (double)hit / totalCount);

	// 5) 프레임 테이블을 파일에 작성하는 처리
	for (i = 0; i < 256; i++)
		fprintf(fframe, "%3d %3d %3d\r\n", i, frameTable[i][0], frameTable[i][1]);

	// 6) 최종 TLB 내부 테이블을 파일에 작성하는 처리
	for (i = 0; i < 32; i++)
		fprintf(ftlb, "%3d %3d\r\n", TLB_table[i][0], TLB_table[i][1]);

	return 0;
}

void paging(int pnum)
{
	static int pagingCount = 0; // 페이징한 횟수 카운트
	if (pageTable[pnum] == -1) // 해당 페이지 넘버가 비어있는 경우
	{
		frameNum = pagingCount;
		pageTable[pnum] = frameNum; // 페이지 테이블에 프레임 넘버를 등록
		frameTable[pagingCount][0] = 1; // 프레임 테이블의 사용여부를 사용중으로 갱신
		frameTable[pagingCount][1] = pnum; // 프레임 테이블에 페이지 넘버 등록
		pagingCount++;
	}
	else
	{
		pagingCount--;
		frameNum = pagingCount;
		pageTable[pnum] = pagingCount;
		frameTable[pagingCount][0] = 1; // 프레임 테이블의 사용여부를 사용중으로 갱신
		frameTable[pagingCount][1] = pnum; // 프레임 테이블에 페이지 넘버 등록
		pagingCount++;
	}
}

int TLB_lookUp(int pnum)
{
	int flag = 0; // TLB 내부 테이블에 존재하는지 판단하는 플래그

	// TLB 내부 테이블에서 찾아보는 처리
	for (i = 0; i < 32; i++)
	{
		// 페이지넘버가 TLB 내부 테이블에 있는 경우
		if (TLB_table[i][0] == pnum)
		{
			printf("TLB hit\n");
			flag = 1; // 존재한다고 플래그 설정
			frameNum = TLB_table[i][1]; // 프레임 넘버를 set
			hit++; // TLB hit count 증가 
			break; // 탐색종료
		}
	}
	if (flag == 0) // TLB 내부 테이블에 없는 경우
	{
		printf("TLB miss\n");
		// 1_빈 공간 있는 경우
		if (FIFO_count < 32)
		{
			TLB_table[FIFO_count][0] = pnum; // TLB 내부 테이블에 페이지 넘버 저장
			TLB_table[FIFO_count][1] = pageTable[pnum]; // TLB 내부 테이블에 프레임 넘버 저장

			FIFO_queue[FIFO_count] = pnum; // FIFO queue의 마지막에 페이지 넘버 등록
			FIFO_count++; // enqueue가 되었으므로 카운트 증가

			frameNum = TLB_table[FIFO_count][1]; // 프레임 넘버를 set
		}
		// 2_빈 공간 없는 경우
		else
		{
			// FIFO queue에서 삭제 작업을 수행
			for (i = 0; i < 31; i++)
				FIFO_queue[i] = FIFO_queue[i + 1]; // FIFO queue의 원소를 한 칸씩 앞으로 당김
			FIFO_queue[31] = -1; // 마지막 것이 비어있다 표시
			FIFO_count--; // dequeue가 되었으므로 카운트 감소

			// TLB 내부 테이블에서도 삭제 작업을 수행
			for (i = 0; i < 31; i++)
			{
				TLB_table[i][0] = TLB_table[i + 1][0]; // TLB 내부 페이지 넘버를 한 칸씩 앞으로 당김
				TLB_table[i][1] = TLB_table[i + 1][1]; // TLB 내부 프레임 넘버를 한 칸씩 앞으로 당김
			}

			/* 빈 공간에 새로 넣는 작업 수행 */
			TLB_table[31][0] = pnum; // TLB 내부 테이블에 페이지 넘버 저장
			TLB_table[31][1] = pageTable[pnum]; // TLB 내부 테이블에 프레임 넘버 저장
			FIFO_queue[31] = pnum; // FIFO queue의 마지막에 페이지 넘버 등록
			FIFO_count++; // enqueue가 되었으므로 카운트 증가

			frameNum = TLB_table[31][1]; // 프레임 넘버를 set
		}
	}
	return flag;
}