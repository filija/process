
CFLAGS=-std=gnu99 -pedantic -Wall -Wextra
 
all: aqua
 
aqua: aqua.o oxygen.o hydrogen.o
	gcc $(CFLAGS) -pthread aqua.o oxygen.o hydrogen.o -o aqua -lrt
 
aqua.o: aqua.c aqua.h
	gcc $(CFLAGS) -pthread -c aqua.c -o aqua.o -lrt
 
oxygen.o: oxygen.c aqua.h
	gcc $(CFLAGS) -pthread -c oxygen.c -o oxygen.o -lrt
 
hydrogen.o: hydrogen.c aqua.h
	gcc $(CGLAGS) -pthread -c hydrogen.c -o hydrogen.o -lrt
 
clean:
	rm -f aqua oxygen.o hydrogen.o aqua.o