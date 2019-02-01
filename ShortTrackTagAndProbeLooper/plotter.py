import ROOT
from math import sqrt
import re
import sys
import os
import subprocess
import ppmUtils as utils

# Suppresses warnings about TH1::Sumw2
ROOT.gROOT.SetBatch(True)
ROOT.gErrorIgnoreLevel = ROOT.kError
ROOT.gStyle.SetOptStat(False)

simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(600,600)
simplecanvas.SetTicks(1,2)
simplecanvas.SetLeftMargin(0.14)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetRightMargin(0.12)
simplecanvas.cd()

doMaps = True

os.system("mkdir -p pngs")

f16 = ROOT.TFile.Open("data_2016_STs.root")
f17 = ROOT.TFile.Open("data_2017_STs.root")
f18 = ROOT.TFile.Open("data_2018_STs.root")
fDY = ROOT.TFile.Open("DY_STs.root")


etaphi_names = ["etaphi_all_mu", "etaphi_nonPF_mu", "etaphi_STC_mu", "etaphi_ST_mu", "etaphi_all_el", "etaphi_nonPF_el", "etaphi_STC_el", "etaphi_ST_el"]

for name in etaphi_names:
    h16 = f16.Get(name)
    h16.Draw("")
    simplecanvas.SaveAs("pngs/"+name+"_16.png")
    h17 = f17.Get(name)
    h17.Draw("")
    simplecanvas.SaveAs("pngs/"+name+"_17.png")
    h18 = f18.Get(name)
    h18.Draw("")
    simplecanvas.SaveAs("pngs/"+name+"_18.png")
    hDY = fDY.Get(name)
    hDY.Draw("")
    simplecanvas.SaveAs("pngs/"+name+"_DY.png")

mLT_names = ["mLT_all_mu", "mLT_nonPF_mu", "mLT_STC_mu", "mLT_ST_mu", "mLT_all_el", "mLT_nonPF_el", "mLT_STC_el", "mLT_ST_el"]

for name in mLT_names:
    h16 = f16.Get(name)
    h16.Draw("")
    simplecanvas.SaveAs("pngs/"+name+"_16.png")
    h17 = f17.Get(name)
    h17.Draw("")
    simplecanvas.SaveAs("pngs/"+name+"_17.png")
    h18 = f18.Get(name)
    h18.Draw("")
    simplecanvas.SaveAs("pngs/"+name+"_18.png")
    hDY = fDY.Get(name)
    hDY.Draw("")
    simplecanvas.SaveAs("pngs/"+name+"_DY.png")
    

#nonPF_over_all_mu
#nonPF_over_all_el
#why_STC_mu
#why_STC_el

moh_16 = f16.Get("h_moh")
moh_17 = f17.Get("h_moh")
moh_18 = f18.Get("h_moh")
moh_DY = fDY.Get("h_moh")
moh_16.Draw()
simplecanvas.SaveAs("pngs/moh_16.png")
moh_17.Draw()
simplecanvas.SaveAs("pngs/moh_17.png")
moh_18.Draw()
simplecanvas.SaveAs("pngs/moh_18.png")
moh_DY.Draw()
simplecanvas.SaveAs("pngs/moh_DY.png")

simplecanvas.SetLogy()

el_flow_16 = f16.Get("el_flow")
el_flow_17 = f17.Get("el_flow")
el_flow_18 = f18.Get("el_flow")
el_flow_DY = fDY.Get("el_flow")
mu_flow_16 = f16.Get("mu_flow")
mu_flow_17 = f17.Get("mu_flow")
mu_flow_18 = f18.Get("mu_flow")
mu_flow_DY = fDY.Get("mu_flow")
el_flow_16.SetMinimum(1)
mu_flow_16.SetMinimum(1)
el_flow_17.SetMinimum(1)
mu_flow_17.SetMinimum(1)
el_flow_18.SetMinimum(1)
mu_flow_18.SetMinimum(1)
el_flow_DY.SetMinimum(1)
mu_flow_DY.SetMinimum(1)

el_flow_16.Draw()
simplecanvas.SaveAs("pngs/el_flow_16.png")
mu_flow_16.Draw()
simplecanvas.SaveAs("pngs/mu_flow_16.png")
el_flow_17.Draw()
simplecanvas.SaveAs("pngs/el_flow_17.png")
mu_flow_17.Draw()
simplecanvas.SaveAs("pngs/mu_flow_17.png")
el_flow_18.Draw()
simplecanvas.SaveAs("pngs/el_flow_18.png")
mu_flow_18.Draw()
simplecanvas.SaveAs("pngs/mu_flow_18.png")
el_flow_DY.Draw()
simplecanvas.SaveAs("pngs/el_flow_DY.png")
mu_flow_DY.Draw()
simplecanvas.SaveAs("pngs/mu_flow_DY.png")

