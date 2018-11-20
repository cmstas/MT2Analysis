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
doRmtpt = False

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

if doRmtpt:
    if filename.find("16") >= 0:
        #    RmtptName = "../FshortLooper/output/Fshort_mc_2016_fullveto.root"
        RmtptName = "../FshortLooper/output/Fshort_mc_2017_fullveto.root" # for now, use 2017 MC in 2016
    elif filename.find("17") >= 0:
        RmtptName = "../FshortLooper/output/Fshort_mc_2017_fullveto.root";
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

tfile = ROOT.TFile.Open(filename,"UPDATE")
names = tfile.GetKeyNames()

counts_names = []
for rawname in names:
    if rawname.find("SR") < 0 and rawname.find("VR") < 0: continue    
    if rawname.find("final") >= 0: continue
    name = rawname[1:] # Get rid of leading /
    counts_names.append(name)

simplecanvas.SetLeftMargin(0.22)
simplecanvas.SetRightMargin(0.22)

fsName = "../FshortLooper/output/Fshort_{}.root".format(shortname if shortname.find("_M") < 0 else shortname[:shortname.find("_M")])

fsFile = ROOT.TFile.Open(fsName,"READ")

if fsFile == None:
    print "Can't find", fsName

h_fsFSR = fsFile.Get("h_fsFSR")
h_fsFSR_Nj23 = fsFile.Get("h_fsFSR_23")
h_fsFSR_Nj4 = fsFile.Get("h_fsFSR_4")

fserr = { "h_LL_VR" : (h_fsFSR.GetBinError(1,1),h_fsFSR.GetBinError(2,1),h_fsFSR.GetBinError(3,1)),
          "h_LH_VR" : (h_fsFSR.GetBinError(1,1),h_fsFSR.GetBinError(2,1),h_fsFSR.GetBinError(3,1)),
          "h_HL_VR" : (h_fsFSR.GetBinError(1,1),h_fsFSR.GetBinError(2,1),h_fsFSR.GetBinError(3,1)),
          "h_HH_VR" : (h_fsFSR.GetBinError(1,1),h_fsFSR.GetBinError(2,1),h_fsFSR.GetBinError(3,1)),
          "h_LL_VR_23" : (h_fsFSR_Nj23.GetBinError(1,1),h_fsFSR_Nj23.GetBinError(2,1),h_fsFSR_Nj23.GetBinError(3,1)),
          "h_LH_VR_23" : (h_fsFSR_Nj23.GetBinError(1,1),h_fsFSR_Nj23.GetBinError(2,1),h_fsFSR_Nj23.GetBinError(3,1)),
          "h_HL_VR_4" : (h_fsFSR_Nj4.GetBinError(1,1),h_fsFSR_Nj4.GetBinError(2,1),h_fsFSR_Nj4.GetBinError(3,1)),
          "h_HH_VR_4" : (h_fsFSR_Nj4.GetBinError(1,1),h_fsFSR_Nj4.GetBinError(2,1),h_fsFSR_Nj4.GetBinError(3,1)),

          "h_LL_SR" : (h_fsFSR.GetBinError(1,1),h_fsFSR.GetBinError(2,1),h_fsFSR.GetBinError(3,1)),
          "h_LH_SR" : (h_fsFSR.GetBinError(1,1),h_fsFSR.GetBinError(2,1),h_fsFSR.GetBinError(3,1)),
          "h_HL_SR" : (h_fsFSR.GetBinError(1,1),h_fsFSR.GetBinError(2,1),h_fsFSR.GetBinError(3,1)),
          "h_HH_SR" : (h_fsFSR.GetBinError(1,1),h_fsFSR.GetBinError(2,1),h_fsFSR.GetBinError(3,1)),
          "h_LL_SR_23" : (h_fsFSR_Nj23.GetBinError(1,1),h_fsFSR_Nj23.GetBinError(2,1),h_fsFSR_Nj23.GetBinError(3,1)),
          "h_LH_SR_23" : (h_fsFSR_Nj23.GetBinError(1,1),h_fsFSR_Nj23.GetBinError(2,1),h_fsFSR_Nj23.GetBinError(3,1)),
          "h_HL_SR_4" : (h_fsFSR_Nj4.GetBinError(1,1),h_fsFSR_Nj4.GetBinError(2,1),h_fsFSR_Nj4.GetBinError(3,1)),
          "h_HH_SR_4" : (h_fsFSR_Nj4.GetBinError(1,1),h_fsFSR_Nj4.GetBinError(2,1),h_fsFSR_Nj4.GetBinError(3,1))

          }

