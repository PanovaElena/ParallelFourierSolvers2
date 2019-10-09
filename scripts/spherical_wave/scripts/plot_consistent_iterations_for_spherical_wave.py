import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.colors import LinearSegmentedColormap
import math
import sys

N=400
i=0
d=100
nameDir=".\\consistent_results\\E\\iter_"

def UnPack(file):
	res=[]
	i=0
	for line in file:
		res.insert(i,line.split(';'))
		res[i].pop()
		j=0
		for elem in res[i]:
			res[i][j]=float(elem)
			j=j+1
		i=i+1
	return res
	
while(i<=N):
	fileName=nameDir+str(i)+".csv"
	filePath=".\\spherical_wave_graphics_consistent\\"
	graphName=str(i)
	
	file=open(fileName)
	dat = UnPack(file)

	fig = plt.figure()
	ax = fig.add_subplot(111)

	cm='gray'
	#cs = ax.contourf(dat, cmap=cm)
	cs = ax.imshow(dat, cmap=cm)
	fig.colorbar(cs, ax=ax)
	ax.set_title(graphName)

	plt.tight_layout()

	plt.savefig(filePath+graphName+'.png')

	file.close()

	i=i+d
	
