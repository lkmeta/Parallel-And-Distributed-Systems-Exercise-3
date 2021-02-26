#!/bin/bash
#SBATCH --job-name=Seq_NLM
#SBATCH --time=01:00:00
#SBATCH --partition=batch
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1

module load gcc/7.3.0 cuda/10.0.130

make main
mkdir ../results

files=(../images/elon_musk_64.jpg ../images/elon_musk_128.jpg ../images/elon_musk_256.jpg ../images/bitcoin_64.jpg ../images/bitcoin_128.jpg ../images/bitcoin_256.jpg)

echo "seq results" > ../results/seq.txt
	
for f in ${files[@]}; do
    echo "" >> ../results/seq.txt
    echo "Running seq for file: "$f >> ../results/seq.txt
    for k in 3 5 7; do
        echo "Command line: ../out/main" $f $k "0.2 1.67" >> ../results/seq.txt
		../out/main $f $k 0.2 1.67>> ../results/seq.txt
    done
	echo "" >> ../results/seq.txt
done
