
import math
import subprocess
import os
import sys
sys.path.append("../../scripts/")
import plot_graph as pg

SOLVER=sys.argv[1]
OS=sys.argv[2]
FOLDER=""
EXE=""
NAME_MPI="/common/intel/impi/4.1.3.048/intel64/bin/mpirun"
if (OS=="Win"):
	NAME_MPI="mpiexec"
	FOLDER="/Release/"
	EXE=".exe"
	
DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0]) 

NAME_PROGRAM_PAR = "\""+DIR_SCRIPT+"/../../build/src/Examples/RunningWave/ParallelFieldSolver_RunningWave/"+FOLDER+"/runningWave_parallel"+"\""
NAME_PROGRAM_CONS = "\""+DIR_SCRIPT+"/../../build/src/Examples/RunningWave/ConsistentFieldSolver_RunningWave/"+FOLDER+"/runningWave_consistent"+"\""

NAME_FILE_CONS = DIR_SCRIPT+"/consistent_results/E/cons_res.csv"
NAME_FILE_PAR = DIR_SCRIPT+"/parallel_results/second_steps.csv"

NUM_PROCESSES=2
N_ITER=100
DT=1.2834377209334573e-12

def calc_error(data_cons, data_par):	
	error=0
	for i in range(len(data_par)):
		for j in range(len(data_par[i])):
			if (abs(data_par[i][j]-data_cons[i][j])>abs(error)): error=abs(data_par[i][j]-data_cons[i][j])
	return error
	
def calc_error_arr(data_1, data_2):	
	error=[]
	for i in range(len(data_1)):
		tmp=[]
		for j in range(len(data_1[i])):
			tmp.append(data_1[i][j]-data_2[i][j])
		error.append(tmp)			
	return error
	
data_graph=[]
N=10
D=math.sqrt(6)*math.pi
ANGLE=[]
for i in range(0, N+1):
	ANGLE.append(math.pi*i/2/N)
i=0
for angle in ANGLE:

	Nx=128
	Nz=0
	lambd=0
	if (angle==0):
		Nz=256
		lambd=Nx*D
	elif (angle==math.pi/2):
		Nz=256
		lambd=Nx*D
	else:
		Nz=int(Nx/math.tan(angle))
		angle=math.atan(Nx/Nz)
		lambd=Nx*math.cos(angle)*D
	
	command_args = " --nx "+str(Nx*NUM_PROCESSES)+" --ny "+str(1)+" --nz "+str(Nz)+" -d "+str(D)+\
					" --nPar "+str(N_ITER)+ " --nCons %s --nPar %s --guard 128 "+\
					" --solver "+SOLVER+" --mask "+"smooth"+" --lambda " +str(lambd)+\
					" --angle "+str(180/math.pi*angle) + " --dim 2 " +\
					" --dt "+str(DT)+" --guard 128"
					
	process = subprocess.Popen(NAME_MPI + " -n "+str(NUM_PROCESSES)+" "+NAME_PROGRAM_PAR+" "+command_args%(str(0), str(N_ITER)), shell=True)
	process.wait()
	
	data_par=pg.readFile_2d(NAME_FILE_PAR)
	
	process = subprocess.Popen(NAME_PROGRAM_CONS+" "+command_args%(str(N_ITER), str(0)), shell=True)
	process.wait()
	
	data_cons=pg.readFile_2d(NAME_FILE_CONS)
	
	data_graph.append(calc_error(data_cons, data_par))
	
	pg.plot_2d("./", str(i)+"_cons_error_"+SOLVER+"_angle_"+str(angle),data_cons)
	pg.plot_2d("./", str(i)+"_par_error_"+SOLVER+"_angle_"+str(angle),data_par)
	pg.plot_2d("./", str(i)+"_error_"+SOLVER+"_angle_"+str(angle), calc_error_arr(data_cons, data_par))
	
	i+=1
	
pg.plot_1d("./", "error_"+SOLVER+"_angle", data_graph, ANGLE, "angle", "error", True)	
					
	