
#include "aqua.h"

void Oxygen(TSynchWater synch, int *sharedMemory, int maxGo, int i)
{
	printf("%d:\tO %d:\tstarted.\n", sharedMemory[1]++, sharedMemory[3]++);		
}