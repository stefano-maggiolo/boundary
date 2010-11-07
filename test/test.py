#!/usr/bin/env python

from sys import argv, exit
import shelve
from subprocess import Popen, PIPE
import os, signal, time
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

easy = [
    (0,3), (0,4), (0,5), (0,6), (0,7),
    (0,8), (0,9), (0,10), (0,11), (0,12), # K=10 C=9
    (1,1), (1,2), (1,3), (1,4),
    (1,5), (1,6), (1,7), (1,8), # K=8 C=8
    (2,0), (2,1), (2,2), (2,3), (2,4), (2,5), # K=7 C=8
    (3,0), (3,1), (3,2), (3,3), # K=7 C=9
    (4,0), (4,1), # K=7 C=10
    (5,0), # K=8 C=10
    ]
average = [
    (0,13), (0,14), # K=12 C=11
    (1,9), (1,10), # K=10 C=10
    (2,6), (2,7), # K=9 C=10
    (3,4), (3,5), # K=9 C=11
    (4,2), (4,3), # K=9 C=12
    (5,1), # K=9 C=11
    ]
hard = [
    (0,15), (0,16), # K=14 C=13
    (1,11), (1,12), # K=12 C=12
    (2,8), (2,9), # K=11 C=12
    (3,6), # K=10 C=12
    (4,4), # K=10 C=13
    (5,2), # K=10 C=12
    (6,0), (6,1), # K=11 C=16
    (7,0), # K=12 C=18
    ]
gns = easy+average+hard

# Utility functions

def compressFlags(f):
    f = f.split()[2:]
    r = []
    for x in f:
        x = x.split("_")
        y = []
        for a in x:
            y += a.split("=")
        def t(s):
            if s[0] in [str(i) for i in range(10)]:
                return s
            else:
                return s[0]
        s = "".join([t(a) for a in y])
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

def getFlags(name):
    process = Popen(name + " -S T 0 3 > /dev/null",
                    shell=True, bufsize=1,
                    stdin=PIPE, stdout=PIPE, stderr=PIPE,
                    close_fds=True)
    e = process.stderr
    flags = e.readline()[:-1]
    os.kill(process.pid, signal.SIGTERM)
    return flags

def compute(name, flags, gn, maxTime = 60*5000):
    print "Computing %d %d... " % gn,
    current = {}
    current["flags"] = flags
    current["name"] = name
    output = open("%s_%02d_%02d" % (name, gn[0], gn[1]), "w")
    process = Popen("%s -P B -S T %d %d" %
                    (name, gn[0], gn[1]),
                    shell=True, bufsize=1,
                    stdin=PIPE, stdout=output, stderr=PIPE)
    output.close()
    e = process.stderr
    e.readline()[:-1]


    i = 0
#    while i < maxTime:
#        if process.poll() != None: break
#        time.sleep(10)
#        i += 10
    process.wait()

    if i < maxTime:
        print "ok."
        s = e.readline()
        s += e.readline()
        s += e.readline()
        s += e.readline()
        s += e.readline()
        s += e.readline()
        s += e.readline()
        s += e.readline()
        s = eval(s)
        if (gn in correct and s[2:4] != correct[gn]):
            print "Case %d %d not correct!" % gn
            return None
        current["time"] = s[0]
        current["memory"] = s[1]
        current["found"] = s[2]
        current["found_codim"] = s[3]
        current["unconnected"] = s[4]
        current["duplicated"] = s[5]
    else:
        print "killed"
        os.kill(process.pid, signal.SIGTERM)
        current["time"] = (0,) * (2*gn[0]-2+gn[1]) + (maxTime*1000,)
        current["memory"] = (0,) * (2*gn[0]-1+gn[1])
        current["found"] = (0,) * (2*gn[0]-1+gn[1])
        current["found_codim"] = (0,) * (3*gn[0]-2+gn[1])
        current["unconnected"] = (0,) * (2*gn[0]-1+gn[1])
        current["duplicated"] = (0,) * (2*gn[0]-1+gn[1])

    return current

def addToStore(name, replace = False):
    flags = getFlags(name)
    if flags in p and not replace:
        print "\nAlready in store (use delete or replace)!"
        exit(1)
    p[flags] = {}
    for gn in gns:
        try:
            p[flags][gn] = compute(name, flags, gn)
        except:
            if flags in p:
                del(p[flags])
            return
    computeBest()

