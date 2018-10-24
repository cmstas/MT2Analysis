# STCImplementation background estimates and plots

import ROOT
from math import sqrt
import re
import sys
import os
import subprocess

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
os.system("mkdir -p pngs/{0}".format(shortname))

tfile = ROOT.TFile.Open(filename,"READ")
names = tfile.GetKeyNames()

outfile = ROOT.TFile.Open("output/Fshort_{}.root".format(shortname),"RECREATE")
outfile.cd()

# loop over every key and process histograms 
for rawname in names:
    name = rawname[1:] # Get rid of leading /
    # Fshort histograms
    if name.find("fs") > 0:        
        h_fs = tfile.Get(name)
        # Get fshort for inclusive, P, M, L
        for length in range(1,5):
            den = h_fs.GetBinContent(length,3)
            if den == 0:
                h_fs.SetBinContent(length,1,-1)
                h_fs.SetBinError(length,1,0)
                continue
            num = h_fs.GetBinContent(length,2)
            h_fs.SetBinContent(length,1,num / den)
            derr = h_fs.GetBinError(length, 3)
            nerr = h_fs.GetBinError(length, 2)
            numsq = num * num
            densq = den * den
            # binomial errors
            newerr = sqrt((derr*derr * numsq + nerr*nerr * densq) / (densq * densq))
            h_fs.SetBinError(length, 1, newerr)
        h_fs.SetMarkerSize(1.8)
        h_fs.Draw("text E")
        simplecanvas.SaveAs("pngs/{0}/{1}.png".format(shortname,name))
        h_fs.Write()
    # etaphi histograms
    elif name.find("etaphi") > 0:
        h_etaphi = tfile.Get(name)
        h_etaphi.SetTitle(h_etaphi.GetTitle()+";#eta;#phi")
        h_etaphi.Draw("colz")
        simplecanvas.SaveAs("pngs/{0}/{1}.png".format(shortname,name))
    # Look at transfer factor as a function of MT2
    elif name.find("mt2") > 0:
        h_mt2 = tfile.Get(name)
        h_mt2.SetLineWidth(3)
        h_mt2.SetTitle(h_mt2.GetTitle()+";M_{T2} (GeV);Count")
        h_mt2.Draw()
        simplecanvas.SaveAs("pngs/{0}/{1}.png".format(shortname,name))
    # Track mt x pt distribution
    elif name.find("mtpt") > 0:
        h_mtpt = tfile.Get(name)
        h_mtpt.GetXaxis().SetTitle("M_{T}(Track,MET) (GeV)")
        h_mtpt.GetXaxis().SetTitleOffset(2.2)
        h_mtpt.GetYaxis().SetTitle("p_{T} (GeV)")
        h_mtpt.GetYaxis().SetTitleOffset(2.2)
        h_mtpt.Draw("LEGO2Z")
        simplecanvas.SaveAs("pngs/{0}/{1}.png".format(shortname,name))
        # L tracks have electroweak contamination and need a more involved procedure using Rmtpt
        # The histograms we actually use are the removed lepton mtpt hists from electroweak MC.
        if name.find("rl") > 0:
            if name.find("4") > 0:
                njet = "_4"
            elif name.find("23") > 0:
                njet = "_23"
            else: njet = ""
            if name.find("FSR") > 0:
                reg = "FSR"
            elif name.find("VR") > 0:
                reg = "VR"
            else: 
                reg = "SR"
            newname="h_Rmtpt" + reg + njet
            h_Rmtpt = ROOT.TH2F(newname,"L Tracks Outside and Inside the M_{T} x p_{T} Veto Region",1,0,1,3,0,3)
            h_Rmtpt.GetYaxis().SetBinLabel(1,"R_{MtxPt}")
            h_Rmtpt.GetYaxis().SetBinLabel(2,"Vetoed")
            h_Rmtpt.GetYaxis().SetBinLabel(3,"Accepted")
            total = h_mtpt.Integral(1,-1, 1,-1)
            denom = h_mtpt.Integral(1,int(h_mtpt.GetXaxis().FindBin(100))-1, 1,int(h_mtpt.GetYaxis().FindBin(150))-1)
            num = total - denom
            h_Rmtpt.SetBinContent(1,3,num)
            h_Rmtpt.SetBinContent(1,2,denom)
            if denom > 0: h_Rmtpt.SetBinContent(1,1,num / denom)
            h_Rmtpt.SetMarkerSize(1.8)
            h_Rmtpt.Draw("text E")
            simplecanvas.SaveAs("pngs/{0}/{1}.png".format(shortname,newname))
            h_Rmtpt.Write()
        # We also separately extrapolate L STCs inside and outside the mtpt veto region. See ShortTrackLooper

mt2_STC_L=tfile.Get("h_mt2_STC_L")
mt2_ST_L=tfile.Get("h_mt2_ST_L")

mt2_ST_L.Divide(mt2_STC_L)
mt2_ST_L.SetTitle("f_{short} by M_{T2}, L Track;M_{T2} (GeV);f_{short}")
mt2_ST_L.GetXaxis().SetRange(1,14)
mt2_ST_L.SetMinimum(0)
mt2_ST_L.Draw()
simplecanvas.SaveAs("pngs/{0}/fs_by_mt2_L.png".format(shortname))

dphi_L = tfile.Get("h_dphiMet_ST_L")
dphi_M = tfile.Get("h_dphiMet_ST_M")
dphi_P = tfile.Get("h_dphiMet_ST_P")
dphi_L.Scale(1/dphi_L.Integral())
dphi_L.SetLineWidth(3)
dphi_L.SetLineColor(ROOT.kGreen)
dphi_M.Scale(1/dphi_M.Integral())
dphi_M.SetLineWidth(3)
dphi_M.SetLineColor(ROOT.kRed)
dphi_P.Scale(1/dphi_P.Integral())
dphi_P.SetLineWidth(3)
dphi_P.SetLineColor(ROOT.kBlue)
dphi_P.SetMaximum(1.2 * max( [h.GetMaximum() for h in [dphi_P,dphi_M,dphi_L]] ) )
dphi_P.SetMinimum(0)
dphi_P.Draw()
dphi_M.Draw("same")
dphi_L.Draw("same")
tl = ROOT.TLegend(0.65,0.85,0.65,0.85)
tl.AddEntry(dphi_P,"P")
tl.AddEntry(dphi_M,"M")
tl.AddEntry(dphi_L,"L")
tl.Draw()
simplecanvas.SaveAs("pngs/{0}/dphiMet.png".format(shortname))

outfile.Close()

tfile.Close()

print "Done"
