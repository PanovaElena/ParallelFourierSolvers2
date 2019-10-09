
import math
import subprocess
import os
import sys
sys.path.append("../../scripts/")
import graphics as gr
import read_file as fr
import compute_error as ce

import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.colors import LinearSegmentedColormap
import pylab as pl

SOLVER="PSATD"
OS="Win"
FOLDER=""
EXE=""
NAME_MPI="/common/intel/impi/4.1.3.048/intel64/bin/mpirun"
if (OS=="Win"):
	NAME_MPI="mpiexec"
	FOLDER="/Release/"
	EXE=".exe"
	
DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0]) 

NAME_PROGRAM_SEQ = "\""+DIR_SCRIPT+"/../../../build/src/tasks/running_wave/running_wave_sequential/"+FOLDER+"/running_wave_sequential"+"\""
NAME_PROGRAM_PAR = "\""+DIR_SCRIPT+"/../../../build/src/tasks/running_wave/running_wave_parallel/"+FOLDER+"/running_wave_parallel"+"\""

DIR_RESULTS = "./results/"

NAME_FILE_SEQ = DIR_RESULTS+"/sequential_result.csv"
NAME_FILE_PAR = DIR_RESULTS+"/parallel_result.csv"

NUM_PROCESSES=2
N_ITER=1

def calc_error(data_cons, data_par):	
	error=0
	for i in range(len(data_par)):
		if (abs(data_par[i]-data_cons[i])>abs(error)): error=abs(data_par[i]-data_cons[i])
	return error
	
data_graph=[]
data_graph_sqr=[]
D=math.sqrt(6)*math.pi
#N_GUARDS=[10, 20, 30, 50, 70, 90, 120, 150, 180, 220, 256]
N_GUARDS=range(10, 256, 10)
N_GUARDS_SQR=[]



with open("error_"+SOLVER+"_n_guards.csv", 'w') as file:
	for n_guards in N_GUARDS:
		
		command_args = " -nx 512 -ny 1 -nz 512"+\
						" -nseqi %s -npari %s "+\
						" -lambda 16 "+\
						" -gx "+str(n_guards)+" -gy 0 -gz 0"+\
						" -npx 2 -npy 1 -npz 1"+\
						" -mask smooth -filter on "+\
						" -fwx 64 -fwy 0 -fwz 0 -fnzx 64 -fnzy 0 -fnzz 0 "+\
						" -solver "+SOLVER+\
						" -scheme copy "+\
						" -dir "+str(DIR_SCRIPT+DIR_RESULTS)+" ";
						
		process = subprocess.Popen(NAME_MPI + " -n "+str(NUM_PROCESSES)+" "+NAME_PROGRAM_PAR+" "+command_args%(str(0), str(N_ITER)), shell=True)
		process.wait()
		
		data_par=fr.readFile1d(NAME_FILE_PAR)
		
		process = subprocess.Popen(NAME_PROGRAM_SEQ+" "+command_args%(str(N_ITER), str(0)), shell=True)
		process.wait()
		
		data_cons=fr.readFile1d(NAME_FILE_SEQ)
		
		error=calc_error(data_cons, data_par)
		data_graph.append(error)
		
		
		if((n_guards-20)%40==0):
			data_graph_sqr.append(error)
			N_GUARDS_SQR.append(n_guards)
		
		file.write(str(error)+"\n")
	
mpl.rcParams.update({'font.size': 16})
	
#gr.plot1d("./", "error_"+SOLVER+"_n_guards", data_graph, N_GUARDS, "n_guards", "error", True, _log=True)	
fig = plt.figure()
ax = fig.add_subplot(111)
# ax.semilogy(N_GUARDS, data_graph)
# ax.semilogy(N_GUARDS, data_graph, "*")
# ax.semilogy(N_GUARDS_SQR, data_graph_sqr, '--')
ax.plot(N_GUARDS, data_graph)
ax.plot(N_GUARDS, data_graph, "*")
ax.plot(N_GUARDS_SQR, data_graph_sqr, '--')
pl.xlabel('Nguards')
pl.ylabel('error')
ax.grid()

plt.tight_layout()
plt.savefig("error_"+SOLVER+"_n_guards"+".png")

fig = plt.figure()
ax = fig.add_subplot(111)
ax.semilogy(N_GUARDS_SQR, data_graph_sqr, '--g')
pl.xlabel('Nguards')
pl.ylabel('error')
ax.grid()

plt.tight_layout()
plt.savefig("error_"+SOLVER+"_n_guards_log"+".png")