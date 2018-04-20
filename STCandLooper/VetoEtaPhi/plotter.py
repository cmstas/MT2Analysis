import ROOT
from math import sqrt
import re
from sys import argv,exit
import os
import subprocess
import ppmUtils as utils

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
ROOT.gStyle.SetLegendTextSize(0.1)
canvas = ROOT.TCanvas()
canvas.SetCanvasSize(800,800)
canvas.SetTicks(2,1)
canvas.SetLeftMargin(0.14)
canvas.SetTopMargin(0.12)
canvas.SetRightMargin(0.12)
canvas.cd()

verbose = False # Print more status messages

indir = "../vetoHists_GV"

print "Processing input..."

f = ROOT.TFile("{0}/total.root".format(indir))
    
if f.IsZombie():
    print "{0} does not have a total file, please hadd".format(indir)
    exit(1)

# Where we see a lot of short tracks, assume detector issue.
h_short_bar = f.Get("h_st_etaphi_bar").Clone("h_short_bar")
h_short_ecn = f.Get("h_st_etaphi_ecn").Clone("h_short_ecn")
h_short_ecp = f.Get("h_st_etaphi_ecp").Clone("h_short_ecp")

h_all_bar = f.Get("h_etaphi_bar").Clone("h_all_bar")
h_all_ecn = f.Get("h_etaphi_ecn").Clone("h_all_ecn")
h_all_ecp = f.Get("h_etaphi_ecp").Clone("h_all_ecp")

for h_short in [h_short_bar,h_short_ecn,h_short_ecp,h_all_bar,h_all_ecn,h_all_ecp]:
    for x in xrange(10,h_short.GetNbinsX(),10):
        binx = x+1
        h_short.GetXaxis().SetBinLabel(binx,str(binx))
    for y in xrange(10,h_short.GetNbinsY(),10):
        biny = y+1
        h_short.GetYaxis().SetBinLabel(biny,str(biny))

h_short_bar.Draw("colz")
canvas.SaveAs("h_short_etaphi_bar.pdf")
h_short_ecn.Draw("colz")
canvas.SaveAs("h_short_etaphi_ecn.pdf")
h_short_ecp.Draw("colz")
canvas.SaveAs("h_short_etaphi_ecp.pdf")

h_all_bar.Draw("colz")
canvas.SaveAs("h_all_etaphi_bar.pdf")
h_all_ecn.Draw("colz")
canvas.SaveAs("h_all_etaphi_ecn.pdf")
h_all_ecp.Draw("colz")
canvas.SaveAs("h_all_etaphi_ecp.pdf")

veto_file = ROOT.TFile("VetoHists.root")
h_veto_bar = veto_file.Get("h_VetoEtaPhi_bar")
h_veto_ecp = veto_file.Get("h_VetoEtaPhi_ecp")
h_veto_ecn = veto_file.Get("h_VetoEtaPhi_ecn")
h_veto_bar.Draw("colz")
canvas.SaveAs("h_veto_bar.pdf")
h_veto_ecp.Draw("colz")
canvas.SaveAs("h_veto_ecp.pdf")
h_veto_ecn.Draw("colz")
canvas.SaveAs("h_veto_ecn.pdf")
