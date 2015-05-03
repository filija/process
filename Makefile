CC=gcc
CFLAGS=-std=gnu99 -pedantic -Wall -Wextra
 
all: h2o
 
h2o: h2o.o oxygen.o hydrogen.o
	$(CC) $(CFLAGS) -pthread h2o.o oxygen.o hydrogen.o -o h2o -lrt
 
h2o.o: h2o.c h2o.h
	$(CC) $(CFLAGS) -pthread -c h2o.c -o h2o.o -lrt
 
oxygen.o: oxygen.c h2o.h
	$(CC) $(CFLAGS) -pthread -c oxygen.c -o oxygen.o -lrt
 
hydrogen.o: hydrogen.c h2o.h
	$(CC) $(CGLAGS) -pthread -c hydrogen.c -o hydrogen.o -lrt
 
clean:
	rm -f h2o oxygen.o hydrogen.o h2o.o
