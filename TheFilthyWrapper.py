import os,sys
import copy
import codecs
import re
import glob
import fnmatch
from subprocess import call

path = 'D:\\NewDev'
N = "512"
fnames = []
f2 = open("filenames.txt","w")
for root, dirs, files in os.walk(path):
    for basename in files:
        if fnmatch.fnmatch(basename, '*position*'):
            filename = os.path.join(root, basename)
            fnames.append(filename)
strng = ""
strng = str(len(fnames))
f2.write(strng + "\n")
for name in fnames:
	f2.write(name)
	f2.write("\n");
f2.close()
os.system("analyzer.exe " + N)
			
