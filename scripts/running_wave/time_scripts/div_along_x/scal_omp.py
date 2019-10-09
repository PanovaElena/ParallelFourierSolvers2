import commandArgs
import subprocess

PROGRAM_NAME="..."

NUM_REPETITION=3

NUM_THREADS=[ 1, 2, 4, 8, 12, 14]

for nThreads in THREADS:

	args = {\
		'nx':64*14, 'ny':128, 'nz':128,\
		"nseqi":128,\
		"dump":"off",\
		"nthreads":nThreads,\
		}
		
	for nRep in NUM_REPETITION:
		process = subprocess.Popen(PROGRAM_NAME+commandArgs.makeArgs(args), shell=True)
		process.wait()
	