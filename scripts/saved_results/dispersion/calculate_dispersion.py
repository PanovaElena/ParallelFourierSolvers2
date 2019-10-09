import subprocess;
import sys
sys.path.append("../../scripts/")
import math
import matplotlib.pyplot as plt
from pylab import *
import matplotlib as mpl
import graphics as pg
import read_file as rf
import os

#NUM_OF_PROCESSES = int(sys.argv[1])
#FIELD_SOLVER = sys.argv[2];

fig = plt.figure()
ax = fig.add_subplot(111)

DIR_OUT = "./dispersion/"
if (not os.path.exists(DIR_OUT)): os.mkdir(DIR_OUT)
NAME_FILE_OUT=DIR_OUT+"/dispersion.csv"
NAME_GRAPH_OUT=DIR_OUT+"/dispersion"

N = 256
GUARD = 32
LIGHT_SPEED = 29979245800
D = 1
COEF=2
DT = D/LIGHT_SPEED/COEF
		
lambdaN=16
lambd = lambdaN*D
omega=2*math.pi*LIGHT_SPEED/lambd

N_T=20
dt_s=DT/100
dt_f=DT/2
dt_step=(dt_f-dt_s)/N_T
T=2*math.pi/omega
		
def findV(arr, lambd, t):
	#return LIGHT_SPEED*(1-math.asin(arr[0])/lambd)
	return -math.asin(arr[0])*lambd/(2*math.pi*t)
	
def dispPSTD(_omega, _dt):
	return 2/LIGHT_SPEED/_dt*math.sin(_omega*_dt/2)

with open(NAME_FILE_OUT, 'w') as file:
	print("file csv was cleared")
		
for FIELD_SOLVER in ["PSTD","PSATD"]:
	for NUM_OF_PROCESSES in [1]:
		
		if (NUM_OF_PROCESSES==1):
			NAME_FILE_IN = DIR_OUT+"/sequential_result.csv"
			NAME_PROGRAM = "\""+"../../../build/src/tasks/running_wave/running_wave_sequential/Release/running_wave_sequential"+"\""
			CALL = "seq"
		else:
			NAME_FILE_IN = DIR_OUT+"/parallel_result.csv"
			NAME_PROGRAM = "\""+"../../../build/src/tasks/running_wave/running_wave_parallel/Release/running_wave_parallel"+"\""
			CALL = "par"
			
		def callProcess(_lambda, _dt, _nIter):
			if (CALL == "seq"):
				args=" -dt "+str(_dt)+" -solver "+FIELD_SOLVER+\
					" -nx "+str(N)+" -ny "+str(1)+" -nz "+str(N)+\
					" -lambda "+str(_lambda)+\
					" -nseqi "+str(_nIter)+\
					" -dx "+str(D)+" -dy "+str(D)+" -dz "+str(D)+\
					" -dir "+DIR_OUT
				process = subprocess.Popen(NAME_PROGRAM+" "+args, shell=True)
				process.wait()
			else:
				args=" -dt "+str(_dt)+" -solver "+FIELD_SOLVER+\
					" -nx "+str(N)+" -nz "+str(N)+" -ny "+str(1)+\
					" -gx "+str(GUARD)+" -gy 0 -gz 0 "+\
					" -dx "+str(D)+" -dy "+str(D)+" -dz "+str(D)+\
					" -npx "+str(NUM_OF_PROCESSES)+" -npy 1 -npz 1 "+\
					" -lambda "+str(_lambda)+\
					" -mask smooth -mwx 8 -mwy 0 -mwz 0 "+\
					" -nseqi 0 -npari "+str(_nIter)+" -d "+str(D)+\
					" -filter on -fwx 8 -fwy 0 -fwz 0 -fnzx 4 -fnzy 0 -fnzz 0 "+\
					" -dir "+DIR_OUT
				process = subprocess.Popen("mpiexec -n "+str(NUM_OF_PROCESSES)+" "+NAME_PROGRAM+" "+args, shell=True)
				process.wait()
		
		arrDt=[]; arrV=[]
		
		for i in range(N_T+1):
			print(i, "\n")
			
			dt=dt_s+dt_step*i
			# nIter=int(T/dt)+1
			# dt=T/nIter
			nIter=1
			
			print(nIter)
			print(dt)
			
			callProcess(lambd, dt, nIter)
			dat=rf.readFile1d(NAME_FILE_IN)
			v=findV(dat, lambd, nIter*dt)
			
			arrDt.insert(i, T/dt)
			arrV.insert(i, (v-LIGHT_SPEED)/LIGHT_SPEED)
			
		with open(NAME_FILE_OUT, 'a') as file:
			file.write(str(FIELD_SOLVER)+"\n")
			for i in range(len(arrV)):
				file.write(str(arrDt[i])+";"+str(arrV[i])+"\n")
			file.write("\n")
		
		if (CALL=="seq"):
			if (FIELD_SOLVER=="PSTD"): ax.plot(arrDt, arrV, 'y')
			else: ax.plot(arrDt, arrV, 'g')
		else:
			ax.plot(arrDt, arrV, "--")
			
