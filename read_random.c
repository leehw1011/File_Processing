#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define SUFFLE_NUM 10000

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);

int main(int argc, char **argv)
{
	int *read_order_list;
	int num;

	struct timeval start, end;
	int diff;
	char *fname = argv[1];
	char buf[250];
	FILE *fp;

	if((fp = fopen(fname,"rb"))==NULL){
		printf("error : file open\n");
		return 0;
	}
	
	fread(&num,4,1,fp);
	
	read_order_list = (int*)malloc(sizeof(int)*num);
	GenRecordSequence(read_order_list, num);

	gettimeofday(&start,NULL);
	for(int i=0;i<num;i++){
		fseek(fp,(4+250*read_order_list[i]),SEEK_SET);
		fread(buf,250,1,fp);
	}
	
	gettimeofday(&end,NULL);

	diff = (end.tv_sec*1000000+end.tv_usec) - (start.tv_sec*1000000+start.tv_usec);

	printf("#records: %d elapsed_time: %d us\n",num,diff);
	fclose(fp);
	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i,j,k;

	srand((unsigned int)time(0));

	for(i=0;i<n;i++)
	{
		list[i]=i;
	}

	for(i=0;i<SUFFLE_NUM;i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}
