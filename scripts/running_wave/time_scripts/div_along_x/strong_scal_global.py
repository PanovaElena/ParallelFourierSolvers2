import commandArgs
import subprocess

PROGRAM_NAME="..."

MAX_NUM_NODES_DEG=4
NUM_NODES=[ 2**n for n in range(1, MAX_NUM_NODES_DEG)]

NUM_THREADS=14

NUM_REPETITION=3

for nNodes in NUM_NODES:

	args = {\
		'nx':3584, 'ny':256, 'nz':256,\  # 3584=256*14
		#'nx':128, 'ny':32, 'nz':32,\
		"npari":128, "ndomi":1,\
		"dump":"off",\
		"nthreads":NUM_THREADS,\
		}
		
	for nRep in NUM_REPETITION:
		process = subprocess.Popen("mpirun -np "+str(nNodes)+" -ppn 1 "+PROGRAM_NAME+commandArgs.makeArgs(args), shell=True)
		process.wait()
	