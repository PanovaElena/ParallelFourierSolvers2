import sys
import math
sys.path.append("../../scripts/")
import read_file as rf
import subprocess
import os
import shutil
import args
import graphics as gr
import read_file as fr

DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0])
if (sys.platform == "win32"):
	MPI = ""
else: MPI = "mpirun"

NAME_SEQ_PROGRAM = "\""+DIR_SCRIPT+"/../../../build/bin/running_wave_sequential"+"\""

DIR_RESULTS = "./results/"
DIR_PICTURES = "./pictures/"

if (os.path.exists(DIR_PICTURES)): 
	for (dirpath, dirnames, filenames) in os.walk(DIR_PICTURES):
		for file in filenames:
			os.remove(DIR_PICTURES+file)
else: os.mkdir(DIR_PICTURES)

if (os.path.exists(DIR_RESULTS)): 
	for (dirpath, dirnames, filenames) in os.walk(DIR_RESULTS):
		for file in filenames:
			os.remove(DIR_RESULTS+file)
else: os.mkdir(DIR_RESULTS)

if (args.dimension_of_output_data==1):
	funcRead = fr.readFile1d
	funcPlot = gr.plot1d
else: 
	funcRead = fr.readFile2d
	funcPlot = gr.plot2d

# sequential

command_args_seq = "-ax "+str(args.ax)+" "+\
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
					"-npari "+str(0)+" "+\
					\
					"-dim "+str(args.dimension_of_output_data)+" "\
					\
					"-dir "+str(DIR_SCRIPT+DIR_RESULTS)+" "\
					\
					"-lambda "+str(args.lambd)+" "+\
					"-angle "+str(args.angle)+" ";
					
					
process_seq = subprocess.Popen(MPI+" "+NAME_SEQ_PROGRAM+" "+command_args_seq, shell=True)
process_seq.wait()
	

os.walk(DIR_RESULTS)
for (dirpath, dirnames, filenames) in os.walk(DIR_RESULTS):
	for file in filenames:
		funcPlot(DIR_PICTURES, file, funcRead(DIR_RESULTS+file))



