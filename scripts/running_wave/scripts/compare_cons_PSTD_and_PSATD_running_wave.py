
import math
import subprocess
import os
import sys
sys.path.append("../../scripts/")
import plot_graph as pg

OS=sys.argv[1]
FOLDER=""
EXE=""
NAME_MPI="/common/intel/impi/4.1.3.048/intel64/bin/mpirun"
if (OS=="Win"):
	NAME_MPI="mpiexec"
	FOLDER="/Release/"
	EXE=".exe"

DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0]) 

NAME_PROGRAM = "\""+DIR_SCRIPT+"/../../build/src/Examples/RunningWave/ParallelFieldSolver_RunningWave/"+FOLDER+"/runningWave_parallel"+"\""

NAME_FILE = DIR_SCRIPT+"/parallel_results/second_steps.csv"

LIST_FIELD_SOLVERS = ["PSTD", "PSATD"]

LIGHT_SPEED = 2997924580000
D=math.sqrt(6)*math.pi
COURANT_CONDITION_PSTD = math.sqrt(2)*D/(LIGHT_SPEED*math.pi)

DT=[COURANT_CONDITION_PSTD/8, COURANT_CONDITION_PSTD/4, COURANT_CONDITION_PSTD/2]
SOLVER=["PSTD", "PSATD"]

def calc_error(data_1, data_2):	
	error=0
	for i in range(len(data_1)):
		for j in range(len(data_1[i])):
			if (abs(data_1[i][j]-data_2[i][j])>abs(error)): error=data_1[i][j]-data_2[i][j]
	return error
	
data_graph=[]
for dt in DT:

	nIter=int(500*COURANT_CONDITION_PSTD/dt)
	command_args = " --nx "+str(256)+" --ny "+str(256)+" --nz "+str(1)+\
					" --nPar "+str(nIter)+ " --nCons " +str(0)+\
					" --solver %s"+" --mask "+"smooth"+ " --dt "+str(dt)+" --lambda " +str(128)
					
					
	process = subprocess.Popen(NAME_MPI + " -n 2 "+NAME_PROGRAM+" "+(command_args % "PSTD"), shell=True)
	process.wait()
	
	data_PSTD=pg.readFile_2d(NAME_FILE)
	
	process_2 = subprocess.Popen(NAME_MPI + " -n 2 "+NAME_PROGRAM+" "+(command_args % "PSATD"), shell=True)
	process_2.wait()
	
	data_PSATD=pg.readFile_2d(NAME_FILE)
	
	data_graph.append(calc_error(data_PSATD, data_PSTD))
	
	
pg.plot_1d("./", "PSATD_PSTD_par_error", data_graph, DT, "dt", "error", True)	
					
					
					
					