// 주의사항
// 1. blockmap.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. blockmap.h에 정의되어 있지 않을 경우 본인이 이 파일에서 만들어서 사용하면 됨
// 3. 필요한 data structure가 필요하면 이 파일에서 정의해서 쓰기 바람(blockmap.h에 추가하면 안됨)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "blockmap.h"
// 필요한 경우 헤더 파일을 추가하시오.

//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// file system에 의해 반드시 먼저 호출이 되어야 한다.
//
extern FILE *flashfp;
int mapping_table[DATABLKS_PER_DEVICE];	//0~15
int offset_table[DATABLKS_PER_DEVICE];
int free_block=100;

void set_free_block();

void ftl_open()
{
	//
	// address mapping table 초기화 또는 복구
	// free block's pbn 초기화
    	// address mapping table에서 lbn 수는 DATABLKS_PER_DEVICE 동일
	
	char pagebuf[PAGE_SIZE];
	int ppn;
	int lbn=0;
	//우선 mapping table 을 모두 -1로 설정한 후
	for(int i=0;i<BLOCKS_PER_DEVICE;i++){
		mapping_table[i]=-1;
	}
	//flash memory 읽어서 기존에 있던 내용 불러오기
	for(int i=0;i<BLOCKS_PER_DEVICE;i++){
		ppn = i*4;
		dd_read(ppn,pagebuf);
		//spare영역의 lbn정보 - SECTOR_SIZE+3을 읽어야하나?
		//lbn = pagebuf[SECTOR_SIZE+3];
		lbn = pagebuf[SECTOR_SIZE];
		if(lbn!=0xffffffff){
			mapping_table[lbn]=i;
		}
	}

	printf("\n초기화 후 mapping table : \n");
	for(int i=0;i<BLOCKS_PER_DEVICE;i++){
                printf("%d ",mapping_table[i]);
        }

	//free block 초기화
	set_free_block();
	printf("free block : %d\n",free_block);
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_read(int lsn, char *sectorbuf)
{
	char pagebuf[PAGE_SIZE];
	int lbn, pbn, psn;
	//lsn이 사용 가능한 범위가 아니면 에러
	if(lsn<0||lsn>DATAPAGES_PER_DEVICE){
		fprintf(stderr,"ftl_read error\n");
		exit(1);
	}
	
	lbn = lsn/PAGES_PER_BLOCK;
	offset_table[lbn] = lsn%PAGES_PER_BLOCK;

	pbn = mapping_table[lbn];
	if(pbn==-1){
		printf("해당 영역에는 data가 존재하지 않음\n");
		return;
	}

	psn = pbn*PAGES_PER_BLOCK + offset_table[lbn];
	dd_read(psn,pagebuf);

	memcpy(sectorbuf,pagebuf,SECTOR_SIZE);
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_write(int lsn, char *sectorbuf)
{
	char pagebuf[PAGE_SIZE];
	int temp;	//temp block num

	//lsn이 사용 가능한 범위가 아니면 에러
	if(lsn<0||lsn>DATAPAGES_PER_DEVICE){
		fprintf(stderr,"ftl_write error\n");
		exit(1);
	}

	//lsn으로 psn찾기
	int lbn, pbn, psn;
	lbn = lsn/PAGES_PER_BLOCK;
	offset_table[lbn]=lsn%PAGES_PER_BLOCK;

	pbn = mapping_table[lbn];
	//pbn == -1이면 최초로 쓰는 것
	if(pbn==-1){
		pbn = free_block;
		mapping_table[lbn]=pbn;
		set_free_block();
	}
	//pbn != -1이면 갱신
	else{
		temp = pbn;
		pbn = free_block;
		free_block = temp;
		dd_erase(free_block);
		mapping_table[lbn]=pbn;
	}
	psn = pbn*PAGES_PER_BLOCK + offset_table[lbn];
	
	//pagebuf내용 = sectorbuf(512) + lbn(4) + lsn(4)
	memcpy(pagebuf,sectorbuf,SECTOR_SIZE);
	memcpy(pagebuf+SECTOR_SIZE,&lbn,4);
	memcpy(pagebuf+SECTOR_SIZE+4,&lsn,4);

	printf("%s\n",pagebuf);
	dd_write(psn,pagebuf);

	//확인
	/*printf("mapping table 확인 \n");
	for(int i=0;i<DATABLKS_PER_DEVICE;i++){
                printf("%d ",mapping_table[i]);
        }
	printf("\n free block : %d",free_block);*/

	return;
}

void ftl_print()
{
	printf("lbn pbn\n");
	for(int i=0;i<BLOCKS_PER_DEVICE;i++){
		printf("%-3d %-3d\n",i,mapping_table[i]);
	}
	printf("free block's pbn=%d",free_block);
	return;
}

void set_free_block(){
	//왜 여기서 mapping_table[15] 값이 바뀌지????
	int pbn[BLOCKS_PER_DEVICE];	//0~15
	int num;
	for(int i=0;i<BLOCKS_PER_DEVICE;i++){
		pbn[i]=i;
	}
	for(int i=0;i<BLOCKS_PER_DEVICE;i++){
		num = mapping_table[i];
		//-1이 아니면 이미 사용하고 있는 block
		if(num!=-1){
			//그 block은 free block이 될 수 없다
			pbn[num]=-1;
		}
	}
	for(int i=0;i<BLOCKS_PER_DEVICE;i++){
		if(pbn[i]!=-1){
			free_block = i;
			break;
		}
	}
	return;
}
