# STCImplementation background estimates and plots

import ROOT
from math import sqrt
import re
import sys
import os
from os.path import isfile,join
import subprocess
from array import array

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
#ROOT.gStyle.SetLegendTextSize(0.01)
ROOT.gStyle.SetPaintTextFormat("#3.3g")
ROOT.gStyle.SetLegendBorderSize(0)
simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(800,600)
simplecanvas.SetTicks(1,2)
simplecanvas.SetLeftMargin(0.14)
simplecanvas.SetRightMargin(0.1)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetBottomMargin(0.12)

tl=ROOT.TLegend(0.45,0.75,0.85,0.85)

ROOT.TH1.SetDefaultSumw2()

if len(sys.argv) < 2: 
    print "Which tag?"
    exit(1)

os.system("mkdir -p sigeff_plots")
format = "pdf"

tag = sys.argv[1]
includeEtaphi = True # include etaphi in denom, or not?

filepath = "output_unmerged/2017_{0}/signal/".format(tag)
sigfiles = [ROOT.TFile.Open(filepath+f) for f in os.listdir(filepath) if isfile(join(filepath, f))]

signamelist = [f.GetName()[f.GetName().rfind("/")+1:f.GetName().rfind(".root")].replace("-","_") for f in sigfiles]

for sigindex,sigfile in enumerate(sigfiles):
    signame = signamelist[sigindex]
    bins = array('d', [0,10,20,30,40,50,60,70,80,90,100] if signame.find("90cm") >= 0 else [0,10,20,30,40,100])    
    h_sigeff = ROOT.TH1D("h_sigeff_"+signame,"Signal Efficiency;Transverse Decay Length (cm);Efficiency",len(bins)-1,bins)
    h_sigeff.SetLineWidth(3)
    h_denom = h_sigeff.Clone("h_denom_"+signame)
    h_sigeff.SetLineColor(ROOT.kRed)
    h_denom.SetLineColor(ROOT.kGray)
    h_raw = sigfile.Get("h_sigpass").Clone("h_raw_"+signame)
    h_DecayXY = sigfile.Get("h_CharLength").Clone("h_DecayXY_"+signame) if not includeEtaphi else sigfile.Get("h_CharLength_etaphi").Clone("h_DecayXY_etaphi_"+signame)
    if signame.find("90cm_1800_1400") >= 0 :
        h_raw.Print("all")
        h_DecayXY.Print("all")
    full_integral = h_DecayXY.Integral(1,-1)
    integral_fraction = h_DecayXY.Integral()/full_integral
    if signame.find("90cm") >= 0:
        for bin in range(1,h_sigeff.GetNbinsX()+1):
            h_sigeff.SetBinContent(bin,h_raw.GetBinContent(bin))
            h_sigeff.SetBinError(bin,h_raw.GetBinError(bin))
            h_denom.SetBinContent(bin,h_DecayXY.GetBinContent(bin))
            h_denom.SetBinError(bin,h_DecayXY.GetBinError(bin))
    else: # merge last bins for 10cm signals
        for bin in range(1,h_sigeff.GetNbinsX()):
            h_sigeff.SetBinContent(bin,h_raw.GetBinContent(bin))
            h_sigeff.SetBinError(bin,h_raw.GetBinError(bin))
            h_denom.SetBinContent(bin,h_DecayXY.GetBinContent(bin))
            h_denom.SetBinError(bin,h_DecayXY.GetBinError(bin))
        lastbins_integral_num = h_raw.Integral(h_sigeff.GetNbinsX(),h_raw.GetNbinsX())
        lastbins_integral_den = h_DecayXY.Integral(h_sigeff.GetNbinsX(),h_raw.GetNbinsX())
        error_num = 0
        error_den = 0
        for bin in range(h_sigeff.GetNbinsX(),h_raw.GetNbinsX()+1):
            error_num += h_raw.GetBinError(bin)**2
            error_den += h_DecayXY.GetBinError(bin)**2
        error_num = sqrt(error_num)
        error_den = sqrt(error_den)
        h_sigeff.SetBinContent(h_sigeff.GetNbinsX(),lastbins_integral_num)
        h_sigeff.SetBinError(h_sigeff.GetNbinsX(),error_num)
        h_denom.SetBinContent(h_sigeff.GetNbinsX(),lastbins_integral_den)
        h_denom.SetBinError(h_sigeff.GetNbinsX(),error_den)
    h_sigeff.Divide(h_sigeff,h_denom,1,1,"B")
    h_sigeff.SetMinimum(0)
    h_sigeff.SetMaximum(1)
    tl.Clear()
    tl.AddEntry(h_sigeff,"Signal ST Efficiency")
    tl.AddEntry(h_denom,"Denominator; disappearing #tilde{{#chi}}^{{#pm}}_{{1}} ({:.0f}%) in selected events".format(integral_fraction*100))
    h_sigeff.Draw()
    h_denom.Scale(1/full_integral) # normalize with overflow, but without including overflow bin
    h_denom.Draw("same")
    tl.Draw()
    simplecanvas.SaveAs("sigeff_plots/{}.pdf".format(signame))

    sigfile.Close()
print "Done"
