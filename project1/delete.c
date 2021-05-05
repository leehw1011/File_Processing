#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 100

int main(int argc, char* argv[]){

	int length1, length2;
	char *fname = argv[1];
	int offset = atoi(argv[2]);
	int byte = atoi(argv[3]);

	char *buf1[BUFFER_SIZE];
	char *buf2[BUFFER_SIZE];

	int fd = open(fname,O_RDONLY);
	if(fd<0){printf("error\n");}

	length1 = read(fd,buf1,offset);
	length2 = pread(fd,buf2,BUFFER_SIZE,offset+byte);

	close(fd);

	int fd2 = open(fname,O_WRONLY|O_TRUNC);
	if(fd2<0){printf("error2\n");}

	write(fd2,buf1,length1);
	write(fd2,buf2,length2);

	close(fd2);
	exit(0);

}
