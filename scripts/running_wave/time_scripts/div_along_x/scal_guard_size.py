import commandArgs
import subprocess

PROGRAM_NAME="..."

NUM_GUARDS=[4, 8, 16, 32, 48]

NUM_NODES=16
NUM_THREADS=14

NUM_REPETITION=3

for nGuards in NUM_GUARDS:

	args = {\
		'nx':1024, 'ny':256, 'nz':256,\
		#'nx':128, 'ny':32, 'nz':32,\
		'npx':NUM_NODES, 'npy':1, 'npz':1,\
		'gx':nGuards, 'gy':0, 'gz':0,\
		"npari":128, "ndomi":1,\
		"dump":"off",\
		"nthreads":NUM_THREADS,\
		}
		
	for nRep in NUM_REPETITION:
		process = subprocess.Popen("mpirun -np "+str(NUM_NODES)+" -ppn 1 "+PROGRAM_NAME+commandArgs.makeArgs(args), shell=True)
		process.wait()
	