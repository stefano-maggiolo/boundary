#!/usr/bin/env python

from subprocess import Popen, PIPE
from sys import argv, exit
import shelve

from test_graph import plot

# Database of known cases

correct = {}

correct[(0,3)] = ((0,1,),
                  (1,))
correct[(0,4)] = ((0,1,1,),
                  (1,1,))
correct[(0,5)] = ((0,1,1,1,),
                  (1,1,1,))
correct[(0,6)] = ((0,1,2,2,2,),
                  (1,2,2,2,))
correct[(0,7)] = ((0,1,2,4,4,2,),
                  (1,2,4,4,2,))
correct[(0,8)] = ((0,1,3,6,10,8,4,),
                  (1,3,6,10,8,4,))
correct[(0,9)] = ((0,1,3,9,17,22,15,6,),
                  (1,3,9,17,22,15,6,))
correct[(0,10)] = ((0,1,4,12,30,47,53,32,11,),
                   (1,4,12,30,47,53,32,11,))
correct[(0,11)] = ((0,1,4,16,44,91,127,121,66,18,),
                   (1,4,16,44,91,127,121,66,18,))
correct[(0,12)] = ((0,1,5,20,67,158,282,346,292,142,37,),
                   (1,5,20,67,158,282,346,292,142,37,))
correct[(0,13)] = ((0,1,5,25,91,258,539,841,921,688,306,66,),
                   (1,5,25,91,258,539,841,921,688,306,66,))
correct[(0,14)] = ((0,1,6,30,126,397,978,1804,2498,2456,1662,672,135,),
                   (1,6,30,126,397,978,1804,2498,2456,1662,672,135,))
correct[(0,15)] = ((0,1,6,36,163,588,1636,3550,5856,7260,6489,3978,1483,265,),
                   (1,6,36,163,588,1636,3550,5856,7260,6489,3978,1483,265,))

correct[(1,1)] = ((0,2,),
                  (1,1,))
correct[(1,2)] = ((0,2,3,),
                  (1,2,2,))
correct[(1,3)] = ((0,2,5,4,),
                  (1,3,4,3,))
correct[(1,4)] = ((0,2,8,12,8,),
                  (1,4,9,10,6,))
correct[(1,5)] = ((0,2,10,24,27,13,),
                  (1,5,14,24,22,10,))
correct[(1,6)] = ((0,2,13,41,73,65,27,),
                  (1,6,22,49,69,53,21,))
correct[(1,7)] = ((0,2,15,61,144,202,151,50,),
                  (1,7,30,85,156,184,123,39,))
correct[(1,8)] = ((0,2,18,86,259,485,565,361,107,),
                  (1,8,41,137,319,494,504,295,84,))
correct[(1,9)] = ((0,2,20,115,414,990,1546,1528,858,218,),
                  (1,9,52,207,573,1122,1511,1343,701,172,))
correct[(1,10)] = ((0,2,23,148,630,1812,3596,4792,4138,2063,474,),
                   (1,10,66,297,968,2270,3848,4550,3602,1690,376,))
correct[(1,11)] = ((0,2,25,185,900,3059,7335,12420,14457,11042,4960,1016,),
                   (1,11,80,410,1526,4205,8558,12744,13442,9545,4070,809,))
    
correct[(2,0)] = ((0,3,4,),
                  (1,2,2,2,))
correct[(2,1)] = ((0,3,7,6,),
                  (1,2,5,5,3,))
correct[(2,2)] = ((0,3,16,25,16,),
                  (1,4,10,19,17,9,))
correct[(2,3)] = ((0,3,22,62,71,33,),
                  (1,5,18,41,60,47,19,))
correct[(2,4)] = ((0,3,31,120,233,212,84,),
                  (1,7,29,87,164,204,141,50,))
correct[(2,5)] = ((0,3,37,196,526,782,591,190,),
                  (1,8,43,151,371,603,642,392,114,))
correct[(2,6)] = ((0,3,46,293,1039,2132,2579,1663,474,),
                  (1,10,60,255,744,1545,2178,2037,1108,291,))
correct[(2,7)] = ((0,3,52,408,1778,4779,7967,8108,4570,1126,),
                  (1,11,80,388,1355,3380,6057,7533,6234,3055,697,))
correct[(2,8)] = ((0,3,61,544,2847,9392,20303,28450,25083,12532,2794,),
                  (1,13,103,576,2298,6774,14620,23008,25477,18961,8422,1756,))

correct[(3,0)] = ((0,4,12,15,11,),
                  (1,2,5,9,12,8,5,))
