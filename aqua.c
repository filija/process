
#include "aqua.h"

char *errmsg[]={
	"",	//EOK
	"Neodpovidajici argumenty", //EARGS
	"Chyba pri vytvareni potomka", //EFORK
};

/*Funkce pro ziskani parametru*/
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

	param.err=EOK;
	return param;
}



int main(int argc, char **argv)
{
	TParams param;
	param=getparams(argc, argv);
	pid_t pid;
	int i;
	int j;

	if(param.err!=EOK)
	{
		fprintf(stderr, "%s\n", errmsg[EARGS]);
		return 1;
	}

	else{
			/*Zde se budou generovat procesy pro kyslik*/
			for(i=0; i<param.N; i++)
			{	
					pid=fork();

					if(pid<0)
					{
						fprintf(stderr, "%s\n", errmsg[EFORK]);
						return 1;
					}

					if(pid==0)
					{
						testOxygen();	
						exit(0);
					}

					else{
						
					}					
			}

			/*Zde se budou generovat procesy pro vodik*/
			param.N=param.N*2;			
			for(j=0; j<param.N; j++)
			{
				pid=fork();

				if(pid<0)
				{
					fprintf(stderr, "%s\n", errmsg[EFORK]);
					return 1;
				}

				if(pid==0)
				{
					testHydrogen();
					exit(0);
				}

				else{
					
				}
				testHydrogen();
			}
	}
	return 0;
}