import math

# ligth speed, cm/s

LIGHT_SPEED = 29979245800

np = 2

# grid

factor = 0.5
block = int(256 * factor)
nx = block
ny = block
nz = block
print(nx, "x", ny, "x", nz)

# area, cm

ax = 0
bx = nx
ay = 0
by = ny
az = 0
bz = nz

# grid step

dx = (bx-ax)/nx
dy = (by-ay)/ny
dz = (bz-az)/nz

# time step, s

#dt = (math.sqrt(2)*dx)/(LIGHT_SPEED*math.pi)  # courant condition for PSTD
dt = dx/(LIGHT_SPEED*4.0) * 16

# field solver: PSTD, PSATD, PSATD_omp or FDTD (for sequential launch only)

solver = "PSATD"

# parameters of wave

lambd = nx*dx*0.5    # wavelength
angle = 0          # angle between OX and direction of movement of the wave (grad)

# output

dimension_of_output_data = 1   # 1 (OX) or 2 (XOZ)

# number of iterations for sequention launch

n_iter = 2 #32

# parameters for parallel launch

n_seq_iter = 1
n_par_iter = n_iter - n_seq_iter

scheme = "copy"    # scheme of exchange: sum or copy

# num of processes

npx = np
npy = 1
npz = 1

# dimension of guard (number of cells of the grid)

gx = 32
gy = 0
gz = 0

# max number of iterations in every domain
# if scheme=copy max distance should be half of the guard width

number_of_iterations_in_domain = min([int(0.8*gx*dx/LIGHT_SPEED/dt*(0.5 if scheme=="copy" else 1.0)) if gx>0 else 10**10,
                                      int(0.8*gy*dy/LIGHT_SPEED/dt*(0.5 if scheme=="copy" else 1.0)) if gy>0 else 10**10,
                                      int(0.8*gz*dz/LIGHT_SPEED/dt*(0.5 if scheme=="copy" else 1.0)) if gz>0 else 10**10,
                                     ])
#print("max n_iter_betw_exc:", min([int(0.8*gx*dx/LIGHT_SPEED/dt*(0.5 if scheme=="copy" else 1.0)) if gx>0 else 10**10,
#                                   int(0.8*gy*dy/LIGHT_SPEED/dt*(0.5 if scheme=="copy" else 1.0)) if gy>0 else 10**10,
#                                   int(0.8*gz*dz/LIGHT_SPEED/dt*(0.5 if scheme=="copy" else 1.0)) if gz>0 else 10**10,
#                                   ]))

#number_of_iterations_in_domain = 1

#print("cur n_iter_betw_exc:", number_of_iterations_in_domain)

# parameters of mask

mask = "simple"   # simple or smooth

mwx = 6        # number of cells for mask (for smooth only)
mwy = 0
mwz = 0 

# parameters of low frequency filter

filter = "off"    # on or off

fwx = 64        # number of frequences in fourier space which will be changed
fwy = 0
fwz = 0

fnzx = 64-14      # number of frequences in fourier space which will be set to zero
fnzy = 0
fnzz = 0




