#!/bin/bash
#SBATCH --job-name=Par_NLM
#SBATCH --time=00:10:00
#SBATCH --partition=gpu
#SBATCH --nodes=1
#SBATCH --gres=gpu:1

module load gcc/7.3.0 cuda/10.0.130

make main_cuda
mkdir ../results

files=(../images/elon_musk_64.jpg ../images/elon_musk_128.jpg ../images/elon_musk_256.jpg ../images/bitcoin_64.jpg ../images/bitcoin_128.jpg ../images/bitcoin_256.jpg)

echo "par results" > ../results/par.txt
	
for f in ${files[@]}; do
	echo "" >> ../results/par.txt
	echo "Running par for file: "$f >> ../results/par.txt
    for k in 3 5 7; do
        echo "Command line: ../out/main_cuda" $f $k "0.2 1.67" >> ../results/par.txt
		../out/main_cuda $f $k 0.2 1.67>> ../results/par.txt
	done
	echo "" >> ../results/par.txt
done
