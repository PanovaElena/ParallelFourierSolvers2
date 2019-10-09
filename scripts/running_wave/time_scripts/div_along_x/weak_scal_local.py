import commandArgs
import subprocess

PROGRAM_NAME="..."

MAX_NUM_NODES_DEG=4
NUM_NODES=[ 2**n for n in range(1, MAX_NUM_NODES_DEG)]

NUM_THREADS=14

NUM_REPETITION=3

for nNodes in NUM_NODES:

	args = {\
		'nx':256*14*nNodes, 'ny':128, 'nz':128,\
		#'nx':128*nNodes, 'ny':32, 'nz':32,\
		'npx':nNodes, 'npy':1, 'npz':1,\
		'gx':8, 'gy':0, 'gz':0,\
		"npari":128, "ndomi":1,\
		"dump":"off",\
		"nthreads":NUM_THREADS,\
		}
		
	for nRep in NUM_REPETITION:
		process = subprocess.Popen("mpirun -np "+str(nNodes)+" -ppn 1 "+PROGRAM_NAME+commandArgs.makeArgs(args), shell=True)
		process.wait()
	