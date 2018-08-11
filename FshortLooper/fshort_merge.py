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
simplecanvas.SetLeftMargin(0.16)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetRightMargin(0.16)

if len(sys.argv) < 2: 
    print "Which file?"
    exit()

filename = sys.argv[1]
shortname = filename[filename.rfind("/")+1:filename.find(".")]

os.system("mkdir -p output")
os.system("mkdir -p pngs/{0}".format(shortname))

tfile = ROOT.TFile.Open(filename,"READ")
names = tfile.GetKeyNames()

outfile = ROOT.TFile.Open("output/Fshort_{}.root".format(shortname),"RECREATE")
outfile.cd()

for rawname in names:
    if rawname.find("FSR") < 0: continue
    name = rawname[1:] # Get rid of leading /
    h_fsr = tfile.Get(name)
    for length in range(1,5):
        den = h_fsr.GetBinContent(length,3)
        if den == 0:
            h_fsr.SetBinContent(length,1,-1)
            h_fsr.SetBinError(length,1,0)
            continue
        num = h_fsr.GetBinContent(length,2)
        h_fsr.SetBinContent(length,1,num / den)
        derr = h_fsr.GetBinError(length, 3)
        nerr = h_fsr.GetBinError(length, 2)
        numsq = num * num
        densq = den * den
        # binomial errors
        newerr = sqrt((derr*derr * numsq + nerr*nerr * densq) / (densq * densq))
        h_fsr.SetBinError(length, 1, newerr)
    h_fsr.SetMarkerSize(1.8)
    h_fsr.Draw("text E")
    simplecanvas.SaveAs("pngs/{0}/{1}.png".format(shortname,name))
    h_fsr.Write()

outfile.Close()

tfile.Close()

print "Done"
