#!/bin/bash
#SBATCH --job-name=DeviceQuery
#SBATCH --nodes=1
#SBATCH --partition=gpu
#SBATCH --gres=gpu:1
#SBATCH --time=5:00

module load gcc/7.3.0 cuda/10.0.130

../out/main_cuda ../images/musk.jpg 3 0.2 1.67

#nvcc -o ../out/main_cuda main_cuda.cu -lm -L/stb_image.h -L/stb_image_write.h