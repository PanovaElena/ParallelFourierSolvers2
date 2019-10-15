import commandArgs
import subprocess

PROGRAM_NAME="../../../../../build/bin/running_wave_global_fft_mpi"

NUM_REPETITION=3

NUM_PROCESSES=[1, 2, 4, 8, 16]

GRID_SIZES=[{'nx':4096, 'ny':128, 'nz':128},\
           {'nx':128,  'ny':4096, 'nz':128},\
           {'nx':128,  'ny':128,  'nz':4096},\
           {'nx':2048, 'ny':2048, 'nz':16},\
           {'nx':16,   'ny':2048, 'nz':2048},\
           {'nx':2048, 'ny':16,   'nz':2048},\
           ]

for gridSize in GRID_SIZES:
    for nProcesses in NUM_PROCESSES:
    
        args = {\
            'nx':gridSize['nx'], 'ny':gridSize['ny'], 'nz':gridSize['nz'],\
            "nseqi":32,\
            "dump":"off",\
            "nthreads":1,\
            }
            
        for nRep in range(NUM_REPETITION):
            output = "output_"+str(gridSize['nx'])+"x"+str(gridSize['ny'])+"x"+str(gridSize['nz'])+"_prc_"+str(nProcesses)+"_"+str(nRep)+".txt"
            process = subprocess.Popen("mpirun -np "+str(nProcesses)+" -ppn "+str(nProcesses)+" -wait -maxtime 3 -stdout "+output+\
                " "+PROGRAM_NAME+" "+commandArgs.makeArgs(args), shell=True)
            process.wait()