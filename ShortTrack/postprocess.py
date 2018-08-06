# STCImplementation background estimates and plots

import ROOT
from math import sqrt
import re
import sys
import os
import subprocess

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

indir="output/"
outdir="pngs/"
os.system("mkdir -p {0}".format(outdir))

dy_samples=["dy_ht1200to2500", "dy_ht400to600",  "dy_ht200to400", "dy_ht600to800", "dy_ht200to400", "dy_ht400to600"]    
qcd_samples=["qcd_ht2000toInf", "qcd_ht500to700", "qcd_ht300to500", "qcd_ht700to1000", "qcd_ht1000to1500", "qcd_ht1500to2000"]
tt_samples=["ttsl", "ttdl"]
wj_samples=[ "wjets_ht400to600","wjets_ht200to400","wjets_ht2500toInf"]
zinv_samples=["zinv_ht400to600","zinv_ht600to800"]

samples = dy_samples + qcd_samples + tt_samples + wj_samples + zinv_samples

filedict=dict([(sample,ROOT.TFile.Open("{0}/{1}.root".format(indir,sample))) for sample in samples])

names = filedict["ttdl"].GetKeyNames()

outfile = ROOT.TFile.Open("Result.root","RECREATE")
outfile.cd()

counts_names = []
for rawname in names:
    if rawname.find("SR") < 0 and rawname.find("VR") < 0: continue    
    name = rawname[1:] # Get rid of leading /
    counts_names.append(name)

simplecanvas.SetLeftMargin(0.22)
simplecanvas.SetRightMargin(0.22)

for name in counts_names:
    h_counts = filedict[samples[0]].Get(name)
    for sample in samples[1:]:
        h_counts.Add(filedict[sample].Get(name))
    h_counts.SetMarkerSize(1.5)
    h_counts.Draw("text E")
    simplecanvas.SaveAs("{0}/{1}.png".format(outdir,name))
    h_counts.Write()

outfile.Close()

for name,f in filedict.iteritems():
    f.Close()

print "Done"
