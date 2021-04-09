#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int num = atoi(argv[1]);
	char *fname = argv[2];
	
	char buf[250] = {'\n',};
	FILE *fp;

	if((fp = fopen(fname,"wb"))==NULL){
		printf("error : file open");
		return 0;
	}


	fwrite(&num,4,1,fp);

	for(int i=0;i<num;i++){
		fwrite(buf,250,1,fp);
	}

	fclose(fp);
	return 0;
}

