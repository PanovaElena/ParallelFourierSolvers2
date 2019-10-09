import commandArgs
import subprocess

PROGRAM_NAME="../../../../../build/src/tasks/running_wave/running_wave_just_parallel/running_wave_just_parallel"

MAX_NUM_PROCESS_DEG=4
NUM_PROCESS=[ 2**n for n in range(1, MAX_NUM_PROCESS_DEG+1)]

NUM_GUARDS=[8, 16, 32, 48, 64, 96, 128]

NUM_THREADS=1

NUM_REPETITION=3

for nProcess in NUM_PROCESS:
	for nGuards in NUM_GUARDS:
	
		args = {\
			'nx':4096, 'ny':64, 'nz':64,\
			#'nx':128, 'ny':32, 'nz':32,\
			'npx':nProcess, 'npy':1, 'npz':1,\
			'gx':nGuards, 'gy':0, 'gz':0,\
			"npari":32, "ndomi":1,\
			"dump":"off",\
			"nthreads":NUM_THREADS,\
			}
			
		for nRep in range(NUM_REPETITION):
			output = "output_gs_"+str(nProcess)+"_"+str(nGuards)+"_"+str(nRep)+".txt"
			process = subprocess.Popen("mpirun -np "+str(nProcess)+" -ppn "+str(nProcess)+" -wait -stdout "+\
				output+" "+PROGRAM_NAME+" "+commandArgs.makeArgs(args), shell=True)
			process.wait()