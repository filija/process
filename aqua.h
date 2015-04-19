/*
	TADY BUDE HLAVICKA
*/

#ifndef HYDROGEN_OXYGEN_H_INCLUDED
#define HYDROGEN_OXYGEN_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>

#define MAXTIME 5001 //max hodnota doby 

typedef struct tparams
{
	int N; //pocet reprezentujici kyslik
	int GH; //maximalni hodnota doby pro vodik
	int GO; //maximalni hodnota doby pro kyslik
	int B; //maximalni hodnota doby pro funkci bond
	int err;
}TParams;

typedef struct synchWater
{
	sem_t *mutex;
	int oxygen; //citac kysliku
	int hydrogen; //citac vodiku
	sem_t *oxyQueue; //fronta kysliku
	sem_t *hydroQueue; //fronta vodiku
	FILE *outFile; //vystupni soubor
} TSynchWater;

enum errors
{
	EOK=0,
	EARGS,
	EFORK,
	ESHM,
	EFTR,
	EMAP,
	ESEM,
	EFIL,
};


/*PROTOTYPY////////////////////*/
void Oxygen(TSynchWater synch, int *sharedMemory, int maxGo, int i);
void Hydrogen(TSynchWater synch, int *sharedMemory, int maxGh, int i);

#endif // HYDROGEN_OXYGEN_H_INCLUDED