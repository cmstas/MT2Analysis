# Calculate effect of signal contamination on fshort.

import ROOT
from math import sqrt
import re
import sys
import os
from os.path import isfile,join
import subprocess

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

ROOT.gPad.SetPhi(-135)

if len(sys.argv) < 2: 
    print "Which tag?"
    exit()

ROOT.TH1.SetDefaultSumw2()

tag = sys.argv[1]
comparefile = sys.argv[2]

rescale = 1+(58.83/41.97) if comparefile.find("2016") < 0 else 35.9/41.97
year = "2016" if comparefile.find("2016") >= 0 else "2017and2018"

filepath = "output_unmerged/2017_{0}/signal/".format(tag)
inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
inputnamelist = [filename[filename.rfind("/")+1:filename.rfind(".root")].replace("-","_") for filename in inputlist]
filelist = [ROOT.TFile.Open(infile) for infile in inputlist]

os.system("mkdir -p output")

datafile = ROOT.TFile.Open(comparefile,"READ")
names = datafile.GetKeyNames()
        
outfile = ROOT.TFile.Open("output/Sigcontam_{}_{}.root".format(year,tag),"RECREATE")
outfile.cd()

for index,signalfile in enumerate(filelist):
    signal = inputnamelist[index]
    print signalfile.GetName()
    # Loop over all fshort hists and find effect of signal contamination
    for rawname in names:
        name = rawname[1:] # Get rid of leading /
        # Fshort histograms
        if name.find("fsMR") < 0: continue
        if name.find("max_weight") >= 0: continue
        if name.find("Baseline") < 0: continue
        if name.find("alt") >= 0: continue
        if name.find("syst") >= 0: continue
        if verbose: print name
        h_fs = datafile.Get(name)
        print_debug = name == "h_fsMR_4_Baseline_lowpt"
        if print_debug: h_fs.Print("all")
        h_corrected = h_fs.Clone(name+"_corrected_"+signal)
        h_sig = signalfile.Get(name)
        h_sig.Scale(rescale)
        h_corrected.Add(h_sig,-1.0)
        if print_debug: h_corrected.Print("all")
        h_reldeltafs = ROOT.TH1D(name+"_contam_"+signal,"Relative #Delta f_{short}",5,0,5)
        h_deltafs = ROOT.TH1D(name+"_deltafs_"+signal,"Absolute #Delta f_{short}",5,0,5)
        for length in range(1,5): # P, P3, P4, M
            corrected_STC = h_corrected.GetBinContent(length,3)
            corrected_ST = h_corrected.GetBinContent(length,2)
            # The relevant error is the error on the signal counts
            sig_STC_rel_err = 0 if h_sig.GetBinContent(length,3) == 0 else h_sig.GetBinError(length,3)/h_sig.GetBinContent(length,3)
            sig_ST_rel_err = 0 if h_sig.GetBinContent(length,2) == 0 else h_sig.GetBinError(length,2)/h_sig.GetBinContent(length,2)
            sig_rel_err = sqrt(sig_STC_rel_err**2 + sig_ST_rel_err**2)
            old_fs = h_fs.GetBinContent(length,1)
            new_fs = -1 if corrected_STC == 0 else corrected_ST / corrected_STC
            h_reldeltafs.SetBinContent(length,1-(new_fs/old_fs) if old_fs > 0 and new_fs > 0 else -1)
            h_reldeltafs.SetBinError(length,sig_rel_err * h_reldeltafs.GetBinContent(length))
            h_deltafs.SetBinContent(length,h_reldeltafs.GetBinContent(length)*old_fs)
            h_deltafs.SetBinError(length,h_reldeltafs.GetBinError(length)*old_fs)
        h_reldeltafs.SetBinContent(5,h_reldeltafs.GetBinContent(4)) # L adj is M adj
        h_reldeltafs.SetBinError(5,h_reldeltafs.GetBinError(4)) # L adj is M adj
        if print_debug: 
            h_reldeltafs.Print("all")
            h_deltafs.Print("all")
        h_reldeltafs.Write()
        h_deltafs.SetBinContent(5,h_deltafs.GetBinContent(4)) # L adj is M adj
        h_deltafs.SetBinError(5,h_deltafs.GetBinError(4)) # L adj is M adj
        h_deltafs.Write()
        
outfile.Close()

datafile.Close()
for f in filelist:
    f.Close()

print "Done"
