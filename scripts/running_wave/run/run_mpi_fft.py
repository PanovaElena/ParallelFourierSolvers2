import sys
import math
sys.path.append("../../scripts/")
import read_file as fr
import graphics as gr
import subprocess
import os
import shutil
import args

DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0])
if (sys.platform == "win32"):
    MPI = "mpiexec"
else: MPI = "mpirun"

NUM_PROCESSES=2

NAME_PROGRAM = "\""+DIR_SCRIPT+"/../../../build/bin/running_wave_mpi_fftw"+"\""

DIR_RESULTS = "./results/"

if (os.path.exists(DIR_RESULTS)): 
	for (dirpath, dirnames, filenames) in os.walk(DIR_RESULTS):
		for file in filenames:
			os.remove(DIR_RESULTS+file)
else: os.mkdir(DIR_RESULTS)
		
command_args = "-ax "+str(args.ax)+" "+\
					"-ay "+str(args.ay)+" "+\
					"-az "+str(args.az)+" "+\
					\
					"-dx "+str(args.dx)+" "+\
					"-dy "+str(args.dy)+" "+\
					"-dz "+str(args.dz)+" "+\
					\
					"-nx "+str(args.nx)+" "+\
					"-ny "+str(args.ny)+" "+\
					"-nz "+str(args.nz)+" "+\
					\
					"-dt "+str(args.dt)+" "+\
					\
					"-solver "+str(args.solver)+" "+\
					\
					"-nseqi "+str(args.n_iter)+" "+\
					\
					"-dim "+str(args.dimension_of_output_data)+" "\
					\
					"-dir "+str(DIR_SCRIPT+DIR_RESULTS)+" "\
					\
					"-lambda "+str(args.lambd)+" "+\
					"-angle "+str(args.angle)+" ";
					

process = subprocess.Popen(MPI+" -np "+str(NUM_PROCESSES)+" -ppn "+str(NUM_PROCESSES)+" "+NAME_PROGRAM+" "+command_args, shell=True)
process.wait()

	





