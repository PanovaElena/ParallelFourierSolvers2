
import math
import subprocess
import os
import sys

OS=sys.argv[1]
FOLDER=""
EXE=""
NAME_MPI="mpiexec"
HOSTS=""
if (OS=="Win"):
	NAME_MPI="mpiexec"
	FOLDER="/Release/"
	EXE=".exe"
elif (OS=="Unix"):
	HOSTS=" -hosts "+sys.argv[2]

DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0]) 
print(DIR_SCRIPT)

NAME_CONS_PROGRAM = "\""+DIR_SCRIPT+"/../../build/src/Examples/RunningWave/ConsistentFieldSolver_RunningWave/"+FOLDER+"/runningWave_consistent"+"\""
NAME_PAR_PROGRAM = "\""+DIR_SCRIPT+"/../../build/src/Examples/RunningWave/ParallelFieldSolver_RunningWave/"+FOLDER+"/runningWave_parallel"+"\""

DIR_CONS_RES = DIR_SCRIPT+"/consistent_results/E/"
DIR_PAR_RES = DIR_SCRIPT+"/parallel_results/"

NAME_FILE_CONS = "/cons_res.csv"
NAME_FILE_PAR = "/second_steps.csv"
#NAME_FILE_PAR = "/num_domains_2/second_steps.csv"

def create_dir(name_dir):
	if (os.path.exists(name_dir)==False):
		try:
			os.mkdir(name_dir)
		except OSError:
			print ("cannot create dir %s" % name_dir)
			sys.exit(1)
		print("created "+name_dir)
	else:
		print("exists "+name_dir)

def move_file(file_from, file_to):
	if (os.path.exists(file_to)):
		os.remove(file_to)
	os.rename(file_from, file_to)

DIR=DIR_SCRIPT+"/collection_of_results/"
create_dir(DIR)

NX=256
NY= 1
NZ=NX
D=math.sqrt(6)*math.pi
N_CONS=0

LIGHT_SPEED = 29979245800
COURANT_CONDITION_PSTD = math.sqrt(2)*D/(LIGHT_SPEED*math.pi)

GUARD_PART = 0.5
ITER_BETWEEN_DUMPS=100

LIST_FIELD_SOLVERS = ["PSATD"]#["PSTD", "PSATD"]
LIST_MASK = ["simple"]#["simple", "smooth"]
LIST_NUM_OF_DOMAINS = [2]#[2, 4, 8, 16, 32]
LIST_LAMBDA = [4, 8, 32, 64, 128, 256]
LIST_NUM_OF_ITER = [100]#[100, 200, 400, 600, 800, 1000]
LIST_DT = [] # later

def do_call(nIter, dt, lambd, dir):
	command_args_cons = " --nCons "+str(nIter)+\
						" --solver "+fs+" --mask "+mask+\
						" --lambdaN " +str(lambd)+" --dt "+str(dt)
					
	process_cons = subprocess.Popen(NAME_CONS_PROGRAM+" "+command_args_cons, shell=True)
	process_cons.wait()
	
	move_file(DIR_CONS_RES+NAME_FILE_CONS, dir+NAME_FILE_CONS)
	
	for numDom in LIST_NUM_OF_DOMAINS:
	
		current_dir=dir+"num_domains_"+str(numDom)+"/"	
		create_dir(current_dir)
		
		size_of_domain = NX/numDom
		guard = int(GUARD_PART * size_of_domain)
		
		command_args_par = " --nCons "+str(0)+" --nPar "+str(nIter)+\
			" --solver "+fs+" --mask "+mask+\
			" --lambdaN " +str(lambd)+" --dt "+str(dt)
		
		process_par = subprocess.Popen(NAME_MPI + " -n " + str(numDom) +" -ppn 1 "+HOSTS+" "+NAME_PAR_PROGRAM+" "+command_args_par, shell=True)
		process_par.wait()
	
		files = os.listdir(DIR_PAR_RES)
		move_file(DIR_PAR_RES+NAME_FILE_PAR, current_dir+NAME_FILE_PAR)

for fs in LIST_FIELD_SOLVERS:
	
	if (fs=="PSTD"):
		max_dt=COURANT_CONDITION_PSTD
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
	
	current_dir_fs=DIR+fs+"/"	
	create_dir(current_dir_fs)
		
	for mask in LIST_MASK:
		
		current_dir_mask=current_dir_fs+"mask_"+mask+"/"	
		create_dir(current_dir_mask)
		
			
		# #fix dt, nIter, calc lambda
		# dt=LIST_DT[0]
		# nIter=LIST_NUM_OF_ITER[0]
		# current_dir_nIter_dt=current_dir_mask+"nIter_"+	str(nIter)+"_dt_"+str(dt)+"/"
		# create_dir(current_dir_nIter_dt)
		# for lambd in LIST_LAMBDA:
		
			# current_dir_lambd=current_dir_nIter_dt+"lambda_"+str(lambd)+"/"	
			# create_dir(current_dir_lambd)
			
			# do_call(nIter, dt, lambd, current_dir_lambd)
			
		
		# #fix dt, lambda, calc nIter
		# dt=LIST_DT[0]
		# lambd=LIST_LAMBDA[0]
		# current_dir_lambd_dt=current_dir_mask+"lambda_"+str(lambd)+"_dt_"+str(dt)+"/"
		# create_dir(current_dir_lambd_dt)
		# for nIter in LIST_NUM_OF_ITER:
		
			# current_dir_nIter=current_dir_lambd_dt+"nIter_"+str(nIter)+"/"	
			# create_dir(current_dir_nIter)
			
			# do_call(nIter, dt, lambd, current_dir_nIter)	
			
		for nIter in LIST_NUM_OF_ITER:	
			#fix nIter, lambda, calc dt
			lambd=64
			current_dir_lambd_nIter=current_dir_mask+"lambda_"+str(lambd)+"_nIter_"+str(nIter)+"/"
			create_dir(current_dir_lambd_nIter)
			for dt in LIST_DT:
			
				current_dir_dt=current_dir_lambd_nIter+"dt_"+str(dt)+"/"	
				create_dir(current_dir_dt)
				
				do_call(nIter, dt, lambd, current_dir_dt)		
					
					
					
					
						
			



