
#include "aqua.h"

void Oxygen(TSynchWater synch, int *sharedMemory, TParams param, int i)
{	

	srand(time(NULL));
	sem_wait(synch.mutex);
	
	if(param.GO!=0)
	{		
		sleep((rand()%param.GO)/1000);
	}
	
	sharedMemory[3]++;
	sharedMemory[4]++;
	
	printf("%d:\tO %d:\tstarted.\n", sharedMemory[1]++, sharedMemory[4]);
	

	if(sharedMemory[2]>=2)
	{
		sem_post(synch.hydroQueue);
		sem_post(synch.hydroQueue);

		printf("%d:\tO %d:\tready\n", sharedMemory[1]++, sharedMemory[4]);

		sharedMemory[2]=sharedMemory[2]-2;
		sem_post(synch.oxyQueue);
		sharedMemory[3]--;
	}

	else{
		
		printf("%d:\tO %d:\twaiting\n", sharedMemory[1]++, sharedMemory[4]);
		
		sem_post(synch.mutex);
	}

	sem_wait(synch.oxyQueue);

	printf("%d:\tO %d:\tbegin bonding\n", sharedMemory[1]++, sharedMemory[4]);	

	if(param.B!=0)
	{
		sleep((rand()%param.B)/1000);
	}

	printf("%d:\tO %d:\tbonded\n", sharedMemory[1]++, sharedMemory[4]);

	sem_post(synch.mutex);

}