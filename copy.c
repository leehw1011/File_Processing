#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]){

	int length;
	char buf[10];
	char *fname1 = argv[1];
	char *fname2 = argv[2];

	int fd1 = open(fname1,400);
	int fd2 = creat(fname2,700);
	
	if(fd1<0||fd2<0){
		printf("error");
	}

	while((length=read(fd1,buf,10))>0){
		write(fd2,buf,length);
	}

	close(fd1);
	close(fd2);
	exit(0);
}
