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

lscpu | grep -i "model name\|cache"    # records this node's cache sizes for your analysis

STEPS=30                   # number of measurement points (>= 30)
RESULT=result_1_3.csv
echo "ArraySize,AdditionsPerSecond,ActualRuntime,MinimalRuntime" > $RESULT

# Returns the array size in KiB for step i of n:
# exponentially spaced from 2^0 = 1 KiB to 2^15 = 32768 KiB = 32 MiB
step_kib() {
    awk -v i="$1" -v n="$2" 'BEGIN { printf "%d\n", 2^(15*i/(n-1)) + 0.5 }'
}

for (( i = 0; i < STEPS; i++ ))
do
    KIB=$(step_kib $i $STEPS)
    srun ../bin/vecSum $KIB 1000 >> $RESULT
done