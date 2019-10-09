def readFile1d(file):
	data=[]
	with open(file) as file:
		for x in file: 
			try:
				if(x!="\n" and x!=" "):
					data.append(float(x))
				else:
					break
			except:
				print("ERROR, can't convert data to float")
	return data
	
def readFile2d(file):
	data=[]
	with open(file) as file:
		for line in file: 
			try:
				arr=line.strip().split(';'); arr.pop()
				row=[float(x) for x in arr]
				data.append(row)
			except:
				print("ERROR, can't convert data to float")
	return data
