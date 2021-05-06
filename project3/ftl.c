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
int free_block;

int set_free_block();

void ftl_open()
{
	//
	// address mapping table 초기화 또는 복구
	// free block's pbn 초기화
    	// address mapping table에서 lbn 수는 DATABLKS_PER_DEVICE 동일

	//mapping table의 pbn 값을 모두 -1 로 초기화한다
	//for(int i=0;i<BLOCKS_PER_DEVICE;i++){
	//	mapping_table[i]=-1;
	//}
	//사용하던 flash memory파일이 존재하는지 체크
	//존재하면
	//if(1){
		//기존 파일에서 각 블록의 첫 번째 페이지의 spare 영역에서
		//lbn에 대한 정보를 읽어서 address mapping table을 복구한다.
	//	for(int i=0;i<BLOCKS_PER_DEVICE;i++){
	//		int *lbn;
	//		fseek(flashfp,BLOCK_SIZE*i,SEEK_SET);
	//		fread(lbn,4,1,flashfp);
	//		if(*lbn!=0xff){	//0xffffffff?
	//			mapping_table[*lbn]=i;
	//		}
	//		else free_block = i;
	//	}
	//}
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
		//printf("pagebuf의 내용 : %s\n",pagebuf);
		//for(int j=0;j<4;j++){
		//	lbn += (pagebuf[SECTOR_SIZE+j])<<4*(3-j);
		//}
		//
		//spare영역의 lbn정보 - SECTOR_SIZE+3을 읽어야하나?
		//lbn = pagebuf[SECTOR_SIZE+3];
		lbn = pagebuf[SECTOR_SIZE];
		//printf("lbn : %x\n",lbn);
		if(lbn!=0xffffffff){
			mapping_table[lbn]=i;
		}
	}

	printf("\n초기화 후 mapping table : \n");
	for(int i=0;i<DATABLKS_PER_DEVICE;i++){
                printf("%d ",mapping_table[i]);
        }

	//free block 초기화
	free_block = set_free_block();
	printf("free block : %d\n",free_block);
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_read(int lsn, char *sectorbuf)
{


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
	printf("ftl_write 호출됨! \n");
	//lsn이 사용 가능한 범위가 아니면 에러
	if(lsn<0||lsn>DATAPAGES_PER_DEVICE){
		fprintf(stderr,"ftl_write error");
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
		free_block = set_free_block();
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
	printf("mapping table 확인 \n");
	for(int i=0;i<DATABLKS_PER_DEVICE;i++){
                printf("%d ",mapping_table[i]);
        }
	printf("\n free block : %d",free_block);

	return;
}

void ftl_print()
{

	return;
}

int set_free_block(){
	//이미 사용중인 block을 -1로
	int block_num[DATABLKS_PER_DEVICE];
	for(int i=0;i<DATABLKS_PER_DEVICE;i++){
		block_num[i]=i;
	}
	for(int i=0;i<DATABLKS_PER_DEVICE;i++){
		if(mapping_table[i]!=-1){
			block_num[mapping_table[i]]=-1;
		}
	}
	for(int i=0;i<DATABLKS_PER_DEVICE;i++){
		if(block_num[i]!=-1){
			return i;
		}
	}
}
