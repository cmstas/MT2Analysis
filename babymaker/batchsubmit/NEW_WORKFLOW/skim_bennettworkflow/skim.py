#! /usr/bin/env python

import sys, os
import glob
import ROOT as r

if len(sys.argv) < 4:
    print "usage : python skim.py <selection file> <infile> <outfile>"
    print "        Selection file is a text file with the skim selection."
    print "        If there are multiple lines they will be appended together as strings."
    print "        Everything after a '//' will be ignored"
    exit(0)

selfile = sys.argv[1]
infile = sys.argv[2]
outname = sys.argv[3]

with open(selfile) as f:
    sel = ""
    for line in f:
        line = line.split("//")[0].strip()
        if line.startswith("#"):
            samptype = line.split("#")[1]
            if samptype in samp:
                sel += line.split("#")[-1].strip()
        else:
            sel += line

print "Skimming with selection:", sel
print ""

ch = r.TChain("mt2")
ch.Add(infile)

print "\nWriting to:", outname

r.TTree.SetMaxTreeSize(2500000000)

nevt = ch.GetEntries()
print "\nInput has {0} entries".format(nevt)

fout = r.TFile(outname, "RECREATE")
outtree = ch.CopyTree(sel)
nevt_out = outtree.GetEntries()
print "Output has {0} entries".format(nevt_out)
print "Reduction factor of", 1.0*nevt/nevt_out
outtree.Write()
fout.Close()

