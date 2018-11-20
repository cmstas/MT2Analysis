# STCImplementation background estimates and plots

import ROOT
from math import sqrt
import re
import sys
import os
import subprocess
import ppmUtils as utils

sys.path.append("../Nifty")

import NiftyUtils

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

verbose = False # Print more status messages

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
ROOT.gStyle.SetLegendTextSize(0.1)
ROOT.gStyle.SetPaintTextFormat("#3.3g")
ROOT.gStyle.SetLegendBorderSize(0)
simplecanvas = ROOT.TCanvas("plotter")
simplecanvas.SetCanvasSize(600,600)
simplecanvas.SetTicks(1,1)
simplecanvas.SetLeftMargin(0.16)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetRightMargin(0.16)

ROOT.gPad.SetPhi(-135)

if len(sys.argv) < 2: 
    print "Which file?"
    exit()

filename = sys.argv[1]
shortname = filename[filename.rfind("/")+1:filename.find(".")]

os.system("mkdir -p output")
os.system("mkdir -p pngs_dists/{0}".format(shortname))

tfile = ROOT.TFile.Open(filename,"READ")
names = tfile.GetKeyNames()

reliso_ewk = utils.GetUnderOverHist(tfile,"h_reliso_ewk_STC_L")
reliso_newk = utils.GetUnderOverHist(tfile,"h_reliso_newk_STC_L",color=ROOT.kRed)

iso_ewk = utils.GetUnderOverHist(tfile,"h_iso_ewk_STC_L")
iso_newk = utils.GetUnderOverHist(tfile,"h_iso_newk_STC_L",color=ROOT.kRed)

minireliso_ewk = utils.GetUnderOverHist(tfile,"h_minireliso_ewk_STC_L")
minireliso_newk = utils.GetUnderOverHist(tfile,"h_minireliso_newk_STC_L",color=ROOT.kRed)

reliso_ewk.Draw()
reliso_newk.Draw("same")
simplecanvas.SaveAs("pngs_dists/{0}/reliso.png".format(shortname))

iso_ewk.Draw()
iso_newk.Draw("same")
simplecanvas.SaveAs("pngs_dists/{0}/iso.png".format(shortname))

minireliso_ewk.Draw()
minireliso_newk.Draw("same")
simplecanvas.SaveAs("pngs_dists/{0}/minireliso.png".format(shortname))

tfile.Close()

print "Done"
