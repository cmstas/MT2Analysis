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
simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(600,600)
simplecanvas.SetTicks(1,2)
simplecanvas.SetLeftMargin(0.16)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetRightMargin(0.16)

if len(sys.argv) < 2: 
    print "Which file?"
    exit()

filename = sys.argv[1]
shortname = filename[filename.rfind("/")+1:filename.find(".")]

os.system("mkdir -p output")
os.system("mkdir -p pngs/{0}".format(shortname))

tfile = ROOT.TFile.Open(filename,"READ")
names = tfile.GetKeyNames()

outfile = ROOT.TFile.Open("output/Fshort_{}.root".format(shortname),"RECREATE")
outfile.cd()

for rawname in names:
    name = rawname[1:] # Get rid of leading /
    if name.find("FSR") > 0:        
        h_fsr = tfile.Get(name)
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
        simplecanvas.SaveAs("pngs/{0}/{1}.png".format(shortname,name))
        h_fsr.Write()
    elif name.find("etaphi") > 0:
        h_etaphi = tfile.Get(name)
        h_etaphi.SetTitle(h_etaphi.GetTitle()+";#eta;#phi")
        h_etaphi.Draw("colz")
        simplecanvas.SaveAs("pngs/{0}/{1}.png".format(shortname,name))
    elif name.find("mt2") > 0:
        h_mt2 = tfile.Get(name)
        h_mt2.SetLineWidth(3)
        h_mt2.SetMinimum(0)
        h_mt2.SetTitle(h_mt2.GetTitle()+";M_{T2} (GeV);Count")
        h_mt2.Draw()
        simplecanvas.SaveAs("pngs/{0}/{1}.png".format(shortname,name))

for length in ["P","M","L"]:
    mt2_STC=tfile.Get("h_mt2_STC_{}".format(length))
    mt2_ST=tfile.Get("h_mt2_ST_{}".format(length))
    
    mt2_STC_23=tfile.Get("h_mt2_STC_{}_23".format(length))
    mt2_ST_23=tfile.Get("h_mt2_ST_{}_23".format(length))
    
    mt2_STC_4=tfile.Get("h_mt2_STC_{}_4".format(length))
    mt2_ST_4=tfile.Get("h_mt2_ST_{}_4".format(length))
    
    mt2_ST.Divide(mt2_STC)
    mt2_ST.SetTitle("f_{{short}} by M_{{T2}}, {} Track;M_{{T2}} (GeV);f_{{short}}".format(length))
    mt2_ST.SetMinimum(0)
    mt2_ST.Draw()
    simplecanvas.SaveAs("pngs/{0}/fs_by_mt2_{1}.png".format(shortname,length))
    
    mt2_ST_23.Divide(mt2_STC_23)
    mt2_ST_23.SetTitle("f_{{short}} by M_{{T2}}, {} Track;M_{{T2}} (GeV);f_{{short}}".format(length))
    mt2_ST_23.SetMinimum(0)
    mt2_ST_23.Draw()
    simplecanvas.SaveAs("pngs/{0}/fs_by_mt2_{1}_23.png".format(shortname,length))
    
    mt2_ST_4.Divide(mt2_STC_4)
    mt2_ST_4.SetTitle("f_{{short}} by M_{{T2}}, {} Track;M_{{T2}} (GeV);f_{{short}}".format(length))
    mt2_ST_4.SetMinimum(0)
    mt2_ST_4.Draw()
    simplecanvas.SaveAs("pngs/{0}/fs_by_mt2_{1}_4.png".format(shortname,length))

outfile.Close()

tfile.Close()

print "Done"
