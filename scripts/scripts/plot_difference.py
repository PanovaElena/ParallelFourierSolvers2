import sys
import graphics as gr
import read_file as rf
import compute_error as ce

file1 = sys.argv[1]
file2 = sys.argv[2]
dimension = int(sys.argv[3])

if (dimension == 1):
	funcRead = rf.readFile1d
	funcPlot = gr.plot1d
	funcCompError = ce.computeError1d
elif (dimension == 2):
	funcRead = rf.readFile2d
	funcPlot = gr.plot2d
	funcCompError = ce.computeError2d
	
funcPlot("./", "plot_diff_res", funcCompError(funcRead(file1), funcRead(file2)))