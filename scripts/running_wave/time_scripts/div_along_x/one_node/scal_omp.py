import commandArgs
import subprocess

PROGRAM_NAME="../../../../../build/src/tasks/running_wave/running_wave_sequential/running_wave_sequential"

NUM_REPETITION=3

NUM_THREADS=[1, 2, 4, 8, 12, 14, 16]

for nThreads in NUM_THREADS:

	args = {\
		'nx':4096, 'ny':128, 'nz':128,\
		"nseqi":32,\
		"dump":"off",\
		"nthreads":nThreads,\
		}
		
	for nRep in range(NUM_REPETITION):
		output = "output_"+str(nThreads)+"_"+str(nRep)+".txt"
		process = subprocess.Popen("mpirun -wait -stdiodir ./scal_omp -stdout "+output+" "+PROGRAM_NAME+" "+commandArgs.makeArgs(args), shell=True)
		process.wait()