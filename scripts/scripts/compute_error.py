def computeError1d(data1, data2, _abs=False):
	data=[]
	for i in range(len(data1)):
		if (_abs): data.append(abs(data1[i]-data2[i]))
		else: data.append(data1[i]-data2[i])
	return data
	
def computeError2d(data1, data2, _abs=False):
	data=[]
	for i in range(len(data1)):
		tmp=[]
		for j in range(len(data1[i])):
			if (_abs): tmp.append(abs(data1[i][j]-data2[i][j]))
			else: tmp.append(data1[i][j]-data2[i][j])
		data.append(tmp)
	return data
