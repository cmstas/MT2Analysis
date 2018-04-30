# STCandLooper plots

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
canvas.SetCanvasSize(1200,800)
pads = []
pads.append(ROOT.TPad("1","1",0,0,0.67,1))
pads.append(ROOT.TPad("2","2",0.67,0,1,1))
pads[0].SetTicks(1,2)
pads[0].SetLeftMargin(0.14)
pads[0].SetTopMargin(0.12)
pads[0].SetRightMargin(0.12)
pads[0].Draw()
pads[1].Draw()
pads[0].cd()

ROOT.gStyle.SetLegendBorderSize(0)
ROOT.gStyle.SetLegendTextSize(0.05)
tl = ROOT.TLegend(0.0,0.2,1,0.8)
tl.SetMargin(0.1)

colorlist=[ROOT.kBlack,ROOT.kBlue,ROOT.kRed,ROOT.kGreen,ROOT.kCyan,ROOT.kMagenta]

indirs=argv[2:]

mode = argv[1]
doHtSlice = mode == "-bothslices" or mode == "-sliceht" or mode == "-all"
doLenSlice = mode == "-bothslices" or mode == "-slicelen" or mode == "-all"
doCompare = mode == "-compare" or mode == "-all"

if len(indirs) > len(colorlist):
    print len(indirs)
    print len(colorlist)
    raise Exception("Not enough colors")

if doCompare:
    taglist = [path[0:path.find("/")] + " " + path[path.find("pngs_")+5:] for path in indirs]
    print "Comparing: "
    for tag in taglist: print tag
    
flist = []

for indir in indirs:
    flist.append(ROOT.TFile.Open("{0}/factor_hists.root".format(indir)))

for i,f in enumerate(flist):
    if f.IsZombie():
        raise Exception("{0} does not have a factor_hists.root".format(argv[i]))

if doCompare:
    keylist = [k.GetName() for k in flist[0].GetListOfKeys()]
    for key in keylist:
        histlist = []
        tl.Clear()
        for i,f in enumerate(flist):
            histlist.append(utils.GetUnderOverHist(f,key,color=colorlist[i],under=False,over=False))
        histlist[0].SetMaximum(1.2 * max([h.GetMaximum() for h in histlist]))
        histlist[0].SetMinimum(0)
        tl.AddEntry(histlist[0],taglist[0])
        histlist[0].Draw()
        for i,h in enumerate(histlist[1:]):
            idx=i+1
            rescale = int(histlist[0].Integral() / h.Integral())
            h.Scale(rescale)
            tl.AddEntry(h,"{0} x {1}".format(taglist[idx],rescale))
            h.Draw("same")
        pads[1].cd()
        tl.Draw()
        pads[0].cd()
        canvas.SaveAs("{0}_compare.png".format(key))

if doHtSlice:
    htslice=["h_factor_st1","h_factor_st1L","h_factor_st1M","h_factor_st1H"]
    httaglist=["Incl","L","M","H"]
    for i,f in enumerate(flist):
        histlist = []
        tl.Clear()
        for k,key in enumerate(htslice):
            histlist.append(utils.GetUnderOverHist(f,key,color=colorlist[k],under=False,over=False))
        histlist[0].SetMaximum(1.2 * max([h.GetMaximum() for h in histlist]))
        histlist[0].SetMinimum(0)
        tl.AddEntry(histlist[0],httaglist[0])
        histlist[0].Draw()
        for j,h in enumerate(histlist[1:]):
            tl.AddEntry(h,httaglist[j+1])
            h.Draw("same")
        pads[1].cd()
        tl.Draw()
        pads[0].cd()
        canvas.SaveAs("{0}/htslice.png".format(indirs[i]))

if doLenSlice:
    lenslice=["h_factor_st1","h_factor_st1p","h_factor_st1s","h_factor_st1l"]
    lentaglist=["Inclusive","Pixel-Only","Tracker-Short","Tracker-Long"]
    for i,f in enumerate(flist):
        histlist = []
        tl.Clear()
        for k,key in enumerate(lenslice):
            h = utils.GetUnderOverHist(f,key,color=colorlist[k],under=False,over=False)
            if k > 0:
                rescale = int(h.Integral() / histlist[0].Integral())
                h.Scale(1.0/rescale)
                tl.AddEntry(h,"{0} x 1/{1}".format(lentaglist[k],rescale))
            else:
                tl.AddEntry(h,lentaglist[k])
            histlist.append(h)
        histlist[0].SetMaximum(1.2 * max([h.GetMaximum() for h in histlist]))
        histlist[0].SetMinimum(0)
        histlist[0].Draw()
        for h in histlist[1:]:
            h.Draw("same")
        pads[1].cd()
        tl.Draw()
        pads[0].cd()
        canvas.SaveAs("{0}/lenslice.png".format(indirs[i]))
