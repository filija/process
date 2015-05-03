/*
	Author: Jakub Filipek (xfilip34)
	Projekt: proj2 IOS, h2oProblem
	Datum: 3.5.2015
*/

#include "h2o.h"

void Oxygen(TSynchWater synch, int *sharedMemory, TParams param, int i)
{
	sem_wait(synch.locker);
	fprintf(synch.outFile, "%d:\tO %d:\tstarted\n", sharedMemory[1]++, i);
	sem_post(synch.locker);

	sem_wait(synch.mutex); //cekam pri tvoreni molekuly
	sharedMemory[3]++; //pocet kysliku zvysuji o 1

	/*pokud jsou pripraveny vsechny atomy je pripraveno k vytvoreni molekuly*/
	if(sharedMemory[3]>=1 && sharedMemory[2]>=2)
	{
		sem_wait(synch.locker);
		fprintf(synch.outFile, "%d:\tO %d:\tready\n", sharedMemory[1]++, i);
		sem_post(synch.locker);
		sharedMemory[3]--; //odecteni kysliku
		sharedMemory[2]=sharedMemory[2]-2; //odecteni vodiku
		sem_post(synch.oxyQueue);
		sem_post(synch.hydroQueue);
		sem_post(synch.hydroQueue);
		sem_post(synch.ready);
	}	

	/*pokud ne cekej na dalsi atomy*/
	else{
		sem_wait(synch.locker);
		fprintf(synch.outFile, "%d:\tO %d:\twaiting\n", sharedMemory[1]++, i);
		sem_post(synch.locker);
		sem_post(synch.mutex); //muze generovat dalsi atom
	}

	sem_wait(synch.oxyQueue);
	sem_wait(synch.locker);
	fprintf(synch.outFile, "%d:\tO %d:\tbegin bonding\n", sharedMemory[1]++, i);
	sem_post(synch.locker);

 	/*generovani doby pro vytvoreni molekuly*/
	if(param.B!=0)
	{
		srand(time(NULL));
		usleep((rand()%param.B)*1000);
	}

	sem_wait(synch.ready);
	sharedMemory[5]++;

	/*prvky ktere cekaly*/
	if(sharedMemory[5]%3!=0)
	{
		//cekame na 3 prvek
		sem_post(synch.ready);
		sem_wait(synch.bonded);
		sem_wait(synch.locker);
		fprintf(synch.outFile, "%d:\tO %d:\tbonded\n", sharedMemory[1]++, i);
		sem_post(synch.locker);
		sem_post(synch.waiting);
		sem_wait(synch.done);
	}

	else{ /*vytvoreni molekul*/
		sem_post(synch.bonded);
		sem_post(synch.bonded);
		sem_wait(synch.locker);
		fprintf(synch.outFile, "%d:\tO %d:\tbonded\n", sharedMemory[1]++, i);
		sem_post(synch.locker);
		sem_wait(synch.waiting);
		sem_wait(synch.waiting);
		sem_post(synch.done);
		sem_post(synch.done);
		sem_post(synch.mutex);
	}
	/*pokud uz jsou poskladany vsechny atomy*/
	if(sharedMemory[5]==param.N*3)
	{
			int j;
		for(j=0; j<=param.N*3; j++)
		{
			/*kazdem atomu dat finished*/
			sem_post(synch.finished);
		}
	}
 	/*ukonceni*/
	sem_wait(synch.finished);
	sem_wait(synch.locker);
	fprintf(synch.outFile, "%d:\tO %d:\tfinished\n", sharedMemory[1]++, i);
	sem_post(synch.locker);
	sem_post(synch.bonding);

}