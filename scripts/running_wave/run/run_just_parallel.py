import sys
import math
sys.path.append("../../scripts/")
import subprocess
import os
import graphics as gr
import read_file as fr
import compute_error as ce
import args

DIR_PICTURES = "./pictures/"
DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0]) 

if (sys.platform == "win32"):
    MPI = "mpiexec"
else: MPI = "mpirun"

NAME_PAR_PROGRAM = "\""+DIR_SCRIPT+"/../../../build/bin/running_wave_parallel"+"\""

DIR_PICTURES = "./pictures/"
DIR_RESULTS = "./results/"

if (os.path.exists(DIR_PICTURES)): 
    for (dirpath, dirnames, filenames) in os.walk(DIR_PICTURES):
        for file in filenames:
            os.remove(DIR_PICTURES+file)
else: os.mkdir(DIR_PICTURES)

if (os.path.exists(DIR_RESULTS)): 
    for (dirpath, dirnames, filenames) in os.walk(DIR_RESULTS):
        for file in filenames:
            os.remove(DIR_RESULTS+file)
else: os.mkdir(DIR_RESULTS)

if (args.dimension_of_output_data==1):
    funcRead = fr.readFile1d
    funcPlot = gr.plot1d
    funcCalcError = ce.computeError1d
else: 
    funcRead = fr.readFile2d
    funcPlot = gr.plot2d
    funcCalcError = ce.computeError2d

# parallel

command_args_par = "-ax "+str(args.ax)+" "+\
                    "-ay "+str(args.ay)+" "+\
                    "-az "+str(args.az)+" "+\
                    \
                    "-dx "+str(args.dx)+" "+\
                    "-dy "+str(args.dy)+" "+\
                    "-dz "+str(args.dz)+" "+\
                    \
                    "-nx "+str(args.nx)+" "+\
                    "-ny "+str(args.ny)+" "+\
                    "-nz "+str(args.nz)+" "+\
                    \
                    "-dt "+str(args.dt)+" "+\
                    \
                    "-solver "+str(args.solver)+" "+\
                    \
                    "-npx "+str(args.npx)+" "+\
                    "-npy "+str(args.npy)+" "+\
                    "-npz "+str(args.npz)+" "+\
                    \
                    "-gx "+str(args.gx)+" "+\
                    "-gy "+str(args.gy)+" "+\
                    "-gz "+str(args.gz)+" "+\
                    \
                    "-npari "+str(args.n_iter)+" "+\
                    "-ndomi "+str(args.number_of_iterations_in_domain)+" "+\
                    \
                    "-scheme "+str(args.scheme)+" "+\
                    \
                    "-mask "+str(args.mask)+" "+\
                    "-mwx "+str(args.mwx)+" "+\
                    "-mwy "+str(args.mwy)+" "+\
                    "-mwz "+str(args.mwz)+" "+\
                    \
                    "-filter "+str(args.filter)+" "+\
                    "-fwx "+str(args.fwx)+" "+\
                    "-fwy "+str(args.fwy)+" "+\
                    "-fwz "+str(args.fwz)+" "+\
                    "-fnzx "+str(args.fnzx)+" "+\
                    "-fnzy "+str(args.fnzy)+" "+\
                    "-fnzz "+str(args.fnzz)+" "+\
                    \
                    "-dim "+str(args.dimension_of_output_data)+" "+\
                    \
                    "-dir "+str(DIR_SCRIPT+DIR_RESULTS)+" "+\
                    "-dump on "+\
                    \
                    "-lambda "+str(args.lambd)+" "+\
                    "-angle "+str(args.angle)+" ";
                    
np=args.np

if(args.n_iter!=0):
    maxtime = 120
    bash_command = ("mpiexec -n %d " % (np)) + NAME_PAR_PROGRAM + " " + command_args_par
    process_par = subprocess.Popen(bash_command, shell=True)
    process_par.wait()


# plot

def invert_data(data):
    data1=[]
    n=int(len(data))
    for i in range(n//2+1, n):
        if(data[i]<1e-12):
            data1.append(1e-12)
        else: data1.append(data[i])
    for i in range(0, n//2+1):
        if(data[i]<1e-12):
            data1.append(1e-12)
        else: data1.append(data[i])
    return data1
        
def create_x(n):
    data=[]
    for i in range(int(n)):
        data.append(-n/2+i)
    return data

os.walk(DIR_RESULTS)
for (dirpath, dirnames, filenames) in os.walk(DIR_RESULTS):
    for file in filenames:
        funcPlot(DIR_PICTURES, file, funcRead(DIR_RESULTS+file))
  