correct[(3,1)] = ((0,4,24,60,65,28,),
                  (1,3,10,26,44,51,34,12,))
correct[(3,2)] = ((0,4,43,168,317,279,107,),
                  (1,5,21,65,150,232,246,149,49,))
correct[(3,3)] = ((0,4,59,337,935,1373,1017,316,),
                  (1,7,35,134,373,754,1058,988,544,147,))
correct[(3,4)] = ((0,4,78,580,2213,4682,5653,3596,990,),
                  (1,9,56,249,834,2046,3678,4610,3890,1948,475,))
correct[(3,5)] = ((0,4,94,884,4323,12318,21091,21490,11969,2869,),
                  (1,11,80,419,1631,4776,10412,16643,18819,14317,6534,1399,))

correct[(4,0)] = ((0,5,29,79,126,98,42,),
                  (1,3,7,21,43,75,89,81,42,17,))
correct[(4,1)] = ((0,5,56,270,657,886,608,184,),
                  (1,4,17,57,161,344,565,657,534,259,67,))
correct[(4,2)] = ((0,5,94,654,2354,4705,5408,3283,887,),
                  (1,7,34,143,471,1248,2515,3844,4190,3163,1436,338,))
correct[(4,3)] = ((0,5,126,1218,5939,16633,27707,27348,14725,3432,),
                  (1,9,58,284,1116,3458,8439,15777,22167,22434,15529,6535,1326,))

correct[(5,0)] = ((0,6,55,262,743,1252,1298,726,213,),
                  (1,3,11,34,100,239,492,784,1002,926,632,260,71,))
correct[(5,1)] = ((0,6,109,854,3574,8906,13516,12388,6289,1423,),
                  (1,5,25,105,371,1112,2748,5510,8690,10549,9397,5855,2256,441,))
correct[(5,2)] = ((0,6,175,1936,11223,38259,81297,108570,89321,41251,8517,),
                  (1,8,51,255,1072,3731,10848,25706,49123,73639,84689,71450,42005,15233,2744,))

correct[(6,0)] = ((0,7,97,699,3100,8662,15961,19043,14525,6338,1376,),
                  (1,4,14,56,185,571,1511,3500,6740,10831,13853,13997,10508,5712,1937,388,))

# Easy cases and hard cases

easy = [(0,3), (0,4), (0,5), (0,6), (0,7), (0,8),
        (0,9), (0,10), (0,11), (0,12), (0,13),
        (1,1), (1,2), (1,3), (1,4), (1,5),
        (1,6), (1,7), (1,8), (1,9),
        (2,0), (2,1), (2,2), (2,3), (2,4), (2,5), (2,6),
        (3,0), (3,1), (3,2), (3,3), (3,4),
        (4,0), (4,1), (4,2),
        (5,0)]
hard = [(0,14), (0,15),
        (1,10), (1,11),
        (2,7), (2,8),
        (3,5), (3,6),
        (4,3), (4,4),
        (5,1), (5,2),
        (6,0)]
        
# Utility functions

def compressFlags(f):
    f = f.split()[2:]
    r = []
    for x in f:
        x = x.split("_")
        y = []
        for a in x:
            y += a.split("=")
        s = "".join([a[0] for a in y])
        r.append(s)
    return " ".join(r)
    
def strSec(s):
    s, ds = s/1000, (s/100) % 10
    return "%5d.%01d" % (s, ds)

def strSecRel(s, tot):
    if tot < 100 or abs((100.0*s) / tot) < 5:
        if s > 0: return strSec(s)
        else: return strSec(-s)
    # s > 0 => bad => red
    elif s > 0: return "\033[31m" + strSec(s) + "\033[39m"
    # s < 0 => good => green
    else: return "\033[32m" + strSec(-s) + "\033[39m"

def strMem(s):
    s, ds = s/1024, (s/102) % 10
    return "%5d.%01d" % (s, ds)

def strMemRel(s, tot):
    if tot < 100 or abs((100.0*s) / tot) < 5:
        if s > 0: return strMem(s)
        else: return strMem(-s)
    if s > 0: return "\033[31m" + strMem(s) + "\033[39m"
    else: return "\033[32m" + strMem(-s) + "\033[39m"

# Testing

