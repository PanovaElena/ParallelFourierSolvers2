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

DIR_RES="./../../files/running_wave/collection_of_results/"
DIR_NUM_DOM="/num_domains_2/"

DIR_MASK="./collection_of_results/%s/mask_%s/"
NAME_CONS_FILE="/cons_res.csv"
NAME_PAR_FILE="/second_steps.csv"

LIST_FIELD_SOLVERS = ["PSATD"]#["PSTD", "PSATD"]
LIST_MASK = ["simple"]#["simple", "smooth"]
LIST_NUM_OF_DOMAINS = [2, 4, 8, 16] #[2, 4, 8, 16, 32]
LIST_LAMBDA = [4, 8, 32, 64, 128, 256]
LIST_NUM_OF_ITER = [100]#[100, 200, 400, 600, 800, 1000]
LIST_DT = [] # later

def calc_error(data_cons, data_par, file_name_cons, file_name_par):
	if (len(data_cons)!=len(data_par) and len(data_cons)-1!=len(data_par) and len(data_cons)!=len(data_par)-1): 
		print(file_name_cons)
		print(file_name_par)
		print("ERROR! Different sizes")
		print(len(data_cons),len(data_par))
		return -1
	error=0
	for i in range(len(data_par)):
		if (abs(data_cons[i]-data_par[i])>error): error=abs(data_cons[i]-data_par[i])
	return error
	
def generate_list_dt(fs):
	D=math.sqrt(6)*math.pi
	LIGHT_SPEED = 29979245800
	COURANT_CONDITION_PSTD = math.sqrt(2)*D/(LIGHT_SPEED*math.pi)
	global LIST_DT
	global DT
	if (fs=="PSTD"):
		max_dt = COURANT_CONDITION_PSTD
		LIST_DT=[D/LIGHT_SPEED/4/100, D/LIGHT_SPEED/8/100, D/LIGHT_SPEED/16/100, D/LIGHT_SPEED/128/100, D/LIGHT_SPEED/1000/100]
		LIST_DT.reverse()
	elif (fs=="PSATD"):
		max_dt = 3*D/LIGHT_SPEED/100
		min_dt = D/LIGHT_SPEED/100/1000
		N=16
		step=(max_dt-min_dt)/N
		LIST_DT=[]
		for i in range(N):
			LIST_DT.append(min_dt+i*step)
		LIST_DT.sort()
			
