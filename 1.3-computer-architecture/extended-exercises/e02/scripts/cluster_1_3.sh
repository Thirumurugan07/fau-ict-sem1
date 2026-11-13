#!/bin/bash -l

#SBATCH --partition=work
#SBATCH --time=00:04:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --exclusive
#SBATCH --cpu-freq=2400000-2400000:performance
#SBATCH --export=NONE

set -x
set -v

export SLURM_EXPORT_ENV=ALL
export SRUN_CPUS_PER_TASK=1

module load intel
make -C .. clean all       # build on the compute node (-xHost)

lscpu | grep -i "model name\|cache"    # cache sizes of this node, for your analysis

STEPS=30                   # number of measurement points (>= 30)
RESULT=result_2_3.csv
echo "ArraySize,MUp/s,ActualRuntime,MinimalRuntime,EdgeLength" > $RESULT

# Edge length for step i of n.
# Total memory n_kib goes from 1 KiB (2^0) to 128 MiB (2^17 KiB), log-spaced.
# Slide formula: edge = sqrt(n_kib * 1024 / (2 * 8)) = sqrt(64 * n_kib)
edge_for_step() {
    awk -v i="$1" -v n="$2" 'BEGIN {
        kib = 2^(17 * i / (n - 1));
        printf "%d\n", sqrt(64 * kib) + 0.5
    }'
}

for (( i = 0; i < STEPS; i++ ))
do
    EDGE=$(edge_for_step $i $STEPS)
    srun ../bin/jacobi $EDGE >> $RESULT
done