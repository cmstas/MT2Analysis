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

tl=ROOT.TLegend(0.18,0.18,0.8,0.35)

ROOT.TH1.SetDefaultSumw2()

region = "SR" if len(sys.argv) == 1 else sys.argv[1]

filepath = "output_unmerged/2016_Sigeff/"
sigfiles = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
signamelist = [f[f.rfind("/")+1:f.rfind(".root")] for f in sigfiles]

print sigfiles

def getScaleFactors(h16, h17, h18):
    factors = []
    h1718 = h17.Clone(h17.GetName()+"1718")
    h1718.Add(h18)
    for length in [1,4,5]:
        n16 = h16.GetBinContent(length,2)/35.9
        e16 = h16.GetBinError(length,2)/35.9
        n1718 = h1718.GetBinContent(length,2)/(41.5+59.8)
        e1718 = h1718.GetBinError(length,2)/(41.5+59.8)
        factors.append( (-1,-1) if n1718 == 0 or n16 == 0 else (n16/n1718, sqrt( (e1718/n1718)**2 + (e16/n16)**2 )))
    return factors

for sigindex,sigfilename in enumerate(sigfiles):
    sigfile16 = ROOT.TFile.Open(sigfilename)
    sigfile17 = ROOT.TFile.Open(sigfilename.replace("16","17"))
    sigfile18 = ROOT.TFile.Open(sigfilename.replace("16","18"))
    signame = signamelist[sigindex]
    tokens = signame.split("_")
    m1 = int(tokens[2])
    m2 = int(tokens[3])
    ctau = tokens[1]

    h16_hi = sigfile16.Get("h_fs{}_Baseline_hipt".format(region))
    h16_lo = sigfile16.Get("h_fs{}_Baseline_lowpt".format(region))

    h17_hi = sigfile17.Get("h_fs{}_Baseline_hipt".format(region))
    h17_lo = sigfile17.Get("h_fs{}_Baseline_lowpt".format(region))

    h18_hi = sigfile18.Get("h_fs{}_Baseline_hipt".format(region))
    h18_lo = sigfile18.Get("h_fs{}_Baseline_lowpt".format(region))

    print ""
    print signame
    print ""
    print "pt > 50 GeV"
    factors = getScaleFactors(h16_hi,h17_hi,h18_hi)
    print "P","%#.2g" % factors[0][0],"+/-","%#.2g" % factors[0][1]
    print "M","%#.2g" % factors[1][0],"+/-","%#.2g" % factors[1][1]
    print "L","%#.2g" % factors[2][0],"+/-","%#.2g" % factors[2][1]
    print "pt < 50 GeV"
    factors = getScaleFactors(h16_lo,h17_lo,h18_lo)
    print "P","%#.2g" % factors[0][0],"+/-","%#.2g" % factors[0][1]

    sigfile16.Close()
    sigfile17.Close()
    sigfile18.Close()
