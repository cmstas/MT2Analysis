# Print cards for use with combine. See MT2Analysis/limits/SignalScan.
# Reimplementation of cardMaker.C.

import ROOT
import re
from math import sqrt
from sys import argv,exit
import os
import NiftyHists as nh

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

verbose=True

f = ROOT.TFile.Open("background{}.root".format(argv[1]))

iterator = f.GetListOfKeys()
keep = "sr"
skip = "srbase"
# Loop through every directory in the signal file
for key in iterator:
    directory = key.GetTitle()
    if verbose: print directory
    # If the directory name contains "srbase", skip it
    if directory.find(skip) >= 0: continue
    # If the directory name contains "sr" but not "srbase", process it
    if directory.find(keep) >= 0:
        sr_number = int(re.findall("\d+",directory)[0]) # Search for the first integer of any length in the directory string
        # We're either doing super signal regions or we're not; handle appropriately
        if (sr_number >= 50): continue
        th1 = f.Get(directory+"/h_mt2bins")
        nh.SimplePlot(th1,"regions{}/".format(argv[1])+directory+".png",True)
