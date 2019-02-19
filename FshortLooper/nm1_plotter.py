# STCImplementation background estimates and plots

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
simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(800,600)
simplecanvas.SetTicks(1,1)
simplecanvas.SetLeftMargin(0.14)
simplecanvas.SetRightMargin(0.1)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetBottomMargin(0.12)

ROOT.gStyle.SetLegendBorderSize(0)
tl=ROOT.TLegend(0.2,0.65,0.4,0.85)

ROOT.TH1.SetDefaultSumw2()

if len(sys.argv) < 2: 
    print "Which tag?"
    exit(1)

tag = sys.argv[1]

fbg = ROOT.TFile.Open("output_unmerged/2017_{}/ttsl.root".format(tag))
fsig = ROOT.TFile.Open("output_unmerged/2017_{}/signal/fastsim_90cm_1800-1400.root".format(tag))

histname_sig = "h_reliso_Incl_chargino_ST_P_NM1"
histname_bg = "h_reliso_Incl_ST_P_NM1"

hsig_P = utils.GetUnderOverHist(fsig,histname_sig,"[N-1] ST PF RelIso (#DeltaR=0.3, #Delta#beta-corrected);RelIso;Fraction",ROOT.kBlack)
hbg_P = utils.GetUnderOverHist(fbg,histname_bg,color=ROOT.kGreen)

hsig_M = utils.GetUnderOverHist(fsig,histname_sig.replace("_P_","_M_"))
hbg_M = utils.GetUnderOverHist(fbg,histname_bg.replace("_P_","_M_"))

hsig_L = utils.GetUnderOverHist(fsig,histname_sig.replace("_P_","_L_"))
hbg_L = utils.GetUnderOverHist(fbg,histname_bg.replace("_P_","_L_"))

hsig_P.Add(hsig_M)
hsig_P.Add(hsig_L)
hsig_P.Scale(1/hsig_P.Integral())

hbg_P.Add(hbg_M)
hbg_P.Add(hbg_L)
hbg_P.Scale(1/hbg_P.Integral())

tl.AddEntry(hsig_P,"Signal (Matched)")
tl.AddEntry(hbg_P,"SM (1-lep t#bar{t})")

simplecanvas.SetLogy()
hsig_P.Draw()
hbg_P.Draw("same")
tl.Draw()
simplecanvas.SaveAs("nm1_reliso.png")

print "Done"
