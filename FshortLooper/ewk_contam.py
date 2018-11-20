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
os.system("mkdir -p pngs_ewk/{0}".format(shortname))

tfile = ROOT.TFile.Open(filename,"READ")
names = tfile.GetKeyNames()

doPNGs = False

# loop over every key and process histograms 
if doPNGs:
    for rawname in names:
        name = rawname[1:] # Get rid of leading /
        if name.find("_mtpt") > 0:
            h_mtpt = tfile.Get(name)
            h_mtpt.GetXaxis().SetTitle("M_{T}(Track,MET) (GeV)")
            h_mtpt.GetXaxis().SetTitleOffset(2.2)
            h_mtpt.GetYaxis().SetTitle("p_{T} (GeV)")
            h_mtpt.GetYaxis().SetTitleOffset(2.2)
            h_mtpt.Draw("LEGO2Z")
            simplecanvas.SaveAs("pngs_ewk/{0}/{1}.png".format(shortname,name))
            # L tracks have electroweak contamination and need a more involved procedure using Rmtpt
            # The histograms we actually use are the removed lepton mtpt hists from electroweak MC.
            if name.find("4") > 0:
                njet = "_4"
                ht = name[name.find("4")+1:]
            elif name.find("23") > 0:
                njet = "_23"
                ht = name[name.find("23")+2:]
            else: 
                njet = ""
                ht = ""
            if name.find("FSR") > 0:
                reg = "FSR"
            elif name.find("VR") > 0:
                reg = "VR"
            else: 
                reg = "SR"
            if name.find("newk") > 0:
                ewk = "_newk"
            else:
                ewk = "_ewk"
            if name.find("zll") > 0:
                zll = "_zll"
            else:
                zll = ""
            if name.find("STC") > 0:
                cat = "_STC"
            else:
                cat = "_ST"
            newname="h" + zll + "_Rmtpt" + reg + njet + ht + ewk + cat
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
            simplecanvas.SaveAs("pngs_ewk/{0}/{1}.png".format(shortname,newname))
        elif name.find("nb") > 0:
            h_nb = tfile.Get(name)
            h_nb.SetTitle("N_{Tag};N_{Tag};Count")
            h_nb.Draw()
            simplecanvas.SaveAs("pngs_ewk/{0}/{1}.png".format(shortname,name))

def RejInt(th2):
    err = ROOT.Double(0)
    integral = th2.IntegralAndError(1,th2.GetXaxis().FindBin(100)-1,1,th2.GetYaxis().FindBin(150)-1,err)
    return integral,err

def AllInt(th2):
    err = ROOT.Double(0)
    integral = th2.IntegralAndError(1,-1,1,-1,err)
    return integral,err

