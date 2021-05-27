#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "person.h"
//필요한 경우 헤더 파일과 함수를 추가할 수 있음

// 과제 설명서대로 구현하는 방식은 각자 다를 수 있지만 약간의 제약을 둡니다.
// 레코드 파일이 페이지 단위로 저장 관리되기 때문에 사용자 프로그램에서 레코드 파일로부터 데이터를 읽고 쓸 때도
// 페이지 단위를 사용합니다. 따라서 아래의 두 함수가 필요합니다.
// 1. readPage(): 주어진 페이지 번호의 페이지 데이터를 프로그램 상으로 읽어와서 pagebuf에 저장한다
// 2. writePage(): 프로그램 상의 pagebuf의 데이터를 주어진 페이지 번호에 저장한다
// 레코드 파일에서 기존의 레코드를 읽거나 새로운 레코드를 쓰거나 삭제 레코드를 수정할 때나
// 위의 readPage() 함수를 호출하여 pagebuf에 저장한 후, 여기에 필요에 따라서 새로운 레코드를 저장하거나
// 삭제 레코드 관리를 위한 메타데이터를 저장합니다. 그리고 난 후 writePage() 함수를 호출하여 수정된 pagebuf를
// 레코드 파일에 저장합니다. 반드시 페이지 단위로 읽거나 써야 합니다.
//
// 주의: 데이터 페이지로부터 레코드(삭제 레코드 포함)를 읽거나 쓸 때 페이지 단위로 I/O를 처리해야 하지만,
// 헤더 레코드의 메타데이터를 저장하거나 수정하는 경우 페이지 단위로 처리하지 않고 직접 레코드 파일을 접근해서 처리한다.
 
//
// 헤더 영역의 내용을 갱신한다.
//
void headerUpdate(FILE *fp, int total_page, int total_record, int last_pagenum, int last_recordnum)
{
	char headerRecord[16];
	memset(headerRecord,(char)0xFF,16);
	memcpy(headerRecord,&total_page,4);
	memcpy(headerRecord+4,&total_record,4);
	memcpy(headerRecord+8,&last_pagenum,4);
	memcpy(headerRecord+12,&last_recordnum,4);

	fseek(fp,0,SEEK_SET);
	fwrite(headerRecord,16,1,fp);
}

//
// 페이지 번호에 해당하는 페이지를 주어진 페이지 버퍼에 읽어서 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
//
void readPage(FILE *fp, char *pagebuf, int pagenum)
{
	if(fseek(fp,16+PAGE_SIZE*pagenum,SEEK_SET)<0){
		printf("fseek error\n");
	}
	if(fread(pagebuf,PAGE_SIZE,1,fp)<0){
		printf("fread error\n");
	}
}
 
//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 레코드 파일의 위치에 저장한다. 
// 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	if(fseek(fp,16+PAGE_SIZE*pagenum,SEEK_SET)<0){
		printf("fseek error\n");
	}
	if(fwrite(pagebuf,PAGE_SIZE,1,fp)<0){
		printf("fwrite error\n");
	}

}
 
//
// 새로운 레코드를 저장할 때 터미널로부터 입력받은 정보를 Person 구조체에 먼저 저장하고, pack() 함수를 사용하여
// 레코드 파일에 저장할 레코드 형태를 recordbuf에 만든다. 
// 
void pack(char *recordbuf, const Person *p)
{
	sprintf(recordbuf,"%s#%s#%s#%s#%s#%s#",p->id,p->name,p->age,p->addr,p->phone,p->email);
}
 
// 
// 아래의 unpack() 함수는 recordbuf에 저장되어 있는 레코드를 구조체로 변환할 때 사용한다.
//
void unpack(const char *recordbuf, Person *p)
{

}
 
