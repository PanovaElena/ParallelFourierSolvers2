import matplotlib
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import matplotlib.lines as mlines
import matplotlib.ticker as ticker


NITER = 32
NCORES_PER_NODE = 48

NODES = [1, 2, 4, 8, 12, 16, 20]

CORES = [nodes*NCORES_PER_NODE for nodes in NODES]

TIME_RTOC = {cores:0 for cores in CORES}
TIME_SOLV = {cores:0 for cores in CORES}
TIME_CTOR = {cores:0 for cores in CORES}
TIME_EXCH = {cores:0 for cores in CORES}
TIME_ITER = {cores:0 for cores in CORES}

for cores in CORES:
    n = cores * NITER / NCORES_PER_NODE
    with open("./res_weak_%d_cores/output" % (cores), "r") as file:

        lines = file.readlines()
        for line in lines:
            arr = line.split(" ")
            if arr[0]=="RANK":
                if arr[4]=="RtoC":
                    TIME_RTOC[cores] += float(arr[6])
                if arr[4]=="PSATD":
                    TIME_SOLV[cores] += float(arr[6])
                if arr[4]=="CtoR":
                    TIME_CTOR[cores] += float(arr[6])
                if arr[4]=="exchange":
                    TIME_EXCH[cores] += float(arr[6])

    TIME_RTOC[cores] /= n
    TIME_SOLV[cores] /= n
    TIME_CTOR[cores] /= n
    TIME_EXCH[cores] /= n
    TIME_ITER[cores] = TIME_RTOC[cores]+TIME_SOLV[cores]+TIME_CTOR[cores]+TIME_EXCH[cores]

# ----------- plot scal ---------------------

SCAL_RTOC = {cores:TIME_RTOC[CORES[0]]/TIME_RTOC[cores] for cores in CORES}
SCAL_SOLV = {cores:TIME_SOLV[CORES[0]]/TIME_SOLV[cores] for cores in CORES}
SCAL_CTOR = {cores:TIME_CTOR[CORES[0]]/TIME_CTOR[cores] for cores in CORES}
SCAL_EXCH = {cores:TIME_EXCH[CORES[0]]/TIME_EXCH[cores] for cores in CORES}
SCAL_ITER = {cores:TIME_ITER[CORES[0]]/TIME_ITER[cores] for cores in CORES}

matplotlib.rcParams.update({"font.size" : 17})

fig = plt.figure()
ax = fig.add_subplot(1,1,1)

#ax.plot(NODES, [time for cores,time in SCAL_RTOC.items()], '-->', linewidth=1.5, label="Прямое БПФ")
#ax.plot(NODES, [time for cores,time in SCAL_SOLV.items()], '-->', linewidth=1.5, label="Обновление полей")
#ax.plot(NODES, [time for cores,time in SCAL_CTOR.items()], '-->', linewidth=1.5, label="Обратное БПФ")
#ax.plot(NODES, [time for cores,time in SCAL_EXCH.items()], '-->', label="Scal exchange")
ax.plot(NODES, [time for cores,time in SCAL_ITER.items()], '->r', markersize=8, linewidth=2.5)

ax.grid()
ax.set_xlabel('Количество вычислительных узлов')
ax.set_ylabel('Эффективность')
ax.text(3.4, -0.34, '1 узел = 48 ядер, 20 узлов = 960 ядер', fontsize = 12)
ax.legend(loc="lower right", fontsize = 14)
ax.set_ylim((0.0, 1.2))

ax.grid(b=True, which='major', color="#777777")
ax.grid(b=True, which='minor', color="#999999", linestyle='--')
ax.xaxis.set_major_locator(ticker.FixedLocator(NODES))
#ax.xaxis.set_minor_locator(ticker.FixedLocator(THRS))
ax.yaxis.set_major_locator(ticker.FixedLocator([0, 1]))
ax.yaxis.set_minor_locator(ticker.MultipleLocator(0.2))
#ax.xaxis.set_minor_formatter(ticker.FormatStrFormatter("%d"))
ax.yaxis.set_minor_formatter(ticker.FormatStrFormatter("%.1f"))

fig.tight_layout()
plt.savefig("weak_scalability")
plt.show()

# ----------- plot time ---------------------

#matplotlib.rcParams.update({"font.size" : 17})
#
#fig = plt.figure()
#ax = fig.add_subplot(1,1,1)
#
#ax.plot(CORES, [time for cores,time in TIME_RTOC.items()], '-p', label="Time RtoC")
#ax.plot(CORES, [time for cores,time in TIME_SOLV.items()], '-p', label="Time PSATD")
#ax.plot(CORES, [time for cores,time in TIME_CTOR.items()], '-p', label="Time CtoR")
#ax.plot(CORES, [time for cores,time in TIME_EXCH.items()], '-p', label="Time exchange")
#ax.plot(CORES, [time for cores,time in TIME_ITER.items()], '-p', label="Time iter")
#
#ax.grid()
#ax.set_xlabel('Number of cores')
#ax.set_ylabel('Time, s')
#ax.legend()
#
#fig.tight_layout()
#plt.show()

# ------------------------------------------