def GetContamL(ewke, ewkm, tewke, tewkm, newk, newk1, newk3):
    rej_ewke,rej_ewke_err = RejInt(ewke)
    tot_ewke,tot_ewke_err = AllInt(ewke)
    acc_ewke = tot_ewke - rej_ewke
    acc_ewke_err = sqrt(tot_ewke_err*tot_ewke_err + rej_ewke_err*rej_ewke_err)

    rej_ewkm,rej_ewkm_err = RejInt(ewkm)
    tot_ewkm,tot_ewkm_err = AllInt(ewkm)
    acc_ewkm = tot_ewkm - rej_ewkm
    acc_ewkm_err = sqrt(tot_ewkm_err*tot_ewkm_err + rej_ewkm_err*rej_ewkm_err)

    rej_tewke,rej_tewke_err = RejInt(tewke)
    tot_tewke,tot_tewke_err = AllInt(tewke)
    acc_tewke = tot_tewke - rej_tewke
    acc_tewke_err = sqrt(tot_tewke_err*tot_tewke_err + rej_tewke_err*rej_tewke_err)

    rej_tewkm,rej_tewkm_err = RejInt(tewkm)
    tot_tewkm,tot_tewkm_err = AllInt(tewkm)
    acc_tewkm = tot_tewkm - rej_tewkm
    acc_tewkm_err = sqrt(tot_tewkm_err*tot_tewkm_err + rej_tewkm_err*rej_tewkm_err)

    rej_newk,rej_newk_err = RejInt(newk)
    tot_newk,tot_newk_err = AllInt(newk)
    acc_newk = tot_newk - rej_newk
    acc_newk_err = sqrt(tot_newk_err*tot_newk_err + rej_newk_err*rej_newk_err)

    rej_1newk,rej_1newk_err = RejInt(newk1)
    tot_1newk,tot_1newk_err = AllInt(newk1)
    acc_1newk = tot_1newk - rej_1newk
    acc_1newk_err = sqrt(tot_1newk_err*tot_1newk_err + rej_1newk_err*rej_1newk_err)

    rej_3newk,rej_3newk_err = RejInt(newk3)
    tot_3newk,tot_3newk_err = AllInt(newk3)
    acc_3newk = tot_3newk - rej_3newk
    acc_3newk_err = sqrt(tot_3newk_err*tot_3newk_err + rej_3newk_err*rej_3newk_err)

    rej_tot = rej_ewke + rej_ewkm + rej_tewke + rej_tewkm + rej_newk + rej_1newk + rej_3newk
    acc_tot = acc_ewke + acc_ewkm + acc_tewke + acc_tewkm + acc_newk + acc_1newk + acc_3newk
    rej_err = sqrt(rej_ewke_err**2 + rej_ewkm_err**2 + rej_tewke_err**2 + rej_tewkm_err**2 + rej_newk_err**2 + rej_1newk_err**2 + rej_3newk_err**2)
    acc_err = sqrt(acc_ewke_err**2 + acc_ewkm_err**2 + acc_tewke_err**2 + acc_tewkm_err**2 + acc_newk_err**2 + acc_1newk_err**2 + acc_3newk_err**2)

    rej_ewke_raterr = sqrt( (rej_ewke_err/rej_ewke)**2 + (rej_err/rej_tot)**2 ) if rej_ewke > 0.0 else 0.0
    rej_ewkm_raterr = sqrt( (rej_ewkm_err/rej_ewkm)**2 + (rej_err/rej_tot)**2 ) if rej_ewkm > 0.0 else 0.0
    rej_tewke_raterr = sqrt( (rej_tewke_err/rej_tewke)**2 + (rej_err/rej_tot)**2 ) if rej_tewke > 0.0 else 0.0
    rej_tewkm_raterr = sqrt( (rej_tewkm_err/rej_tewkm)**2 + (rej_err/rej_tot)**2 ) if rej_tewkm > 0.0 else 0.0
    rej_newk_raterr = sqrt( (rej_newk_err/rej_newk)**2 + (rej_err/rej_tot)**2 ) if rej_newk > 0.0 else 0.0
    rej_1newk_raterr = sqrt( (rej_1newk_err/rej_1newk)**2 + (rej_err/rej_tot)**2 ) if rej_1newk > 0.0 else 0.0
    rej_3newk_raterr = sqrt( (rej_3newk_err/rej_3newk)**2 + (rej_err/rej_tot)**2 ) if rej_3newk > 0.0 else 0.0
    acc_ewke_raterr = sqrt( (acc_ewke_err/acc_ewke)**2 + (acc_err/acc_tot)**2 ) if acc_ewke > 0.0 else 0.0
    acc_ewkm_raterr = sqrt( (acc_ewkm_err/acc_ewkm)**2 + (acc_err/acc_tot)**2 ) if acc_ewkm > 0.0 else 0.0
    acc_tewke_raterr = sqrt( (acc_tewke_err/acc_tewke)**2 + (acc_err/acc_tot)**2 ) if acc_tewke > 0.0 else 0.0
    acc_tewkm_raterr = sqrt( (acc_tewkm_err/acc_tewkm)**2 + (acc_err/acc_tot)**2 ) if acc_tewkm > 0.0 else 0.0
    acc_newk_raterr = sqrt( (acc_newk_err/acc_newk)**2 + (acc_err/acc_tot)**2 ) if acc_newk > 0.0 else 0.0
    acc_1newk_raterr = sqrt( (acc_1newk_err/acc_1newk)**2 + (acc_err/acc_tot)**2 ) if acc_1newk > 0.0 else 0.0
    acc_3newk_raterr = sqrt( (acc_3newk_err/acc_3newk)**2 + (acc_err/acc_tot)**2 ) if acc_3newk > 0.0 else 0.0

    if rej_tot > 0:
        print "Rej Boson e/mu / Tau e/mu / Tau 1/3 / Non-EWK: {0:.1f}%/{1:.1f}% / {2:.1f}%/{3:.1f} / {4:.1f}%/{5:.1f}% / {6:.1f} (+/- {7:.1f}%/{8:.1f}% / {9:.1f}/{10:.1f} / {11:.1f}/{12:.1f} / {13:.1f}%)".format(rej_ewke/rej_tot*100,rej_ewkm/rej_tot*100,rej_tewke/rej_tot*100,rej_tewkm/rej_tot*100,rej_1newk/rej_tot*100,rej_3newk/rej_tot*100,rej_newk/rej_tot*100,rej_ewke_raterr*100,rej_ewkm_raterr*100,rej_tewke_raterr*100,rej_tewkm_raterr*100,rej_1newk_raterr*100,rej_3newk_raterr*100,rej_newk_raterr*100)
    else:
        print "0 Total Rejected Counts"
    if acc_tot > 0:
        print "Acc Boson e/mu / Tau e/mu / Tau 1/3 / Non-EWK: {0:.1f}%/{1:.1f}% / {2:.1f}%/{3:.1f} / {4:.1f}%/{5:.1f}% / {6:.1f} (+/- {7:.1f}%/{8:.1f}% / {9:.1f}/{10:.1f} / {11:.1f}/{12:.1f} / {13:.1f}%)".format(acc_ewke/acc_tot*100,acc_ewkm/acc_tot*100,acc_tewke/acc_tot*100,acc_tewkm/acc_tot*100,acc_1newk/acc_tot*100,acc_3newk/acc_tot*100,acc_newk/acc_tot*100,acc_ewke_raterr*100,acc_ewkm_raterr*100,acc_tewke_raterr*100,acc_tewkm_raterr*100,acc_1newk_raterr*100,acc_3newk_raterr*100,acc_newk_raterr*100)
    else:
        print "0 Total Accepted Counts"