def addToStore(name, replace = False):
    firstIteration = True
    current = {}
    for gn in easy:
        process = Popen(name + " -S T %d %d > /dev/null" % gn,
                        shell=True, bufsize=1,
                        stdin=PIPE, stdout=PIPE, stderr=PIPE,
                        close_fds=True)
        e = process.stderr

        flags = e.readline()[:-1]
        if not replace and firstIteration and p.has_key(flags):
            print "Already in store (use replace)!"
            exit(1)
        firstIteration = False
        
        s = e.readline()
        s += e.readline()
        s += e.readline()
        s += e.readline()
        s += e.readline()
        s += e.readline()
        s = eval(s)
        if (s[2:4] != correct[gn]):
            print "Case %d %d not correct!" % gn
            break;
        current[gn] = {}
        current[gn]["flags"] = flags
        current[gn]["time"] = s[0]
        current[gn]["memory"] = s[1]
    p[flags] = current

    compare(flags, write = False)

def compare(flags, write = True):
    if not p.has_key(flags):
        print "Flags not in store!"
        exit(1)
    else:
        current = p[flags]
    replaceBest = []
    tot = {}
    totB = {}
    totO = {}
    mtot = {}
    mtotB = {}
    mtotO = {}
    overallTot = 0
    overallTotB = 0
    overallTotO = 0
    moverallTot = 0
    moverallTotB = 0
    moverallTotO = 0
    for gn in easy:
        curm = current[gn]["memory"]
        curt = current[gn]["time"]
        bestm = p["best"][gn]["memory"]
        bestt = p["best"][gn]["time"]
        overallm = p["overall"][gn]["memory"]
        overallt = p["overall"][gn]["time"]
        print "\033[1mCase %d %d\033[22m" % gn
        print " Current (%s)" % compressFlags(flags)
        if not notBest:
            print " Best (%s):" % compressFlags(p["best"][gn]["flags"])
        if not notOverall:
            print " Overall best (%s):" % compressFlags(p["overall"][gn]["flags"])

        mtot[gn] = 0
        mtotB[gn] = 0
        mtotO[gn] = 0
        print "\033[1mMemory (MB):\033[22m"
        for x in range(len(curm)):
            print "K=%2d:      " % x,
            print strMem(curm[x]), " ",
            mtot[gn] = max(mtot[gn], curm[x])
            if not notBest:
                print strMemRel(curm[x] - bestm[x], curm[x]), " ",
                mtotB[gn] = max(mtotB[gn], bestm[x])
            if not notOverall:
                print strMemRel(curm[x] - overallm[x], curm[x]), " ",
                mtotO[gn] = max(mtotO[gn], overallm[x])
            print
        print "\033[1mMax:        " + strMem(mtot[gn]), " ",
        if not notBest:
            print strMemRel(mtot[gn]-mtotB[gn], mtot[gn]), " ",
        if not notOverall:
            print strMemRel(mtot[gn]-mtotO[gn], mtot[gn]), " ", 
        print "\033[22m"
        moverallTot = max(moverallTot, mtot[gn])
        moverallTotB = max(moverallTotB, mtotB[gn])
        moverallTotO = max(moverallTotO, mtotO[gn])

        tot[gn] = 0
        totB[gn] = 0
        totO[gn] = 0
        print "\033[1mTime (s):\033[22m"
        for x in range(len(curt)):
            print "K=%2d:      " % x,
            print strSec(curt[x]), " ",
            tot[gn] += curt[x]
            if not notBest:
                print strSecRel(curt[x] - bestt[x], curt[x]), " ",
                totB[gn] += bestt[x]
            if not notOverall:
                print strSecRel(curt[x] - overallt[x], curt[x]), " ",
                totO[gn] += overallt[x]
            print
        print "\033[1mTot:        " + strSec(tot[gn]), " ",
        if not notBest:
            print strSecRel(tot[gn]-totB[gn], tot[gn]), " ",
        if not notOverall:
            print strSecRel(tot[gn]-totO[gn], tot[gn]), " ", 
        print "\033[22m"
        print
        overallTot += tot[gn]
        overallTotB += totB[gn]
        overallTotO += totO[gn]

        if notBest or tot < totB: # Replace best
            replaceBest.append(gn)

    print "\033[1mOverall\033[22m"
    print " Current (%s)" % compressFlags(flags)
    if not notOverall:
        print " Overall best (%s):" % compressFlags(p["overall"][easy[0]]["flags"])
    print "\033[1mMemory (MB):\033[22m"
    for gn in easy:
        print "g,n=%2d,%2d: " % gn,
        print strMem(mtot[gn]), " ",
        if not notBest:
            print strMemRel(mtot[gn] - mtotB[gn], mtot[gn]), " ",
        if not notOverall:
            print strMemRel(mtot[gn] - mtotO[gn], mtot[gn]), " ",
        print
    print "\033[1mMax:        " + strMem(moverallTot), " ",
    if not notBest:
        print strMemRel(moverallTot-moverallTotB, moverallTot), " ",
    if not notOverall:
        print strMemRel(moverallTot-moverallTotO, moverallTot), " ",
    print "\033[22m"

    print "\033[1mTime (s):\033[22m"
    for gn in easy:
        print "g,n=%2d,%2d: " % gn,
        print strSec(tot[gn]), " ",
        if not notBest:
            print strSecRel(tot[gn] - totB[gn], tot[gn]), " ",
        if not notOverall:
            print strSecRel(tot[gn] - totO[gn], tot[gn]), " ",
        print
    print "\033[1mTot:        " + strSec(overallTot), " ",
    if not notBest:
        print strSecRel(overallTot-overallTotB, overallTot), " ",
    if not notOverall:
        print strSecRel(overallTot-overallTotO, overallTot), " ",
    print "\033[22m"
    print

    for gn in replaceBest:
        p["best"][gn]["flags"] = current[gn]["flags"]
        p["best"][gn]["time"] = current[gn]["time"]
        p["best"][gn]["memory"] = current[gn]["memory"]

    if notOverall or overallTot < overallTotO: # Replace overall
        for g,n in easy:
            p["overall"][(g,n)]["flags"] = current[(g,n)]["flags"]
            p["overall"][(g,n)]["time"] = current[(g,n)]["time"]
            p["overall"][(g,n)]["memory"] = current[(g,n)]["memory"]

    p[current[easy[0]]["flags"]] = current

