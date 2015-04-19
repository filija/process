
#include "aqua.h"

//nazvy sdilenych promennych
const char *shmName="/aquaShm";
const char *semMutexNm="/mutexSem";
const char *semOxyqueNm="/oxyqueSem";
const char *semHydroqueNm="/hydroqueSem";
const char *fileName="h2o.out"; //nazev souboru

char *errmsg[]={
	"",	//EOK
	"Neodpovidajici argumenty", //EARGS
	"Chyba pri vytvareni potomka", //EFORK
	"Nepodarilo se vytvorit sdilenou pamet", //ESHM
	"Chyba pri orezavani sdilene pameti", //EFTR
	"Chyba pri mapovani na sdilenou pamet", //EMAP
	"Chyba pri vytvareni semaforu", //ESEM
	"chyba pri otevirani souboru", //EFIL
};

void unlinker(void)
{
	sem_unlink(semMutexNm);
	sem_unlink(semOxyqueNm);
	sem_unlink(semHydroqueNm);	
}

void cleaner(TSynchWater synch, int sham)
{
	sem_close(synch.mutex);
	sem_close(synch.oxyQueue);
	sem_close(synch.hydroQueue);
	munmap(NULL, 3*sizeof(int));
	shm_unlink(shmName);
	fclose(synch.outFile);
	if(sham!=-1)
	{
		close(sham);
	}
}

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
	int countH; //pocet generovanych vodiku
	countH=param.N*2; //pocet vodiku je 2*pocet kysliku
	pid_t pid;
	int i;
	int j;
	int *sharedMemory; //sdilena pamet
	TSynchWater synch={.mutex=NULL, .oxygen=0, .hydrogen=0, .oxyQueue=NULL, .hydroQueue=NULL, .outFile=NULL};

	if(param.err!=EOK)
	{
		fprintf(stderr, "%s\n", errmsg[EARGS]);
		return 1;
	}
	
	else{
		//vytvoreni sdilene pameti
		int sham=shm_open(shmName, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
		if(sham<0)
		{
			fprintf(stderr, "%s\n", errmsg[ESHM]);
			return 1;
		}

		if(ftruncate(sham, 3*sizeof(int))==-1)
		{
			fprintf(stderr, "%s\n", errmsg[EFTR]);
			return 1;
		}

		//mapovani na sdilenou pamet
		sharedMemory=mmap(NULL, 3*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, sham, 0);
		if(sharedMemory==MAP_FAILED)
		{
			fprintf(stderr, "%s\n", errmsg[EMAP]);
			return 1;
		}

		sharedMemory[0]=-1; //sdielna pamet
		sharedMemory[1]=1; //poradove cislo
		sharedMemory[2]=0; //pocet vodiku

		/*synchronizace semaforu*/
		synch.mutex=sem_open(semMutexNm, O_CREAT, S_IRUSR|S_IWUSR, 1);
		synch.oxyQueue=sem_open(semOxyqueNm, O_CREAT, S_IRUSR|S_IWUSR, 0);
		synch.hydroQueue=sem_open(semHydroqueNm, O_CREAT, S_IRUSR|S_IWUSR, 0);
		synch.outFile=fopen(fileName, "w");
		synch.hydrogen=countH;
		synch.oxygen=param.N;

		if(synch.mutex==SEM_FAILED || synch.oxyQueue==SEM_FAILED || synch.hydroQueue==SEM_FAILED)
		{
			cleaner(synch, sham);
			unlinker();
			fprintf(stderr, "%s\n", errmsg[ESEM]);
			return 1;
		}

		if(synch.outFile==NULL)
		{
			fprintf(stderr, "%s\n", errmsg[EFIL]);
			return 1;
		}

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
						Oxygen(synch, sharedMemory, param.GO, i);	
						cleaner(synch, sham);
						exit(0);
					}

					else{
						
					}					
			}

			/*Zde se budou generovat procesy pro vodik*/
						
			for(j=0; j<countH; j++)
			{
				pid=fork();

				if(pid<0)
				{
					fprintf(stderr, "%s\n", errmsg[EFORK]);
					return 1;
				}

				if(pid==0)
				{
					Hydrogen(synch, sharedMemory, param.GH, j);
					cleaner(synch, sham);
					exit(0);
				}

				else{
					
				}
			}
	}

	cleaner(synch, -1);
	unlinker();
	return 0;
}