FSR_ewke_ST_L = tfile.Get("h_mtptFSR_ewke_ST_L")
FSR_ewke_STC_L = tfile.Get("h_mtptFSR_ewke_STC_L")
FSR_tewke_ST_L = tfile.Get("h_mtptFSR_tewke_ST_L")
FSR_tewke_STC_L = tfile.Get("h_mtptFSR_tewke_STC_L")
FSR_ewkm_ST_L = tfile.Get("h_mtptFSR_ewkm_ST_L")
FSR_ewkm_STC_L = tfile.Get("h_mtptFSR_ewkm_STC_L")
FSR_tewkm_ST_L = tfile.Get("h_mtptFSR_tewkm_ST_L")
FSR_tewkm_STC_L = tfile.Get("h_mtptFSR_tewkm_STC_L")
FSR_newk_ST_L = tfile.Get("h_mtptFSR_newk_ST_L")
FSR_newk_STC_L = tfile.Get("h_mtptFSR_newk_STC_L")
FSR_1newk_ST_L = tfile.Get("h_mtptFSR_1newk_ST_L")
FSR_1newk_STC_L = tfile.Get("h_mtptFSR_1newk_STC_L")
FSR_3newk_ST_L = tfile.Get("h_mtptFSR_3newk_ST_L")
FSR_3newk_STC_L = tfile.Get("h_mtptFSR_3newk_STC_L")

print "FSR ST L"
GetContamL(FSR_ewke_ST_L,FSR_ewkm_ST_L,FSR_tewke_ST_L,FSR_tewkm_ST_L,FSR_newk_ST_L,FSR_1newk_ST_L,FSR_3newk_ST_L)
print "FSR STC L"
GetContamL(FSR_ewke_STC_L,FSR_ewkm_STC_L,FSR_tewke_STC_L,FSR_tewkm_STC_L,FSR_newk_STC_L,FSR_1newk_STC_L,FSR_3newk_STC_L)

