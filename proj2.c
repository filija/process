#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define MAXTIME 5001 //max hodnota doby 

typedef struct tparams
{
	int N; //pocet reprezentujici kyslik
	int GH; //maximalni hodnota doby pro vodik
	int GO; //maximalni hodnota doby pro kyslik
	int B; //maximalni hodnota doby pro funkci bond
	int err;
}TParams;

enum errors
{
	EARGS,
};

char *errmsg[]={
	"Neodpovidajici argumenty\n",
};

TParams getparams(int argc, char **argv)
{
	TParams param;
	if(argc!=5)
	{
		param.err=1;
		return param;
	}

	else{
		param.N=atoi(argv[1]);
		param.GH=atoi(argv[2]);
		param.GO=atoi(argv[3]);
		param.B=atoi(argv[4]);

		if(!(param.N>0))
		{
			param.err=1;
			return param;
		}

		if(!(param.GH>=0 && param.GH<MAXTIME))
		{
			param.err=1;
			return param;
		}

		if(!(param.GO>=0 && param.GO<MAXTIME))
		{
			param.err=1;
			return param;	
		}

		if(!(param.B>=0 && param.B<MAXTIME))
		{
			param.err=1;
			return param;
		}	
	}

	param.err=0;
	return param;
}

int main(int argc, char **argv)
{
	TParams param;
	param=getparams(argc, argv);

	if(param.err==1)
	{
		fprintf(stderr, "%s", errmsg[EARGS]);
		return 1;
	}

	else{
		printf("cajk\n");
	}

	return 0;
}