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

if len(sys.argv) < 3: 
    print "Usage: python sigcontam.py signame tag"
    exit(1)

ROOT.TH1.SetDefaultSumw2()

signalname = sys.argv[1]
shorttrack_tag = sys.argv[2]
fshort_tag = shorttrack_tag if len(sys.argv) == 3 else sys.argv[3]

signalfile = ROOT.TFile.Open("output_merged/{}_{}.root".format(signalname,shorttrack_tag))

d16 = "output_merged/data_2016_{}_{}_contam.root".format(signalname,shorttrack_tag)
d1718 = "output_merged/data_2017and2018_{}_{}_contam.root".format(signalname,shorttrack_tag)

filenames = [d1718, d16]

histnames = ["h_" + nj + ht + "_" + reg + njsplit + ptstring for nj in ["H"] for ht in ["L","LM","M","H"] for reg in ["SR"] for njsplit in ["_4"] for ptstring in ["","_lo","_hi"]]
histnames += ["h_" + nj + ht + "_" + reg + njsplit + ptstring for nj in ["L"] for ht in ["L","LM","M","H"] for reg in ["SR"] for njsplit in ["_23"] for ptstring in ["","_lo","_hi"]]

def GetFShortName(histname):
    nj_h = histname[2]
    if nj_h == "L":
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

max_adj = 1.0
max_adj_reg = ""
max_h = None

for filename in filenames:

    outfile = ROOT.TFile.Open(filename,"RECREATE")

    if filename == d1718:
        print "2017-2018 Data"
        datafile = ROOT.TFile.Open("../ShortTrack/output_merged/data_2017and2018_{}.root".format(shorttrack_tag))
        rescale = 1+(58.83/41.97)
    elif filename == d16:
        print "2016 Data"
        datafile = ROOT.TFile.Open("../ShortTrack/output_merged/data_2016_{}.root".format(shorttrack_tag))
        rescale = 35.9/41.97

    for histname in histnames:
#        print histname
        hdata = datafile.Get(histname)
        if hdata == None:
            print histname
            exit(1)
        scanname = histname.replace("_23","").replace("_4","")
        print ">"+scanname
        hsig_STC_scan = signalfile.Get(scanname+"_STC")
        try:
            hsig_STC_scan.Scale(rescale)
        except:
            print histname
            exit(1)
        hsig_ST_scan = signalfile.Get(scanname+"_ST")
        hsig_ST_scan.Scale(rescale)
        hsig_FS_scan = signalfile.Get(GetFShortName(histname))
        h_correction_scan = hsig_STC_scan.Clone(histname+"_contam_"+signalname)
        h_correction_scan.Scale(0)
        h_correction_scan.SetTitle("Total Efficiency;Mass 1 (GeV);Mass 2 (GeV)")
        h_STC_scan = h_correction_scan.Clone(histname+"_contamSTC_"+signalname)
        h_STC_scan.SetTitle("Efficiency from STC Contam;Mass 1 (GeV);Mass 2 (GeV)")
        h_FS_scan = h_correction_scan.Clone(histname+"_contamFS_"+signalname)
        h_FS_scan.SetTitle("Efficiency from f_{short} Contam;Mass 1 (GeV);Mass 2 (GeV)")
        for biny in range(1,hsig_STC_scan.GetNbinsY()+1):
#        for biny in range(47,47+1):
            for binz in range(1,biny): # mLSP < mPrimary
#            for binz in range(34,34+1): # mLSP < mPrimary
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
                    if pred_data == 0 or st_sig == 0: # No need for a correction if prediction is already 0 due to 0 stcs, or if signal count is already 0
                        h_correction_scan.SetBinContent(length,biny,binz,1.0)
                        h_correction_scan.SetBinError(length,biny,binz,0)
                        continue
                    lost_from_fs = hsig_FS.GetBinContent(length)*pred_data # = fs(sig)/fs(data) => eff * Npred = fs(sig)*Nstc
                    eff_from_fs = max(1 - lost_from_fs/st_sig,0)
                    h_FS_scan.SetBinContent(length,biny,binz,eff_from_fs)

                    lost_from_stc = hsig_STC.GetBinContent(length)/hdata.GetBinContent(length,3)*pred_data # = Nstc(sig)/Nstc(data) => eff * Npred = fs(data)*Nstc(sig)
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
outfile.Close()

print "Done"
