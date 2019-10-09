import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.colors import LinearSegmentedColormap
import pylab as pl
	
def plot1d(dir, name, data, arg=[], xlabel="", ylabel="", points=False, _log=False):
	mpl.rcParams.update({'font.size': 20})
	if (arg==[]): arg=range(0, len(data))
	fig = plt.figure()
	ax = fig.add_subplot(111)
	if (_log):
		ax.semilogy(arg, data)
		if (points==True): ax.semilogy(arg, data, "*")
	else:
		ax.plot(arg, data)
		if (points==True): ax.plot(arg, data, "*")
	pl.xlabel(xlabel)
	pl.ylabel(ylabel)
	#ax.set_title(name)
	plt.tight_layout()
	plt.savefig(dir+"/"+name+".png")
	
def plot2d(dir, name, data):
	mpl.rcParams.update({'font.size': 20})
	fig = plt.figure()
	ax = fig.add_subplot(111)
	cs = ax.imshow(data, cmap='gray')
	fig.colorbar(cs, ax=ax)
	#ax.set_title(name)
	plt.tight_layout()
	plt.savefig(dir+"/"+name+".png")
	