fs = { "h_LL_VR" : (h_fsFSR.GetBinContent(1,1),h_fsFSR.GetBinContent(2,1),h_fsFSR.GetBinContent(3,1)),
          "h_LH_VR" : (h_fsFSR.GetBinContent(1,1),h_fsFSR.GetBinContent(2,1),h_fsFSR.GetBinContent(3,1)),
          "h_HL_VR" : (h_fsFSR.GetBinContent(1,1),h_fsFSR.GetBinContent(2,1),h_fsFSR.GetBinContent(3,1)),
          "h_HH_VR" : (h_fsFSR.GetBinContent(1,1),h_fsFSR.GetBinContent(2,1),h_fsFSR.GetBinContent(3,1)),
          "h_LL_VR_23" : (h_fsFSR_Nj23.GetBinContent(1,1),h_fsFSR_Nj23.GetBinContent(2,1),h_fsFSR_Nj23.GetBinContent(3,1)),
          "h_LH_VR_23" : (h_fsFSR_Nj23.GetBinContent(1,1),h_fsFSR_Nj23.GetBinContent(2,1),h_fsFSR_Nj23.GetBinContent(3,1)),
          "h_HL_VR_4" : (h_fsFSR_Nj4.GetBinContent(1,1),h_fsFSR_Nj4.GetBinContent(2,1),h_fsFSR_Nj4.GetBinContent(3,1)),
          "h_HH_VR_4" : (h_fsFSR_Nj4.GetBinContent(1,1),h_fsFSR_Nj4.GetBinContent(2,1),h_fsFSR_Nj4.GetBinContent(3,1)),

          "h_LL_SR" : (h_fsFSR.GetBinContent(1,1),h_fsFSR.GetBinContent(2,1),h_fsFSR.GetBinContent(3,1)),
          "h_LH_SR" : (h_fsFSR.GetBinContent(1,1),h_fsFSR.GetBinContent(2,1),h_fsFSR.GetBinContent(3,1)),
          "h_HL_SR" : (h_fsFSR.GetBinContent(1,1),h_fsFSR.GetBinContent(2,1),h_fsFSR.GetBinContent(3,1)),
          "h_HH_SR" : (h_fsFSR.GetBinContent(1,1),h_fsFSR.GetBinContent(2,1),h_fsFSR.GetBinContent(3,1)),
          "h_LL_SR_23" : (h_fsFSR_Nj23.GetBinContent(1,1),h_fsFSR_Nj23.GetBinContent(2,1),h_fsFSR_Nj23.GetBinContent(3,1)),
          "h_LH_SR_23" : (h_fsFSR_Nj23.GetBinContent(1,1),h_fsFSR_Nj23.GetBinContent(2,1),h_fsFSR_Nj23.GetBinContent(3,1)),
          "h_HL_SR_4" : (h_fsFSR_Nj4.GetBinContent(1,1),h_fsFSR_Nj4.GetBinContent(2,1),h_fsFSR_Nj4.GetBinContent(3,1)),
          "h_HH_SR_4" : (h_fsFSR_Nj4.GetBinContent(1,1),h_fsFSR_Nj4.GetBinContent(2,1),h_fsFSR_Nj4.GetBinContent(3,1))

          }

fsFile.Close()

tfile.cd()
for name in counts_names:
    print name
    h_counts = tfile.Get(name).Clone(name+"_final")
    h_counts.SetTitle("Event Counts by Length")
    h_counts.GetXaxis().SetBinLabel(3,"L (acc)")
    h_counts.GetXaxis().SetRange(1,3)
    h_counts.SetMarkerSize(1.5)
    for i in range(1,4):
        count = h_counts.GetBinError(i,2)
        if count > 0:
            # propagate fshort error: total absolute error = bin content * sqrt( bin statistical relative error ^2 + fshort relative error ^2 )
            h_counts.SetBinError(i,2,count*sqrt((h_counts.GetBinError(i,1)/count)**2 + ( (fserr[name][i-1])/(fs[name][i-1]) )**2))
    if doRmtpt:
        obsRejST = h_counts.GetBinContent(3,1)
        expRejST = h_counts.GetBinContent(3,2)
        ewkRejST = max(obsRejST - expRejST, 0.0)
        extraAccST = ewkRejST * Rmtpt[name]
        corrected_expected_accepted_STs = h_counts.GetBinContent(4,2) + extraAccST
        h_counts.SetBinContent(5,1,h_counts.GetBinContent(4,1))
        h_counts.SetBinError(5,1,h_counts.GetBinError(4,1))
        h_counts.SetBinContent(5,2,corrected_expected_accepted_STs)
        h_counts.SetBinContent(5,3,ewkRejST)
    h_counts.Write()
    h_counts.Draw("text E")
    simplecanvas.SaveAs("pngs/{0}/{1}.png".format(shortname,name))

tfile.Purge()

tfile.Close()

print "Done"
