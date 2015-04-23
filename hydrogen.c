
#include "aqua.h"

void Hydrogen(TSynchWater synch, int *sharedMemory, TParams param, int i)
{
	srand(time(NULL));
	sem_wait(synch.mutex);
	sharedMemory[2]++;
	printf("%d:\tH %d:\tstarted.\n", sharedMemory[1]++, i);
	printf("%d:\tH %d:\twaiting.\n", sharedMemory[1]++, i);

	if(sharedMemory[2]>=2 && sharedMemory[3]>=1)
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

	sem_wait(synch.hydroQueue);

	printf("%d:\tH %d:\tbegin bonding.\n", sharedMemory[1]++, i);
	sleep(rand()%(param.B)/1000);
	printf("%d:\tH %d:\tbonded.\n", sharedMemory[1]++, i);

	
}