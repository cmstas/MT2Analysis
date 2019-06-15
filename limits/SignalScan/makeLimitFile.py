#!/usr/bin/env python
from math import *
import os, commands
from sys import argv,exit
#from optparse import OptionParser
import ROOT

print "running:", argv

if len(argv)<4:
    print "Usage: "+argv[0]+" [model] [date] [pointsFile]"
    exit(1)

MODEL = argv[1]
DATE = argv[2]
POINTSFILE = argv[3]
user = os.environ['USER']
theDir = "/hadoop/cms/store/user/"+user+"/combine/limits/"+MODEL+"_"+DATE+"/"

if not os.path.isdir(theDir):
	print theDir+" does not exist! Exiting..."
	exit(1)

if not os.path.isfile(POINTSFILE):
	print POINTSFILE+" does not exist! Exiting..."
	exit(1)
	
f = open("limits_"+MODEL+"_"+DATE+".txt","w+")

print "Looping over points..."

for line in open(POINTSFILE,"r"):
	
	line = line.rstrip("\r\n")
	print line
	m1 = line.split("_")[-2]
	m2 = line.split("_")[-1]

	if not os.path.isfile(theDir+"limit_"+line+".root"):
            print "No limit file exists for "+line+"! Skipping..."
            continue
	
	f_limit = ROOT.TFile.Open(theDir+"limit_"+line+".root")
	tree = f_limit.Get("limit")
	if not tree:
            print "No limit tree exists in file file for "+line+"! Skipping..."
            continue
	# for i in tree:
	# 	print i.limit
	tree.GetEntry(0)
	em2s = tree.limit
	tree.GetEntry(1)
	em1s = tree.limit
	tree.GetEntry(2)
	exp = tree.limit
	tree.GetEntry(3)
	ep1s = tree.limit
	tree.GetEntry(4)
	ep2s = tree.limit
	tree.GetEntry(5)
	obs = tree.limit

        f_limit.Close()
	
	f_sig = ROOT.TFile.Open(theDir+"sig_"+line+".root")
        if f_sig:
            tree = f_sig.Get("limit")
	if not f_sig or not tree:
            print "No tree in sig file for {0}. Setting to 0".format(line)
            sig = 0.0
        else:
            tree.GetEntry(0)
            sig = tree.limit
        if f_sig:
            f_sig.Close()
	

	f.write(m1+" "+m2+" "+str(exp)+" "+str(obs)+" "+str(ep1s)+" "+str(em1s)+" "+str(ep2s)+" "+str(em2s)+" "+str(sig)+"\n")

f.close()

print "Done."

