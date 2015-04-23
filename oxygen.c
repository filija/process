
#include "aqua.h"

void Oxygen(TSynchWater synch, int *sharedMemory, TParams param, int i)
{	
	srand(time(NULL));
	sem_wait(synch.mutex);
	sharedMemory[3]++;
	printf("%d:\tO %d:\tstarted.\n", sharedMemory[1]++, i);
	printf("%d:\tO %d:\twaiting.\n", sharedMemory[1]++, i);
	if(sharedMemory[2]>=2)
	{
		sem_post(synch.hydroQueue);
		sem_post(synch.hydroQueue);
		sharedMemory[2]=sharedMemory[2]-2;
		sem_post(synch.oxyQueue);
		sharedMemory[3]--;
	}

	else{
		sem_post(synch.mutex);
	}

	sem_wait(synch.oxyQueue);

	printf("%d:\tO %d:\tbegin bonding.\n", sharedMemory[1]++, i);
	sleep(rand()%(param.B)/1000);
	printf("%d:\tO %d:\tbonded.\n", sharedMemory[1]++, i);
	sem_post(synch.mutex);

}