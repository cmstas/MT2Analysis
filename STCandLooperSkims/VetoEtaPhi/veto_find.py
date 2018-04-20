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
ROOT.gStyle.SetLegendTextSize(0.1)
canvas = ROOT.TCanvas()
canvas.SetCanvasSize(800,800)
canvas.SetTicks(2,1)
canvas.SetLeftMargin(0.14)
canvas.SetTopMargin(0.12)
canvas.SetRightMargin(0.12)
canvas.cd()

verbose = False # Print more status messages

interpolate = False
rebin = 5


indir = "../vetoHists"

if len(argv) == 3:
    indir = argv[1]

print "Input is {0}/total.root, output is VetoEtaPhi.root, .pngs will be placed here in VetoEtaPhi".format(indir)

print "Processing input..."

f = ROOT.TFile("{0}/total.root".format(indir))
    
if f.IsZombie():
    print "{0} does not have a total file, please hadd".format(indir)
    print "Usage: python plotter.py <input dir = ../vetoHists> <output dir = ../vetoHists>"
    exit(1)

outfile = ROOT.TFile("VetoEtaPhi.root","RECREATE")
outfile.cd()

h_empty = f.Get("h_etaphi").Clone("h_all_empty")
h_empty.Scale(0)
h_empty.Rebin2D(rebin,rebin)

# Where we see a lot of short tracks, assume detector issue.
h_short = f.Get("h_st_etaphi").Clone("h_short")
h_short.Rebin2D(rebin,rebin)
# May need to normalize using frequency of tracks in each bin. Let's try both.
h_all = f.Get("h_etaphi").Clone("h_all")
h_all.Rebin2D(rebin,rebin)

h_all.Draw("colz")
canvas.SaveAs("AllTracks.png")
h_short.Draw("colz")
canvas.SaveAs("ShortTracks.png")

z_mean = h_short.GetMean(3)
z_sigma = h_short.GetStdDev(3)
upper_limit2 = z_mean + 2 * z_sigma
upper_limit3 = z_mean + 3 * z_sigma

xmax = h_empty.GetNbinsX()+1
ymax = h_empty.GetNbinsY()+1

h_dead2 = h_empty.Clone("h_dead2")
h_dead3 = h_empty.Clone("h_dead3")
h_dead2.SetTitle("Dead Cell Locations (2 Sigma);#eta;#phi")
h_dead3.SetTitle("Dead Cell Locations (3 Sigma);#eta;#phi")

for x in xrange(1,xmax):
    for y in xrange(1,ymax):
        bc = h_short.GetBinContent(x,y)
        if (bc > upper_limit2):
            h_dead2.SetBinContent(x,y,1)
            if (bc > upper_limit3):
                h_dead3.SetBinContent(x,y,1);

h_dead2.Draw("colz")        
canvas.SaveAs("dead2.png")
h_dead3.Draw("colz")        
canvas.SaveAs("dead3.png")

h_veto2 = h_empty.Clone("h_veto2")
h_veto3 = h_empty.Clone("h_veto3")
h_veto2.SetTitle("Interpolated Veto Regions (2 Sigma);#eta;#phi")
h_veto3.SetTitle("Interpolated Veto Regions (3 Sigma);#eta;#phi")

for x in xrange(1,xmax):
    for y in xrange(1,ymax):
        bc2 = h_dead2.GetBinContent(x,y)
        if (bc2 > 0):
            h_veto2.SetBinContent(x,y,1)
        elif interpolate:
            # Count "out-of-acceptance" as a veto bin for interpolation purposes (default to 1)
            bc2_xp1 = 1
            bc2_xm1 = 1
            bc2_yp1 = 1
            bc2_ym1 = 1
            if (x < xmax):
                bc2_xp1 = h_dead2.GetBinContent(x+1,y)
            if (x > 1):
                bc2_xm1 = h_dead2.GetBinContent(x-1,y)
            if (y < ymax):
                bc2_yp1 = h_dead2.GetBinContent(x,y+1)
            if (y > 1): 
                bc2_ym1 = h_dead2.GetBinContent(x,y-1)
            # if at least 2 adjacent bins are vetoed, veto this guy too
            if (bc2_xp1 + bc2_xm1 + bc2_yp1 + bc2_ym1 > 1):
                h_veto2.SetBinContent(x,y,1)

        bc3 = h_dead2.GetBinContent(x,y)        
        if (bc3 > 0):
            h_veto3.SetBinContent(x,y,1)
        elif interpolate:
            # Count "out-of-acceptance" as a veto bin for interpolation purposes (default to 1)
            bc3_xp1 = 1
            bc3_xm1 = 1
            bc3_yp1 = 1
            bc3_ym1 = 1
            if (x < xmax):
                bc3_xp1 = h_dead3.GetBinContent(x+1,y)
            if (x > 1):
                bc3_xm1 = h_dead3.GetBinContent(x-1,y)
            if (y < ymax):
                bc3_yp1 = h_dead3.GetBinContent(x,y+1)
            if (y > 1): 
                bc3_ym1 = h_dead3.GetBinContent(x,y-1)
            # if at least 2 adjacent bins are vetoed, veto this guy too
            if (bc3_xp1 + bc3_xm1 + bc3_yp1 + bc3_ym1 > 1):
                h_veto3.SetBinContent(x,y,1)
        
