
import math
import subprocess
import os
import sys
sys.path.append("../../scripts/")
import graphics as gr
import read_file as rf

FOLDER="Release"

DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0]) 
DIR_RES="./"

DIR_CONS_PROGRAM = "\""+DIR_SCRIPT+"/../../../build/src/examples/spherical_wave/spherical_wave_sequential/"+FOLDER
NAME_CONS_PROGRAM = DIR_CONS_PROGRAM+"/spherical_wave_sequential"+"\""

NAME_FILE_CONS  = DIR_SCRIPT +"/sequential_result.csv"

LIST_FIELD_SOLVERS = ["PSTD", "PSATD"]

LIGHT_SPEED = 29979245800
D=LIGHT_SPEED
COURANT_CONDITION_PSTD = math.sqrt(2)*D/(LIGHT_SPEED*math.pi)

DT=[0.1, 0.05, 0.01] #, COURANT_CONDITION_PSTD/4, COURANT_CONDITION_PSTD/2]
SOLVER=["PSTD", "PSATD"]

def calc_error(data_1, data_2):	
	error=0
	for i in range(len(data_1)):
		for j in range(len(data_1[i])):
			if (abs(data_1[i][j]-data_2[i][j])>abs(error)): error=data_1[i][j]-data_2[i][j]
	return error
	
def calc_ampl(data):
	ampl=0
	for i in range(len(data)):
		for j in range(len(data[i])):
			if (abs(data[i][j])>ampl):
				ampl=abs(data[i][j])
	return ampl
	
def calc_error_arr(data_1, data_2):	
	error=[]
	for i in range(len(data_1)):
		tmp=[]
		for j in range(len(data_1[i])):
			tmp.append(data_1[i][j]-data_2[i][j])
		error.append(tmp)			
	return error

arr_err=[]
for dt in DT:

	nIter = int(30/dt)

	command_args = " -solver %s -dt %f -nseqi %d"			
					
	process = subprocess.Popen(NAME_CONS_PROGRAM+" "+(command_args % ("PSTD",dt,nIter)), shell=True)
	process.wait()
	
	data_PSTD=rf.readFile2d(NAME_FILE_CONS)
	
	process_2 = subprocess.Popen(NAME_CONS_PROGRAM+" "+(command_args % ("PSATD",dt,nIter)), shell=True)
	process_2.wait()
	
	data_PSATD=rf.readFile2d(NAME_FILE_CONS)
	
	arr_err.append(calc_error(data_PSATD, data_PSTD))
	#gr.plot2d(DIR_RES, "PSATD_nIter_"+str(nIter), data_PSATD)
	

# gr.plot2d(DIR_RES, "PSTD", data_PSTD)
# gr.plot2d(DIR_RES, "PSATD", data_PSATD)
# gr.plot2d(DIR_RES, "FDTD", data_FDTD)
# gr.plot2d(DIR_RES, "PSATD_PSTD_error", calc_error_arr(data_PSATD, data_PSTD))
# gr.plot2d(DIR_RES, "PSTD_FDTD_error", calc_error_arr(data_FDTD, data_PSTD))
	
gr.plot1d("./", "PSATD_PSTD_cons_error", arr_err, DT, "dt", "error", True)	
					
					
					
					