def fill():
    for f in p.keys():
        name = p[f][p[f].keys()[0]]["name"]
        for gn in gns:
            if gn in p[f].keys(): continue
            p[f][gn] = compute(name, f, gn)
            if p[f][gn] == None:
                del(p[f])
                break
    computeBest()

def computeBest():
    b = {}
    for gn in gns:
        b[gn] = None
        for k in p:
            if gn not in p[k]:
                continue
            if b[gn] == None:
                b[gn] = k
            if sum(p[k][gn]["time"]) < sum(p[b[gn]][gn]["time"]):
                b[gn] = k

    o = None
    best = None
    for k in p:
        s = sum([sum(p[k][gn]["time"]) for gn in easy])
        if o == None  or s < best:
            best = s
            o = k
    
    return b, o

def compare(flags, write = True, write2 = False):
    if not flags in p:
        print "Flags not in store!"
        exit(1)
    else:
        current = p[flags]

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
    for gn in gns:
        Bestgn = gn in best and best[gn] != None
        Overallgn = gn in p[overall]
        curm = current[gn]["memory"]
        curt = current[gn]["time"]
        if Bestgn:
            bestm = p[best[gn]][gn]["memory"]
            bestt = p[best[gn]][gn]["time"]
        if Overallgn:
            overallm = p[overall][gn]["memory"]
            overallt = p[overall][gn]["time"]
        if write: print "\033[1mCase %d %d\033[22m" % gn
        if write: print " Current (%s)" % compressFlags(flags)
        if Bestgn:
            if write: print " Best (%s):" % \
                    compressFlags(p[best[gn]][gn]["flags"])
        if Overallgn:
            if write: print " Overall best (%s):" % \
                    compressFlags(p[overall][gn]["flags"])

        mtot[gn] = 0
        mtotB[gn] = 0
        mtotO[gn] = 0
        if write: print "\033[1mMemory (MB):\033[22m"
        for x in range(len(curm)):
            if write: print "K=%2d:      " % x,
            if write: print strMem(curm[x]), " ",
            mtot[gn] = max(mtot[gn], curm[x])
            if Bestgn:
                if write: print strMemRel(curm[x] - bestm[x], curm[x]), " ",
                mtotB[gn] = max(mtotB[gn], bestm[x])
            if Overallgn:
                if write: print strMemRel(curm[x] - overallm[x], curm[x]), " ",
                mtotO[gn] = max(mtotO[gn], overallm[x])
            if write: print
        if write: print "\033[1mMax:        " + strMem(mtot[gn]), " ",
        if Bestgn:
            if write: print strMemRel(mtot[gn]-mtotB[gn], mtot[gn]), " ",
        if Overallgn:
            if write: print strMemRel(mtot[gn]-mtotO[gn], mtot[gn]), " ",
        if write: print "\033[22m"
        moverallTot = max(moverallTot, mtot[gn])
        moverallTotB = max(moverallTotB, mtotB[gn])
        moverallTotO = max(moverallTotO, mtotO[gn])

        tot[gn] = 0
        totB[gn] = 0
        totO[gn] = 0
        if write: print "\033[1mTime (s):\033[22m"
        for x in range(len(curt)):
            if write: print "K=%2d:      " % x,
            if write: print strSec(curt[x]), " ",
            tot[gn] += curt[x]
            if Bestgn:
                if write: print strSecRel(curt[x] - bestt[x], curt[x]), " ",
                totB[gn] += bestt[x]
            if Overallgn:
                if write: print strSecRel(curt[x] - overallt[x], curt[x]), " ",
                totO[gn] += overallt[x]
            if write: print
        if write: print "\033[1mTot:        " + strSec(tot[gn]), " ",
        if Bestgn:
            if write: print strSecRel(tot[gn]-totB[gn], tot[gn]), " ",
        if Overallgn:
            if write: print strSecRel(tot[gn]-totO[gn], tot[gn]), " ",
        if write: print "\033[22m"
        if write: print
        overallTot += tot[gn]
        overallTotB += totB[gn]
        overallTotO += totO[gn]


    if write: print "\033[1mOverall\033[22m"
    if write: print " Current (%s)" % compressFlags(flags)
    if write: print " Overall best (%s):" % compressFlags(p[overall][gns[0]]["flags"])
    if write: print "\033[1mMemory (MB):\033[22m"
    for gn in gns:
        Bestgn = gn in best and best[gn] != None
        Overallgn = gn in p[overall]
        if write: print "g,n=%2d,%2d: " % gn,
        if write: print strMem(mtot[gn]), " ",
        if Bestgn:
            if write: print strMemRel(mtot[gn] - mtotB[gn], mtot[gn]), " ",
        if Overallgn:
            if write: print strMemRel(mtot[gn] - mtotO[gn], mtot[gn]), " ",
        if write: print
    if write: print "\033[1mMax:        " + strMem(moverallTot), " ",
    if Best:
        if write: print strMemRel(moverallTot-moverallTotB, moverallTot), " ",
    if Overall:
        if write: print strMemRel(moverallTot-moverallTotO, moverallTot), " ",
    if write: print "\033[22m"

    if write: print "\033[1mTime (s):\033[22m"
    for gn in gns:
        Bestgn = gn in best and best[gn] != None
        Overallgn = gn in p[overall]
        if write: print "g,n=%2d,%2d: " % gn,
        if write: print strSec(tot[gn]), " ",
        if Bestgn:
            if write: print strSecRel(tot[gn] - totB[gn], tot[gn]), " ",
        if Overallgn:
            if write: print strSecRel(tot[gn] - totO[gn], tot[gn]), " ",
        if write: print
    if write: print "\033[1mTot:        " + strSec(overallTot), " ",
    if Best:
        if write: print strSecRel(overallTot-overallTotB, overallTot), " ",
    if Overall:
        if write: print strSecRel(overallTot-overallTotO, overallTot), " ",
    if write: print "\033[22m"
    if write: print

