
import math
import subprocess
import os
import sys

OS=sys.argv[1]
FOLDER=""
EXE=""
NAME_MPI="/common/intel/impi/4.1.3.048/intel64/bin/mpirun"
if (OS=="Win"):
	NAME_MPI="mpiexec"
	FOLDER="/Release/"
	EXE=".exe"

DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0]) 
print(DIR_SCRIPT)

NAME_CONS_PROGRAM = "\""+DIR_SCRIPT+"/../../build/src/Examples/SphericalWave/ConsistentFieldSolver_SphericalWave/"+FOLDER+"/sphericalWave_consistent"+"\""
NAME_PAR_PROGRAM = "\""+DIR_SCRIPT+"/../../build/src/Examples/SphericalWave/ParallelFieldSolver_SphericalWave/"+FOLDER+"/sphericalWave_parallel"+"\""


#NAME_CONS_PROGRAM = os.path.abspath(DIR_SCRIPT+"/../../build/src/Examples/SphericalWave/ConsistentFieldSolver_SphericalWave/"+FOLDER+"/sphericalWave_consistent")
#NAME_PAR_PROGRAM = os.path.abspath(DIR_SCRIPT+"/../../build/src/Examples/SphericalWave/ParallelFieldSolver_SphericalWave/"+FOLDER+"/sphericalWave_parallel")

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

LIGHT_SPEED = 2997924580000

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
LIST_MASK = ["simple", "smooth"]
LIST_NUM_OF_DOMAINS=[2] #[2, 4, 8, 16, 32, 64]

NUM_OF_CONS_ITER = 300
NUM_OF_PAR_ITER = 100


for fs in LIST_FIELD_SOLVERS:
	
	if (fs=="PSTD"):
		max_dt = COURANT_CONDITION_PSTD
		LIST_DT=[max_dt, max_dt/2, max_dt/4, max_dt/8]
	elif (fs=="PSATD"):
		LIST_DT=[DT, DT*1e-3, DT*1e-6, DT*1e-9, DT*1e-12, DT*1e-15]
	
	current_dir_fs=DIR+fs+"/"	
	create_dir(current_dir_fs)
		
	for mask in LIST_MASK:
		
		current_dir_mask=current_dir_fs+"mask_"+mask+"/"	
		create_dir(current_dir_mask)
					
		for dt in LIST_DT:
			
			current_dir_dt=current_dir_mask+"_dt_"+str(dt)+"/"	
			create_dir(current_dir_dt)
			
			command_args_cons = " --nx "+str(NX)+" --ny "+str(NY)+" --nz "+str(NZ)+\
				" --nCons "+str(NUM_OF_CONS_ITER+NUM_OF_PAR_ITER)+\
				" --solver "+fs+" --mask "+mask+\
				" --dt "+str(dt)+ " -d " +str(D*dt/DT)+\
				" --T "+str(T*dt/DT)+" --TCoord "+str(T_COORD*dt/DT)+\
				" --omega "+str(OMEGA*DT/dt) + " --omegaEnv "+str(OMEGA_ENV*DT/dt)
			
			process_cons = subprocess.Popen(NAME_CONS_PROGRAM+" "+command_args_cons, shell=True)
			process_cons.wait()
			
			move_file(DIR_CONS_RES+NAME_FILE_CONS, current_dir_dt+NAME_FILE_CONS)
			
			for numDom in LIST_NUM_OF_DOMAINS:
	
				current_dir=current_dir_dt+"num_domains_"+str(numDom)+"/"	
				create_dir(current_dir)
				
				size_of_domain = NX/numDom
				guard = int(GUARD_PART * size_of_domain)
				
				command_args_par = " --nx "+str(NX)+" --ny "+str(NY)+" --nz "+str(NZ)+\
				" --nCons "+str(NUM_OF_CONS_ITER) + " --nPar "+str(NUM_OF_PAR_ITER)+\
				" --solver "+fs+" --mask "+mask+\
				" --dt "+str(dt)+ " -d " +str(D*dt/DT)+\
				" --T "+str(T*dt/DT)+" --TCoord "+str(T_COORD*dt/DT)+\
				" --omega "+str(OMEGA*DT/dt) + " --omegaEnv "+str(OMEGA_ENV*DT/dt)
				
				process_par = subprocess.Popen(NAME_MPI + " -n " + str(numDom) +" "+NAME_PAR_PROGRAM+" "+command_args_par, shell=True)
				process_par.wait()
			
				files = os.listdir(DIR_PAR_RES)
				move_file(DIR_PAR_RES+NAME_FILE_PAR, current_dir+NAME_FILE_PAR)
				
				
				
					
		



