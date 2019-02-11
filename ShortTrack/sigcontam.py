# Calculate impact of signal contamination (requires that sigcontam.py in FshortLooper be run first).

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
simplecanvas.SetCanvasSize(600,600)
simplecanvas.SetTicks(1,2)
simplecanvas.SetLeftMargin(0.2)
simplecanvas.SetRightMargin(0.2)
simplecanvas.SetTopMargin(0.12)

if len(sys.argv) < 2: 
    print "Which tag?"
    exit(1)

ROOT.TH1.SetDefaultSumw2()

shorttrack_tag = sys.argv[1]
fshort_tag = shorttrack_tag if len(sys.argv) == 2 else sys.argv[2]

d16 = "output_merged/data_2016_{}_contam.root".format(shorttrack_tag)
d1718 = "output_merged/data_2017and2018_{}_contam.root".format(shorttrack_tag)

filenames = [d1718, d16]

filepath = "output_unmerged/2017_{0}/signal/".format(shorttrack_tag)
inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f)) if f.find("GENMET") < 0]
inputnamelist = [filename[filename.rfind("/")+1:filename.rfind(".root")].replace("-","_") for filename in inputlist]
filelist = [ROOT.TFile.Open(infile) for infile in inputlist]

histnames = ["h_" + nj + ht + "_" + reg + njsplit + ptstring for nj in ["H"] for ht in ["L","LM","M","H"] for reg in ["VR","SR"] for njsplit in ["_4"] for ptstring in ["","_lo","_hi"]]
histnames += ["h_" + nj + ht + "_" + reg + njsplit + ptstring for nj in ["L"] for ht in ["L","LM","M","H"] for reg in ["VR","SR"] for njsplit in ["_23"] for ptstring in ["","_lo","_hi"]]


max_adj = 1.0
max_adj_reg = ""
max_h = None

for filename in filenames:

    outfile = ROOT.TFile.Open(filename,"RECREATE")

    if filename == d1718:
        print "2017-2018 Data"
        datafile = ROOT.TFile.Open("output_merged/data_2017and2018_{}.root".format(shorttrack_tag))
        rescale = 1+(58.83/41.97)
        fsfile = ROOT.TFile.Open("../FshortLooper/output/Sigcontam_2017and2018_{}.root".format(fshort_tag))
    elif filename == d16:
        print "2016 Data"
        datafile = ROOT.TFile.Open("output_merged/data_2016_{}.root".format(shorttrack_tag))
        rescale = 35.9/41.97
        fsfile = ROOT.TFile.Open("../FshortLooper/output/Sigcontam_2016_{}.root".format(fshort_tag))

    for index,signalfile in enumerate(filelist):
        signalname = inputnamelist[index]
        for histname in histnames:
            hdata = datafile.Get(histname)
            if hdata == None:
                print histname
                exit(1)
            hsig = signalfile.Get(histname)
            hsig.Scale(rescale)
            h_correction = ROOT.TH1D(histname+"_contam_"+signalname,"Total Relative Correction",hdata.GetNbinsX(),0,hdata.GetNbinsX())
            h_STC = ROOT.TH1D(histname+"_contamSTC_"+signalname,"STC Contamination Relative Correction",hdata.GetNbinsX(),0,hdata.GetNbinsX())
            h_FS = ROOT.TH1D(histname+"_contamFS_"+signalname,"MR Contamination Relative Correction",hdata.GetNbinsX(),0,hdata.GetNbinsX())
            tokens = histname.split("_")
            nj = tokens[3]
            ptstring = "" if len(tokens) < 5 else ("_lowpt" if tokens[4] == "lo" else "_hipt")
            deltafs_name = "h_fsMR_"+nj+"_Baseline"+ptstring+"_deltafs_"+signalname
            h_deltafs = fsfile.Get(deltafs_name)
            if h_deltafs == None: 
                print "couldn't find deltafs hist",deltafs_name
                exit(1)
            for length in range(1,hdata.GetNbinsX()+1):
                if length == 1 and filename == d1718:
                    continue
                elif (length == 2 or length == 3) and filename == d16:
                    continue
                stc_data = hdata.GetBinContent(length,3) # 3 is STC, 2 is pred, 1 is obs
                if stc_data == 0: # No need for a correction if prediction is already 0 due to 0 stcs
                    h_correction.SetBinContent(length,1.0)
                    h_correction.SetBinError(length,0)
                    continue
                stc_sig = hsig.GetBinContent(length,3)
                st_sig = hsig.GetBinContent(length,1)
                inferred_raw_fs = hdata.GetBinContent(length,2)/stc_data
                delta_count_stc = stc_sig * inferred_raw_fs
                if delta_count_stc > stc_data:
                    print "More signal STCs than data in",histname,length
                    print "Capping at 100% efficiency loss"
                    delta_count_stc = stc_data                    
                relative_delta_stc = delta_count_stc / st_sig if st_sig > 0 else 0
                h_STC.SetBinContent(length,delta_count_stc)

                new_st_count = st_sig - delta_count_stc

                # No do impact of MR contamination, through fshort
                #adj_stc = stc_data - stc_sig
                delta_fs = max(h_deltafs.GetBinContent(length),0)                    
                delta_count_fs = stc_data * delta_fs # instead of using adj_stc, use stc_data. This is conservative, and makes contam proportional to signal strength.
                if deltafs_name == "h_fsMR_4_Baseline_lowpt_deltafs_fastsim_90cm_1800_1400" and filename == d1718 and length == 2 and histname.find("_HL_") > 0 and histname.find("_SR_") > 0:
                    h_deltafs.Print("all")
                    hsig.Print("all")
                    print "delta fs",delta_fs
                    #print "new Nstc",adj_stc
                    print "change in count due to fs",delta_count_fs
                    print "Nst sig",st_sig
                    print "Nst'/Nst",(new_st_count-delta_count_fs)/st_sig if st_sig > 0 else "0 denom"
                h_FS.SetBinContent(length,delta_count_fs / new_st_count if new_st_count > 0 else 0)
                h_FS.SetBinError(length,0 if h_FS.GetBinContent(length) <= 0 else h_deltafs.GetBinError(length)/delta_fs * h_FS.GetBinContent(length))

                # Now, the total change is this new count over the original
                final_st_count = new_st_count - delta_count_fs
                h_correction.SetBinContent(length,final_st_count/st_sig if st_sig > 0 else 1.0)
                h_correction.SetBinError(length,0.5*(1-h_correction.GetBinContent(length)))
#                if h_FS.GetBinContent(length) <= 0 or h_STC.GetBinContent(length) <= 0:
#                    h_correction.SetBinError(length,0.15*h_correction.GetBinContent(length))
#                else: 
#                    h_correction.SetBinError(length,max(sqrt( (h_FS.GetBinError(length)/h_FS.GetBinContent(length))**2 + (h_STC.GetBinError(length)/h_STC.GetBinContent(length))**2) * h_correction.GetBinContent(length),0.15*h_correction.GetBinContent(length)))
            outfile.cd()
            h_STC.Write()
            h_FS.Write()
            h_correction.Write()

datafile.Close()
fsfile.Close()
for f in filelist:
    f.Close()
outfile.Close()

print "Done"
