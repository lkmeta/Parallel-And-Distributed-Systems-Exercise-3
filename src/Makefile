SHELL := /bin/bash

# ============================================
# COMMANDS

CC = gcc -O3
RM = rm -f

# ==========================================
# TARGETS

EXECUTABLES = main cuda

default: all

all: $(EXECUTABLES)

main: main.c
	$(CC) $< -o ../out/$@ -lm

cuda: main_cuda.cu
	nvcc -o ../out/main_cuda main_cuda.cu -lm -L/stb_image.h -L/stb_image_write.h

.PHONY: clean

# ==========================================
# TESTS

testMain:
	../out/main ../image/elon_musk_64.jpg 5 0.2 1.67

testCuda:
	../out/cuda ../image/elon_musk_128.jpg 7 0.2 1.67


# ==========================================
# CLEAN 

clean:
	$(RM) *.o *~ $(EXECUTABLES)
