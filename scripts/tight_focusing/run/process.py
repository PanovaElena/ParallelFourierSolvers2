import sys
import math as ma
sys.path.append("../../scripts/")
import numpy as np
import subprocess
import os
import shutil
import args

def process():
    if (args.STRIP == 0):
        Nx = 160
        coordXMin = -20e-4
        coordXMax = 20e-4
    else:
        Nx = 16
        coordXMin = -19e-4
        coordXMax = -15e-4
        args.NUM_PROCESSES = 1
    
    Ny = 128
    
    DIR_RESULTS = "./results_%s/" % str(args.STRIP)
    DIR_RESULTS_2 = "./results_E_%s/" % str(args.STRIP)
    
    def createDir(dir):
        if (os.path.exists(dir)): 
             for (dirpath, dirnames, filenames) in os.walk(dir):
                for file in filenames:
                    os.remove(dir+file)
        else: 
            os.mkdir(dir)
            
    createDir(DIR_RESULTS_2)
     
    def readFile(iter, field):
        data=np.zeros(shape=(Nx,Ny))
        NxLocal = Nx // args.NUM_PROCESSES
        for npr in range(args.NUM_PROCESSES):
            with open(DIR_RESULTS + "global_fft_result_%s_rank_%d_iter_%d.csv" % (field, npr, iter), 'r') as file:
                j = 0
                for line in file.readlines():
                    st = line.split(";")
                    i = 0
                    for elem in st:
                        try:
                            data[i + NxLocal * npr, j] = float(elem)
                        except:
                            pass
                        i += 1
                    j += 1
        return data
    
    
    for iter in range(args.N_ITER + 1):
    
        def compE(Ex, Ey, Ez):
            field = np.zeros(shape=(Nx,Ny))
            for i in range(Nx):
                for j in range(Ny):
                    E = ma.sqrt(Ex[i, j]**2 + Ey[i, j]**2 + Ez[i, j]**2)
                    field[i, j] = E
            return field  
        	
        Ex = readFile(iter, "Ex")
        Ey = readFile(iter, "Ey")
        Ez = readFile(iter, "Ez")                
        
        E = compE(Ex, Ey, Ez)
        
        with open(DIR_RESULTS_2+"global_fft_result_E_iter_%d.csv" % iter, 'w') as file:
            for j in range(Ny):
                for i in range(Nx):
                    file.write(str(E[i, j])+";")
                file.write("\n")
 
process() 