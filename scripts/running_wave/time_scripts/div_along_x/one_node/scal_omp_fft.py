import commandArgs
import subprocess

PROGRAM_NAME="../../../../../build/bin/running_wave_global_fft_omp"

NUM_REPETITION=3

NUM_THREADS=[1, 2, 4, 8, 16]

GRID_SIZES=[{'nx':4096, 'ny':128, 'nz':128},\
           {'nx':128,  'ny':4096, 'nz':128},\
           {'nx':128,  'ny':128,  'nz':4096},\
           {'nx':2048, 'ny':2048, 'nz':16},\
           {'nx':16,   'ny':2048, 'nz':2048},\
           {'nx':2048, 'ny':16,   'nz':2048}]

for gridSize in GRID_SIZES:
    for nThreads in NUM_THREADS:
    
        args = {\
            'nx':gridSize['nx'], 'ny':gridSize['ny'], 'nz':gridSize['nz'],\
            "nseqi":32,\
            "dump":"off",\
            "nthreads":nThreads,\
            }
            
        for nRep in range(NUM_REPETITION):
            output = "output_"+str(gridSize['nx'])+"x"+str(gridSize['ny'])+"x"+str(gridSize['nz'])+"_thr_"+str(nThreads)+"_"+str(nRep)+".txt"
            process = subprocess.Popen("mpirun -wait -maxtime 4 -stdout "+output+" "+PROGRAM_NAME+" "+commandArgs.makeArgs(args), shell=True)
            process.wait()