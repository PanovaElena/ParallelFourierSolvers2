import sys
import math as ma
sys.path.append("../../scripts/")
import read_file as fr
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.ticker as ticker
import subprocess
import os
import shutil

N_ITER = 180

if (sys.argv[1] == "0"):
    Nx = 320
    coordXMin = -20e-4
    coordXMax = 20e-4
    NUM_PROCESSES = 10
else:
    Nx = 32
    coordXMin = -19e-4
    coordXMax = -15e-4
    NUM_PROCESSES = 1

Ny = 256

DIR_PICTURES = "./pictures_%s/" % sys.argv[1]
DIR_RESULTS = "./results_%s/" % sys.argv[1]
DIR_RESULTS_2 = "./results_E_%s/" % sys.argv[1]

def createDir(dir):
    if (os.path.exists(dir)): 
         for (dirpath, dirnames, filenames) in os.walk(dir):
            for file in filenames:
                os.remove(dir+file)
    else: 
        os.mkdir(dir)
        
createDir(DIR_PICTURES)
createDir(DIR_RESULTS_2)

fig = plt.figure()    
       
def createFieldAx(a, b, c, text, field):
    ax = fig.add_subplot(a, b, c)
    ax.title.set_text(text)
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    ax.tick_params(axis='both', which='major', labelsize=10)
    ax.xaxis.set_major_locator(ticker.MultipleLocator(0.001))
    ax.xaxis.set_minor_locator(ticker.MultipleLocator(0.0005))
    ax.yaxis.set_major_locator(ticker.MultipleLocator(0.001))
    ax.yaxis.set_minor_locator(ticker.MultipleLocator(0.0005))
    im = ax.imshow(field, cmap='RdYlBu', interpolation='none', extent=(coordXMin, coordXMax, -20e-4, 20e-4),\
        animated = True, aspect='auto', vmax=5*10**8, vmin=0)
    fig.colorbar(im, ax=ax)
    return im
 
 
def readFile(iter, field):
    data=np.zeros(shape=(Ny,Nx))
    NxLocal = Nx // NUM_PROCESSES
    for npr in range(NUM_PROCESSES):
        with open(DIR_RESULTS + "global_fft_result_%s_rank_%d_iter_%d.csv" % (field, npr, iter), 'r') as file:
            i = 0
            for line in file.readlines():
                st = line.split(";")
                j = 0
                for elem in st:
                    try:
                        data[i + NxLocal * npr, j] = float(elem)
                    except:
                        pass
                    j += 1
                i += 1
    return data

    
im = createFieldAx(1, 1, 1, "|E|", np.zeros(shape = (Ny,Nx)))


for iter in range(N_ITER + 1):

    def compE(Ex, Ey, Ez):
        field = np.zeros(shape=(Ny,Nx))
        for i in range(Nx):
            for j in range(Ny):
                E = ma.sqrt(Ex[j, i]**2 + Ey[j, i]**2 + Ez[j, i]**2)
                field[j, i] = E
        return field  
    	
    Ex = readFile(iter, "Ex")
    Ey = readFile(iter, "Ey")
    Ez = readFile(iter, "Ez")                
    
    E = compE(Ex, Ey, Ez)
    
    with open(DIR_RESULTS_2+"global_fft_result_E_iter_%d.csv" % iter, 'w') as file:
        for j in range(Ny):
            for i in range(Nx):
                file.write(str(E[j, i])+";")
            file.write("\n")
    
    im.set_array(E)
    
    plt.savefig(DIR_PICTURES + 'field%04d.png' % (iter))
