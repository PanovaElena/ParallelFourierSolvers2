import math

# ligth speed, cm/s

LIGHT_SPEED = 29979245800

# area, cm

ax = -64*LIGHT_SPEED
bx = 64*LIGHT_SPEED
ay = -64*LIGHT_SPEED
by = 64*LIGHT_SPEED
az = 0
bz = LIGHT_SPEED

# grid

nx = 128
ny = 128
nz = 1

dx = (bx-ax)/nx
dy = (by-ay)/ny
dz = (bz-az)/nz

# time step, s

dt = 0.1  # less than courant condition for PSTD

# field solver: PSTD, PSATD or FDTD (for sequential launch only)

solver = "PSATD"

# parameters of wave

coord_source_x = 0                # coordinates of source
coord_source_y = 0   
coord_source_z = 0

time_source = 8                   # time of working of source
start_time_source = 0             # start time of working of source
omega = 2*math.pi / time_source   # frequency of source
omega_envelope = omega
            
width_source_x = dx*8             # width of source
width_source_y = dy*8   
width_source_z = dz/4      

# output

dimension_of_output_data = 2   # 1 (OX) or 2 (XOZ)

# number of iterations for sequention launch

n_iter = 400


# parameters for parallel launch

scheme = "sum"    # scheme of exchange: sum or copy

# num of processes

npx = 2
npy = 2
npz = 1

# dimension of guard (number of cells of the grid)

gx = 32
gy = 32
gz = 0

# number of sequential (first) and parallel (second) iterations

n_sequential_iter = 300
n_parallel_iter = n_iter-n_sequential_iter

# max number of iterations in every domain
# if scheme=copy max distance should be half of the guard width

number_of_iterations_in_domain = int(0.4*gx*dx/LIGHT_SPEED/dt)

# parameters of mask

mask = "simple"   # simple or smooth

mwx = gx/4        # number of cells for mask (for smooth only)
mwy = gy/4
mwz = gz/4

# parameters of low frequency filter

filter = "off"    # on or off

fwx = 8           # number of frequences in fourier space which will be changed
fwy = 0
fwz = 0

fnzx = 4          # number of frequences in fourier space which will be set to zero
fnzy = 0
fnzz = 0




