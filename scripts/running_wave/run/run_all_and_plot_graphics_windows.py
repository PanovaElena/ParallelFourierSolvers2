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


NAME_SEQ_PROGRAM = "\""+DIR_SCRIPT+"/../../../build/bin/running_wave_sequential"+"\""
NAME_PAR_PROGRAM = "\""+DIR_SCRIPT+"/../../../build/bin/running_wave_seq_and_par"+"\""

DIR_PICTURES = "./pictures/"
DIR_RESULTS = "./results/"

NAME_FILE_SEQ = DIR_RESULTS+"/sequential_result_Ey.csv"
NAME_FILE_PAR = DIR_RESULTS+"/parallel_result_Ey.csv"

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

funcFindPar = None

if (args.dimension_of_output_data==1):
    funcRead = fr.readFile1d
    funcPlot = gr.plot1d
    funcCalcError = ce.computeError1d
    
    def func(np, guard):
        npx, npy, = np
        gx, gy, = guard
        data_par_in_process = []
        for prc in range(npx):
            data_par_in_process.append(funcRead(DIR_RESULTS+"/after_last_exchange_rank_%d.csv" % prc))
        data_par = []
        for prc in range(npx):
            data_par.extend(data_par_in_process[prc][gx:len(data_par_in_process[prc])-gx])
        return data_par
                
    funcFindPar = func
    
else: 
    funcRead = fr.readFile2d
    funcPlot = gr.plot2d
    funcCalcError = ce.computeError2d
    
    def func(np, guard):
        npx, npy, = np
        gx, gy, = guard
        data_par_in_process = [[] for prcx in range(npx)]
        for prcx in range(npx):
            for prcy in range(npy):
                data_par_in_process[prcx].append(funcRead(DIR_RESULTS+"/after_last_exchange_rank_%d.csv" % npx*prcy+prcx))
        n_lines = len(data_par_in_process[0][0])
        data_par = [[] for i in range(n_lines-2*gy)]
        for prcx in range(npx):
            for prcy in range(npy):
                for i in range(n_lines):
                    data_par[i].extend(data_par_in_process[prcx][prcy][i][gx:len(data_par_in_process[prcx][prcy][i])-gx])
        return data_par
        
    funcFindPar = func

# sequential

command_args_seq = "-ax "+str(args.ax)+" "+\
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
                    "-ni "+str(args.n_iter)+" "+\
                    \
                    "-dim "+str(args.dimension_of_output_data)+" "+\
                    "-dump on "+\
                    \
                    "-dir "+str(DIR_SCRIPT+DIR_RESULTS)+" "\
                    \
                    "-lambda "+str(args.lambd)+" "+\
                    "-angle "+str(args.angle)+" ";
                    
                    
process_seq = subprocess.Popen(NAME_SEQ_PROGRAM+" "+command_args_seq, shell=True)
process_seq.wait()


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
                    "-ni "+str(args.n_iter)+" "+\
                    "-npari "+str(args.n_par_iter)+" "+\
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
                    
np=args.npx*args.npy*args.npz

if(args.n_iter!=0):                
    process_par = subprocess.Popen(MPI+" -n "+str(np)+" "+NAME_PAR_PROGRAM+" "+command_args_par, shell=True)
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
        if (file.find('csv')!=-1):
            if (file.find('spectrum')!=-1):
                data=invert_data(funcRead(DIR_RESULTS+file))
                arg=create_x(len(data))
                funcPlot(DIR_PICTURES, file, data, arg, _log=True)
            else:
                funcPlot(DIR_PICTURES, file, funcRead(DIR_RESULTS+file))

if (args.n_iter != 0):

    data_par = funcRead(NAME_FILE_PAR)               
    data_seq = funcRead(NAME_FILE_SEQ)
    funcPlot(DIR_PICTURES, "error", funcCalcError(data_seq, data_par, _abs=True), _log=True)





