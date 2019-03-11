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

if len(sys.argv) < 3: 
    print "Usage: python genmet_merge.py signal tag"
    exit(1)

signame = sys.argv[1]
tag = sys.argv[2]

# Can give the output a new name using the third argument, if you wish.
outtag = sys.argv[3] if len(sys.argv) > 3 else tag

os.system("mkdir -p output_merged")

outname = "output_merged/{}_{}.root".format(signame,outtag)

inname = "output_unmerged/2017_{}/signal/{}.root".format(outtag,signame)
inname_GENMET = "output_unmerged/2017_{}/signal/{}_GENMET.root".format(outtag,signame)

outfile = ROOT.TFile.Open(outname,"RECREATE")
infile = ROOT.TFile.Open(inname)
infile_GENMET = ROOT.TFile.Open(inname_GENMET)

infile.cd()
histnames = [key.GetName() for key in ROOT.gDirectory.GetListOfKeys() if not key.IsFolder()]

outfile.cd()
for hname in histnames:
    h = infile.Get(hname)
    h_GENMET = infile_GENMET.Get(hname)
    h_err = h.Clone(h.GetName()+"_RECO_MINUS_GENMET")
    h.Add(h_GENMET)
    h.Scale(0.5)
    h_err.Add(h_GENMET,-1)
    h.Write()
    h_err.Write()

outfile.Close()
infile.Close()
infile_GENMET.Close()

print "Done"
