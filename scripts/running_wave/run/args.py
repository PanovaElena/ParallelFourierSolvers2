import math

# ligth speed, cm/s

LIGHT_SPEED = 29979245800

# area, cm

ax = 0
bx = 256
ay = 0
by = 1
az = 0
bz = 256

# grid

nx = 256
ny = 1
nz = 256

dx = (bx-ax)/nx
dy = (by-ay)/ny
dz = (bz-az)/nz

# time step, s

#dt = (math.sqrt(2)*dx)/(LIGHT_SPEED*math.pi)  # courant condition for PSTD
dt = dx/(LIGHT_SPEED*4)

# field solver: PSTD, PSATD, PSATD_omp or FDTD (for sequential launch only)

solver = "PSATD"

# parameters of wave

lambd = 16 * dx    # length of wave
angle = 0          # angle between OX and direction of movement of the wave

# output

dimension_of_output_data = 1   # 1 (OX) or 2 (XOZ)

# number of iterations for sequention launch

n_iter = 10


# parameters for parallel launch

scheme = "copy"    # scheme of exchange: sum or copy

# num of processes

npx = 2
npy = 1
npz = 2

# dimension of guard (number of cells of the grid)

gx = 64
gy = 0
gz = 64

# number of sequential (first) and parallel (second) iterations

n_sequential_iter = 0
n_parallel_iter = n_iter

# max number of iterations in every domain
# if scheme=copy max distance should be half of the guard width

number_of_iterations_in_domain = 1 #int(0.4*gx*dx/LIGHT_SPEED/dt)

# parameters of mask

mask = "simple"   # simple or smooth

mwx = gx/4        # number of cells for mask (for smooth only)
mwy = 0
mwz = gz/4 

# parameters of low frequency filter

filter = "off"    # on or off

fwx = 64        # number of frequences in fourier space which will be changed
fwy = 0
fwz = 0

fnzx = 64-14      # number of frequences in fourier space which will be set to zero
fnzy = 0
fnzz = 0




