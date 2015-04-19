
#include "aqua.h"

void Oxygen(TSynchWater synch, int *sharedMemory, int maxGo, int i)
{
	srand(time(NULL));

	sem_wait(synch.mutex); //mutex wait
	synch.oxygen++;

	if(synch.hydrogen>=2)
	{
		printf("oxygen if\n");
		sem_post(synch.hydroQueue);
		sem_post(synch.hydroQueue);
		synch.hydrogen=synch.hydrogen-2;
		sem_post(synch.oxyQueue);
		synch.oxygen--;
	}		

	else{
		printf("oxygen else\n");
		sem_post(synch.mutex);
	}

	sem_wait(synch.oxyQueue);
	//bond();
	sem_post(synch.mutex);

}