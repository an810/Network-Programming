CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

# This is the makefile for the exercise1
all: exercise1

exercise1: exercise1.o
	$(CC) exercise1.o -o exercise1 $(LIBS)

exercise1.o: exercise1.c
	$(CC) $(CFLAGS) exercise1.c

clean:
	rm -rf *o exercise1
