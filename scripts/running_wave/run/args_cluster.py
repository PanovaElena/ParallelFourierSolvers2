import math

# ligth speed, cm/s

LIGHT_SPEED = 29979245800

np = 4

# grid

factor = 1
nx = int(256 * factor) * np
ny = int(128 * factor)
nz = int(256 * factor)
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
dt = dx/(LIGHT_SPEED*4)

# field solver: PSTD, PSATD, PSATD_omp or FDTD (for sequential launch only)

solver = "PSATD"

# parameters of wave

lambd = nx * dx / np    # wavelength
angle = 0          # angle between OX and direction of movement of the wave (grad)

# output

dimension_of_output_data = 1   # 1 (OX) or 2 (XOZ)

# number of iterations for sequention launch

n_iter = 3 #32


# parameters for parallel launch

scheme = "copy"    # scheme of exchange: sum or copy

# num of processes

npx = np
npy = 1
npz = 1

# dimension of guard (number of cells of the grid)

gx = 8
gy = 0
gz = 0

# max number of iterations in every domain
# if scheme=copy max distance should be half of the guard width

# number_of_iterations_in_domain = min([int(0.8*gx*dx/LIGHT_SPEED/dt*(0.5 if scheme=="copy" else 1.0)) if gx>0 else 10**10,
                                      # int(0.8*gy*dy/LIGHT_SPEED/dt*(0.5 if scheme=="copy" else 1.0)) if gy>0 else 10**10,
                                      # int(0.8*gz*dz/LIGHT_SPEED/dt*(0.5 if scheme=="copy" else 1.0)) if gz>0 else 10**10,
                                     # ])
#print("max n_iter_betw_exc:", min([int(0.8*gx*dx/LIGHT_SPEED/dt*(0.5 if scheme=="copy" else 1.0)) if gx>0 else 10**10,
#                                   int(0.8*gy*dy/LIGHT_SPEED/dt*(0.5 if scheme=="copy" else 1.0)) if gy>0 else 10**10,
#                                   int(0.8*gz*dz/LIGHT_SPEED/dt*(0.5 if scheme=="copy" else 1.0)) if gz>0 else 10**10,
#                                   ]))

number_of_iterations_in_domain = 1

#print("cur n_iter_betw_exc:", number_of_iterations_in_domain)

# parameters of mask

mask = "simple"   # simple or smooth

mwx = gx/4        # number of cells for mask (for smooth only)
mwy = gy/4
mwz = gz/4 

# parameters of low frequency filter

filter = "off"    # on or off

fwx = 64        # number of frequences in fourier space which will be changed
fwy = 0
fwz = 0

fnzx = 64-14      # number of frequences in fourier space which will be set to zero
fnzy = 0
fnzz = 0




