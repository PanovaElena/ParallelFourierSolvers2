import sys
import math
sys.path.append("../../scripts/")
import read_file as fr
import graphics as gr
import subprocess
import os
import shutil

DIR_SCRIPT = os.path.abspath("./").replace("\\", "/")
MPI = "mpirun"

if (sys.argv[1] == "1"):
    NUM_PROCESSES = 1
else: NUM_PROCESSES = 10

NAME_PROGRAM = "\""+DIR_SCRIPT+"/../../../build/bin/tight_focusing"+"\""

DIR_RESULTS = DIR_SCRIPT+"/results_%s/" % sys.argv[1]

if (os.path.exists(DIR_RESULTS)): 
	for (dirpath, dirnames, filenames) in os.walk(DIR_RESULTS):
		for file in filenames:
			os.remove(DIR_RESULTS+file)
else: 
    os.mkdir(DIR_RESULTS)
		
command_args = \
					"-solver "+str("PSATD")+" "+\
					\
					"-nseqi "+str(180)+" "+\
					\
					"-dir "+str(DIR_RESULTS)+" "\
					\
					"-factor "+str(4)+" "\
                    "-strip "+str(sys.argv[1])+" ";
					

process = subprocess.Popen(MPI + " -np " + str(NUM_PROCESSES) + " -ppn 1 " + NAME_PROGRAM + " " + command_args, shell=True)
process.wait()

	





