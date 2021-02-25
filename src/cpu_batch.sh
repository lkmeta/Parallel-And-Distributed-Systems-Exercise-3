#!/bin/bash
#SBATCH --nodes=1
#SBATCH --partition=batch
#SBATCH --time=5:00

module load gcc

../out/main ../images/musk_256.jpg 3 0.2 1.67