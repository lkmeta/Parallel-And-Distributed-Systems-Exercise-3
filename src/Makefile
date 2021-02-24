SHELL := /bin/bash

CC = gcc -O3
RM = rm -f

EXECUTABLES = main2 main_cuda

all: $(EXECUTABLES)

main: main.c
	$(CC) $< -o ../out/$@ -lm

main2: main2.c
	$(CC) $< -o ../out/$@ -lm

test: test.c
	$(CC) $< -o ../out/$@ -lm

main_cuda: main_cuda.cu
	nvcc -o ../out/main_cuda main_cuda.cu -lm -L/stb_image.h -L/stb_image_write.h
	
clean:
	$(RM) *.o *~ $(EXECUTABLES)

default:
	all