# STCImplementation background estimates and plots

import ROOT
from math import sqrt
import re
import sys
import os
from os.path import isfile,join
import subprocess
from array import array
import ppmUtils as utils

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

variableBins = False
suppressHorizontal = False

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
if not variableBins and suppressHorizontal: 
    ROOT.gStyle.SetErrorX(0)
    legOpt = "pE"
    errOpt = "E1"
else:
    legOpt = "pLE"
    errOpt = ""
ROOT.gStyle.SetLegendTextSize(0.025)
ROOT.gStyle.SetPaintTextFormat("#3.3g")
ROOT.gStyle.SetLegendBorderSize(0)
simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(800,600)
simplecanvas.SetTicks(1,2)
simplecanvas.SetLeftMargin(0.14)
simplecanvas.SetRightMargin(0.1)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetBottomMargin(0.12)

tl=ROOT.TLegend(0.18,0.68,0.8,0.85)

ROOT.TH1.SetDefaultSumw2()

if len(sys.argv) < 2: 
    print "Which tag?"
    exit(1)

os.system("mkdir -p sigeff_plots")
format = "pdf"

tag = sys.argv[1]
includeEtaphi = False # include etaphi in denom, or not?

filepath = "output_unmerged/2017_{0}/signal/".format(tag)
sigfiles = [ROOT.TFile.Open(filepath+f) for f in os.listdir(filepath) if isfile(join(filepath, f))]

signamelist = [f.GetName()[f.GetName().rfind("/")+1:f.GetName().rfind(".root")].replace("-","_") for f in sigfiles]

#ttext = ROOT.TLatex()
#ttext.SetNDC(1)
#ttext.SetTextFont(42)
#ttext.SetTextAlign(31)
#ttext.SetTextSize(0.035)


for sigindex,sigfile in enumerate(sigfiles):
    sigtag = signamelist[sigindex]
    signame = sigtag.replace("fastsim_","").replace("_"," ").replace("90cm","c#tau_{0}(#tilde{#chi}^{#pm}_{1}) = 90 cm,").replace("1800","M_{#tilde{g}} = 1800 GeV, M_{#tilde{#chi}_{1}^{0}} = ") + " GeV"
    sigmass = signame[signame.find("M"):]
    sigctau = signame[:signame.find("M")]
    if variableBins:
        bins = array('d', [5, 7.5, 10, 12.5, 15.0, 17.5, 20, 30, 40, 50, 60, 70, 80, 90, 95, 97.5, 100])        
    else:
        bins = array('d', [0,10,20,30,40,50,60,70,80,90,100] if sigtag.find("90cm") >= 0 else [0,10,20,30,40,100])    
    if not sigtag.find("90cm"): continue
    h_sigeff = ROOT.TH1D("h_sigeff_"+sigtag,";Transverse decay length [cm];Track selection efficiency",len(bins)-1,bins)
#    h_sigeff.SetLineWidth(3)
    h_denom = h_sigeff.Clone("h_denom_"+sigtag)
    h_sigeff.SetLineColor(ROOT.kBlack)
    h_denom.SetLineColor(ROOT.kGray)
    h_raw = sigfile.Get("h_sigpass").Clone("h_raw_"+sigtag)
    h_DecayXY = sigfile.Get("h_CharLength").Clone("h_DecayXY_"+sigtag) if not includeEtaphi else sigfile.Get("h_CharLength_etaphi").Clone("h_DecayXY_etaphi_"+sigtag)
    full_integral = h_DecayXY.Integral(1,-1)
    partial_integral = h_DecayXY.Integral(2,h_DecayXY.GetNbinsX())
    integral_fraction = h_DecayXY.Integral()/full_integral
    if sigtag.find("90cm") >= 0:
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
    if not variableBins: h_sigeff.GetXaxis().SetRange(2,h_sigeff.GetNbinsX())
    h_sigeff.GetXaxis().SetLabelOffset(0.015)
    h_sigeff.GetXaxis().SetTitleOffset(1.4)
    tl.Clear()
    tl.AddEntry(h_sigeff,"#splitline{pp #rightarrow #tilde{g}#tilde{g}, #tilde{g} #rightarrow q#bar{q}'#tilde{#chi}_{1}^{#pm}, #tilde{#chi}_{1}^{#pm} #rightarrow #tilde{#chi}_{1}^{0}#pi^{#pm}, "+sigmass+"}{"+sigctau+"#DeltaR(track, #tilde{#chi}_{1}^{#pm}) < 0.01}",legOpt)
#    tl.AddEntry(h_denom,"#splitline{{Denominator: disappearing #tilde{{#chi}}^{{#pm}}_{{1}} ({:.0f}%) in selected events}}{{Normalized to unity}}".format(integral_fraction*100))
#    tl.AddEntry(h_denom,"Denominator: #tilde{#chi}^{#pm}_{1} in events passing selections for any kinematic region (normalized to unity)")
    h_sigeff.SetMarkerStyle(22)
    h_sigeff.SetMarkerColor(ROOT.kBlack)
    h_sigeff.Draw(errOpt)
#    h_denom.Scale(1/full_integral) # normalize with overflow, but without including overflow bin
    h_denom.Scale(1/partial_integral) # normalize to disappearing chargino range
#    h_denom.Draw("same")
    tl.Draw()
#    utils.DrawCmsText(simplecanvas,"CMS Simulation Preliminary")
#    ttext.DrawLatex(1.0-simplecanvas.GetRightMargin()-0.01, 1.0-simplecanvas.GetTopMargin()+0.01, "(13 TeV)")
    utils.CMS_Style(simplecanvas,extraText = "Simulation Preliminary")
    simplecanvas.SaveAs("sigeff_plots/{}.pdf".format(sigtag))

    sigfile.Close()
print "Done"