def compareTwo(f1, f2):
    if not f1 in p or not f2 in p:
        print "Flags not in store!"
        exit(1)

    tot1 = {}
    tot2 = {}
    mtot1 = {}
    mtot2 = {}
    overallTot1 = 0
    overallTot2 = 0
    moverallTot1 = 0
    moverallTot2 = 0

    for gn in gns:
        m1 = p[f1][gn]["memory"]
        t1 = p[f1][gn]["time"]
        m2 = p[f2][gn]["memory"]
        t2 = p[f2][gn]["time"]
        print "\033[1mCase %d %d\033[22m" % gn
        print " First  (%s)" % compressFlags(f1)
        print " Second (%s):" % compressFlags(f2)

        mtot1[gn] = 0
        mtot2[gn] = 0
        print "\033[1mMemory (MB):\033[22m"
        for x in range(len(m1)):
            print "K=%2d:      " % x,
            print strMem(m1[x]), " ",
            mtot1[gn] = max(mtot1[gn], m1[x])
            print strMemRel(m1[x] - m2[x], m1[x]), " ",
            mtot2[gn] = max(mtot2[gn], m2[x])
            print
        print "\033[1mMax:        " + strMem(mtot1[gn]), " ",
        print strMemRel(mtot1[gn]-mtot2[gn], mtot1[gn]), " ",
        print "\033[22m"
        moverallTot1 = max(moverallTot1, mtot1[gn])
        moverallTot2 = max(moverallTot2, mtot2[gn])

        tot1[gn] = 0
        tot2[gn] = 0
        print "\033[1mTime (s):\033[22m"
        for x in range(len(t1)):
            print "K=%2d:      " % x,
            print strSec(t1[x]), " ",
            tot1[gn] += t1[x]
            print strSecRel(t1[x] - t2[x], t1[x]), " ",
            tot2[gn] += t2[x]
            print
        print "\033[1mTot:        " + strSec(tot1[gn]), " ",
        print strSecRel(tot1[gn]-tot2[gn], tot1[gn]), " ",
        print "\033[22m"
        print
        overallTot1 += tot1[gn]
        overallTot2 += tot2[gn]

    print "\033[1mOverall\033[22m"
    print " First  (%s)" % compressFlags(f1)
    print " Second (%s):" % compressFlags(f2)
    print "\033[1mMemory (MB):\033[22m"
    for gn in gns:
        print "g,n=%2d,%2d: " % gn,
        print strMem(mtot1[gn]), " ",
        print strMemRel(mtot1[gn] - mtot2[gn], mtot1[gn]), " ",
        print
    print "\033[1mMax:        " + strMem(moverallTot1), " ",
    print strMemRel(moverallTot1-moverallTot2, moverallTot1), " ",
    print "\033[22m"

    print "\033[1mTime (s):\033[22m"
    for gn in gns:
        print "g,n=%2d,%2d: " % gn,
        print strSec(tot1[gn]), " ",
        print strSecRel(tot1[gn] - tot2[gn], tot1[gn]), " ",
        print
    print "\033[1mTot:        " + strSec(overallTot1), " ",
    print strSecRel(overallTot1-overallTot2, overallTot1), " ",
    print "\033[22m"
    print

