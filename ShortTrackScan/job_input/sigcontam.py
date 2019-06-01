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

eff_2016_lo = [2.32,0,0,1.17,0.78] # rescale 2017 fastsim counts based on 2016 count ratios
eff_2016_hi = [0.45,0,0,1.17,0.78] 

ROOT.TH1.SetDefaultSumw2()

filename = sys.argv[1][sys.argv[1].rfind("/")+1:]

tokens = filename[0:filename.find(".root")].split("_") # signal ctau tag year
signalname = tokens[0]+"_"+tokens[1]
shorttrack_tag = tokens[2]
year = tokens[3]

signalfile = ROOT.TFile.Open("{}_{}_{}.root".format(signalname,shorttrack_tag,year))

d16 = "data_2016_{}_{}_contam.root".format(signalname,shorttrack_tag)
d1718 = "data_2017and2018_{}_{}_contam.root".format(signalname,shorttrack_tag)

filenames = [d1718, d16] if len(sys.argv) == 3 else ([d16] if year == "2016" else [d1718])

histnames = ["h_" + nj + ht + "_" + reg + njsplit + ptstring for nj in ["1"] for ht in ["L","LM","M","H"] for reg in ["SR"] for njsplit in ["_1"] for ptstring in ["","_lo","_hi"]]
histnames += ["h_" + nj + ht + "_" + reg + njsplit + ptstring for nj in ["L"] for ht in ["L","LM","M","H"] for reg in ["SR"] for njsplit in ["_23"] for ptstring in ["","_lo","_hi"]]
histnames += ["h_" + nj + ht + "_" + reg + njsplit + ptstring for nj in ["H"] for ht in ["L","LM","M","H"] for reg in ["SR"] for njsplit in ["_4"] for ptstring in ["","_lo","_hi"]]

def GetFShortName(histname):
    nj_h = histname[2]
    if nj_h == "1":
        nj = "1"
    elif nj_h == "L":
        nj = "23"
    else:
        nj = "4"
    if histname.find("lo") >= 0:
        pt = "_lo"
    elif histname.find("hi") >= 0:
        pt = "_hi"
    else:
        pt = ""
    return "h_fracFS_{}{}".format(nj,pt)

#f_weights_16 = ROOT.TFile.Open("{}/2016/nsig_weights_{}.root".format("../babymaker/data/short_track",signalname))
#h_nevents_16_temp = f_weights_16.Get("h_nsig")
#h_nevents_16 = h_nevents_16_temp.Clone("h_nsig_16")
#h_nevents_16.SetDirectory(0)
#f_weights_16.Close()                             
#f_weights_17 = ROOT.TFile.Open("{}/2017/nsig_weights_{}.root".format("../babymaker/data/short_track",signalname))
#h_nevents_17_temp = f_weights_17.Get("h_nsig")
#h_nevents_17 = h_nevents_17_temp.Clone("h_nsig_17")
#h_nevents_17.SetDirectory(0)
#f_weights_17.Close()                             

max_adj = 1.0
max_adj_reg = ""
max_h = None

for filename in filenames:

    outfile = ROOT.TFile.Open(filename,"RECREATE")

    if filename == d1718:
        print "2017-2018 Data"
        datafile = ROOT.TFile.Open("data_2017and2018_{}.root".format(shorttrack_tag))
    elif filename == d16:
        print "2016 Data"
        datafile = ROOT.TFile.Open("data_2016_{}.root".format(shorttrack_tag))        

    for histname in histnames:
#        print histname
        hdata = datafile.Get(histname)
        if hdata == None:
            print histname
            exit(1)
        nj = "23"
        if histname.find("4") > 0: nj = "4"
        elif histname.find("1") > 0: nj = "1"
        end = histname.rfind("_"+nj)
        pt = ""
        eff = eff_2016_hi
        if histname.find("lo") > 0: 
            pt = "_lo"
            eff = eff_2016_lo
        elif histname.find("hi") > 0: 
            pt = "_hi"
            eff = eff_2016_hi
        scanname = histname[:end]+pt
        print ">"+scanname
        # STC and ST
        #hsig_STC_scan = signalfile.Get(scanname+"_STC")
        hsig_STC_scan = signalfile.Get(scanname+"_STC")
        hsig_ST_scan = signalfile.Get(scanname+"_ST")
        if year == "2016":
            hsig_STC_scan.Scale(35.9/41.97)
            hsig_ST_scan.Scale(35.9/41.97)
        # FS
        hsig_FS_scan = signalfile.Get(GetFShortName(histname))
        print GetFShortName(histname)
        # Final point-by-point signal contamination efficiency correction
        h_correction_scan = hsig_STC_scan.Clone(histname+"_contam_"+signalname)
        h_correction_scan.Scale(0)
        h_correction_scan.SetTitle("Total Efficiency;Mass 1 (GeV);Mass 2 (GeV)")
        h_STC_scan = h_correction_scan.Clone(histname+"_contamSTC_"+signalname)
        h_STC_scan.SetTitle("Efficiency from STC Contam;Mass 1 (GeV);Mass 2 (GeV)")
        h_FS_scan = h_correction_scan.Clone(histname+"_contamFS_"+signalname)
        h_FS_scan.SetTitle("Efficiency from f_{short} Contam;Mass 1 (GeV);Mass 2 (GeV)")
        for biny in range(1,hsig_STC_scan.GetNbinsY()+1):
