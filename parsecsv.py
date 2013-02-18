import os, os.path;

f = open('randomVPLs.csv', 'r')

pos = list()
norm = list()
color = list()

for i in range(2048):
    pos.append([0,0,0])
    norm.append([0,0,0])
    color.append([0,0,0])

i=0
stav=0
for line in f:
    
    if line.startswith('Red'):
        stav+=1
        i=0
    elif line.startswith('Green'):
        i=1
    elif line.startswith('Blue'):
        i=2
    else :
        continue
        
    s = line.partition(':')[2]
    spl_s = s.strip(', \n').split(',')

    j=0
    for p in spl_s:
        if stav == 1:
            color[j][i] = str(p)
        if stav == 2:            
           pos[j][i] = str(p)
        if stav == 3:
           norm[j][i] = str(p)
        j+=1

f.close()

o = open('output.txt', 'w')
for i in range(2048):
    o.write(','.join(color[i]))
    o.write('\t')
    o.write(','.join(pos[i]))
    o.write('\t')
    o.write(','.join(norm[i]))
    o.write('\t')
    o.write('\n')
o.close()
