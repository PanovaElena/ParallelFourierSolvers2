import sys
import math
sys.path.append("../../scripts/")
import subprocess
import os
import shutil
import graphics as gr
import read_file as rf
import compute_error as ce
import args

DIR_PICTURES = "./pictures/"
DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0]) 

if (sys.platform == "win32"):
	MPI = "mpiexec"
else: MPI = "mpirun"

NAME_SEQ_PROGRAM = "\""+DIR_SCRIPT+"/../../../build/bin/spherical_wave_sequential"+"\""
NAME_PAR_PROGRAM = "\""+DIR_SCRIPT+"/../../../build/bin/spherical_wave_parallel"+"\""

DIR_PICTURES = "./pictures/"
DIR_RESULTS = "./results/"

NAME_FILE_SEQ = DIR_RESULTS+"/sequential_result.csv"
NAME_FILE_PAR = DIR_RESULTS+"/parallel_result.csv"

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
		
funcRead = rf.readFile2d
funcPlot = gr.plot2d
funcCalcError = ce.computeError2d

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
					\
					"-dim "+str(args.dimension_of_output_data)+" "+\
					\
					"-dir "+str(DIR_SCRIPT+DIR_RESULTS)+" "+\
					"-dump on "+\
					\
					"-scx "+str(args.coord_source_x)+" "+\
					"-scy "+str(args.coord_source_y)+" "+\
					"-scz "+str(args.coord_source_z)+" "+\
					\
					"-swx "+str(args.width_source_x)+" "+\
					"-swy "+str(args.width_source_y)+" "+\
					"-swz "+str(args.width_source_z)+" "+\
					\
					"-somega "+str(args.omega)+" "+\
					"-somenv "+str(args.omega_envelope)+" "+\
					"-stimest "+str(args.start_time_source)+" "+\
					"-stime "+str(args.time_source)+" ";
					
		
process_seq = subprocess.Popen(NAME_SEQ_PROGRAM+" "+command_args_seq, shell=True)
process_seq.wait()


# parallel

command_args_par = "-ax "+str(args.ax)+" "+\
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
					"-npx "+str(args.npx)+" "+\
					"-npy "+str(args.npy)+" "+\
					"-npz "+str(args.npz)+" "+\
					\
					"-gx "+str(args.gx)+" "+\
					"-gy "+str(args.gy)+" "+\
					"-gz "+str(args.gz)+" "+\
					\
					"-nseqi "+str(args.n_sequential_iter)+" "+\
					"-npari "+str(args.n_parallel_iter)+" "+\
					"-ndomi "+str(args.number_of_iterations_in_domain)+" "+\
					\
					"-scheme "+str(args.scheme)+" "+\
					\
					"-mask "+str(args.mask)+" "+\
					"-mwx "+str(args.mwx)+" "+\
					"-mwy "+str(args.mwy)+" "+\
					"-mwz "+str(args.mwz)+" "+\
					\
					"-filter "+str(args.filter)+" "+\
					"-fwx "+str(args.fwx)+" "+\
					"-fwy "+str(args.fwy)+" "+\
					"-fwz "+str(args.fwz)+" "+\
					"-fnzx "+str(args.fnzx)+" "+\
					"-fnzy "+str(args.fnzy)+" "+\
					"-fnzz "+str(args.fnzz)+" "+\
					\
					"-dim "+str(args.dimension_of_output_data)+" "+\
					\
					"-dir "+str(DIR_SCRIPT+DIR_RESULTS)+" "+\
					"-dump on "+\
					\
					"-scx "+str(args.coord_source_x)+" "+\
					"-scy "+str(args.coord_source_y)+" "+\
					"-scz "+str(args.coord_source_z)+" "+\
					\
					"-swx "+str(args.width_source_x)+" "+\
					"-swy "+str(args.width_source_y)+" "+\
					"-swz "+str(args.width_source_z)+" "+\
					\
					"-somega "+str(args.omega)+" "+\
					"-somenv "+str(args.omega_envelope)+" "+\
					"-stimest "+str(args.start_time_source)+" "+\
					"-stime "+str(args.time_source)+" ";
					
np=args.npx*args.npy*args.npz

if(args.n_parallel_iter!=0):					
	process_par = subprocess.Popen(MPI+" -n "+str(np)+" "+NAME_PAR_PROGRAM+" "+command_args_par, shell=True)
	process_par.wait()


# plot

os.walk(DIR_RESULTS)
for (dirpath, dirnames, filenames) in os.walk(DIR_RESULTS):
	for file in filenames:
		funcPlot(DIR_PICTURES, file, funcRead(DIR_RESULTS+file))

if (args.n_parallel_iter != 0):
	data_par = funcRead(NAME_FILE_PAR)
	data_seq = funcRead(NAME_FILE_SEQ)
	funcPlot(DIR_PICTURES, "error", funcCalcError(data_seq, data_par))

	





