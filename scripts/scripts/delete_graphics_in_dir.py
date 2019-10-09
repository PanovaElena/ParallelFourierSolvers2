import sys
import os
DIR=sys.argv[1]

def del_for_all_files(dir):
	files = []
	dirs=[]
	for (dirpath, dirnames, filenames) in os.walk(dir):
		files.extend(filenames)
		dirs.extend(dirnames)
		break
	for file in files:
		if (file.find(".png")!=-1):
			print(dir+"/"+file)
			os.remove(dir+"/"+file)
	if (len(dirs)>0):
		for dir_ in dirs:
			del_for_all_files(dir+"/"+dir_)	
			
del_for_all_files(DIR)