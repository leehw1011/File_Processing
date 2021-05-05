#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
int main(int argc, char* argv[]){

	int length;	
	char buf[BUFFER_SIZE];
	char *fname1 = argv[1]; //병합해서 생성할 파일
	char *fname2 = argv[2]; //먼저 써질 파일
	char *fname3 = argv[3];	//뒤에 써질 파일

	int fd1 = creat(fname1,700);
	int fd2 = open(fname2,400);
	int fd3 = open(fname3,400);

	if(fd1<0||fd2<0||fd3<0){printf("error\n");}

	length = read(fd2,buf,BUFFER_SIZE);
	write(fd1,buf,length);

	length = read(fd3,buf,BUFFER_SIZE);
	write(fd1,buf,length);

	close(fd1);
	close(fd2);
	close(fd3);

	exit(0);
}
