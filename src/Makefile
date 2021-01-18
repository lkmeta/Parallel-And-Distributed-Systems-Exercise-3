SHELL := /bin/bash

CC = gcc -O3
RM = rm -f

EXECUTABLES = main

all: $(EXECUTABLES)

main: main.c
	$(CC) $< -o ../out/$@ -lm
	
clean:
	$(RM) *.o *~ $(EXECUTABLES)

default:
	all