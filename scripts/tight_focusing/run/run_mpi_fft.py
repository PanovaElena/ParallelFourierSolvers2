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

NUM_PROCESSES=10

NAME_PROGRAM = "\""+DIR_SCRIPT+"/../../../build/bin/tight_focusing"+"\""

DIR_RESULTS = "./results/"

if (os.path.exists(DIR_RESULTS)): 
	for (dirpath, dirnames, filenames) in os.walk(DIR_RESULTS):
		for file in filenames:
			os.remove(DIR_RESULTS+file)
else: 
    os.mkdir(DIR_RESULTS)
		
command_args = \
					"-solver "+str(args.solver)+" "+\
					\
					"-nseqi "+str(args.n_iter)+" "+\
					\
					"-dir "+str(DIR_SCRIPT+DIR_RESULTS)+" "\
					\
					"-factor "+str(args.factor)+" ";
					

process = subprocess.Popen(MPI+" -np "+str(NUM_PROCESSES)+" -ppn 1 "+NAME_PROGRAM+" "+command_args, shell=True)
process.wait()

	





