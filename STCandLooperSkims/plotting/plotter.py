# STCandLooper plots

import ROOT
from math import sqrt
import re
from sys import argv,exit
import os
import subprocess
import ppmUtils as utils
import NiftyHists as nh

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
#if len(argv) > 3: 
#    subprocess.call(["hadd","-f","{0}/ttsl.root".format(indir),"{0}/ttsl_fromT.root".format(indir),"{0}/ttsl_fromTbar.root".format(indir)])

cats = ["st","stc"]
htsplits = ["I","L","M","H"]
lensplits = ["i","p","s","l"]
# observables connected to individual tracks
observables = ["el_genDR","el_ptr","mu_genDR","mu_ptr","tauel_genDR","tauel_ptr","taumu_genDR","taumu_ptr","tau1_genDR","tau1_ptr","tau3_genDR","tau3_ptr",
"el_recoDR","mu_recoDR","eta","pt","bardz","ecdz","bardxy","ecdxy","iso","niso","chiso","chreliso","nreliso","reliso","pterr"]

# Event level observables not connected to individual tracks
othernames = ["nem","chem","em"]

vardescs = ["Gen e #DeltaR",
"p_{T}^{Gen e} / p_{T}^{trk} - 1",
"Gen #mj #DeltaR",
"p_{T}^{Gen #mj} / p_{T}^{trk} - 1",
"Gen #tau_{e} #DeltaR",
"p_{T}^{Gen #tau_{e}} / p_{T}^{trk} - 1",
"Gen #tau_{#mu} #DeltaR",
"p_{T}^{Gen #tau_{#mu}} / p_{T}^{trk} - 1",
"Gen #tau_{1} #DeltaR",
"p_{T}^{Gen #tau_{1}} / p_{T}^{trk} - 1",
"Gen #tau_{3} #DeltaR",
"p_{T}^{Gen #tau_{3}} / p_{T}^{trk} - 1",
"Reco e #DeltaR",
"Reco #mu #DeltaR",
"Track |#eta|",
"Track p_{T}",
"Barrel dz",
"Endcap dz",
"Barrel dxy",
"Endcap dxy",
"Abs Iso (0.3)",
"Neut Iso (0.05)",
"Ch Iso (0.05)",
"ChRelIso (0.05)",
"NeutRelIso (0.05)",
"RelIso (0.3)",
"Track p_{T}^{Err}/p_{T}^2"]

vartags = dict(zip(observables,vardescs))

lendescs = ["Incl","Pix-Only","<7 Layers","7+ Layers"]
lentags=dict(zip(lensplits,lendescs))

htdescs=["Incl","100 < H_{T} <175 GeV","175 < H_{T} < 250 GeV","H_{T} > 250 GeV"]
httags=dict(zip(htsplits,htdescs))

catdescs=["STs","STCs"]
cattags=dict(zip(cats,catdescs))

samples=["ttsl","ttdl","qcd300to500","qcd500to700","qcd700to1000","qcd1000to1500","qcd1500to2000","qcd2000toInf","zpt100to200","zpt200toInf"]
colors = [ROOT.kGreen,ROOT.kGreen+2,ROOT.kCyan,ROOT.kBlue,ROOT.kRed-9,ROOT.kRed-7,ROOT.kRed,ROOT.kRed+2,ROOT.kRed+3,ROOT.kRed-1,ROOT.kMagenta,ROOT.kMagenta+2]
if (len(samples) > len(colors)):
    print str(len(samples)) + " samples and only " + str(len(colors)) + " colors"
    exit(1)
sample_colors = dict(zip(samples,colors))
len_colors=dict(zip(lensplits,colors))
ht_colors=dict(zip(lensplits,colors))
cat_colors=dict(zip(cats,colors))

for sample in samples:
    subdir = "{0}/{1}".format(outdir,sample)
    if (not os.path.exists(subdir)): os.mkdir(subdir)

filedict = {}

print "Collecting files"

