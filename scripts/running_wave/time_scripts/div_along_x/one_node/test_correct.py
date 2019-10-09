import commandArgs
import subprocess
import os
import sys

PROGRAM_NAME="../../../../../build/src/tasks/running_wave/running_wave_just_parallel/running_wave_just_parallel"
DIR_RESULTS="./results/"

if (os.path.exists(DIR_RESULTS)): 
	for (dirpath, dirnames, filenames) in os.walk(DIR_RESULTS):
		for file in filenames:
			os.remove(DIR_RESULTS+file)
else: os.mkdir(DIR_RESULTS)

NUM_THREADS=4
NUM_PROCESS=2

args = {\
	"nx":32, "ny":32, "nz":32,\
	"npx":NUM_PROCESS, "npy":1, "npz":1,\
	"gx":8, "gy":0, "gz":0,\
	"npari":1, "ndomi":1,\
	"dump":"on",\
	"dir":DIR_RESULTS,\
	"dim":1,\
	"nthreads":NUM_THREADS,\
	}
	
process = subprocess.Popen("mpirun -np "+str(NUM_PROCESS)+" -ppn "+str(NUM_PROCESS)+" "+PROGRAM_NAME+" "+commandArgs.makeArgs(args), shell=True)
process.wait()