/*
	Author: Jakub Filipek (xfilip34)
	Projekt: proj2 IOS, h2oProblem
	Datum: 3.5.2015
*/

#include "h2o.h"

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

/*uzavreni a odpojeni semaforu*/
void unlinker(TSynchWater synch)
{
	sem_close(synch.mutex);
	sem_close(synch.bonding);
	sem_close(synch.waiting);
	sem_close(synch.locker);
	sem_close(synch.bonded);
	sem_close(synch.hydroQueue);
	sem_close(synch.oxyQueue);
	sem_close(synch.ready);
	sem_close(synch.finished);
	sem_close(synch.done);
	sem_unlink("/mutexsem");
	sem_unlink("/waitingsem");
	sem_unlink("/bondingsem");
	sem_unlink("/lockersem");
	sem_unlink("/bondedsem");
	sem_unlink("/hydroQueuesem");
	sem_unlink("/oxyQueuesem");
	sem_unlink("/readysem");
	sem_unlink("/finishedsem");
	sem_unlink("/donesem");
	return;
}

/*odmapovani pameti */
void cleaner(TSynchWater synch, int sham)
{
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

/********************************Hlavni Program******************************************************************/
int main(int argc, char **argv)
{
	TParams param;
	param=getparams(argc, argv);
	int countH; //pocet generovanych vodiku
	countH=param.N*2; //pocet vodiku je 2*pocet kysliku
	pid_t pid;
	int i; //iterrator
	int *sharedMemory; //sdilena pamet

	TSynchWater synch={.mutex=NULL, .hydroQueue=NULL, .bonding=NULL, .waiting=NULL, 
	.locker=NULL, .bonded=NULL, .oxyQueue=NULL, .outFile=NULL, .ready=NULL, .finished=NULL, .done=NULL};

	/*pri chybnych argumentech*/
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
			return 2;
		}

		//rozrezani na 7 prvkove pole
		if(ftruncate(sham, 6*sizeof(int))==-1)
		{
			fprintf(stderr, "%s\n", errmsg[EFTR]);
			shm_unlink(shmName);
			return 2;
		}

		//mapovani na sdilenou pamet
		sharedMemory=mmap(NULL, 6*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, sham, 0);
		if(sharedMemory==MAP_FAILED)
		{
			fprintf(stderr, "%s\n", errmsg[EMAP]);
			shm_unlink(shmName);
			return 2;
		}

		sharedMemory[0]=1; //index vodiku
		sharedMemory[1]=1; //index operaci
		sharedMemory[2]=0; //poradove cislo vodiku
		sharedMemory[3]=0; //poradove cislo kysliku
		sharedMemory[4]=1; //index kysliku
		sharedMemory[5]=0; //counter bond

		/*synchronizace semaforu*/
	if(((synch.mutex=sem_open("/mutexsem", O_CREAT | O_EXCL, 0644, 1))==SEM_FAILED) ||
	   ((synch.waiting=sem_open("/waitingsem", O_CREAT | O_EXCL, 0644, 0))==SEM_FAILED) ||
	   ((synch.bonding=sem_open("/bondingsem", O_CREAT | O_EXCL, 0644, 0))==SEM_FAILED) ||
	   ((synch.locker=sem_open("/lockersem", O_CREAT | O_EXCL, 0644, 1))==SEM_FAILED) ||
	   ((synch.bonded=sem_open("/bondedsem", O_CREAT | O_EXCL, 0644, 0))==SEM_FAILED) ||
	   ((synch.hydroQueue=sem_open("/hydroQueuesem", O_CREAT | O_EXCL, 0644, 0))==SEM_FAILED) ||
	   ((synch.oxyQueue=sem_open("/oxyQueuesem", O_CREAT | O_EXCL, 0644, 0))==SEM_FAILED) ||
	   ((synch.ready=sem_open("/readysem", O_CREAT | O_EXCL, 0644, 0))==SEM_FAILED) ||
	   ((synch.finished=sem_open("/finishedsem", O_CREAT | O_EXCL, 0644, 0))==SEM_FAILED) ||
	   ((synch.done=sem_open("/donesem", O_CREAT | O_EXCL, 0644, 0))==SEM_FAILED))
	   {	
			fprintf(stderr, "%s\n", errmsg[ESEM]);
			shm_unlink(shmName);
			return 2;
		}
		
		/*otevreni vystupniho souboru*/
		synch.outFile=fopen(fileName, "w");
		if(synch.outFile==NULL)
		{
			fprintf(stderr, "%s\n", errmsg[EFIL]);
			return 2;
		}
		setbuf(synch.outFile, NULL); //vypnuti buffer
		int check;

			/*Generovani procesu pro vodik*/
			for(i=1; i<=countH; i++)
			{
				/*generovani procesu po nahodne dobe*/
				if(param.GH!=0)
				{
					srand(time(NULL)*getpid());
					usleep((rand()%param.GH)*1000);
				}

				pid=fork();

				if(pid<0)
				{
					fprintf(stderr, "%s\n", errmsg[EFORK]);
					return 1;
				}

				if(pid==0)
				{				
					Hydrogen(synch, sharedMemory, param, i);
					fflush(synch.outFile);
					exit(0);
				}

				else{
					;
				}	
			}

			/*Generovani procesu pro kyslik*/
			for(i=1; i<=param.N; i++)
			{	
				/*generovani procesu po nahodne dobe*/
				if(param.GO!=0)
				{
					srand(time(NULL));
					usleep((rand()%param.GO)*1000);
				}

					pid=fork();

					if(pid<0)
					{
						fprintf(stderr, "%s\n", errmsg[EFORK]);
						return 1;
					}

					if(pid==0)
					{
						Oxygen(synch, sharedMemory, param, i);	
						fflush(synch.outFile);
						exit(0);
					}

					else{
						; 
					}				
			}

			/*cekani na dokonceni procesu*/
			while(wait(&check) > 0);
			cleaner(synch, -1);
			close(sham);

			  /*odpojeni semaforu */
			unlinker(synch);
	}

	exit(EXIT_SUCCESS);
}

/********************************Konec Programu******************************************************************/