VR_ewke_ST_L = tfile.Get("h_mtptVR_ewke_ST_L")
VR_ewke_STC_L = tfile.Get("h_mtptVR_ewke_STC_L")
VR_tewke_ST_L = tfile.Get("h_mtptVR_tewke_ST_L")
VR_tewke_STC_L = tfile.Get("h_mtptVR_tewke_STC_L")
VR_ewkm_ST_L = tfile.Get("h_mtptVR_ewkm_ST_L")
VR_ewkm_STC_L = tfile.Get("h_mtptVR_ewkm_STC_L")
VR_tewkm_ST_L = tfile.Get("h_mtptVR_tewkm_ST_L")
VR_tewkm_STC_L = tfile.Get("h_mtptVR_tewkm_STC_L")
VR_newk_ST_L = tfile.Get("h_mtptVR_newk_ST_L")
VR_newk_STC_L = tfile.Get("h_mtptVR_newk_STC_L")
VR_1newk_ST_L = tfile.Get("h_mtptVR_1newk_ST_L")
VR_1newk_STC_L = tfile.Get("h_mtptVR_1newk_STC_L")
VR_3newk_ST_L = tfile.Get("h_mtptVR_3newk_ST_L")
VR_3newk_STC_L = tfile.Get("h_mtptVR_3newk_STC_L")

print "VR ST L"
GetContamL(VR_ewke_ST_L,VR_ewkm_ST_L,VR_tewke_ST_L,VR_tewkm_ST_L,VR_newk_ST_L,VR_1newk_ST_L,VR_3newk_ST_L)
print "VR STC L"
GetContamL(VR_ewke_STC_L,VR_ewkm_STC_L,VR_tewke_STC_L,VR_tewkm_STC_L,VR_newk_STC_L,VR_1newk_STC_L,VR_3newk_STC_L)

SR_ewke_ST_L = tfile.Get("h_mtptSR_ewke_ST_L")
SR_ewke_STC_L = tfile.Get("h_mtptSR_ewke_STC_L")
SR_tewke_ST_L = tfile.Get("h_mtptSR_tewke_ST_L")
SR_tewke_STC_L = tfile.Get("h_mtptSR_tewke_STC_L")
SR_ewkm_ST_L = tfile.Get("h_mtptSR_ewkm_ST_L")
SR_ewkm_STC_L = tfile.Get("h_mtptSR_ewkm_STC_L")
SR_tewkm_ST_L = tfile.Get("h_mtptSR_tewkm_ST_L")
SR_tewkm_STC_L = tfile.Get("h_mtptSR_tewkm_STC_L")
SR_newk_ST_L = tfile.Get("h_mtptSR_newk_ST_L")
SR_newk_STC_L = tfile.Get("h_mtptSR_newk_STC_L")
SR_1newk_ST_L = tfile.Get("h_mtptSR_1newk_ST_L")
SR_1newk_STC_L = tfile.Get("h_mtptSR_1newk_STC_L")
SR_3newk_ST_L = tfile.Get("h_mtptSR_3newk_ST_L")
SR_3newk_STC_L = tfile.Get("h_mtptSR_3newk_STC_L")

print "SR ST L"
GetContamL(SR_ewke_ST_L,SR_ewkm_ST_L,SR_tewke_ST_L,SR_tewkm_ST_L,SR_newk_ST_L,SR_1newk_ST_L,SR_3newk_ST_L)
print "SR STC L"
GetContamL(SR_ewke_STC_L,SR_ewkm_STC_L,SR_tewke_STC_L,SR_tewkm_STC_L,SR_newk_STC_L,SR_1newk_STC_L,SR_3newk_STC_L)


