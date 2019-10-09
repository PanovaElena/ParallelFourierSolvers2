import sys
sys.path.append("../../../scripts/")
import graphics as gr
#import read_file as rf
import compute_error as ce

file1 = sys.argv[1]
file2 = sys.argv[2]

counter = 0
def readFile2d(file):
	global counter
	data=[]
	with open(file) as file:
		for line in file: 
			try:
				arr=line.strip().split(';'); arr.pop()
				row=[]
				if (counter == 0):
					a = 0
					b = 64 + 14
				else:
					a = 28
					b = 28 + 64 + 14
				for j in range(a, b):
					row.append(float(arr[j]))
				data.append(row)
			except:
				print("ERROR, can't convert data to float")
	counter+=1
	return data

funcRead = readFile2d
funcPlot = gr.plot2d
funcCompError = ce.computeError2d

funcPlot("./", file1+"__"+file2, funcCompError(funcRead(file1), funcRead(file2)))