for solver in LIST_FIELD_SOLVERS:
	for mask in LIST_MASK:
		
		generate_list_dt(solver)			
		
		for nIter in LIST_NUM_OF_ITER:			
			#fix nIter, lambda, calc dt
			lambd=64
			dir=DIR_MASK % (solver, mask)+"lambda_"+str(lambd)+"_nIter_"+str(nIter)+"/"
			data_graph=[]
			name="dt__"+solver+"_"+mask+"_l_"+str(lambd)+"_n_"+str(nIter)
			with open(DIR+name+".csv", "w") as file:
				file.write("dt;error\n")
				for dt in LIST_DT:
				
					#file_name_cons=dir+"dt_"+"{0:.11e}".format(dt)+"/"+NAME_CONS_FILE
					#file_name_par=dir+"dt_"+'{0:.11e}'.format(dt)+"/"+NAME_PAR_FILE
					
					file_name_cons=dir+"dt_"+str(dt)+"/"+NAME_CONS_FILE
					file_name_par=dir+"dt_"+str(dt)+"/"+DIR_NUM_DOM+"/"+NAME_PAR_FILE
	
					data_cons=pg.readFile_1d(file_name_cons)
					data_par=pg.readFile_1d(file_name_par)
					
					error=calc_error(data_cons, data_par, file_name_cons, file_name_par)
					
					if (error>=0):
						data_graph.append(error)
						file.write(str(LIST_DT[LIST_DT.index(dt)])+";"+str(error)+"\n")
					else: 
						print(dt)
						print(LIST_DT[LIST_DT.index(dt)])
			pg.plot_1d(DIR, name, data_graph, LIST_DT, "dt", "error", True) 
		
		# #fix dt, lambda, calc nIter
		# dt=LIST_DT[2]
		# lambd=LIST_LAMBDA[3]
		# dir=DIR_MASK % (solver, mask)+"lambda_"+str(lambd)+"_dt_"+'{0:.11e}'.format(dt)+"/"
		# data_graph=[]
		# name="nIter__"+solver+"_"+mask+"_l_"+str(lambd)+"_dt_"+'{0:.11e}'.format(dt)
		# with open(DIR+name+".csv", "w") as file:
			# file.write("num_iter;error\n")
			# for num_iter in LIST_NUM_OF_ITER:
			
				# file_name_cons=dir+"nIter_"+str(nIter)+"/"+NAME_CONS_FILE
				# file_name_par=dir+"nIter_"+str(nIter)+"/"+NAME_PAR_FILE

				# data_cons=pg.readFile_1d(file_name_cons)
				# data_par=pg.readFile_1d(file_name_par)
				
				# error=calc_error(data_cons, data_par, file_name_cons, file_name_par)
				
				# if (error>=0):
					# data_graph.append(error)
					# file.write(str(LIST_NUM_OF_ITER[LIST_NUM_OF_ITER.index(num_iter)])+";"+str(error)+"\n")
				# else: 
					# print(num_iter)
					# print(LIST_NUM_OF_ITER[LIST_NUM_OF_ITER.index(num_iter)])
		# pg.plot_1d(DIR, name, data_graph, LIST_NUM_OF_ITER, "nIter", "error", True) 
		
		
		# #fix dt, nIter, calc lambda
		# dt=LIST_DT[2]
		# nIter=LIST_NUM_OF_ITER[1]
		# dir=DIR_MASK % (solver, mask)+"nIter_"+	str(nIter)+"_dt_"+'{0:.11e}'.format(dt)+"/"
		# data_graph=[]
		# name="lambda__"+solver+"_"+mask+"_n_"+str(nIter)+"_dt_"+'{0:.11e}'.format(dt)
		# with open(DIR+name+".csv", "w") as file:
			# file.write("lambda;error\n")
			# for lambd in LIST_LAMBDA:
			
				# file_name_cons=dir+"lambda_"+str(lambd)+"/"	+NAME_CONS_FILE
				# file_name_par=dir+"lambda_"+str(lambd)+"/"+NAME_PAR_FILE

				# data_cons=pg.readFile_1d(file_name_cons)
				# data_par=pg.readFile_1d(file_name_par)
				
				# error=calc_error(data_cons, data_par, file_name_cons, file_name_par)
				
				# if (error>=0):
					# data_graph.append(error)
					# file.write(str(LIST_LAMBDA[LIST_LAMBDA.index(lambd)])+";"+str(error)+"\n")
				# else: 
					# print(lambd)
					# print(LIST_LAMBDA[LIST_LAMBDA.index(lambd)])
		# pg.plot_1d(DIR, name, data_graph, LIST_LAMBDA, "lambda", "error", True) 
		
		
		# #fix dt, nIter, lambda, calc numDom
		# nIter=LIST_NUM_OF_ITER[1]
		# lambd=LIST_LAMBDA[4]
		# dt=LIST_DT[2]
		# dir=DIR_MASK % (solver, mask)+"lambda_"+str(lambd)+"_nIter_"+str(nIter)+"_dt_"+'{0:.11e}'.format(dt)+"/"
		# data_graph=[]
		# name="numDom__"+solver+"_"+mask+"_l_"+str(lambd)+"_n_"+str(nIter)+"_dt_"+'{0:.11e}'.format(dt)
		# with open(DIR+name+".csv", "w") as file:
			# file.write("numDom;error\n")
			# for numDom in LIST_NUM_OF_DOMAINS:
			
				# file_name_cons=dir+"/"+NAME_CONS_FILE
				# file_name_par=dir+"/"+"num_domains_"+str(numDom)+"/"+NAME_PAR_FILE

				# data_cons=pg.readFile_1d(file_name_cons)
				# data_par=pg.readFile_1d(file_name_par)
				
				# error=calc_error(data_cons, data_par, file_name_cons, file_name_par)
				
				# if (error>=0):
					# data_graph.append(error)
					# file.write(str(LIST_NUM_OF_DOMAINS[LIST_NUM_OF_DOMAINS.index(numDom)])+";"+str(error)+"\n")
				# else: 
					# print(dt)
					# print(LIST_NUM_OF_DOMAINS[LIST_NUM_OF_DOMAINS.index(numDom)])
		# pg.plot_1d(DIR, name, data_graph, LIST_NUM_OF_DOMAINS, "num_of_domains", "error", True)
		
		