def GetContamM(ewke, ewkm, tewke, tewkm, newk, newk1, newk3):
    tot_ewke,tot_ewke_err = AllInt(ewke)
    tot_ewkm,tot_ewkm_err = AllInt(ewkm)

    tot_tewke,tot_tewke_err = AllInt(tewke)
    tot_tewkm,tot_tewkm_err = AllInt(tewkm)

    tot_newk,tot_newk_err = AllInt(newk)

    tot_1newk,tot_1newk_err = AllInt(newk1)
    tot_3newk,tot_3newk_err = AllInt(newk3)

    integral = tot_ewke + tot_ewkm + tot_tewke + tot_tewkm + tot_newk + tot_1newk + tot_3newk
    tot_err = sqrt(tot_ewke_err**2 + tot_ewkm_err**2 + tot_tewke_err**2 + tot_tewkm_err**2 + tot_newk_err**2 + tot_1newk_err**2 + tot_3newk_err**2)

    tot_ewke_raterr = sqrt( (tot_ewke_err / tot_ewke)**2 + (tot_err/integral)**2 ) if tot_ewke > 0.0 else 0.0
    tot_ewkm_raterr = sqrt( (tot_ewkm_err / tot_ewkm)**2 + (tot_err/integral)**2 ) if tot_ewkm > 0.0 else 0.0
    tot_tewke_raterr = sqrt( (tot_tewke_err / tot_tewke)**2 + (tot_err/integral)**2 ) if tot_tewke > 0.0 else 0.0
    tot_tewkm_raterr = sqrt( (tot_tewkm_err / tot_tewkm)**2 + (tot_err/integral)**2 ) if tot_tewkm > 0.0 else 0.0
    tot_newk_raterr = sqrt( (tot_newk_err / tot_newk)**2 + (tot_err/integral)**2 ) if tot_newk > 0.0 else 0.0
    tot_1newk_raterr = sqrt( (tot_1newk_err / tot_1newk)**2 + (tot_err/integral)**2 ) if tot_1newk > 0.0 else 0.0
    tot_3newk_raterr = sqrt( (tot_3newk_err / tot_3newk)**2 + (tot_err/integral)**2 ) if tot_3newk > 0.0 else 0.0

    if integral > 0.0:
        print "Boson e/mu / Tau e/mu / Tau 1/3 / Non-EWK: {0:.1f}%/{1:.1f}% / {2:.1f}%/{3:.1f}% / {4:.1f}%/{5:.1f}% / {6:.1f}% (+/- {7:.1f}%/{8:.1f}% / {9:.1f}%/{10:.1f}% / {11:.1f}%/{12:.1f}% / {13:.1f}%)".format(tot_ewke/integral*100,tot_ewkm/integral*100,tot_tewke/integral*100,tot_tewkm/integral*100,tot_1newk/integral*100,tot_3newk/integral*100,tot_newk/integral*100,tot_ewke_raterr*100,tot_ewkm_raterr*100,tot_tewke_raterr*100,tot_tewkm_raterr*100,tot_1newk_raterr*100,tot_3newk_raterr*100,tot_newk_raterr*100)
    else:
        print "0 Total Counts"

FSR_ewke_ST_M = tfile.Get("h_mtptFSR_ewke_ST_M")
FSR_ewke_STC_M = tfile.Get("h_mtptFSR_ewke_STC_M")
FSR_tewke_ST_M = tfile.Get("h_mtptFSR_tewke_ST_M")
FSR_tewke_STC_M = tfile.Get("h_mtptFSR_tewke_STC_M")
FSR_ewkm_ST_M = tfile.Get("h_mtptFSR_ewkm_ST_M")
FSR_ewkm_STC_M = tfile.Get("h_mtptFSR_ewkm_STC_M")
FSR_tewkm_ST_M = tfile.Get("h_mtptFSR_tewkm_ST_M")
FSR_tewkm_STC_M = tfile.Get("h_mtptFSR_tewkm_STC_M")
FSR_newk_ST_M = tfile.Get("h_mtptFSR_newk_ST_M")
FSR_newk_STC_M = tfile.Get("h_mtptFSR_newk_STC_M")
FSR_1newk_ST_M = tfile.Get("h_mtptFSR_1newk_ST_M")
FSR_1newk_STC_M = tfile.Get("h_mtptFSR_1newk_STC_M")
FSR_3newk_ST_M = tfile.Get("h_mtptFSR_3newk_ST_M")
FSR_3newk_STC_M = tfile.Get("h_mtptFSR_3newk_STC_M")

