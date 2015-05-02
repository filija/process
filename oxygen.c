
#include "aqua.h"

void Oxygen(TSynchWater synch, int *sharedMemory, TParams param, int i)
{
	int c_hydro=0;
	srand(time(NULL));
	sem_wait(synch.mutex);
	if(param.GO!=0)
	{		
		sleep((rand()%param.GO)/1000);
	}
		
	sharedMemory[3]++;
	sharedMemory[4]++;
	sem_wait(synch.locker);
	fprintf(synch.outFile, "%d:\tO %d:\tstarted.\n", sharedMemory[1]++, sharedMemory[4]);
	sem_post(synch.locker);

	if(sharedMemory[2]>=2)
	{
		sem_post(synch.hydroQueue);
		sem_post(synch.hydroQueue);

		sem_wait(synch.locker);
		fprintf(synch.outFile, "%d:\tO %d:\tready\n", sharedMemory[1]++, sharedMemory[4]);
		sem_post(synch.locker);

		sharedMemory[2]=sharedMemory[2]-2;
		sem_post(synch.oxyQueue);
		sharedMemory[3]--;
	}

	else{
		sem_wait(synch.locker);
		fprintf(synch.outFile, "%d:\tO %d:\twaiting\n", sharedMemory[1]++, sharedMemory[4]);
		sem_post(synch.locker);
		sem_post(synch.mutex);
	}

	sem_wait(synch.oxyQueue);

	sem_wait(synch.locker);
	fprintf(synch.outFile, "%d:\tO %d:\tbegin bonding\n", sharedMemory[1]++, sharedMemory[4]);	
	c_hydro++;
	sem_post(synch.locker);

	if(param.B!=0)
	{
		sleep((rand()%param.B)/1000);
	}

	sem_wait(synch.locker);
	fprintf(synch.outFile, "%d:\tO %d:\tbonded\n", sharedMemory[1]++, sharedMemory[4]);
	sem_post(synch.locker);

	sem_post(synch.mutex);

}