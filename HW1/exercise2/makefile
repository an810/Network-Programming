CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

# This is the makefile for the exercise2
all: exercise2

exercise2: exercise2.o
	$(CC) exercise2.o -o exercise2 $(LIBS)

exercise2.o: exercise2.c
	$(CC) $(CFLAGS) exercise2.c

clean:
	rm -rf *o exercise2
