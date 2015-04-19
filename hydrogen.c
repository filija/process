
#include "aqua.h"

void Hydrogen(TSynchWater synch, int *sharedMemory, int maxGh, int i)
{
	if(sharedMemory[1]==1)
	{
		printf("%d		:H %d	: started", sharedMemory[1], synch.hydrogen);
		sharedMemory[1]++;
	}

	srand(time(NULL));

		sem_wait(synch.mutex);
		synch.hydrogen++;
		if(synch.hydrogen>=2 && synch.oxygen>=1)
		{	
			printf("hydrogen if\n");
			sem_post(synch.hydroQueue);
			sem_post(synch.hydroQueue);
			synch.hydrogen=synch.hydrogen-2;
			sem_post(synch.oxyQueue);
			synch.oxygen--;
		}

		else{
			printf("hydrogen else\n");
			sem_post(synch.mutex);
		}

		sem_wait(synch.hydroQueue);
		//bond();

}