h_veto2.Draw("colz")
canvas.SaveAs("veto2.png")
h_veto3.Draw("colz")
canvas.SaveAs("veto3.png")

h_dead2.Write()
h_dead3.Write()
h_veto2.Write()
h_veto3.Write()

######

# Now normalize using h_all and do everything again
h_short.Divide(h_all)

z_mean = h_short.GetMean(3)
z_sigma = h_short.GetStdDev(3)
upper_limit2 = z_mean + 2 * z_sigma
upper_limit3 = z_mean + 3 * z_sigma

xmax = h_empty.GetNbinsX()+1
ymax = h_empty.GetNbinsY()+1

h_dead2 = h_empty.Clone("h_dead2_norm")
h_dead3 = h_empty.Clone("h_dead3_norm")
h_dead2.SetTitle("Dead Cell Locations (2 Sigma, Normalized);#eta;#phi")
h_dead3.SetTitle("Dead Cell Locations (3 Sigma, Normalized);#eta;#phi")

for x in xrange(1,xmax):
    for y in xrange(1,ymax):
        bc = h_short.GetBinContent(x,y)
        if (bc > upper_limit2):
            h_dead2.SetBinContent(x,y,1)
            if (bc > upper_limit3):
                h_dead3.SetBinContent(x,y,1);

h_dead2.Draw("colz")        
canvas.SaveAs("dead2_norm.png")
h_dead3.Draw("colz")        
canvas.SaveAs("dead3_norm.png")

h_veto2 = h_empty.Clone("h_veto2_norm")
h_veto3 = h_empty.Clone("h_veto3_norm")
h_veto2.SetTitle("Interpolated Veto Regions (2 Sigma, Normalized);#eta;#phi")
h_veto3.SetTitle("Interpolated Veto Regions (3 Sigma, Normalized);#eta;#phi")

for x in xrange(1,xmax):
    for y in xrange(1,ymax):
        bc2 = h_dead2.GetBinContent(x,y)
        if (bc2 > 0):
            h_veto2.SetBinContent(x,y,1)
        elif interpolate:
            # Count "out-of-acceptance" as a veto bin for interpolation purposes (default to 1)
            bc2_xp1 = 1
            bc2_xm1 = 1
            bc2_yp1 = 1
            bc2_ym1 = 1
            if (x < xmax):
                bc2_xp1 = h_dead2.GetBinContent(x+1,y)
            if (x > 1):
                bc2_xm1 = h_dead2.GetBinContent(x-1,y)
            if (y < ymax):
                bc2_yp1 = h_dead2.GetBinContent(x,y+1)
            if (y > 1): 
                bc2_ym1 = h_dead2.GetBinContent(x,y-1)
            # if at least 2 adjacent bins are vetoed, veto this guy too
            if (bc2_xp1 + bc2_xm1 + bc2_yp1 + bc2_ym1 > 1):
                h_veto2.SetBinContent(x,y,1)

        bc3 = h_dead2.GetBinContent(x,y)        
        if (bc3 > 0):
            h_veto3.SetBinContent(x,y,1)
        elif interpolate:
            # Count "out-of-acceptance" as a veto bin for interpolation purposes (default to 1)
            bc3_xp1 = 1
            bc3_xm1 = 1
            bc3_yp1 = 1
            bc3_ym1 = 1
            if (x < xmax):
                bc3_xp1 = h_dead3.GetBinContent(x+1,y)
            if (x > 1):
                bc3_xm1 = h_dead3.GetBinContent(x-1,y)
            if (y < ymax):
                bc3_yp1 = h_dead3.GetBinContent(x,y+1)
            if (y > 1): 
                bc3_ym1 = h_dead3.GetBinContent(x,y-1)
            # if at least 2 adjacent bins are vetoed, veto this guy too
            if (bc3_xp1 + bc3_xm1 + bc3_yp1 + bc3_ym1 > 1):
                h_veto3.SetBinContent(x,y,1)
        
h_veto2.Draw("colz")
canvas.SaveAs("veto2_norm.png")
h_veto3.Draw("colz")
canvas.SaveAs("veto3_norm.png")

h_dead2.Write()
h_dead3.Write()
h_veto2.Write()
h_veto3.Write()
outfile.Close()
