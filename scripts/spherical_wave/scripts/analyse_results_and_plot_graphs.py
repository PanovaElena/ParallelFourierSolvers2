import sys
import math
sys.path.append("../../scripts/")

import plot_graph as pg
import os

DIR= "./analysation_results/"
if (not os.path.exists(DIR)): os.mkdir(DIR)

PSTD="PSTD"
PSATD="PSATD"
simple="simple"
smooth="smooth"

DIR_RES="./../../files/spherical_wave/collection_of_results/"
DIR_NUM_DOM="/num_domains_2/"

CUR_DIR=DIR_RES+"/%s/mask_%s/_dt_%s/"
NAME_CONS_FILE=CUR_DIR+"/cons_res.csv"
NAME_PAR_FILE=CUR_DIR+DIR_NUM_DOM+"/second_steps.csv"

LIGHT_SPEED = 29979245800

PART=1e+12

NX=256 #256
NY=NX
NZ=1 #NX
D=LIGHT_SPEED
OMEGA=0.392699
OMEGA_ENV=0.392699
DT=0.1
T=16
T_COORD=8*LIGHT_SPEED

COURANT_CONDITION_PSTD = math.sqrt(2)*D/(LIGHT_SPEED*math.pi)

GUARD_PART = 0.2

LIST_FIELD_SOLVERS = ["PSATD"] #["PSTD", "PSATD"]
LIST_MASK = ["smooth"]
LIST_NUM_OF_DOMAINS=[2] #[2, 4, 8, 16, 32, 64]

NUM_OF_CONS_ITER = 16
NUM_OF_PAR_ITER = 14

def calc_otn_error(data_cons, data_par, file_name_cons, file_name_par):	
	error=0
	for i in range(len(data_par)):
		for j in range(len(data_par[i])):
			if (abs(data_cons[i][j]-data_par[i][j])>error): error=abs(data_cons[i][j]-data_par[i][j])
	char=0
	for i in range(len(data_cons)):
		for j in range(len(data_cons[i])):
			if (abs(data_cons[i][j])>char): char=abs(data_cons[i][j])
	if (error>=1):
		print(file_name_cons)
		print(file_name_par)
		print("LARGE ERROR")
		return -1
	return error/char
	
def calc_abs_error(data_cons, data_par, file_name_cons, file_name_par):	
	error=0
	for i in range(len(data_par)):
		for j in range(len(data_par[i])):
			if (abs(data_cons[i][j]-data_par[i][j])>error): error=abs(data_cons[i][j]-data_par[i][j])
	if (error>=1):
		print(file_name_cons)
		print(file_name_par)
		print("LARGE ERROR")
		return -1
	return error
	
LIST_DT=[]
def generate_list_dt(solver):
	global LIST_DT
	if (solver=="PSTD"):
		max_dt = COURANT_CONDITION_PSTD
		LIST_DT=[max_dt, max_dt/2, max_dt/4, max_dt/8]
	elif (solver=="PSATD"):
		max_dt=0.1
		min_dt=0.005
		LIST_DT=[]
		N=8
		for i in range(N):
			LIST_DT.append(min_dt+i*(max_dt-min_dt)/N)
		LIST_DT.reverse()
			
for solver in LIST_FIELD_SOLVERS:
	for mask in LIST_MASK:
		
		generate_list_dt(solver)
		
		#dt
		name="solver_"+solver+"_mask_"+mask
		data_graph=[]
		data_dt=[]
		with open(DIR+name+".csv", "w") as file:
			file.write("dt;error\n")
			for dt in LIST_DT:
				file_name_cons=NAME_CONS_FILE % (solver, mask, str(dt))
				file_name_par=NAME_PAR_FILE % (solver, mask, str(dt))
				data_cons=pg.readFile_2d(file_name_cons)
				data_par=pg.readFile_2d(file_name_par)
				error=calc_abs_error(data_cons, data_par, file_name_cons, file_name_par)
				
				data_graph.append(error)
				data_dt.append(dt)
				file.write(str(dt)+";"+str(error)+"\n")
				
		pg.plot_1d(DIR, name, data_graph, data_dt, "dt", "error", True) 
