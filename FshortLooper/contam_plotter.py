# STCImplementation background estimates and plots

import ROOT
from math import sqrt
import re
import sys
import os
from os.path import isfile,join
import subprocess

sys.path.append("../Nifty")

import NiftyUtils

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
#ROOT.gStyle.SetLegendTextSize(0.01)
ROOT.gStyle.SetPaintTextFormat("#3.3g")
simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(800,600)
simplecanvas.SetTicks(1,1)
simplecanvas.SetLeftMargin(0.14)
simplecanvas.SetRightMargin(0.1)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetBottomMargin(0.12)

tl=ROOT.TLegend(0.2,0.65,0.4,0.85)

ROOT.TH1.SetDefaultSumw2()

if len(sys.argv) < 2: 
    print "Which tag?"
    exit(1)

os.system("mkdir -p pngs_contam")

tag = sys.argv[1]

d16 = "output/Sigcontam_2016_{}.root".format(tag)
d1718 = "output/Sigcontam_2017and2018_{}.root".format(tag)

files = [ROOT.TFile.Open(filename) for filename in [d16, d1718]]
tags = ["2016","2017-18"]

filepath = "output_unmerged/2017_{0}/signal/".format(tag)
sigfiles = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
for s in sigfiles:
    if s.find("1700-1600") >= 0: sigfiles.remove(s)
sigfiles.sort()

signamelist = [filename[filename.rfind("/")+1:filename.rfind(".root")].replace("-","_") for filename in sigfiles]

fshorts16 = [length + "_" + nj + ptstring for length in ["P","M"] for nj in ["23","4"] for ptstring in ["_lowpt","_hipt"]]
fshorts16 += ["L_" + nj for nj in ["23","4"]]

fshorts1718 = [length + "_" + nj + ptstring for length in ["P3","P4","M"] for nj in ["23","4"] for ptstring in ["_lowpt","_hipt"]]
fshorts1718 += ["L_" + nj for nj in ["23","4"]]

fshorts_sets = [fshorts16,fshorts1718]

colors = [ROOT.kRed,ROOT.kMagenta,ROOT.kYellow,ROOT.kBlue,ROOT.kGreen,ROOT.kCyan]

lengthdict = {"P" : 1, "P3" : 2, "P4" : 3, "M" : 4, "L" : 5}

for fileindex,f in enumerate(files):
    filetag = tags[fileindex]
    fshorts = fshorts_sets[fileindex]
    print filetag
    h_output_base = ROOT.TH1D("h_output_"+filetag,"Relative Change in f_{short}, "+filetag+";;#Delta f_{short}/f_{short}",len(fshorts),0,len(fshorts))
    for fshortindex,fshort in enumerate(fshorts): 
        bin = fshortindex+1
        h_output_base.GetXaxis().SetBinLabel(bin,fshort.replace("_"," "))
        h_output_base.GetXaxis().LabelsOption("v")
    h_output_base.SetLineWidth(3)
    h_output_base.SetMinimum(0.0)
    tl.Clear()
    hists = []
    for sigindex,signame in enumerate(signamelist):
        h_output = h_output_base.Clone(h_output_base.GetName()+"_"+signame+"_"+filetag)
        for fshortindex,fshort in enumerate(fshorts):
            bin = fshortindex+1
            tokens = fshort.split("_")
            length = lengthdict[tokens[0]]
            nj = tokens[1]            
            ptstring = "_"+tokens[2] if len(tokens) > 2 else ""
            histname = "h_fsMR_"+nj+"_Baseline"+ptstring+"_contam_"+signame
            h = f.Get(histname)
            h_output.SetBinContent(bin,h.GetBinContent(length))
            h_output.SetBinError(bin,h.GetBinError(length))
        simplecanvas.cd()
        h_output.SetLineColor(colors[sigindex])
        tl.AddEntry(h_output,signame.replace("_"," ").replace("fastsim ",""))
        hists.append(h_output)
    hists[0].SetMaximum(2*max([h.GetMaximum() for h in hists]))
    hists[0].Draw("hist")
    for h in hists: h.Draw("same hist")
    tl.Draw()
    simplecanvas.SaveAs("pngs_contam/{}.png".format(filetag))

    err_hists = []
    for h in hists:
        h_err = ROOT.TH1D(h.GetName()+"_err","Relative Error",h.GetNbinsX(),0,h.GetNbinsX())
        h_err.SetLineColor(h.GetLineColor())
        h_err.SetLineWidth(3)
        for bin in range(1,h_err.GetNbinsX()+1): 
            h_err.SetBinContent(bin,h.GetBinError(bin)/h.GetBinContent(bin) if h.GetBinContent(bin) > 0 else 0)
            h_err.GetXaxis().SetBinLabel(bin,h.GetXaxis().GetBinLabel(bin))
        h_err.GetXaxis().LabelsOption("v")
        err_hists.append(h_err)
    err_hists[0].SetMaximum(2*max([h.GetMaximum() for h in err_hists]))
    err_hists[0].Draw("hist")
    for h in err_hists: h.Draw("same hist")
    tl.Draw()
    simplecanvas.SaveAs("pngs_contam/{}_err.png".format(filetag))

print "Done"
