import sys
import math
sys.path.append("../../scripts/")
import read_file as fr
import graphics as gr
import subprocess
import os
import shutil
import args

DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0])

NUM_PROCESSES=10

DIR_PICTURES = "./pictures/"

if (os.path.exists(DIR_PICTURES)): 
     for (dirpath, dirnames, filenames) in os.walk(DIR_PICTURES):
    	for file in filenames:
    		os.remove(DIR_PICTURES+file)
else: 
    os.mkdir(DIR_PICTURES)
    
       


	





