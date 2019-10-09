import sys
import math
import subprocess
import os
import shutil

# build without OpenMP

DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0])
if (sys.platform == "win32"):
	FOLDER="/Release/"
else: FOLDER=""

NAME_SEQ_PROGRAM = "\""+DIR_SCRIPT+"/../../../../build/src/examples/running_wave/running_wave_sequential/"+FOLDER+"/running_wave_sequential"+"\""

LIGHT_SPEED = 29979245800

NX = 512
NY = 1
NZ = 512

D = 1

DT = D/LIGHT_SPEED

SOLVER = "PSATD"

LAMBD = 64 * D
ANGLE = 0

NUM_ITER=1024 

REPEAT=3


print("SEQUENTIAL_VERSION")

command_args_seq = "-ax "+str(0)+" "+\
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
					"-nseqi "+str(NUM_ITER)+" "+\
					\
					"-dim "+str(1)+" "\
					\
					"-dump off "\
					"-dir "+str(DIR_SCRIPT)+" "\
					\
					"-lambda "+str(LAMBD)+" "+\
					"-angle "+str(ANGLE)+" ";
					
for rep in range(REPEAT):						
	process_seq = subprocess.Popen(NAME_SEQ_PROGRAM+" "+command_args_seq, shell=True)
	process_seq.wait()
			