#        for biny in range(47,47+1):
            m1 = hsig_STC_scan.GetYaxis().GetBinLowEdge(biny)
#            nevt_x = h_nevents_16.GetXaxis().FindBin(m1)
            for binz in range(1,biny): # mLSP < mPrimary
#            for binz in range(34,34+1): # mLSP < mPrimary
                m2 = hsig_STC_scan.GetZaxis().GetBinLowEdge(binz)
#                nevt_y = h_nevents_16.GetYaxis().FindBin(m2)
#                nsig_16 = h_nevents_16.GetBinContent(nevt_x,nevt_y)
#                nsig_17 = h_nevents_17.GetBinContent(nevt_x,nevt_y)
#                if nsig_17 == 0:
#                    continue
                doRescale = year == "2016"
                hsig_STC = hsig_STC_scan.ProjectionX("projection_STC_{}_{}".format(biny,binz),biny,biny,binz,binz)
                hsig_ST = hsig_ST_scan.ProjectionX("projection_ST_{}_{}".format(biny,binz),biny,biny,binz,binz)
                hsig_FS = hsig_FS_scan.ProjectionX("projection_FS_{}_{}".format(biny,binz),biny,biny,binz,binz)
#                hsig_STC.Print("all")
#                hsig_ST.Print("all")
#                hsig_FS.Print("all")
                for length in range(1,hdata.GetNbinsX()+1):
                    if length == 1 and filename == d1718:
                        continue
                    elif (length == 2 or length == 3) and filename == d16:
                        continue
                    pred_data = hdata.GetBinContent(length,2) # 3 is STC, 2 is pred, 1 is obs
                    st_sig = hsig_ST.GetBinContent(length)
                    if doRescale: st_sig *= eff[length-1]
                    if pred_data == 0 or st_sig == 0: # No need for a correction if prediction is already 0 due to 0 stcs, or if signal count is already 0
                        h_correction_scan.SetBinContent(length,biny,binz,1.0)
                        h_correction_scan.SetBinError(length,biny,binz,0)
                        continue
                    lost_from_fs = hsig_FS.GetBinContent(length)*pred_data # = fs(sig)/fs(data) => eff * Npred = fs(sig)*Nstc
                    eff_from_fs = max(1 - lost_from_fs/st_sig,0)
                    h_FS_scan.SetBinContent(length,biny,binz,eff_from_fs)

                    stc_sig = hsig_STC.GetBinContent(length)
                    if doRescale: stc_sig *= eff[length-1]
                    lost_from_stc = stc_sig/hdata.GetBinContent(length,3)*pred_data # = Nstc(sig)/Nstc(data) => eff * Npred = fs(data)*Nstc(sig)
                    eff_from_stc = max(1 - lost_from_stc/st_sig,0)
                    h_STC_scan.SetBinContent(length,biny,binz,max(eff_from_stc,0))

                    # Now, the total change is this new count over the original
                    total_eff = max(1 - (lost_from_stc + lost_from_fs)/st_sig,0) # linearized, overestimates contam
                    h_correction_scan.SetBinContent(length,biny,binz,total_eff)
                    h_correction_scan.SetBinError(length,biny,binz,0.5*(1-total_eff))
 #                   print "FS:",lost_from_fs,"STC:",lost_from_stc,"total eff:",total_eff
        outfile.cd()
        h_STC_scan.Write()
        h_FS_scan.Write()
        h_correction_scan.Write()
    datafile.Close()

signalfile.Close()
signalfile.Close()
outfile.Close()

print "Done"
