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
ROOT.gStyle.SetLegendTextSize(0.1)
canvas = ROOT.TCanvas()
canvas.SetCanvasSize(800,800)
canvas.SetTicks(2,1)
canvas.SetLeftMargin(0.14)
canvas.SetTopMargin(0.12)
canvas.SetRightMargin(0.12)
canvas.cd()

veto_file = ROOT.TFile("VetoHists.root")
h_veto_bar = veto_file.Get("h_VetoEtaPhi_bar")
h_veto_ecp = veto_file.Get("h_VetoEtaPhi_ecp")
h_veto_ecn = veto_file.Get("h_VetoEtaPhi_ecn")
h_veto_bar.Draw("colz")
canvas.SaveAs("h_veto_bar.pdf")
h_veto_ecp.Draw("colz")
canvas.SaveAs("h_veto_ecp.pdf")
h_veto_ecn.Draw("colz")
canvas.SaveAs("h_veto_ecn.pdf")
