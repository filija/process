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

enum errors
{
	EOK=0,
	EARGS,
	EFORK,
};


/*PROTOTYPY////////////////////*/
void testOxygen();
void testHydrogen();

#endif // HYDROGEN_OXYGEN_H_INCLUDED