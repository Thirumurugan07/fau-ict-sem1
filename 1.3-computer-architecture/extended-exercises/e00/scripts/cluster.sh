#!/bin/bash -l
  
#SBATCH --partition=work
#SBATCH --time=00:00:30
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --export=NONE

# The script runtime should be less than one millisecond so we choose a very short time for resource allocation (--time parameter).
# Note that the invocation overhead of the bash script might also take some time.

# Enable debug and verbose mode
set -v  # Enable verbose logging
set -e  # Stop script after error occurs

# To ensure 'srun' runs as expected
export SLURM_EXPORT_ENV=ALL
export SRUN_CPUS_PER_TASK=1  # Value should match the SBATCH --cpus-per-task option



run_hello() {
    srun ../bin/hello
}

for (( i = 0; i < 5; i += 1 ))
do
    echo "Run $i"
    run_hello
done
