// 
// 과제3의 채점 프로그램은 기본적으로 아래와 같이 동작함
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "blockmap.h"

FILE *flashfp;

/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);

//
// 이 함수는 file system의 역할을 수행한다고 생각하면 되고,
// file system이 flash memory로부터 512B씩 데이터를 저장하거나 데이터를 읽어 오기 위해서는
// 각자 구현한 FTL의 ftl_write()와 ftl_read()를 호출하면 됨
//
int main(int argc, char *argv[])
{
	char *blockbuf;
	char sectorbuf[SECTOR_SIZE];
	char ch;
	int lsn, i;
	int option;

	flashfp = fopen("flashmemory", "w+b");
	if(flashfp == NULL)
	{
		printf("file open error\n");
		exit(1);
	}
	   
	//
	// flash memory의 모든 바이트를 '0xff'로 초기화한다.
	// 
	blockbuf = (char *)malloc(BLOCK_SIZE);
	memset(blockbuf, 0xFF, BLOCK_SIZE);

	for(i = 0; i < BLOCKS_PER_DEVICE; i++)
	{
		fwrite(blockbuf, BLOCK_SIZE, 1, flashfp);
	}

	free(blockbuf);

	ftl_open();    // ftl_read(), ftl_write() 호출하기 전에 이 함수를 반드시 호출해야 함
	
	//
	// ftl_write() 및 ftl_read() 테스트 코드 작성
	//
	printf("\n(1 : ftl_read 2: ftl_write 3 : ftl_print 0 : exit)\n");
	while(1){
		printf(">> ");
		scanf("%d",&option);

		switch(option){
			case 1:
				//ftl_read() 테스트
				printf("Logical sector number : ");
				scanf("%d",&lsn);

				ftl_read(lsn,sectorbuf);
				
				//sectorbuf 내용 출력
				for(int i=0;i<SECTOR_SIZE;i++){
					ch = sectorbuf[i];
					if(ch=="\0") break;
					printf("%c",ch);
				}
				break;
			case 2:
				//ftl_write() 테스트
				printf("Logical sector number : ");
				scanf("%d",&lsn);
				printf("data : ");
				scanf("%s",sectorbuf);

				ftl_write(lsn,sectorbuf);
				memset(sectorbuf,0,SECTOR_SIZE);

				break;
			case 3:
				//ftl_print()
				ftl_print();
				break;
			case 0:
				exit(0);
				break;
			default:
				printf("존재하지 않는 옵션\n");
				break;
		}
		printf("\n");
		
	}


	fclose(flashfp);

	return 0;
}
