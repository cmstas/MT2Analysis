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

if filename.find("16") >= 0:
#    RmtptName = "../FshortLooper/output/Fshort_mc2016_fullveto.root"
    RmtptName = "../FshortLooper/output/Fshort_mc2017_fullveto.root" # for now, use 2017 MC in 2016
elif filename.find("17") >= 0:
    RmtptName = "../FshortLooper/output/Fshort_mc2017_fullveto.root";
else:
    print "Unsure which Rmtpt file to use, exiting"
    exit(1)

RmtptFile = ROOT.TFile.Open(RmtptName,"READ")

h_RmtptFSR = RmtptFile.Get("h_RmtptFSR")
h_RmtptFSR_Nj23 = RmtptFile.Get("h_RmtptFSR")
h_RmtptFSR_Nj4 = RmtptFile.Get("h_RmtptFSR")
h_RmtptVR = RmtptFile.Get("h_RmtptVR")
h_RmtptVR_Nj23 = RmtptFile.Get("h_RmtptVR")
h_RmtptVR_Nj4 = RmtptFile.Get("h_RmtptVR")
h_RmtptSR = RmtptFile.Get("h_RmtptSR")
h_RmtptSR_Nj23 = RmtptFile.Get("h_RmtptSR")
h_RmtptSR_Nj4 = RmtptFile.Get("h_RmtptSR")

Rmtpt = { "h_LL_FSR" : h_RmtptFSR.GetBinContent(1,1),
          "h_LH_FSR" : h_RmtptFSR.GetBinContent(1,1),
          "h_HL_FSR" : h_RmtptFSR.GetBinContent(1,1),
          "h_HH_FSR" : h_RmtptFSR.GetBinContent(1,1),
          "h_LL_FSR_23" : h_RmtptFSR_Nj23.GetBinContent(1,1),
          "h_LH_FSR_23" : h_RmtptFSR_Nj23.GetBinContent(1,1),
          "h_HL_FSR_4" : h_RmtptFSR_Nj4.GetBinContent(1,1),
          "h_HH_FSR_4" : h_RmtptFSR_Nj4.GetBinContent(1,1),

          "h_LL_VR" : h_RmtptVR.GetBinContent(1,1),
          "h_LH_VR" : h_RmtptVR.GetBinContent(1,1),
          "h_HL_VR" : h_RmtptVR.GetBinContent(1,1),
          "h_HH_VR" : h_RmtptVR.GetBinContent(1,1),
          "h_LL_VR_23" : h_RmtptVR_Nj23.GetBinContent(1,1),
          "h_LH_VR_23" : h_RmtptVR_Nj23.GetBinContent(1,1),
          "h_HL_VR_4" : h_RmtptVR_Nj4.GetBinContent(1,1),
          "h_HH_VR_4" : h_RmtptVR_Nj4.GetBinContent(1,1),

          "h_LL_SR" : h_RmtptSR.GetBinContent(1,1),
          "h_LH_SR" : h_RmtptSR.GetBinContent(1,1),
          "h_HL_SR" : h_RmtptSR.GetBinContent(1,1),
          "h_HH_SR" : h_RmtptSR.GetBinContent(1,1),
          "h_LL_SR_23" : h_RmtptSR_Nj23.GetBinContent(1,1),
          "h_LH_SR_23" : h_RmtptSR_Nj23.GetBinContent(1,1),
          "h_HL_SR_4" : h_RmtptSR_Nj4.GetBinContent(1,1),
          "h_HH_SR_4" : h_RmtptSR_Nj4.GetBinContent(1,1)
          }

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
    obsRejST = h_counts.GetBinContent(3,1)
    expRejST = h_counts.GetBinContent(3,2)
    ewkRejST = max(obsRejST - expRejST, 0.0)
    extraAccST = ewkRejST * Rmtpt[name]
    corrected_expected_accepted_STs = h_counts.GetBinContent(4,2) + extraAccST
    h_counts.SetBinContent(5,1,h_counts.GetBinContent(4,1))
    h_counts.SetBinError(5,1,h_counts.GetBinError(4,1))
    h_counts.SetBinContent(5,2,corrected_expected_accepted_STs)
    h_counts.SetBinContent(5,3,ewkRejST)
    h_counts.Draw("text E")
    simplecanvas.SaveAs("pngs/{0}/{1}.png".format(shortname,name))

tfile.Close()

print "Done"
