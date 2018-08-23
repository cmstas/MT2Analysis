# STCImplementation background estimates and plots

import ROOT
from math import sqrt
import re
import sys
import os
import subprocess
import NitfyUtils

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
    print "Which tag to use?"
    exit()
runtag=sys.argv[1]
indir="output/"+runtag
pngdir="pngs/"+runtag
outdir="output/"
os.system("mkdir -p {0}".format(pngdir))
os.system("mkdir -p {0}".format(outdir))

dy_samples=["dy_ht1200to2500", "dy_ht400to600",  "dy_ht200to400", "dy_ht600to800", "dy_ht200to400", "dy_ht400to600"]    
qcd_samples=["qcd_ht2000toInf", "qcd_ht500to700", "qcd_ht300to500", "qcd_ht700to1000", "qcd_ht1000to1500", "qcd_ht1500to2000"]
tt_samples=["ttsl", "ttdl"]
wj_samples=[ "wjets_ht400to600","wjets_ht200to400","wjets_ht2500toInf"]
zinv_samples=["zinv_ht400to600","zinv_ht600to800",]

samples = dy_samples + qcd_samples + tt_samples + wj_samples + zinv_samples

filedict=dict([(sample,ROOT.TFile.Open("{0}/{1}.root".format(indir,sample))) for sample in samples])

names = filedict["ttdl"].GetKeyNames()

outfile = ROOT.TFile.Open("{0}/Fshort_{1}.root".format(outdir,runtag),"RECREATE")
outfile.cd()

for rawname in names:
    if rawname.find("FSR") < 0: continue
    name = rawname[1:] # Get rid of leading /
    h_fsr = filedict[samples[0]].Get(name)
    for sample in samples[1:]:
        h_fsr.Add(filedict[sample].Get(name))
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
    simplecanvas.SaveAs("{0}/{1}.png".format(pngdir,name))
    h_fsr.Write()

outfile.Close()

for name,f in filedict.iteritems():
    f.Close()

print "Done"
