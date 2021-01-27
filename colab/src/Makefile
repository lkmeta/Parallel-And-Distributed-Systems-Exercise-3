SHELL := /bin/bash

CC = gcc -O3
RM = rm -f

EXECUTABLES = main main2

all: $(EXECUTABLES)

main: main.c
	$(CC) $< -o ../out/$@ -lm

main2: main2.c
	$(CC) $< -o ../out/$@ -lm
	
clean:
	$(RM) *.o *~ $(EXECUTABLES)

default:
	all