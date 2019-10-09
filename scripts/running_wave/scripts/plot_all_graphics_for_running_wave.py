import subprocess;
import sys

NUM_OF_PROCESSES=str(int(sys.argv[1]))
DIR_FROM="parallel_results\\"
DIR_TO="running_wave_graphics\\"
NAME_SCRIPT="..\\..\\scripts\\plot_all_graphs_1d.py"

process = subprocess.Popen("python "+NAME_SCRIPT+ " "+ NUM_OF_PROCESSES + " " + DIR_FROM + " " + DIR_TO)
process.wait()