#!/usr/bin/env python

import os,sys
import copy
import codecs
import re
import glob
import fnmatch
from subprocess import call



numParticles = sys.argv[1]
path = sys.argv[2]
Parameters = numParticles + " "+ sys.argv[3] + " " + sys.argv[4]
print  (Parameters)
fnames = []
f2 = open("filenames.txt","w")
for root, dirs, files in os.walk(path):
    for basename in files:
        if fnmatch.fnmatch(basename, 'posit*.txt'):
            filename = os.path.join(root, basename)
            fnames.append(filename)
            print(filename)
strng = ""
strng = str(len(fnames))
f2.write(strng + "\n")
for name in fnames:
	f2.write(name)
	f2.write("\n");
f2.close()
os.system("./analyzer.exe " + Parameters)
os.system("cp *.txt " + path)
os.system("gnuplot plot.gnu")
os.system("cp *.png " + path)


