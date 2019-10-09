def makeArgs(dict):
	str = ""
	for key in dict:
		str+=" -"+str(key)+" "+dict[key]
	return str