print "FSR ST M"
GetContamM(FSR_ewke_ST_M,FSR_ewkm_ST_M,FSR_tewke_ST_M,FSR_tewkm_ST_M,FSR_newk_ST_M,FSR_1newk_ST_M,FSR_3newk_ST_M)
print "FSR STC M"
GetContamM(FSR_ewke_STC_M,FSR_ewkm_STC_M,FSR_tewke_STC_M,FSR_tewkm_STC_M,FSR_newk_STC_M,FSR_1newk_STC_M,FSR_3newk_STC_M)

VR_ewke_ST_M = tfile.Get("h_mtptVR_ewke_ST_M")
VR_ewke_STC_M = tfile.Get("h_mtptVR_ewke_STC_M")
VR_tewke_ST_M = tfile.Get("h_mtptVR_tewke_ST_M")
VR_tewke_STC_M = tfile.Get("h_mtptVR_tewke_STC_M")
VR_ewkm_ST_M = tfile.Get("h_mtptVR_ewkm_ST_M")
VR_ewkm_STC_M = tfile.Get("h_mtptVR_ewkm_STC_M")
VR_tewkm_ST_M = tfile.Get("h_mtptVR_tewkm_ST_M")
VR_tewkm_STC_M = tfile.Get("h_mtptVR_tewkm_STC_M")
VR_newk_ST_M = tfile.Get("h_mtptVR_newk_ST_M")
VR_newk_STC_M = tfile.Get("h_mtptVR_newk_STC_M")
VR_1newk_ST_M = tfile.Get("h_mtptVR_1newk_ST_M")
VR_1newk_STC_M = tfile.Get("h_mtptVR_1newk_STC_M")
VR_3newk_ST_M = tfile.Get("h_mtptVR_3newk_ST_M")
VR_3newk_STC_M = tfile.Get("h_mtptVR_3newk_STC_M")

print "VR ST M"
GetContamM(VR_ewke_ST_M,VR_ewkm_ST_M,VR_tewke_ST_M,VR_tewkm_ST_M,VR_newk_ST_M,VR_1newk_ST_M,VR_3newk_ST_M)
print "VR STC M"
GetContamM(VR_ewke_STC_M,VR_ewkm_STC_M,VR_tewke_STC_M,VR_tewkm_STC_M,VR_newk_STC_M,VR_1newk_STC_M,VR_3newk_STC_M)

SR_ewke_ST_M = tfile.Get("h_mtptSR_ewke_ST_M")
SR_ewke_STC_M = tfile.Get("h_mtptSR_ewke_STC_M")
SR_tewke_ST_M = tfile.Get("h_mtptSR_tewke_ST_M")
SR_tewke_STC_M = tfile.Get("h_mtptSR_tewke_STC_M")
SR_ewkm_ST_M = tfile.Get("h_mtptSR_ewkm_ST_M")
SR_ewkm_STC_M = tfile.Get("h_mtptSR_ewkm_STC_M")
SR_tewkm_ST_M = tfile.Get("h_mtptSR_tewkm_ST_M")
SR_tewkm_STC_M = tfile.Get("h_mtptSR_tewkm_STC_M")
SR_newk_ST_M = tfile.Get("h_mtptSR_newk_ST_M")
SR_newk_STC_M = tfile.Get("h_mtptSR_newk_STC_M")
SR_1newk_ST_M = tfile.Get("h_mtptSR_1newk_ST_M")
SR_1newk_STC_M = tfile.Get("h_mtptSR_1newk_STC_M")
SR_3newk_ST_M = tfile.Get("h_mtptSR_3newk_ST_M")
SR_3newk_STC_M = tfile.Get("h_mtptSR_3newk_STC_M")

print "SR ST M"
GetContamM(SR_ewke_ST_M,SR_ewkm_ST_M,SR_tewke_ST_M,SR_tewkm_ST_M,SR_newk_ST_M,SR_1newk_ST_M,SR_3newk_ST_M)
print "SR STC M"
GetContamM(SR_ewke_STC_M,SR_ewkm_STC_M,SR_tewke_STC_M,SR_tewkm_STC_M,SR_newk_STC_M,SR_1newk_STC_M,SR_3newk_STC_M)

tfile.Close()

print "Done"
