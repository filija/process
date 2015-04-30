
#include "aqua.h"

void Hydrogen(TSynchWater synch, int *sharedMemory, TParams param, int i)
{

	srand(time(NULL));
	sem_wait(synch.mutex);	
	if(param.GH!=0)
	{		
		sleep((rand()%param.GH)/1000);	
	}
	sharedMemory[2]++;
	sharedMemory[0]++;

	printf("%d:\tH %d:\tstarted.\n", sharedMemory[1]++, sharedMemory[0]);

	if(sharedMemory[2]>=2 && sharedMemory[3]>=1)
	{		
		sem_post(synch.hydroQueue);
		sem_post(synch.hydroQueue);

		printf("%d:\tH %d:\tready.\n", sharedMemory[1]++, sharedMemory[0]);

		sharedMemory[2]=sharedMemory[2]-2;
		sem_post(synch.oxyQueue);
		sharedMemory[3]--;
	}

	else{

		printf("%d:\tH %d:\twaiting.\n", sharedMemory[1]++, sharedMemory[0]);

		sem_post(synch.mutex);
	}

	 sem_wait(synch.hydroQueue);
	
	printf("%d:\tH %d:\tbegin bonding.\n", sharedMemory[1]++, sharedMemory[0]);
	

	if(param.B!=0)
	{
		sleep((rand()%param.B)/1000);
	}

	printf("%d:\tH %d:\tbonded.\n", sharedMemory[1]++, sharedMemory[0]);
	
}