# plots comparing background and signal STs

import ROOT
from math import sqrt
import re
import sys
import os
from os.path import isfile,join
import subprocess
import ppmUtils as utils

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
#ROOT.gStyle.SetLegendTextSize(0.01)
ROOT.gStyle.SetPaintTextFormat("#3.3g")
ROOT.gStyle.SetLegendBorderSize(0)
simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(800,600)
simplecanvas.SetTicks(1,1)
simplecanvas.SetLeftMargin(0.14)
simplecanvas.SetRightMargin(0.1)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetBottomMargin(0.12)

tl=ROOT.TLegend(0.6,0.75,0.8,0.85)

ROOT.TH1.SetDefaultSumw2()

if len(sys.argv) < 2: 
    print "Which tag?"
    exit(1)

tag = sys.argv[1]

os.system("mkdir -p pngs_dists/{}".format(tag))

f16 = ROOT.TFile.Open("output_merged/data_2016_{}.root".format(tag))
f1718 = ROOT.TFile.Open("output_merged/data_2017and2018_{}.root".format(tag))

simplecanvas.SetLogy()

titles = {    "reliso" : "PF RelIso (R < 0.3) [year | trackcat | N-1];PF RelIso (GeV);Fraction",
              "iso"    : "PF Isolation (R < 0.3) [year | trackcat | N-1];PF Iso (GeV);Fraction",
              "dz"     : "Track dz [year | trackcat | N-1];dz (cm);Fraction",
              "dxy"     : "Track dxy [year | trackcat | N-1];dxy (cm);Fraction",
              "niso"    : "Neutral PF Isolation (R < 0.05) [year | trackcat | N-1];PF Neutral Iso (GeV);Fraction",
              "nreliso"    : "Neutral PF RelIso (R < 0.05) [year | trackcat | N-1];PF Neutral RelIso (GeV);Fraction",
              "pterr" : "#deltap_{T}/p_{T}^{2} [year | trackcat | N-1];#deltap_{T}/p_{T}^{2} (1/GeV);Fraction",
              "pt" : "p_{T} [year | trackcat];p_{T} (GeV);Fraction",
              "eta" : "|#eta| [year | trackcat];|#eta|;Fraction"
              }
              

for sel in ["reliso","iso","dz","dxy","niso","nreliso","pterr","pt","eta"]:
    print sel
    useNM1 = "_NM1" if (sel != "pt" and sel != "eta") else ""
    for cat in ["P","P3","P4","M","L"]:
        if cat == "P": files=[f16]
        elif cat == "P3" or cat == "P4": files=[f1718]
        else: files=[f16,f1718]
        for f in files:
            year = "2016" if f == f16 else "2017-18"
            hMR = utils.GetUnderOverHist(f,"h_{}_MR_STC_{}{}".format(sel,cat,useNM1),titles[sel].replace("trackcat",cat).replace("year",year),ROOT.kGreen,norm=True)
            hVR = utils.GetUnderOverHist(f,"h_{}_VR_STC_{}{}".format(sel,cat,useNM1),titles[sel].replace("trackcat",cat).replace("year",year),ROOT.kBlue,norm=True)
            hSR = utils.GetUnderOverHist(f,"h_{}_SR_STC_{}{}".format(sel,cat,useNM1),titles[sel].replace("trackcat",cat).replace("year",year),ROOT.kRed,norm=True)
            # Eta and iso bins are a bit too narrow, merge
            if sel == "eta" or sel.find("iso") >= 0:
                hMR = hMR.Rebin(2)
                hVR = hVR.Rebin(2)
                hSR = hSR.Rebin(2)
            tl.Clear()
            tl.AddEntry(hMR,"MR")
            tl.AddEntry(hVR,"VR")
            tl.AddEntry(hSR,"SR")
#            hMR.SetMaximum(1.5*max([hMR.GetMaximum(),hVR.GetMaximum(),hSR.GetMaximum()]))
            hMR.SetMaximum(1.2)
            hMR.SetMinimum(1e-3)
            hMR.Draw()
            hVR.Draw("same")
            hSR.Draw("same")
            tl.Draw()
            simplecanvas.SaveAs("pngs_dists/{}/{}_{}_{}.png".format(tag,sel,cat,year))

            del hMR, hVR, hSR
f16.Close()
f1718.Close()

print "Done"
