import commandArgs
import subprocess
import os
import sys

PROGRAM_NAME="../../../../../build/src/tasks/running_wave/running_wave_just_parallel/running_wave_just_parallel"

NUM_THREADS=1
NUM_PROCESS=2

args = {\
	"nx":128, "ny":32, "nz":32,\
	"npx":NUM_PROCESS, "npy":1, "npz":1,\
	"gx":8, "gy":0, "gz":0,\
	"nseqi":0, "npari":32, "ndomi":1,\
	"dump":"off",\
	"nthreads":NUM_THREADS,\
	}
	
process = subprocess.Popen("mpirun -np "+str(NUM_PROCESS)+" -ppn "+str(NUM_PROCESS)+" "+PROGRAM_NAME+" "+commandArgs.makeArgs(args), shell=True)
process.wait()