# Produce plots comparing only T5qqqq to itself

import ROOT
from math import sqrt
import re
from sys import argv,exit
import os
import subprocess
import ppmUtils as utils

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
canvas = ROOT.TCanvas()
canvas.SetCanvasSize(800,800)
pad = ROOT.TPad("1","1",0,0,1,1)
pad.SetTicks(2,2)
pad.SetLeftMargin(0.14)
pad.SetTopMargin(0.12)
pad.SetRightMargin(0.12)
pad.Draw()
pad.cd()

ROOT.gStyle.SetLegendBorderSize(0)
ROOT.gStyle.SetLegendTextSize(0.1)
tl=ROOT.TLegend(0.65,0.71,0.88,0.88)
tl.SetMargin(0.1)

verbose = False # Print more status messages

interpolate = False
rebin = 5

if len(argv) > 1:
    indir = "../drHists_" + argv[1]
    outdir = "drpngs_" + argv[1]
else:
    indir = "../drHists_GV" 
    outdir = "drpngs_GV"

print "Input is {0}/*.root, .pngs will be placed here in LepDR".format(indir)

print "Processing input..."

#samples = ["ttsl","ttdl","DY","Wjets","qcd300to500","qcd500to700","qcd700to1000","qcd1000to1500","qcd1500to2000","qcd2000toInf","zinv100to200","zinv200toInf","total"]
samples = ["ttsl","ttdl"]
#filenames = ["ttsl","ttdl","dyjetstoll_incl_ext1","wjets_incl","qcd_ht300to500","qcd_ht500to700","qcd_ht700to1000","qcd_ht1000to1500","qcd_ht1500to2000_ext1","qcd_ht2000toInf","zinv_zpt100to200","zinv_zpt200toInf","total"]
filenames = ["ttsl","ttdl"]
filedict = dict(zip(samples,filenames))

for sample in samples:
    print sample

    f = ROOT.TFile("{0}/{1}.root".format(indir,filedict[sample]))
    h_el_dr = utils.GetUnderOverHist(f,"h_el_dr",over=True) #.Clone(sample+"eldr")
    h_mu_dr = utils.GetUnderOverHist(f,"h_mu_dr",color=ROOT.kRed,over=True) #.Clone(sample+"mudr")
    h_tau_dr = utils.GetUnderOverHist(f,"h_tau_dr",color=ROOT.kGreen,over=True) #.Clone(sample+"taudr")
    integral_el = h_el_dr.Integral()
    integral_mu = h_mu_dr.Integral()
    integral_tau = h_tau_dr.Integral()
    if (integral_el > 0):
        h_el_dr.Scale(1 / integral_el)
    if (integral_mu > 0):
        h_mu_dr.Scale(1 / integral_mu)
    if (integral_tau > 0):
        h_tau_dr.Scale(1 / integral_tau)
    h_el_dr.SetMaximum(1.2 * max([h.GetMaximum() for h in [h_el_dr,h_mu_dr,h_tau_dr]] ) )
    h_el_dr.SetMinimum(0)
    h_el_dr.Draw()
    h_mu_dr.Draw("same")
    h_tau_dr.Draw("same")