f_ttsl = ROOT.TFile("{0}/TTSL_FromTbar.root".format(indir))
f_ttdl = ROOT.TFile("{0}/TTDL.root".format(indir))
#f_dy = ROOT.TFile("{0}/DY.root".format(indir))
#f_wjets = ROOT.TFile("{0}/WJETS.root".format(indir))
f_qcd300to500 = ROOT.TFile("{0}/QCD_HT300to500.root".format(indir))
f_qcd500to700 = ROOT.TFile("{0}/QCD_HT500to700.root".format(indir))
f_qcd700to1000 = ROOT.TFile("{0}/QCD_HT700to1000.root".format(indir))
f_qcd1000to1500 = ROOT.TFile("{0}/QCD_HT1000to1500.root".format(indir))
f_qcd1500to2000 = ROOT.TFile("{0}/QCD_HT1500to2000.root".format(indir))
f_qcd2000toInf = ROOT.TFile("{0}/QCD_HT2000toInf.root".format(indir))
f_zpt100to200 = ROOT.TFile("{0}/Zinv_ZPT100to200.root".format(indir))
f_zpt200toInf = ROOT.TFile("{0}/Zinv_ZPT200toInf.root".format(indir))
    
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
#if f_wjets.IsZombie():
#    print "{0} does not have a wjets_incl file".format(indir)
#    print "Usage: python plotter.py <input dir> <output dir>"
#    exit(1)
#if f_dy.IsZombie():
#    print "{0} does not have a dyjetstoll_incl_ext1 file".format(indir)
#    print "Usage: python plotter.py <input dir> <output dir>"
#    exit(1)

#files = [f_ttsl,f_ttdl,f_dy,f_wjets,f_qcd300to500,f_qcd500to700,f_qcd700to1000,f_qcd1000to1500,f_qcd1500to2000,f_qcd2000toInf,f_zpt100to200,f_zpt200toInf]
files = [f_ttsl,f_ttdl,f_qcd300to500,f_qcd500to700,f_qcd700to1000,f_qcd1000to1500,f_qcd1500to2000,f_qcd2000toInf,f_zpt100to200,f_zpt200toInf]
filedict = dict(zip(samples,files))

incllist = [f.Get("h_inclusive").GetBinContent(1) for f in files]
incl_counts = dict(zip(samples,incllist))

print "Looping over histograms"

print "By Sample"

for cat in cats:
    for htsplit in htsplits:
        for lensplit in lensplits:
            for var in observables:
                histname = "h_{0}_{1}_{2}_{3}".format(cat,htsplit,lensplit,var) 
                fulltag = "{0} {1} N-1 dist for {2} {3}".format(cattags[cat],vartags[var],httags[htsplit],lentags[lensplit])
                histlist = []
                for sample in samples:
                    histlist.append(utils.GetUnderOverHist(filedict[sample],histname,color=sample_colors[sample]))
                nh.OverlayScaledToMax(histlist,tl,samples)
                pads[1].cd()
                tl.Draw()
                pads[0].cd()
                print histname
                canvas.SaveAs("{0}/{1}.png".format(outdir,histname))

print "By HT"

for sample in samples:
    for cat in cats:
        for lensplit in lensplits:
            for var in observables:
                histlist = []
                histtags = []
                for htsplit in htsplits:
                    histname = "h_{0}_{1}_{2}_{3}".format(cat,htsplit,lensplit,var) 
                    histtags.append(fulltag = "{0} {1} N-1 dists by H_{T} for {2}, {3}".format(sample,vartags[var],cattags[cat],lentags[lensplit]))
                    histlist.append(utils.GetUnderOverHist(filedict[sample],histname,color=ht_colors[htsplit]))
                nh.OverlayScaledToMax(histlist,tl,htsplits)
                pads[1].cd()
                tl.Draw()
                pads[0].cd()
                canvas.SaveAs("{0}/{1}/{2}_{3}_{4}_hts.png".format(outdir,sample,var,cat,lensplit))

print "By Length"
for sample in samples:
    for cat in cats:
        for htsplit in htsplits:
            for var in observables:
                histlist = []
                histtags = []
                for lensplit in lensplit:
                    histname = "h_{0}_{1}_{2}_{3}".format(cat,htsplit,lensplit,var) 
                    histtags.append(fulltag = "{0} {1} N-1 dists by Length for {2}, {3}".format(sample,vartags[var],cattags[cat],httags[htsplit]))
                    histlist.append(utils.GetUnderOverHist(filedict[sample],histname,color=len_colors[lensplit]))
                nh.OverlayScaledToMax(histlist,tl,lensplits)
                pads[1].cd()
                tl.Draw()
                pads[0].cd()
                canvas.SaveAs("{0}/{1}/{2}_{3}_{4}_lengths.png".format(outdir,sample,var,cat,htsplit))

