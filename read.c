#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]){
	
	int length;
	char buf[100];
	char *fname = argv[1];
	int offset = atoi(argv[2]);
	int byte = atoi(argv[3]);

	int fd = open(fname,400);
	
	if(fd<0) {printf("error\n");}

	length = pread(fd,buf,byte,offset);
	write(1,buf,length);
	printf("\n");

	close(fd);
	exit(0);
}
