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
ROOT.gStyle.SetLegendBorderSize(0)
simplecanvas = ROOT.TCanvas("plotter")
simplecanvas.SetCanvasSize(600,600)
simplecanvas.SetTicks(1,1)
simplecanvas.SetLeftMargin(0.16)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetRightMargin(0.16)

total = ROOT.TFile.Open("output_merged/mc_2017_NoisoFloorSTCsWith1L_veto_M.root","READ")
hnames = ["h_" + reg + "_" + mt2 for reg in ["LL","LH","HL","HH"] for mt2 in ["VR","VR_23","VR_4","SR","SR_23","SR_4"]]
samples = ["qcd","zinv","ttsl","ttdl","singletop","ttX","wjets","dy","gjets","multiboson"]

samplefiles = [ROOT.TFile.Open("output_unmerged/2017like16_NoisoFloorSTCsWith1L_veto_M/{}.root".format(sample),"READ") for sample in samples]
sampledict = dict(zip(samples,samplefiles))

for name in hnames:
    print ""
    h_total = total.Get(name)
    if h_total == None: 
        print "Skipping", name
        continue
    obs_total_P = h_total.GetBinContent(1,1)
    print ""
    print name, "P"
    if obs_total_P > 0:
        for sample in samples:
            compare = sampledict[sample]
            h_compare = compare.Get(name)
            obs_compare_P = h_compare.GetBinContent(1,1)
            print sample, "{0:.1f}%".format(obs_compare_P/obs_total_P * 100) if obs_compare_P/obs_total_P < 0.1 else "****{0:.1f}%".format(obs_compare_P/obs_total_P * 100), int(h_compare.GetEntries()), round(obs_compare_P,2), h_compare.GetBinError(1,1)
    obs_total_M = h_total.GetBinContent(2,1)
    print ""
    print name, "M"
    if obs_total_M > 0:
        for sample in samples:
            compare = sampledict[sample]
            h_compare = compare.Get(name)
            obs_compare_M = h_compare.GetBinContent(2,1)
            print sample, "{0:.1f}%".format(obs_compare_M/obs_total_M * 100) if obs_compare_M/obs_total_M < 0.1 else "****{0:.1f}%".format(obs_compare_M/obs_total_M * 100), int(h_compare.GetEntries()), round(obs_compare_M,2), h_compare.GetBinError(2,1)
    obs_total_L = h_total.GetBinContent(3,1)
    print ""
    print name, "L"
    if obs_total_L > 0:
        for sample in samples:
            compare = sampledict[sample]
            h_compare = compare.Get(name)
            obs_compare_L = h_compare.GetBinContent(3,1)
            print sample, "{0:.1f}%".format(obs_compare_L/obs_total_L * 100) if obs_compare_L/obs_total_L < 0.1 else "****{0:.1f}%".format(obs_compare_L/obs_total_L * 100), int(h_compare.GetEntries()), round(obs_compare_L,2), h_compare.GetBinError(3,1)
    print ""

for f in samplefiles:
    f.Close()
total.Close()

print "Done"
