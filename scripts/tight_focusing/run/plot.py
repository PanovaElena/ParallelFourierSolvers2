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
import args


if (args.STRIP == 0):
    Nx = 320
    coordXMin = -20e-4
    coordXMax = 20e-4
else:
    Nx = 32
    coordXMin = -19e-4
    coordXMax = -15e-4
    args.NUM_PROCESSES = 1

Ny = 256

DIR_PICTURES = "./pictures_%s/" % str(args.STRIP)
DIR_RESULTS = "./results_%s/" % str(args.STRIP)
DIR_RESULTS_2 = "./results_E_%s/" % str(args.STRIP)

def createDir(dir):
    if (os.path.exists(dir)): 
         for (dirpath, dirnames, filenames) in os.walk(dir):
            for file in filenames:
                os.remove(dir+file)
    else: 
        os.mkdir(dir)
        
createDir(DIR_PICTURES)

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
 
 
def readFile(iter):
    data=np.zeros(shape=(Ny,Nx))
    with open(DIR_RESULTS_2 + "global_fft_result_E_iter_%d.csv" % (iter), 'r') as file:
        j = 0
        for line in file.readlines():
            st = line.split(";")
            i = 0
            for elem in st:
                try:
                    data[j, i] = float(elem)
                except:
                    pass
                i += 1
            j += 1
    return data

    
im = createFieldAx(1, 1, 1, "|E|", np.zeros(shape = (Ny,Nx)))


for iter in range(args.N_ITER + 1):

    	
    E = readFile(iter)
    
    im.set_array(E)
    
    plt.savefig(DIR_PICTURES + 'field%04d.png' % (iter))