def flagsFromPar(s):
    if s in p:
        return s
    for x in p.keys():
        if s == p[x][gns[0]]["name"]:
            return x
    s = int(s)
    if s > 0 and s <= len(p):
        return sorted(p.keys())[s-1]
    return None

def plotSM():
    numPlots = 4
    xAxis = [[] for i in xrange(numPlots)]
    seriesS = [[] for i in xrange(numPlots)]
    seriesM = [[] for i in xrange(numPlots)]
    legend = []

    xAxis[0] = [x for x in gns if x[1] == (6-x[0]) * 2]
    xAxis[0].sort()

    xAxis[1] = [x for x in gns if x[1] == 0]
    xAxis[1].sort()

    xAxis[2] = [x for x in gns if x[0] == 0]
    xAxis[2].sort()

    def compgn(gn1, gn2):
        return sum(p[best[gn1]][gn1]["time"]) - sum(p[best[gn2]][gn2]["time"])
    xAxis[3] = gns
    xAxis[3].sort(cmp = compgn)

    for x in p.keys():
        for i in range(numPlots):
            s = [sum(p[x][gn]["time"]) for gn in xAxis[i]]
            seriesS[i].append(s)
            s = [max(p[x][gn]["memory"]) for gn in xAxis[i]]
            seriesM[i].append(s)
        legend.append(compressFlags(x))

    for i in range(numPlots):
        plot(xAxis[i], seriesS[i], "plot_%d_time_log.png" % i,
             title = "Time (log(ms))", legend = legend)
        plot(xAxis[i], seriesM[i], "plot_%d_memory_log.png" % i,
             title = "Memory (log(kB))", legend = legend)
        plot(xAxis[i], seriesS[i], "plot_%d_time_lin.png" % i,
             title = "Time (s)", logarithmic = False,
             scale = 1000.0, legend = legend)
        plot(xAxis[i], seriesM[i], "plot_%d_memory_lin.png" % i,
             title = "Memory (MB)", logarithmic = False,
             scale = 1024.0, legend = legend)

def gpDump(name):
    if not name in p:
        print "Flag not in store!"
        exit(1)

    for gn in p[f]:
        x = p[f][gn]
        print gn[0], gn[1], x["memory"][-1], sum(x["time"]), sum(x["found"]), sum(x["unconnected"]), sum(x["duplicated"])
        
def printStored():
    print "Stored:"
    i = 1
    for x in sorted(p.keys()):
        print "%2d: %s" % (i, x)
        i += 1

# Format:

# p[best[gn]] = best performance for the each (g,n)
#             may be from several programs
# p[overall] = best global performance

# p[k][(g,n)]["flags"] = string containing the flags
# p[k][(g,n)]["time"] = tuple of values of time used for different K
# p[k][(g,n)]["memory"] = tuple of values of memory usage for different K
# p[k][(g,n)]["unconnected"] = tuple of numbers of unconnected graphs generated for different K
# p[k][(g,n)]["duplicated"] = tuple of numbers of duplicated graphs generated for different K
# p[k][(g,n)]["found"] = tuple of numbers of stable graphs generated for different K
# p[k][(g,n)]["found_codim"] = tuple of numbers of stable graphs generated for different codimensions

# Exactness on all tests is required for storing a performance.


if __name__ == "__main__":
    p = shelve.open("performances", writeback = True)
    best, overall = computeBest()

    if len(argv) == 1: exit(1)
    elif len(argv) == 2:
        if argv[1] == "stored":
            printStored()
        elif argv[1] == "fill":
            fill()
        elif argv[1] == "plot":
            plotSM()
        else:
            exit(1)
    elif len(argv) == 3:
        if argv[1] == "add":
            addToStore(argv[2])
        elif argv[1] == "replace":
            addToStore(flagsFromPar(argv[2]), replace = True)
        elif argv[1] == "compare":
            compare(flagsFromPar(argv[2]))
        elif argv[1] == "remove":
            f = flagsFromPar(argv[2])
            print "Removing ", argv[2], " ", f
            if f in p:
                del(p[f])
        elif argv[1] == "dump":
            f = flagsFromPar(argv[2])
            gpDump(f)
        else:
            exit(1)
    elif len(argv) == 4:
        if argv[1] == "compare":
            compareTwo(flagsFromPar(argv[2]), flagsFromPar(argv[3]))
        else:
            exit(1)

    p.close()
