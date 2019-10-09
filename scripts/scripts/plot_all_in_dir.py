import sys
sys.path.append("../../scripts/")
import os
import graphics as gr
import read_file as fr

DIR = sys.argv[1]
DIM = int(sys.argv[2])

if(DIM == 1):
	funcRead = fr.readFile1d
	funcPlot = gr.plot1d
elif(DIM == 2):
	funcRead = fr.readFile2d
	funcPlot = gr.plot2d
else:
	print("ERROR: Wrong dimension of data, it has to be 1 or 2")

if (not os.path.exists(DIR+"/pictures")):
	os.mkdir(DIR+"/pictures")

os.walk(DIR)
for (dirpath, dirnames, filenames) in os.walk(DIR):
	for file in filenames:
		funcPlot(DIR+"/pictures", file, funcRead(DIR+"/"+file))
	
