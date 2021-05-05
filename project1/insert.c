#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define BUFFER_SIZE 100
int main(int argc, char* argv[]){

	int length;
	char *fname = argv[1];
	int offset = atoi(argv[2]);
	char *data = argv[3];
	char *buf[BUFFER_SIZE];

	int fd = open(fname,O_RDWR);
	if(fd<0) {printf("error\n");}

	length = pread(fd,buf,BUFFER_SIZE,offset+1);
	pwrite(fd,data,strlen(data),offset+1);
	pwrite(fd,buf,length,offset+1+strlen(data));
	
	close(fd);
	exit(0);
	
}
