#!/bin/bash -l
  
#SBATCH --partition=work
#SBATCH --time=00:07:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --exclusive
#SBATCH --cpu-freq=2400000-2400000:performance
#SBATCH --export=NONE

# Enable debug and verbose mode
set -x
set -v

# To ensure 'srun' runs as expected
export SLURM_EXPORT_ENV=ALL
export SRUN_CPUS_PER_TASK=1  # value should match the SBATCH --cpus-per-task option

module load intel          # NEW: --export=NONE starts the job without your modules
make -C .. clean all       # NEW: build on the compute node, so -xHost matches this CPU

# This line creates / overrides a result csv file
echo "ArraySize,AdditionsPerSecond,ActualRuntime,MinimalRuntime" > result.csv

# TODO 1.2 Run benchmark with varying runtime and fixed array size
REPS=3   # repetitions per runtime, to see the spread

for (( MIN_MS = 1; MIN_MS <= 8192; MIN_MS *= 2 ))
do
    for (( REP = 1; REP <= REPS; REP++ ))
    do
        srun ../bin/vecSum 512 $MIN_MS >> result.csv
    done
done


# TODO 1.3 Run benchmark with varying array sizes and fixed runtime
# execute measurement with for loop
# 16 measurement points, exponentially distributed: 1 KiB (2^10 B) - 32 MiB (2^25 B), each with 1000 ms set as the minimal runtime.
# results should be appended to the result.csv (see '>>' operator)
# srun ../bin/vecSum ...

# Since we want to plot the data in upcoming exercises we benefit from a flexible resolution on the step size.
# Increase the number of measurement points from 16 to 30 or higher.
# Add a STEPS variable which specifies this number of measurement points
# Hint: the use of 'awk', 'bc' or a similar program might be helpful to calculate math expressions.
# Hint: you can write a bash function which retrieves the current step and total number of steps and returns the KiB 