arrDtDisp=[]; arrVDisp=[]
for i in range(500):
	dt=dt_s+(dt_f-dt_s)/500*i
	
	arrDtDisp.insert(i, T/dt)
	arrVDisp.insert(i, abs((omega/dispPSTD(omega, dt)-LIGHT_SPEED)/LIGHT_SPEED))
ax.plot(arrDtDisp, arrVDisp, ':r')			

ax.set_xlabel("N_T")
ax.set_ylabel("(v-c)/c")
plt.tight_layout()
#legend(["PSTD sequential", "PSTD parallel", "PSATD sequential", "PSATD parallel"])	
legend(["PSTD", "PSATD", "f(N_T)"])

ax.set_xscale ('log')	
	
plt.savefig(NAME_GRAPH_OUT+".png")
		
			
# arrOmegaDisp=[]; arrDiffKDispAndK=[]
# arrOmega=[]; arrDiffKThAndK=[]
# for i in range(NUM_OF_POINTS, 1, -1):
	# print(i, "\n")
	
	# lambdaN=pow(2, i)
	# lambd = lambdaN*D
	
	# print("lambdaN=", lambdaN, "\n")
		
	# callProcess(lambdaN, DT/4, lambdaN*COEF*4)
	# dat=pg.readFile_1d(NAME_FILE_IN)
	# v=findV(dat, lambd)
	# print(dat[0])
	# print(v)
	
	# omega=2*math.pi*LIGHT_SPEED/lambd
	# kTh=omega/LIGHT_SPEED
	# k=omega/v
	
	# arrOmega.insert(NUM_OF_POINTS-i, omega)
	# arrDiffKThAndK.insert(NUM_OF_POINTS-i, abs(k-kTh))
	# arrOmegaDisp.insert(NUM_OF_POINTS-i, omega)
	# arrDiffKDispAndK.insert(NUM_OF_POINTS-i, abs(dispPSTD(omega, DT/4)-kTh))
	
# # for i in range(500):
	# # omega_s=2*math.pi*LIGHT_SPEED/(2**NUM_OF_POINTS*D)
	# # omega_f=2*math.pi*LIGHT_SPEED/(4*D)
	# # omega=omega_s+i*(omega_f-omega_s)/500
	# # arrOmegaDisp.insert(i, omega)
	# # arrDiffKDispAndK.insert(i, abs(dispPSTD(omega, DT)-omega/LIGHT_SPEED))
	
# fig = plt.figure()
# ax = fig.add_subplot(111)

# ax.plot(arrOmegaDisp, arrDiffKDispAndK)
# ax.plot(arrOmega, arrDiffKThAndK, "*")

# ax.set_xlabel("w")
# ax.set_ylabel("|k-w/c|")

# plt.tight_layout()

# plt.savefig(NAME_GRAPH_OUT+"_diff_k_w.png")

