#!/bin/bash
#SBATCH --job-name=DeviceQuery
#SBATCH --nodes=1
#SBATCH --partition=gpu
#SBATCH --gres=gpu:1
#SBATCH --time=5:00

module load gcc/7.3.0 cuda/10.0.130

../out/main_cuda