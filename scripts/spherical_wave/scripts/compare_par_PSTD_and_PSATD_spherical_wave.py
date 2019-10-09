
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

NAME_PROGRAM = "\""+DIR_SCRIPT+"/../../build/src/Examples/SphericalWave/ParallelFieldSolver_SphericalWave/"+FOLDER+"/sphericalWave_parallel"+"\""

NAME_FILE = DIR_SCRIPT+"/parallel_results/second_steps.csv"

LIST_FIELD_SOLVERS = ["PSTD", "PSATD"]

LIGHT_SPEED = 2997924580000
D=LIGHT_SPEED
COURANT_CONDITION_PSTD = math.sqrt(2)*D/(LIGHT_SPEED*math.pi)

DT=[COURANT_CONDITION_PSTD/8*1e-4, COURANT_CONDITION_PSTD/4*1e-4, COURANT_CONDITION_PSTD/2*1e-4]
SOLVER=["PSTD", "PSATD"]

def calc_error(data_1, data_2):	
	error=0
	for i in range(len(data_1)):
		for j in range(len(data_1[i])):
			if (abs(data_1[i][j]-data_2[i][j])>abs(error)): error=data_1[i][j]-data_2[i][j]
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
for dt in DT:

	nIter=int(100*COURANT_CONDITION_PSTD/dt)
	command_args = " --nx "+str(256)+" --ny "+str(256)+" --nz "+str(1)+\
					" --nPar "+str(int(0.8*nIter))+ " --nCons " +str(int(0.2*nIter))+\
					" --solver %s"+" --mask "+"smooth"+ " --dt "+str(dt)
					
					
	process = subprocess.Popen(NAME_MPI + " -n 2 "+NAME_PROGRAM+" "+(command_args % "PSTD"), shell=True)
	process.wait()
	
	data_PSTD=pg.readFile_2d(NAME_FILE)
	
	process_2 = subprocess.Popen(NAME_MPI + " -n 2 "+NAME_PROGRAM+" "+(command_args % "PSATD"), shell=True)
	process_2.wait()
	
	data_PSATD=pg.readFile_2d(NAME_FILE)
	
	data_graph.append(calc_error(data_PSATD, data_PSTD))
	
	pg.plot_2d("./", "PSTD_dt_%s_nIter_%s" % (dt, nIter), data_PSTD)
	pg.plot_2d("./", "PSATD_dt_%s_nIter_%s" % (dt, nIter), data_PSATD)
	pg.plot_2d("./", "PSATD_PSTD_error_graph_dt_%s" % dt, calc_error_arr(data_PSATD, data_PSTD))
	
pg.plot_1d("./", "PSATD_PSTD_par_error", data_graph, DT, "dt", "error", True)	
					
					
					
					