//
// 새로운 레코드를 저장하는 기능을 수행하며, 터미널로부터 입력받은 필드값들을 구조체에 저장한 후 아래 함수를 호출한다.
//
void add(FILE *fp, const Person *p)
{
	char recordbuf[MAX_RECORD_SIZE];
	char pagebuf[PAGE_SIZE];
	int total_page, total_record, last_pagenum, last_recordnum;
	int record_length;
	char headbuf[16];

	//구조체의 정보를 레코드파일에 저장할 형태로 packing
	memset(recordbuf,(char)0xFF,MAX_RECORD_SIZE);
	pack(recordbuf,p);
	record_length = strlen(recordbuf);

	//헤더 레코드 내용 읽어오기
	fseek(fp,0,SEEK_SET);
	fread(headbuf,16,1,fp);
	memcpy(&total_page,headbuf,4);	//전체 데이터 페이지 수
	memcpy(&total_record,headbuf+4,4);	//레코드 파일에 존재하는 모든 레코드의 수
	memcpy(&last_pagenum,headbuf+8,4);	//가장 최근 삭제된 레코드의 페이지 번호
	memcpy(&last_recordnum,headbuf+12,4);	//가장 최근 삭제된 레코드 번호

	//삭제 레코드가 없는 경우
	if(last_pagenum==-1&&last_recordnum==-1){
		printf("삭제 레코드가 없는 경우\n");
		//최초의 저장인 경우
		printf("total_record : %d\n",total_record);
		if(total_record==0){
			memset(pagebuf,(char)0xFF,PAGE_SIZE);
			memcpy(pagebuf,recordbuf,record_length);
			writePage(fp,pagebuf,1);
			// 페이지 수, 레코드 수 증가
			headerUpdate(fp,total_page+1,total_record+1,last_pagenum,last_recordnum);
		}

		//최초의 저장이 아니라면
		else{
			//마지막 페이지에 레코드를 저장할 수 있는지 확인하자.
			memset(pagebuf,(char)0xFF,PAGE_SIZE);
			readPage(fp,pagebuf,total_page-1);
			int len;
			int recordnum, total_len=0;	//페이지에 저장되어 있는 레코드의 수, 사용중인 공간의 총 길이
			memcpy(&recordnum,pagebuf,4);
			printf("recordnum : %d\n",recordnum);
			for(int i=0;i<recordnum;i++){
				memcpy(&len,pagebuf+4+8*(i+1),4);
				total_len+=len;
			}

			// 페이지에 남은 공간이 저장하려는 레코드보다 크고 & 헤더 영역에도 남은 공간이 있어야함
			if(DATA_AREA_SIZE-total_len>=record_length && HEADER_AREA_SIZE-(4+8*recordnum)>=8){
				printf("여기까지 왔나요?\n");
				memset(pagebuf,(char)0xFF,PAGE_SIZE);
				readPage(fp,pagebuf,total_page-1);

				//왜 앞쪽으로 저장이 되는거지 ?????
				memcpy(pagebuf+HEADER_AREA_SIZE+total_len,recordbuf,record_length);
				writePage(fp,pagebuf,total_page-1);
				// 레코드 수 1 증가
				headerUpdate(fp,total_page,total_record+1,last_pagenum,last_recordnum);

			}

			else{
				// 새로 페이지 할당
			}

		}
	}

	//삭제된 레코드가 있는 경우
	else{
		printf("삭제 레코드가 있는 경우\n");
	
	}
}
 
//
// 주민번호와 일치하는 레코드를 찾아서 삭제하는 기능을 수행한다.
//
void delete(FILE *fp, const char *id)
{
 
}
 
int main(int argc, char *argv[])
{
	FILE *fp;  // 레코드 파일의 파일 포인터
	Person p;
	int filesize;

	// 파일이 존재할 경우 "r+"모드로 열기
	if(access(argv[2],F_OK) == 0){
		if((fp = fopen(argv[2],"r+"))==NULL){
			printf("file open error\n");
		}
	}
	// 파일이 존재하지 않는 경우 "w+"모드로 열기
	else{
		if((fp = fopen(argv[2],"w+"))==NULL){
			printf("file open error\n");
		}
	}

	// open한 파일이 비어있는 경우 헤더 레코드를 만들어야한다.
	fseek(fp,0,SEEK_END);
	filesize = ftell(fp);
	if(filesize==0){
		char headerRecord[16];
		memset(headerRecord,(char)0xFF,16);
		// 초기 헤더레코드 값 0 0 -1 -1
		int num0 = 0, num1 = -1;
		memcpy(headerRecord,&num0,4);
		memcpy(headerRecord+4,&num0,4);
		memcpy(headerRecord+8,&num1,4);
		memcpy(headerRecord+12,&num1,4);
		
                fwrite(headerRecord,16,1,fp);

	}

	// 입력된 옵션이 a이면 add 작업을 실행
	if(argv[1][0]=='a'){
		printf("add를 실행해야함\n");	//
		
		// 입력된 정보를 구조체에  저장하기
		strcpy(p.id,argv[3]);
		strcpy(p.name,argv[4]);
		strcpy(p.age,argv[5]);
		strcpy(p.addr,argv[6]);
                strcpy(p.phone,argv[7]);
                strcpy(p.email,argv[8]);

		add(fp,&p);

	}

	else if(argv[1][0]=='d'){
		printf("delete를 실행해야함\n");
	}

	else return 1;

	return 1;
}
