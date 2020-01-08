import sys
import math
sys.path.append("../../scripts/")
import read_file as fr
import graphics as gr
import subprocess
import os
import shutil
import args

def run():
    
    DIR_SCRIPT = os.path.abspath("./").replace("\\", "/")
    if (sys.platform == "win32"):
    	MPI = "mpiexec"
    else: MPI = "mpirun"
    
    if (args.STRIP == 1):
        args.NUM_PROCESSES = 1
    
    NAME_PROGRAM = "\""+DIR_SCRIPT+"/../../../build/bin/tight_focusing"+"\""
    
    DIR_RESULTS = DIR_SCRIPT+"/results_%s/" % str(args.STRIP)
    
    if (os.path.exists(DIR_RESULTS)): 
    	for (dirpath, dirnames, filenames) in os.walk(DIR_RESULTS):
    		for file in filenames:
    			os.remove(DIR_RESULTS+file)
    else: 
        os.mkdir(DIR_RESULTS)
    
    ALL_TIME = 0.2*180
    		
    command_args = \
    					"-solver "+str("PSATD")+" "+\
    					"-dtm "+str(ALL_TIME/args.N_ITER)+" "+\
    					\
    					"-nseqi "+str(args.N_ITER)+" "+\
    					\
    					"-dir "+str(DIR_RESULTS)+" "\
    					\
    					"-factor "+str(args.FACTOR)+" "\
                        "-strip "+str(args.STRIP)+" ";
    					
    
    #process = subprocess.Popen(MPI + " -np " + str(args.NUM_PROCESSES) + " -ppn 1 " + NAME_PROGRAM + " " + command_args, shell=True)
    process = subprocess.Popen(MPI + " -np " + str(args.NUM_PROCESSES) + " -ppn " + str(args.NUM_PROCESSES) + " " + NAME_PROGRAM + " " + command_args, shell=True)
    process.wait()

	
run()




