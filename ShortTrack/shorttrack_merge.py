# STCImplementation background estimates and plots

import ROOT
from math import sqrt
import re
import sys
import os
import subprocess

sys.path.append("../Nifty")

import NiftyUtils

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

verbose = False # Print more status messages

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
ROOT.gStyle.SetLegendTextSize(0.1)
ROOT.gStyle.SetPaintTextFormat("#3.3g")
simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(600,600)
simplecanvas.SetTicks(1,2)
simplecanvas.SetLeftMargin(0.2)
simplecanvas.SetRightMargin(0.2)
simplecanvas.SetTopMargin(0.12)

if len(sys.argv) < 2: 
    print "Which file to run on?"
    exit(1)

filename = sys.argv[1]
shortname = filename[filename.rfind("/")+1:filename.find(".")]

os.system("mkdir -p pngs/{0}".format(shortname))

tfile = ROOT.TFile.Open(filename,"READ")
names = tfile.GetKeyNames()

counts_names = []
for rawname in names:
    if rawname.find("SR") < 0 and rawname.find("VR") < 0: continue    
    name = rawname[1:] # Get rid of leading /
    counts_names.append(name)

simplecanvas.SetLeftMargin(0.22)
simplecanvas.SetRightMargin(0.22)

for name in counts_names:
    h_counts = tfile.Get(name)
    h_counts.SetMarkerSize(1.5)
    h_counts.Draw("text E")
    simplecanvas.SaveAs("pngs/{0}/{1}.png".format(shortname,name))

tfile.Close()

print "Done"
