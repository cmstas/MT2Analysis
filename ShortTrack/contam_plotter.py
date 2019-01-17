# Make signal contamination plots.

import ROOT
from math import sqrt
import re
import sys
import os
from os.path import isfile,join
import subprocess

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
ROOT.gStyle.SetLegendTextSize(0.1)
ROOT.gStyle.SetPaintTextFormat("#3.3g")
simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(800,600)
simplecanvas.SetTicks(1,1)
simplecanvas.SetLeftMargin(0.14)
simplecanvas.SetRightMargin(0.14)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetBottomMargin(0.12)

ROOT.TH1.SetDefaultSumw2()

if len(sys.argv) < 2: 
    print "Which tag?"
    exit(1)

os.system("mkdir -p pngs_contam")

tag = sys.argv[1]

d16 = "output_merged/data_2016_{}_contam.root".format(tag)
d1718 = "output_merged/data_2017and2018_{}_contam.root".format(tag)

files = [ROOT.TFile.Open(filename) for filename in [d16, d1718]]
tags = ["2016","2017-18"]

filepath = "output_unmerged/2017_{0}/signal/".format(tag)
sigfiles = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
signamelist = [filename[filename.rfind("/")+1:filename.rfind(".root")].replace("-","_") for filename in sigfiles]

#regions16 = [length + "_" + nj + ht + ptstring for length in ["P"] for nj in ["L","H"] for ht in ["L","M","H"] for ptstring in ["_lo","_hi"]]
#regions16 += [length + "_" + nj + ht + ptstring for length in ["M"] for nj in ["L","H"] for ht in ["LM","H"] for ptstring in ["_lo","_hi"]]
regions16 = [length + "_" + nj + ht + ptstring for length in ["P","M"] for nj in ["L","H"] for ht in ["L","M","H"] for ptstring in ["_lo","_hi"]]
regions16 += [length + "_" + nj + ht  for length in ["L"] for nj in ["L","H"] for ht in ["LM","H"]]


#regions1718 = [length + "_" + nj + ht  + ptstring for length in ["P3"] for nj in ["L","H"] for ht in ["L","M","H"] for ptstring in ["_lo","_hi"]]
#regions1718 += [length + "_" + nj + ht  + ptstring for length in ["P4","M"] for nj in ["L","H"] for ht in ["LM","H"]  for ptstring in ["_lo","_hi"]]
regions1718 = [length + "_" + nj + ht  + ptstring for length in ["P3","P4","M"] for nj in ["L","H"] for ht in ["L","M","H"]  for ptstring in ["_lo","_hi"]]
regions1718 += [length + "_" + nj + ht  for length in ["L"] for nj in ["L","H"] for ht in ["LM","H"]]

regions_sets = [regions16,regions1718]

lengthdict = {"P" : 1, "P3" : 2, "P4" : 3, "M" : 4, "L" : 5}

simplecanvas.cd()

for fileindex,f in enumerate(files):
    filetag = tags[fileindex]
    regions = regions_sets[fileindex]
    print filetag
    for correction in ["","FS","STC"]:
        for signame in signamelist:
            h_output = ROOT.TH1D("output{}_".format(correction)+filetag+"_"+signame,"Signal Efficiency Adjustments, "+filetag+", "+signame.replace("_"," ").replace("fastsim ","")+";;Sig Eff Adj",len(regions),0,len(regions))
            h_output.SetLineWidth(3)
            h_output.SetLineColor(ROOT.kBlack)
            h_output.SetMaximum(1.00001)
            h_output.SetMinimum(0)
            for regionindex,region in enumerate(regions):
                bin = regionindex+1
                tokens = region.split("_")
                length = lengthdict[tokens[0]]
                njht = tokens[1]            
                njht_split = "23" if njht[0] == "L" else "4"
                ptstring = "" if len(tokens) == 2 else "_"+tokens[2]
                histname = "h_"+njht+"_SR_"+njht_split+ptstring+"_contam"+correction+"_"+signame
                h = f.Get(histname)
                h_output.SetBinContent(bin,h.GetBinContent(length))
                h_output.SetBinError(bin,h.GetBinError(length))
            for regionindex,region in enumerate(regions):
                bin = regionindex+1
                h_output.GetXaxis().SetBinLabel(bin,region.replace("_"," "))
            h_output.GetXaxis().LabelsOption("v")
            h_output.Draw("E")
            simplecanvas.SaveAs("pngs_contam/{}.png".format(h_output.GetName()))

print "Done"
