# Produce plots comparing only T5qqqq to itself

import ROOT
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
pads[0].SetTicks(2,2)
pads[0].SetLeftMargin(0.14)
pads[0].SetTopMargin(0.12)
pads[0].SetRightMargin(0.12)
pads[0].Draw()
pads[1].Draw()
pads[0].cd()

ROOT.gStyle.SetLegendBorderSize(0)
tl = ROOT.TLegend(0.0,0.2,1,0.8)
tl.SetMargin(0.1)

verbose = False # Print more status messages
#doScan = False # Not implemented

if len(argv) < 3:
    print "Usage: python plotter.py <indir> <outdir> <hadd ttsl>"
    exit(1)
indir = argv[1]
outdir = argv[2]
if (not os.path.exists(outdir)): os.mkdir(outdir)
if len(argv) > 3: 
    subprocess.call(["hadd","-f","{0}/ttsl.root".format(indir),"{0}/ttsl_fromT.root".format(indir),"{0}/ttsl_fromTbar.root".format(indir)])

varsuffixes = ["pt","dz","dxy","iso","niso","chiso","chreliso","nreliso","reliso"]
trklensuffixes = ["","_p","_s","_l","_li"]
histnames = ["h_" + varsuffix + trklensuffix for varsuffix in varsuffixes for trklensuffix in trklensuffixes]
if (verbose): print histnames
samples = ["ttsl","ttdl","DY","Wjets","qcd300to500","qcd500to700","qcd700to1000","qcd1000to1500","qcd1500to2000","qcd2000toInf","zinv100to200","zinv200toInf"]
colors = [ROOT.kGreen,ROOT.kGreen+2,ROOT.kCyan,ROOT.kBlue,ROOT.kRed-9,ROOT.kRed-7,ROOT.kRed,ROOT.kRed+2,ROOT.kRed+3,ROOT.kRed-1,ROOT.kMagenta,ROOT.kMagenta+2]
if (len(samples) > len(colors)):
    print str(len(samples)) + " samples and only " + str(len(colors)) + " colors"
    exit(1)
sample_colors = dict(zip(samples,colors))
if (verbose):
    print sample_colors

filedict = {}

print "Collecting files"

f_ttsl = ROOT.TFile("{0}/ttsl.root".format(indir))
f_ttdl = ROOT.TFile("{0}/ttdl.root".format(indir))
f_dy = ROOT.TFile("{0}/dyjetstoll_incl_ext1.root".format(indir))
f_wjets = ROOT.TFile("{0}/wjets_incl.root".format(indir))
f_qcd300to500 = ROOT.TFile("{0}/qcd_ht300to500.root".format(indir))
f_qcd500to700 = ROOT.TFile("{0}/qcd_ht500to700.root".format(indir))
f_qcd700to1000 = ROOT.TFile("{0}/qcd_ht700to1000.root".format(indir))
f_qcd1000to1500 = ROOT.TFile("{0}/qcd_ht1000to1500.root".format(indir))
f_qcd1500to2000 = ROOT.TFile("{0}/qcd_ht1500to2000_ext1.root".format(indir))
f_qcd2000toInf = ROOT.TFile("{0}/qcd_ht2000toInf.root".format(indir))
f_zpt100to200 = ROOT.TFile("{0}/zinv_zpt100to200.root".format(indir))
f_zpt200toInf = ROOT.TFile("{0}/zinv_zpt200toInf.root".format(indir))
    
if f_ttsl.IsZombie():
    print "{0} does not have a ttsl file".format(indir)
    print "Usage: python plotter.py <input dir> <output dir>"
    exit(1)
if f_ttdl.IsZombie():
    print "{0} does not have a ttdl file".format(indir)
    print "Usage: python plotter.py <input dir> <output dir>"
    exit(1)
if f_qcd300to500.IsZombie():
    print "{0} does not have a qcd_ht300to500 file".format(indir)
    print "Usage: python plotter.py <input dir> <output dir>"
    exit(1)
if f_qcd500to700.IsZombie():
    print "{0} does not have a qcd_ht500to700 file".format(indir)
    print "Usage: python plotter.py <input dir> <output dir>"
    exit(1)
if f_qcd700to1000.IsZombie():
    print "{0} does not have a qcd_ht700to1000 file".format(indir)
    print "Usage: python plotter.py <input dir> <output dir>"
    exit(1)
if f_qcd1000to1500.IsZombie():
    print "{0} does not have a qcd_ht1000to1500 file".format(indir)
    print "Usage: python plotter.py <input dir> <output dir>"
    exit(1)
if f_qcd1500to2000.IsZombie():
    print "{0} does not have a qcd_ht1500to2000_ext1 file".format(indir)
    print "Usage: python plotter.py <input dir> <output dir>"
    exit(1)
if f_qcd2000toInf.IsZombie():
    print "{0} does not have a qcd_ht2000toInf file".format(indir)
    print "Usage: python plotter.py <input dir> <output dir>"
    exit(1)
if f_zpt100to200.IsZombie():
    print "{0} does not have a zinv_zpt100to200 file".format(indir)
    print "Usage: python plotter.py <input dir> <output dir>"
    exit(1)
if f_zpt200toInf.IsZombie():
    print "{0} does not have a zinv_zpt200toInf file".format(indir)
    print "Usage: python plotter.py <input dir> <output dir>"
    exit(1)
if f_wjets.IsZombie():
    print "{0} does not have a wjets_incl file".format(indir)
    print "Usage: python plotter.py <input dir> <output dir>"
    exit(1)
if f_dy.IsZombie():
    print "{0} does not have a dyjetstoll_incl_ext1 file".format(indir)
    print "Usage: python plotter.py <input dir> <output dir>"
    exit(1)

files = [f_ttsl,f_ttdl,f_dy,f_wjets,f_qcd300to500,f_qcd500to700,f_qcd700to1000,f_qcd1000to1500,f_qcd1500to2000,f_qcd2000toInf,f_zpt100to200,f_zpt200toInf]
filedict = dict(zip(samples,files))

#outfile = ROOT.TFile.Open("{0}/output.root".format(outdir),"RECREATE")

print "Looping over histograms"


pads[0].SetLogy(True)
for histname in histnames:
    first = True    
    histlist = []
    tl.Clear()
    for sample in samples:
        hist = utils.GetUnderOverHist(filedict[sample],histname,color=sample_colors[sample])
        norm = hist.Integral()
        if (norm > 0): hist.Scale(1 / norm)
        hist.SetLineWidth(3)
        histlist.append(hist)
        tl.AddEntry(hist,sample)
    histlist[0].SetMaximum(1.5 * max([h.GetMaximum() for h in histlist]))
    histlist[0].SetMinimum(max(0.5 * min([h.GetMinimum() for h in histlist]),0.0001))
    histlist[0].Draw("E")
    for hist in histlist[1:]:
        hist.Draw("sameE")
    pads[1].cd()
    tl.Draw()
    pads[0].cd()
    canvas.SaveAs("{0}/{1}.png".format(outdir,histname))
    
#outfile.Close()
