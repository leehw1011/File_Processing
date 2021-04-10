#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int num;
	struct timeval start, end;
	int diff;
	char *fname = argv[1];
	char buf[250];
	FILE *fp;
	if((fp = fopen(fname,"rb"))==NULL){
		printf("error : file open");
		return 0;
	}
	fread(&num,4,1,fp);
	gettimeofday(&start,NULL);
	for(int i=0;i<num;i++){
		fread(buf,250,1,fp);
	}

	gettimeofday(&end,NULL);
	
	diff = (end.tv_sec*1000000+end.tv_usec) - (start.tv_sec*1000000+start.tv_usec);
	printf("#records: %d elapsed_time: %d us\n",num,diff);
	
	fclose(fp);
	return 0;
}
