
#include "aqua.h"

//nazvy sdilenych promennych
const char *shmName="/aquaShm";
const char *fileName="h2o.out"; //nazev souboru

char *errmsg[]={
	"",	//EOK
	"Neodpovidajici argumenty", //EARGS
	"Chyba pri vytvareni potomka", //EFORK
	"Nepodarilo se vytvorit sdilenou pamet", //ESHM
	"Chyba pri orezavani sdilene pameti", //EFTR
	"Chyba pri mapovani na sdilenou pamet", //EMAP
	"Chyba pri vytvareni semaforu", //ESEM
	"Chyba pri otevirani souboru", //EFIL
	"Chyba pri alokaci", //EALC
};

void unlinker(TSynchWater synch)
{
	sem_destroy(synch.mutex);
	sem_destroy(synch.oxyQueue);
	sem_destroy(synch.hydroQueue);
	return;
}

void cleaner(TSynchWater synch, int sham)
{
	free(synch.mutex);
	free(synch.oxyQueue);
	free(synch.hydroQueue);
	free(synch.locker);
	munmap(NULL, 4*sizeof(int));
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
	int i; //iterrator
	int *sharedMemory; //sdilena pamet
	pid_t *oxygenPid;
	pid_t *hydrogenPid;

	TSynchWater synch={.mutex=NULL, .oxyQueue=NULL, .hydroQueue=NULL, .outFile=NULL};


	if(param.err!=EOK)
	{
		fprintf(stderr, "%s\n", errmsg[EARGS]);
		return 1;
	}
	
	else{
		hydrogenPid=malloc(countH*sizeof(*hydrogenPid));
		if(hydrogenPid==NULL)
		{
			fprintf(stderr, "%s\n", errmsg[EALC]);
			return 1;
		}

		oxygenPid=malloc(param.N*sizeof(*oxygenPid));
		if(oxygenPid==NULL)
		{
			fprintf(stderr, "%s\n", errmsg[EALC]);
			free(hydrogenPid);
			return 1;
		}

		//vytvoreni sdilene pameti
		int sham=shm_open(shmName, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
		if(sham<0)
		{
			fprintf(stderr, "%s\n", errmsg[ESHM]);
			free(hydrogenPid);
			free(oxygenPid);
			return 1;
		}


		if(ftruncate(sham, 5*sizeof(int))==-1)
		{
			fprintf(stderr, "%s\n", errmsg[EFTR]);
			free(hydrogenPid);
			free(oxygenPid);
			shm_unlink(shmName);
			return 1;
		}

		//mapovani na sdilenou pamet
		sharedMemory=mmap(NULL, 5*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, sham, 0);
		if(sharedMemory==MAP_FAILED)
		{
			fprintf(stderr, "%s\n", errmsg[EMAP]);
			free(hydrogenPid);
			free(oxygenPid);
			shm_unlink(shmName);
			return 1;
		}

		sharedMemory[0]=0; //index vodiku
		sharedMemory[1]=1; //index operaci
		sharedMemory[2]=0; //poradove cislo vodiku
		sharedMemory[3]=0; //poradove cislo kysliku
		sharedMemory[4]=0; //index kysliku
		
		/*alokace semaforu*/
		synch.mutex=malloc(sizeof(sem_t));
		synch.oxyQueue=malloc(sizeof(sem_t));
		synch.hydroQueue=malloc(sizeof(sem_t));
		synch.locker=malloc(sizeof(sem_t));

		if(synch.mutex==NULL || synch.oxyQueue==NULL || synch.hydroQueue==NULL || synch.locker==NULL)
		{
			fprintf(stderr, "%s\n", errmsg[EALC]);
			free(hydrogenPid);
			free(oxygenPid);
			shm_unlink(shmName);
			return 1;
		}

		/*synchronizace semaforu*/
		if(sem_init(synch.mutex, 0, 1)!=0)
		{
			fprintf(stderr, "%s\n", errmsg[ESEM]);
			free(hydrogenPid);
			free(oxygenPid);
			shm_unlink(shmName);
			return 1;
		}

		if(sem_init(synch.hydroQueue, 0, 0)!=0)
		{
			fprintf(stderr, "%s\n", errmsg[ESEM]);
			free(hydrogenPid);
			free(oxygenPid);
			shm_unlink(shmName);
			return 1;
		}

		if(sem_init(synch.oxyQueue, 0, 0)!=0)
		{
			fprintf(stderr, "%s\n", errmsg[ESEM]);
			free(hydrogenPid);
			free(oxygenPid);
			shm_unlink(shmName);
			return 1;
		}

		if(sem_init(synch.locker, 0, 1)!=0)
		{
			fprintf(stderr, "%s\n", errmsg[ESEM]);
			free(hydrogenPid);
			free(oxygenPid);
			shm_unlink(shmName);
			return 1;
		}

		synch.outFile=fopen(fileName, "w");
		if(synch.outFile==NULL)
		{
			fprintf(stderr, "%s\n", errmsg[EFIL]);
			free(hydrogenPid);
			free(oxygenPid);
			return 1;
		}
		setlinebuf(synch.outFile); //vypnuti buffer
			/*Zde se budou generovat procesy pro vodik*/
						
			for(i=1; i<=countH; ++i)
			{
				pid=fork();

				if(pid<0)
				{
					fprintf(stderr, "%s\n", errmsg[EFORK]);
					return 1;
				}

				if(pid==0)
				{				
					Hydrogen(synch, sharedMemory, param, i);
					cleaner(synch, sham);
					exit(0);
				}

				else{
					hydrogenPid[i-1]=pid;
				}
			}

			/*Zde se budou generovat procesy pro kyslik*/
			for(i=1; i<=param.N; ++i)
			{	
					pid=fork();

					if(pid<0)
					{
						fprintf(stderr, "%s\n", errmsg[EFORK]);
						return 1;
					}

					if(pid==0)
					{
						Oxygen(synch, sharedMemory, param, i);	
						cleaner(synch, sham);
						exit(0);
					}

					else{
						oxygenPid[i-1]=pid;
					}					
			}
			
			/*
			for(i=0; i<countH; ++i)
			{
				kill((pid_t)hydrogenPid, SIGKILL);
			}

			for(i=0; i<param.N; ++i)
			{
				kill((pid_t)oxygenPid, SIGKILL);
				
			}
			*/
			kill(pid, SIGKILL);
			cleaner(synch, -1);
			close(sham);
			unlinker(synch);
	}

	return 0;
}