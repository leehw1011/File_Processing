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
//int offset_table[DATABLKS_PER_DEVICE];
int free_block;

void ftl_open()
{
	//
	// address mapping table 초기화 또는 복구
	// free block's pbn 초기화
    	// address mapping table에서 lbn 수는 DATABLKS_PER_DEVICE 동일

	//우선 mapping table의 pbn 값을 모두 -1 로 초기화한다
	for(int i=0;i<BLOCKS_PER_DEVICE;i++){
		mapping_table[i]=-1;
	}
	free_block = DATABLKS_PER_DEVICE;
	//사용하던 flash memory파일이 존재하는지 체크
	//존재하면
	if(flashfp != NULL){
		//기존 파일에서 각 블록의 첫 번째 페이지의 spare 영역에서
		//lbn에 대한 정보를 읽어서 address mapping table을 복구한다.
		for(int i=0;i<BLOCKS_PER_DEVICE;i++){
			int *lbn;
			fseek(flashfp,BLOCK_SIZE*i,SEEK_SET);
			fread(lbn,4,1,flashfp);
			if(*lbn!=0xff){
				mapping_table[*lbn]=i;
			}
			else free_block = i;
		}
	}
	
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

	return;
}

void ftl_print()
{

	return;
}
