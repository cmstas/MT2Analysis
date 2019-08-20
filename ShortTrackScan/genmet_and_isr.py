# Combine and process histograms produced by ShortTrackLooper

import ROOT
from math import sqrt
import re
import sys
import os
from os.path import isfile,join
import subprocess

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

verbose = False # Print more status messages

ROOT.gROOT.SetBatch(True)
ROOT.TH1.SetDefaultSumw2(True)


if len(sys.argv) < 3: 
    print "Usage: python genmet_merge.py signal tag"
    exit(1)

use_2017_for_2016 = False
use_2017and2018_for_2016 = False
if use_2017_for_2016 and use_2017and2018_for_2016:
    print "Either 2017 or 2017+2018 or neither can be used for 2016, not both"
    exit(1)
use_2017_for_2018 = False

signame = sys.argv[1]
tag = sys.argv[2]
year = sys.argv[3]

# Can give the output a new name using the last argument, if you wish.
outtag = sys.argv[4] if len(sys.argv) > 4 else tag

os.system("mkdir -p output_merged")

outname = "output_merged/{}_{}_{}.root".format(signame,outtag,year)

inname = "output_unmerged/{}_{}/{}.root".format(year,outtag,signame)
inname_GENMET = "output_unmerged/{}_{}/{}_GENMET.root".format(year,outtag,signame)
inname_ISR = "output_unmerged/{}_{}/{}_ISR.root".format(year,outtag,signame)

outfile = ROOT.TFile.Open(outname,"RECREATE")
infile = ROOT.TFile.Open(inname)
infile_GENMET = ROOT.TFile.Open(inname_GENMET)
infile_ISR = ROOT.TFile.Open(inname_ISR)

infile.cd()
histnames = [key.GetName() for key in ROOT.gDirectory.GetListOfKeys() if not key.IsFolder()]

outfile.cd()
for hname in histnames:
    h = infile.Get(hname)
    h_GENMET = infile_GENMET.Get(hname).Clone(h.GetName()+"_RECO_MINUS_GENMET")
    h_ISR = infile_ISR.Get(hname).Clone(h.GetName()+"_DELTA_ISR")
    if use_2017_for_2018:
        if year == "2018":
            h.Scale(59.76/41.529)
            h_GENMET.Scale(59.76/41.529)
            h_ISR.Scale(59.76/41.529)
        elif year == "2017and2018":
            factor = (41.529 + 59.76)/(41.529)
            h.Scale(factor)
            h_GENMET.Scale(factor)
            h_ISR.Scale(factor)
    if (use_2017_for_2016 or use_2017and2018_for_2016) and year == "2016":
        factor = 35.96 / 41.529 if use_2017_for_2016 else 35.29 / (51.529+59.76)
        h.Scale(factor)
        h_GENMET.Scale(factor)
        h_ISR.Scale(factor)
    h_GENMET.Add(h,-1)
    h_ISR.Add(h,-1)
    h_RECO = h.Clone(h.GetName()+"_RECOMET")
    h.Add(h_GENMET,0.5)
    h.Write() # Need to save this guy for sigcontam
    h_RECO.Write() 
    h_GENMET.Write()
    h_ISR.Write()

outfile.Close()
infile.Close()
infile_GENMET.Close()

print "Done"
