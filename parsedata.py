#!/usr/bin/python

import sys
import os
import os.path
import shutil



inputdir = '.' #sys.argv[1]
dirs = os.listdir(inputdir)

for file in dirs:
    print file.endswith('.cpp') or file.endswith('.h')
    if( file.endswith('.cpp') or file.endswith('.h') ):
        print "Processing ... " + file
        out = open('output.txt','w')
        out.write('/**\n')
        out.write(' * @file '+file+'\n')
        out.write(' *\n')
        out.write(' * @author Jan Navratil <navratil.jan.83@seznam.cz>\n')
        out.write(' * @date 2013\n')
        out.write(' */\n')
        out.write('\n')

        f = open(file)
        out.write(f.read())
        f.close()
        out.close()

        shutil.move('output.txt',file)
    else:
        continue