#    tle = tl.AddEntry(h_el_dr,"Electrons ({0})".format(int(integral_el)))
#    tle.SetTextSize(0.1)
#    tle = tl.AddEntry(h_mu_dr,"Muons ({0})".format(int(integral_mu)))
#    tle.SetTextSize(0.1)
#    tle = tl.AddEntry(h_el_dr,"Taus ({0})".format(int(integral_tau)))
#    tle.SetTextSize(0.1)
#    tl.Draw()
    canvas.SaveAs("{0}/{1}_dr.png".format(outdir,sample))

    h_el_ptr = utils.GetUnderOverHist(f,"h_el_ptr") #.Clone(sample+"elptr")
    h_mu_ptr = utils.GetUnderOverHist(f,"h_mu_ptr",color=ROOT.kRed) #.Clone(sample+"muptr")
    h_tau_ptr = utils.GetUnderOverHist(f,"h_tau_ptr",color=ROOT.kGreen) #.Clone(sample+"tauptr")
    integral_el = h_el_ptr.Integral()
    integral_mu = h_mu_ptr.Integral()
    integral_tau = h_tau_ptr.Integral()
    if (integral_el > 0):
        h_el_ptr.Scale(1 / integral_el)
    if (integral_mu > 0):
        h_mu_ptr.Scale(1 / integral_mu)
    if (integral_tau > 0):
        h_tau_ptr.Scale(1 / integral_tau)
    h_el_ptr.SetMaximum(1.2 * max([h.GetMaximum() for h in [h_el_ptr,h_mu_ptr,h_tau_ptr]] ))
    h_el_ptr.SetMinimum(0)
    h_el_ptr.Draw()
    h_mu_ptr.Draw("same")
    h_tau_ptr.Draw("same")
    canvas.SaveAs("{0}/{1}_ptr.png".format(outdir,sample))

    h_sh_el_dr = utils.GetUnderOverHist(f,"h_sh_el_dr",over=True) #.Clone(sample+"eldr")
    h_sh_el_dr.Rebin(5)
    h_sh_mu_dr = utils.GetUnderOverHist(f,"h_sh_mu_dr",color=ROOT.kRed,over=True) #.Clone(sample+"mudr")
    h_sh_mu_dr.Rebin(5)
    h_sh_tau_dr = utils.GetUnderOverHist(f,"h_sh_tau_dr",color=ROOT.kGreen,over=True) #.Clone(sample+"taudr")
    h_sh_tau_dr.Rebin(5)
    integral_el = h_sh_el_dr.Integral()
    integral_mu = h_sh_mu_dr.Integral()
    integral_tau = h_sh_tau_dr.Integral()
    if (integral_el > 0):
        h_sh_el_dr.Scale(1 / integral_el)
    if (integral_mu > 0):
        h_sh_mu_dr.Scale(1 / integral_mu)
    if (integral_tau > 0):
        h_sh_tau_dr.Scale(1 / integral_tau)
    h_sh_el_dr.SetMaximum(1.2 * max([h.GetMaximum() for h in [h_sh_el_dr,h_sh_mu_dr,h_sh_tau_dr]]) )
    h_sh_el_dr.SetMinimum(0)
    h_sh_el_dr.Draw()
    h_sh_mu_dr.Draw("same")
    h_sh_tau_dr.Draw("same")
#    tle = tl.AddEntry(h_sh_el_dr,"Electrons ({0})".format(int(integral_el)))
#    tle.SetTextSize(0.1)
#    tle = tl.AddEntry(h_sh_mu_dr,"Muons ({0})".format(int(integral_mu)))
#    tle.SetTextSize(0.1)
#    tle = tl.AddEntry(h_sh_el_dr,"Taus ({0})".format(int(integral_tau)))
#    tle.SetTextSize(0.1)
#    tl.Draw()
    canvas.SaveAs("{0}/{1}_dr_sh.png".format(outdir,sample))

    h_sh_el_ptr = utils.GetUnderOverHist(f,"h_sh_el_ptr") #.Clone(sample+"elptr")
    h_sh_mu_ptr = utils.GetUnderOverHist(f,"h_sh_mu_ptr",color=ROOT.kRed) #.Clone(sample+"muptr")
    h_sh_tau_ptr = utils.GetUnderOverHist(f,"h_sh_tau_ptr",color=ROOT.kGreen) #.Clone(sample+"tauptr")
    integral_el = h_sh_el_ptr.Integral()
    integral_mu = h_sh_mu_ptr.Integral()
    integral_tau = h_sh_tau_ptr.Integral()
    if (integral_el > 0):
        h_sh_el_ptr.Scale(1 / integral_el)
    if (integral_mu > 0):
        h_sh_mu_ptr.Scale(1 / integral_mu)
    if (integral_tau > 0):
        h_sh_tau_ptr.Scale(1 / integral_tau)
    h_sh_el_ptr.SetMaximum(1.2 * max([h.GetMaximum() for h in [h_sh_el_ptr,h_sh_mu_ptr,h_sh_tau_ptr]] ))
    h_sh_el_ptr.SetMinimum(0)
    h_sh_el_ptr.Draw()
    h_sh_mu_ptr.Draw("same")
    h_sh_tau_ptr.Draw("same")
    canvas.SaveAs("{0}/{1}_ptr_sh.png".format(outdir,sample))
