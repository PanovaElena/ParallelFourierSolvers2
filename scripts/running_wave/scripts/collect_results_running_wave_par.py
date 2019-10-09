
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

DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0]) 
print(DIR_SCRIPT)

NAME_CONS_PROGRAM = "\""+DIR_SCRIPT+"/../../build/src/Examples/RunningWave/ConsistentFieldSolver_RunningWave/"+FOLDER+"/runningWave_consistent"+"\""
NAME_PAR_PROGRAM = "\""+DIR_SCRIPT+"/../../build/src/Examples/RunningWave/ParallelFieldSolver_RunningWave/"+FOLDER+"/runningWave_parallel"+"\""

DIR_CONS_RES = DIR_SCRIPT+"/consistent_results/E/"
DIR_PAR_RES = DIR_SCRIPT+"/parallel_results/"

NAME_FILE_CONS = "/cons_res.csv"
NAME_FILE_PAR = "/second_steps.csv"

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

LIGHT_SPEED = 2997924580000
COURANT_CONDITION_PSTD = math.sqrt(2)*D/(LIGHT_SPEED*math.pi)

GUARD_PART = 0.5
ITER_BETWEEN_DUMPS=100

LIST_FIELD_SOLVERS = ["PSTD", "PSATD"]
LIST_MASK = ["simple", "smooth"]
LIST_NUM_OF_DOMAINS = [2, 4, 8, 16] #[2, 4, 8, 16, 32]
LIST_LAMBDA = [4, 8, 32, 64, 128, 256]
LIST_NUM_OF_ITER = [400, 800, 1600, 2400, 3200]
LIST_DT = [] # later


def do_call(nIter, dt, lambd, numDom, dir):	
		
		size_of_domain = NX/numDom
		guard = int(GUARD_PART * size_of_domain)
		
		command_args_par = " --nx "+str(NX)+" --ny "+str(NY)+" --nz "+str(NZ)+\
			" -d "+str(D)+" --nCons "+str(0)+" --nPar "+str(nIter)+\
			" --guard "+str(guard) +" --solver "+fs+" --mask "+mask+\
			" --lambdaN " +str(lambd)+" --dt "+str(dt)
		try:
			process_par = subprocess.Popen(NAME_MPI + " -n " + str(numDom) +" "+NAME_PAR_PROGRAM+" "+command_args_par, shell=True)
			process_par.wait()
	
			files = os.listdir(DIR_PAR_RES)
			move_file(DIR_PAR_RES+NAME_FILE_PAR, current_dir+NAME_FILE_PAR)
		except:
			print(nIter, dt, lambd, numDom, dir)


for fs in LIST_FIELD_SOLVERS:
	
	if (fs=="PSTD"):
		max_dt = COURANT_CONDITION_PSTD
		LIST_DT=[max_dt/2, max_dt/4, max_dt/8, max_dt/16]
		LIST_DT.reverse()
	elif (fs=="PSATD"):
		max_dt = D/LIGHT_SPEED
		LIST_DT=[max_dt, max_dt/2, max_dt/4, max_dt/8, max_dt/16, max_dt/128, max_dt/1000]
		LIST_DT.reverse()
	
	current_dir_fs=DIR+fs+"/"	
	create_dir(current_dir_fs)
		
	for mask in LIST_MASK:
		
		current_dir_mask=current_dir_fs+"mask_"+mask+"/"	
		create_dir(current_dir_mask)
		
		numDom=2
			
		#fix dt, nIter, calc lambda
		dt=LIST_DT[2]
		nIter=LIST_NUM_OF_ITER[1]
		current_dir_nIter_dt=current_dir_mask+"nIter_"+	str(nIter)+"_dt_"+str(dt)+"/"
		create_dir(current_dir_nIter_dt)
		for lambd in LIST_LAMBDA:
		
			current_dir_lambd=current_dir_nIter_dt+"lambda_"+str(lambd)+"/"	
			create_dir(current_dir_lambd)
			
			do_call(nIter, dt, lambd, numDom, current_dir_lambd)
			
		
		#fix dt, lambda, calc nIter
		dt=LIST_DT[2]
		lambd=LIST_LAMBDA[3]
		current_dir_lambd_dt=current_dir_mask+"lambda_"+str(lambd)+"_dt_"+str(dt)+"/"
		create_dir(current_dir_lambd_dt)
		for nIter in LIST_NUM_OF_ITER:
		
			current_dir_nIter=current_dir_lambd_dt+"nIter_"+str(nIter)+"/"	
			create_dir(current_dir_nIter)
			
			do_call(nIter, dt, lambd, numDom, current_dir_nIter)
			
			
		#fix nIter, lambda, calc dt
		nIter=LIST_NUM_OF_ITER[1]
		lambd=LIST_LAMBDA[3]
		current_dir_lambd_nIter=current_dir_mask+"lambda_"+str(lambd)+"_nIter_"+str(nIter)+"/"
		create_dir(current_dir_lambd_nIter)
		for dt in LIST_DT:
		
			current_dir_dt=current_dir_lambd_nIter+"dt_"+str(dt)+"/"	
			create_dir(current_dir_dt)
			
			do_call(nIter, dt, lambd, numDom, current_dir_dt)


		#fix nIter, lambda, dt, calc num proc
		nIter=LIST_NUM_OF_ITER[1]
		lambd=LIST_LAMBDA[4]
		dt=LIST_DT[2]
		current_dir_lambd_nIter_dt=current_dir_mask+"lambda_"+str(lambd)+"_nIter_"+str(nIter)+"_dt_"+str(dt)/"
		create_dir(current_dir_lambd_nIter_dt)
		for numDom in LIST_NUM_OF_DOMAINS:
	
			current_dir_numDom=current_dir_lambd_nIter_dt+"num_domains_"+str(numDom)+"/"	
			create_dir(current_dir_numDom)
					
			do_call(nIter, dt, lambd, numDom, current_dir_numDom)		
					
					
						