def plotSM():
    xAxis = [[], [], []]
    i = 0
    while True:
        if (i,(5-i)*2) not in easy: break
        xAxis[0].append((i, (5-i)*2))
        i += 1

    i = 2
    while True:
        if (i,0) not in easy: break
        xAxis[1].append((i,0))
        i += 1

    i = 3
    while True:
        if (0,i) not in easy: break
        xAxis[2].append((0,i))
        i += 1
        
    seriesS = [[], [], []]
    seriesM = [[], [], []]
    legend = []
    for x in p.keys():
        if x != "best" and x != "overall":
            for i in range(3):
                s = [sum(p[x][gn]["time"]) for gn in xAxis[i]]
                seriesS[i].append(s)
                s = [max(p[x][gn]["memory"]) for gn in xAxis[i]]
                seriesM[i].append(s)
            legend.append(compressFlags(x))

    for i in range(3):
        plot(xAxis[i], seriesS[i], "plot_%d_time.png" % i,
             title = "Time (log(s))", legend = legend)
        plot(xAxis[i], seriesM[i], "plot_%d_memory.png" % i,
             title = "Memory (log(MB))", legend = legend)
#        plot(xAxis[i], seriesS[i], "plot_%d_time.png" % i,
#            title = "Time (s)", logarithmic = False, legend = legend)
#        plot(xAxis[i], seriesM[i], "plot_%d_memory.png" % i,
#             title = "Memory (MB)", logarithmic = False, legend = legend)

def printStored():
    print "Stored:"
    for x in p.keys():
        if x != "best" and x != "overall":
            print x
    
# Format:

# p["best"] = best performance for the each (g,n)
#             may be from several programs
# p["overall"] = best global performance (on easy cases)

# p["best"][(g,n)]["flags"] = string containing the flags
# p["best"][(g,n)]["time"] = tuple of values of time used for different K
# p["best"][(g,n)]["memory"] = tuple of values of memory usage for different K

# Exactness on all easy tests is required for storing a performance.

notBest = False
notOverall = False
p = shelve.open("performances", writeback = True)
if not p.has_key("best"):
    notBest = True
    p["best"] = {}
    for g,n in easy:
        p["best"][(g,n)] = {}
if not p.has_key("overall"):
    notOverall = True
    p["overall"] = {}
    for g,n in easy:
        p["overall"][(g,n)] = {}

# MAIN METHOD!

if len(argv) == 1: exit(1)
elif len(argv) == 2:
    if argv[1] == "stored":
        printStored()
    elif argv[1] == "plot":
        plotSM()
    else:
        exit(1)
elif len(argv) == 3:
    if argv[1] == "add":
        addToStore(argv[2])
    elif argv[1] == "replace":
        addToStore(argv[2], replace = True)
    elif argv[1] == "compare":
        compare(argv[2])
    else:
        exit(1)
        
p.close()
