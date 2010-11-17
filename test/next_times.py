#!/usr/bin/env python

f = open("sorted.dat").readlines()
gn = {}
for x in f:
    if x.strip() == "": continue
    x = x.split()
    gn[(int(x[0]), int(x[1]))] = int(x[3])

maxg = max([x[0] for x in gn])
prec = -1;
for g in range(maxg+2):
    l = sorted([(x[1], gn[x]) for x in gn.keys() if x[0] == g])
    if l == []:
        n = 0
        print "g, n = (%2d, %2d): boh." % (g,n)
    elif len(l) == 1:
        n = l[-1][0] + 1
        h = prec * l[-1][1] / 3600000
        print "g, n = (%2d, %2d): %6.2lf ore (stima)." % (g,n,h)
    else:
        n = l[-1][0] + 1
        h = 1.0 * l[-1][1] * l[-1][1] / l[-2][1] / 3600000
        prec = 1.0 * l[-1][1] / l[-2][1]
        print "g, n = (%2d, %2d): %6.2lf ore (slope = %6.2lf)" % (g,n,h,prec)
