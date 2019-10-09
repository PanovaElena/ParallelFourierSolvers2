path1="C:\\Users\\Alyona\\Documents\\Visual Studio 2017\\Projects\\parallel-algorithms\\files\\parallel_field_solver_console\\rank_0_iter_9_E.csv"
path2="C:\\Users\\Alyona\\Documents\\Visual Studio 2017\\Projects\\parallel-algorithms\\files\\parallel_field_solver_console\\rank_1_iter_9_E.csv"
N=128 - 32

def UnPack(file):
	res=[]
	i=0
	for line in file:
		res.insert(i,line.split(';'))
		res[i].pop()
		res[i].pop()
		j=0
		for elem in res[i]:
			res[i][j]=float(res[i][j])
			j=j+1
		i=i+1
	return res
	
file1=open(path1, 'r')
data1=UnPack(file1)

file2=open(path2, 'r')
data2=UnPack(file2)


f=1
i=0
eps=1e-7
while(i<N):
	j=0
	while(j<N):
		diff=abs(float(data1[i][j])-float(data2[i][N-j]))
		if (diff > eps):
			f=0
			print(i, " ", j, " ", diff)
		j=j+1
	i=i+1

if(f==0):
	print("not\n")

if (f==1):
	print("yes\n")	
	
