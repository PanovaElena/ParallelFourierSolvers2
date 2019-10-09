import sys
import math
import subprocess
import os
import shutil

DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0])
if (sys.platform == "win32"):
	FOLDER="/Release/"
	MPI="mpiexec"
else: 
	FOLDER=""
	MPI="$I_MPI_ROOT/intel64/bin/mpiexec"


NAME_PAR_PROGRAM = "\""+DIR_SCRIPT+"/../../../../build/src/examples/running_wave/running_wave_parallel/"+FOLDER+"/running_wave_parallel"+"\""

#HOSTS=["node10","node13","node14","node15","node18","node101","node21","node22","node23","node26","node27","node28","node29","node30","node32","node34"]
HOSTS=["node104","node105","node106","node107"]

LIGHT_SPEED = 29979245800

NX = 2048
NY = 1
NZ = 2048

D = 1

DT = D/LIGHT_SPEED

SOLVER = "PSATD"

LAMBD = 64 * D
ANGLE = 0

#NUM_NODES=[2, 4, 8, 16]
NUM_NODES=[2, 4]

NUM_ITER=1024
NUM_ITER_IN_DOMAIN = 12 # k in article

N_THREADS = 16

GUARD = 32

REPEAT = 3

print("MPI_VERSION_STRONG_SCALABILITY")

for N_NODES in NUM_NODES:
	
	deg_num_nodes = int(math.log(N_NODES)/math.log(2))
	npx=2**int(deg_num_nodes/2)
	npz=int(N_NODES/npx)
	print(str(npx)+"x"+str(npz))
	
	hosts=HOSTS[0]
	for i in range(1,N_NODES):
		hosts+=","+HOSTS[i]
	
	command_args_par = "-ax "+str(0)+" "+\
						"-ay "+str(0)+" "+\
						"-az "+str(0)+" "+\
						\
						"-dx "+str(D)+" "+\
						"-dy "+str(D)+" "+\
						"-dz "+str(D)+" "+\
						\
						"-nx "+str(NX)+" "+\
						"-ny "+str(NY)+" "+\
						"-nz "+str(NZ)+" "+\
						\
						"-dt "+str(DT)+" "+\
						\
						"-solver "+str(SOLVER)+" "+\
						\
						"-npx "+str(npx)+" "+\
						"-npy "+str(1)+" "+\
						"-npz "+str(npz)+" "+\
						\
						"-gx "+str(GUARD)+" "+\
						"-gy "+str(0)+" "+\
						"-gz "+str(GUARD)+" "+\
						\
						"-nseqi "+str(0)+" "+\
						"-npari "+str(NUM_ITER)+" "+\
						"-ndomi "+str(NUM_ITER_IN_DOMAIN)+" "+\
						\
						"-mask simple "+\
						"-filter off "+\
						\
						"-dim "+str(1)+" "\
						\
						"-dump off "\
						"-dir "+str(DIR_SCRIPT)+" "\
						\
						"-nthreads "+str(N_THREADS)+" "\
						\
						"-lambda "+str(LAMBD)+" "+\
						"-angle "+str(ANGLE)+" ";
							
							
	
	for rep in range(REPEAT):
		process_seq = subprocess.Popen(MPI+" -np "+str(N_NODES)+" -ppn 1 -hosts "+hosts+" "+NAME_PAR_PROGRAM+" "+command_args_par, shell=True)
		process_seq.wait()
		