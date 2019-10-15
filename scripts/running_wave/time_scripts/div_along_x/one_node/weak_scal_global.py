import commandArgs
import subprocess

PROGRAM_NAME="../../../../../build/bin/running_wave_global_fft"

MAX_NUM_PROCESS_DEG=4
NUM_PROCESS=[ 2**n for n in range(1, MAX_NUM_PROCESS_DEG+1)]

NUM_THREADS=1

NUM_REPETITION=3

for nProcess in NUM_PROCESS:

	args = {\
		'nx':256*nProcess, 'ny':128, 'nz':128,\
		'gx':16, 'gy':0, 'gz':0,\
		"npari":32, "ndomi":1,\
		"dump":"off",\
		"nthreads":NUM_THREADS,\
		}
		
	for nRep in range(NUM_REPETITION):
		output = "output_wsg_"+str(nProcess)+"_"+str(nRep)+".txt"
		process = subprocess.Popen("mpirun -np "+str(nProcess)+" -ppn "+str(nProcess)+" -wait -maxtime 60 -stdout "+\
			output+" "+PROGRAM_NAME+" "+commandArgs.makeArgs(args), shell=True)
		process.wait()
	