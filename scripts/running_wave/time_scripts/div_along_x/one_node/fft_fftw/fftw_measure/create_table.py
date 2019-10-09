import sys
import os

RUN = ["thr", "prc"]

NUM_REPETITION=3

NUM_CORES=[1, 2, 4, 8, 16]

GRID_SIZES=[\
            {'nx':16,   'ny':2048, 'nz':2048},\
            {'nx':128,  'ny':128,  'nz':4096},\
            {'nx':128,  'ny':4096, 'nz':128},\
            {'nx':2048, 'ny':16,   'nz':2048},\
            {'nx':2048, 'ny':2048, 'nz':16},\
            {'nx':4096, 'ny':128, 'nz':128},\
           ]


with open("fftw_scal.csv", 'w') as csvFile:
    for run in RUN:
        for nCores in NUM_CORES:
            for grid in GRID_SIZES:
                arrNRepRtoC = []
                arrNRepCtoR = []
                for nRep in range(NUM_REPETITION):
                    nameFile = "output_"+str(grid['nx'])+"x"+str(grid['ny'])+"x"+str(grid['nz'])+"_"+run+"_"+str(nCores)+"_"+str(nRep)+".txt"
                    arrRtoC = []
                    arrCtoR = []                   
                    with open(nameFile) as file:
                        for line in file:
                            strArr = line.split(" ")
                            if (strArr[0] == "RtoC"):
                                try:
                                    arrRtoC.append(float(strArr[1]))
                                except:
                                    print(nameFile)
                            if (strArr[0] == "CtoR"):
                                try:
                                    arrCtoR.append(float(strArr[1]))
                                except:
                                    print(nameFile)
                    arrNRepRtoC.append(max(arrRtoC))
                    arrNRepCtoR.append(max(arrCtoR))
                csvFile.write(str(min(arrNRepRtoC)).replace(".",",")+";;"+str(min(arrNRepCtoR)).replace(".",",")+";;")
            csvFile.write("\n")  
        csvFile.write("\n")                     