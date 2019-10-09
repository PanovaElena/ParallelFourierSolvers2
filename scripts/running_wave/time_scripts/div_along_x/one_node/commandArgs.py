def makeArgs(dict):
	st = ""
	for key in dict:
		st+=" -"+key+" "+str(dict[key])
	return st