print "By Category"
for sample in samples:
    for lensplit in lensplits:
        for htsplit in htsplits:
            for var in observables:
                histlist = []
                histtags = []
                for cat in cats:
                    histname = "h_{0}_{1}_{2}_{3}".format(cat,htsplit,lensplit,var) 
                    histtags.append(fulltag = "{0} {1} N-1 dists by Category for {2}, {3}".format(sample,vartags[var],httags[htsplit],lentags[lensplit]))
                    histlist.append(utils.GetUnderOverHist(filedict[sample],histname,color=cat_colors[cat]))
                nh.OverlayScaledToMax(histlist,tl,lensplits)
                pads[1].cd()
                tl.Draw()
                pads[0].cd()
                canvas.SaveAs("{0}/{1}/{2}_{3}_{4}_cats.png".format(outdir,sample,var,htsplit,lensplit))

print "Other Variables"
for var in othernames:
    histlist = []
    histname = "h_"+var
    for sample in samples:
        histlist.append(utils.GetOverUnderHist(filedict[sample],histname,color=sample_colors[sample]))
    nh.OverlayScaledToMax(histlist,tl,lensplits)
    pads[1].cd()
    tl.Draw()
    pads[0].cd()
    canvas.SaveAs("{0}/{1}.png".format(outdir,var))

print "Reduction Factors"                    
h_factor_stc = ROOT.TH1F("h_factor_stc","Reduction Factor: N_{STC} > 0",len(samples),0,len(samples));
h_factor_st = ROOT.TH1F("h_factor_st","Reduction Factor: N_{ST} > 0 when N_{STC} > 0",len(samples),0,len(samples));
for index,sample in enumerate(samples):
    bin = index + 1
    incl_count = incl_counts[sample]
    h_st = (utils.GetUnderOverHist(filedict[sample],"h_st_I_i_N",color=sample_colors[sample])).Clone("h_st_tmp")
    h_stc = (utils.GetUnderOverHist(filedict[sample],"h_stc_I_i_N",color=sample_colors[sample])).Clone("h_stc_tmp")
    n_st_gt0 = h_st.Integral(2,-1)
    n_stc_gt0 = h_stc.Integral(2,-1)

    toSet = 0
    if (n_stc_gt0 > 0):        
        toSet = incl_count / n_stc_gt0
        dsq = n_stc_gt0 * n_stc_gt0
        h_factor_stc.SetBinError(bin, sqrt(incl_count * n_stc_gt0 + n_stc_gt0 * incl_count * incl_count) / dsq )
    h_factor_stc.SetBinContent(bin,toSet)
    h_factor_stc.GetXaxis().SetBinLabel(bin, "{0} ({1})".format( sample,str(int(n_stc_gt0)) ) )

    toSet1 = 0
    if (n_st_gt0 > 0):        
        toSet1 = n_stc_gt0 / n_st_gt0
        dsq = n_st_gt0 * n_st_gt0
        h_factor_st.SetBinError(bin, sqrt(n_stc_gt0 * n_st_gt0 + n_st_gt0 * n_stc_gt0 * n_stc_gt0) / dsq )
    h_factor_st.SetBinContent(bin,toSet1)
    h_factor_st.GetXaxis().SetBinLabel(bin, "{0} ({1})".format( sample,str(int(n_st_gt0)) ) )
    

canvas2 = ROOT.TCanvas()
canvas2.SetCanvasSize(800,800)
canvas2.cd()
canvas2.SetTicks(1,2)
canvas2.SetLeftMargin(0.14)
canvas2.SetTopMargin(0.12)
canvas2.SetRightMargin(0.16)
for hist in [h_factor_stc1,h_factor_st1]:
    hist.SetLineColor(ROOT.kRed)
    hist.SetLineWidth(3)
    hist.SetMinimum(0)
    hist.Write()
    hist.Draw("E")
    canvas2.SaveAs("{0}/{1}.png".format(outdir,hist.GetName()))

