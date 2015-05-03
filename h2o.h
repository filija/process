/*
	Author: Jakub Filipek (xfilip34)
	Projekt: proj2 IOS, h2oProblem
	Datum: 3.5.2015
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
#include <signal.h>

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
	sem_t *mutex; //hlavni semafor
	sem_t *oxyQueue; //ready kyslik
	sem_t *hydroQueue; //ready vodik
	sem_t *bonding; //fronta kysliku
	sem_t *waiting; //fronta vodiku
	sem_t *locker; //zamek pro tisk
	sem_t *bonded; 
	sem_t *ready;
	sem_t *finished; 
	sem_t *done;
	FILE *outFile; //vystupni soubor
} TSynchWater;

enum errors
{
	EOK=0, //OK
	EARGS, //chybne argumenty
	EFORK, //nepodarilo se vytvorit proces
	ESHM, //nepodarilo se vytvorit sdilenou pamet
	EFTR, //nepodarilo se orezat sdilenou pamet
	EMAP, //nepodarilo se namapovat pamet
	ESEM, //nepodarilo se vytvorit semafor
	EFIL, //nepodarilo se otevrit soubor
	EALC, //nepodarilo se nalokovat pamet
};


/*PROTOTYPY////////////////////*/
void Oxygen(TSynchWater synch, int *sharedMemory, TParams param, int i);
void Hydrogen(TSynchWater synch, int *sharedMemory, TParams param, int i);

#endif // HYDROGEN_OXYGEN_H_INCLUDED