#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[]){

	char *fname = argv[1];
	int offset = atoi(argv[2]);
	char *buf = argv[3];

	int fd = open(fname,O_WRONLY);

	if(fd<0) {printf("error\n");}

	pwrite(fd,buf,strlen(argv[3]),offset);

	close(fd);
	exit(0);
}
