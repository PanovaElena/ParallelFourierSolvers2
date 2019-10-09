import subprocess;
import sys
sys.path.append("../../scripts/")
import math
import matplotlib.pyplot as plt
from pylab import *
import matplotlib as mpl
import plot_graph as pg

#NUM_OF_PROCESSES = int(sys.argv[1])
#FIELD_SOLVER = sys.argv[2];

N = 256
GUARD = 40
LIGHT_SPEED = 29979245800
D = 1
COEF=2
DT = D/LIGHT_SPEED/COEF

arrL=[[8, 32],[32]]
arrNT=[20, 100]
arrDt_s=[DT/100, DT/2]
arrSolver=["PSTD", "PSATD"]

for ind in range(1, 2):
	FIELD_SOLVER=arrSolver[ind]
	fig = plt.figure()
	ax = fig.add_subplot(111)
	
	for lambdaN in arrL[ind]:
		DIR_OUT = "./courant_results/"
		NAME_FILE_OUT=DIR_OUT+"/courant_solver_"+FIELD_SOLVER+".csv"
		NAME_GRAPH_OUT=DIR_OUT+"/courant_solver_"+FIELD_SOLVER+".png"
		
		for NUM_OF_PROCESSES in range(1,3):
			
			if (NUM_OF_PROCESSES==1):
				NAME_FILE_IN = "./consistent_results/E/cons_res.csv"
				NAME_PROGRAM = "\""+"../../build/src/Examples/RunningWave/ConsistentFieldSolver_RunningWave/Release/runningWave_consistent"+"\""
				CALL = "cons"
			else:
				NAME_FILE_IN = "./parallel_results/second_steps.csv"
				NAME_PROGRAM = "\""+"../../build/src/Examples/RunningWave/ParallelFieldSolver_RunningWave/Release/runningWave_parallel"+"\""
				CALL = "par"
				
			def callProcess(_lambdaN, _dt, _nIter):
				if (CALL == "cons"):
					args=" --dt "+str(_dt)+" --solver "+FIELD_SOLVER+\
						" --nx "+str(N)+" --ny "+str(N)+" --nz "+str(1)+\
						" --lambdaN "+str(_lambdaN)+\
						" --nCons "+str(_nIter)+" -d "+str(D)
					process = subprocess.Popen(NAME_PROGRAM+" "+args, shell=True)
					process.wait()
				else:
					args=" --dt "+str(_dt)+" --solver "+FIELD_SOLVER+\
						" --nx "+str(N)+" --ny "+str(N)+" --nz "+str(1)+\
						" --lambdaN "+str(_lambdaN)+\
						" --nCons 0 --nPar "+str(_nIter)+" -d "+str(D)
					process = subprocess.Popen("mpiexec -n "+str(NUM_OF_PROCESSES)+" "+NAME_PROGRAM+" "+args, shell=True)
					process.wait()
			
			def f(x, t, lambdaN):
				return sin(2*math.pi*(x-LIGHT_SPEED*t)/(lambdaN*D))
					
			def calcError(data, t, lambdaN):
				error=0
				for j in range(N):
					error+=(data[j]-f(j*D, t, lambdaN))**2
				return error/N
			
			arrCour=[]; arrError=[]
			N_T=arrNT[ind]
			lambd = lambdaN*D
			omega=2*math.pi*LIGHT_SPEED/lambd
			dt_s=arrDt_s[ind]
			dt_step=(DT-dt_s)/N_T
			for i in range(N_T):
				print(i, "\n")
				
				dt=dt_s+dt_step*i
				T=2*math.pi/omega
				nIter=10
				#nIter=int(T/dt_s)+1
				#dt=T/nIter
				
				print(nIter)
				print(dt)
				
				callProcess(lambdaN, dt, nIter)
				dat=pg.readFile_1d(NAME_FILE_IN)
				error=calcError(dat, dt*nIter, lambdaN)
				
				arrCour.insert(i, LIGHT_SPEED*dt/D)
				arrError.insert(i, error)
				
			with open(NAME_FILE_OUT, 'a') as file:
				for i in range(len(arrError)):
					file.write(str(arrCour[i])+";"+str(arrError[i])+"\n")
				file.write("\n")
		
	
			#ax.plot(arrDtDisp, arrVDisp)
			if (CALL=="cons"):
				ax.plot(arrCour, arrError)
			else:
				ax.plot(arrCour, arrError, "--")
				
	
	ax.set_xlabel("c*dt/dx")
	ax.set_ylabel("error")
	plt.tight_layout()
	#legend(["sequential, lambda=8", "parallel, lambda=8", "sequential, lambda=32", "parallel, lambda=32"])	
	legend(["sequential, lambda=32", "parallel, lambda=32"])
		
	plt.savefig(NAME_GRAPH_OUT)