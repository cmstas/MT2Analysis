# plots comparing background and signal STs

import ROOT
from math import sqrt
import re
import sys
import os
from os.path import isfile,join
import subprocess
import ppmUtils as utils

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
#ROOT.gStyle.SetLegendTextSize(0.01)
ROOT.gStyle.SetPaintTextFormat("#3.3g")
ROOT.gStyle.SetLegendBorderSize(0)
simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(800,600)
simplecanvas.SetTicks(1,1)
simplecanvas.SetLeftMargin(0.14)
simplecanvas.SetRightMargin(0.1)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetBottomMargin(0.12)

tl=ROOT.TLegend(0.6,0.75,0.8,0.85)

ROOT.TH1.SetDefaultSumw2()

if len(sys.argv) < 2: 
    print "Which tag?"
    exit(1)

os.system("mkdir -p pdfs_SvsBG")

tag = sys.argv[1]

#fbg16 = ROOT.TFile.Open("output_unmerged/2016_{}/ttsl.root".format(tag))
fbg17 = ROOT.TFile.Open("output_unmerged/2017_{}/ttsl.root".format(tag))

filepath = "output_unmerged/2017_{0}/signal/".format(tag)
sigfiles = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
for s in sigfiles:
    if s.find("1700-1600") >= 0: sigfiles.remove(s)
sigfiles.sort()

signamelist = [filename[filename.rfind("/")+1:filename.rfind(".root")].replace("-","_") for filename in sigfiles]

simplecanvas.SetLogy()

for reg in ["SR","VR"]:
    print reg

    hbg_nlayer = utils.GetUnderOverHist(fbg17,"h_nlayer_{}_ST_All".format(reg),"N_{Layer} for Signal and Background STs;N_{Layer};Fraction",ROOT.kGreen,norm=True)
    hbg_pt = utils.GetUnderOverHist(fbg17,"h_pt_{}_ST_All".format(reg),"p_{T} for Signal and Background STs;p_{T} (GeV);Fraction",ROOT.kGreen,norm=True)
    hbg_mt2 = utils.GetUnderOverHist(fbg17,"h_mt2_{}_ST_All".format(reg),"M_{T2} for Signal and Background STs;M_{T2} (GeV);Fraction",ROOT.kGreen,norm=True)
    hbg_met = utils.GetUnderOverHist(fbg17,"h_met_{}_ST_All".format(reg),"MET for Signal and Background STs;MET (GeV);Fraction",ROOT.kGreen,norm=True)
    hbg_ht = utils.GetUnderOverHist(fbg17,"h_ht_{}_ST_All".format(reg),"H_{T} for Signal and Background STs;H_{T} (GeV);Fraction",ROOT.kGreen,norm=True)
    hbg_nj = utils.GetUnderOverHist(fbg17,"h_nj_{}_ST_All".format(reg),"N_{Jet} for Signal and Background STs;N_{Jet};Fraction",ROOT.kGreen,norm=True)
    hbg_mt = utils.GetUnderOverHist(fbg17,"h_mt_{}_ST_All".format(reg),"N_{Jet} for Signal and Background STs;N_{Jet};Fraction",ROOT.kGreen,norm=True)
    hbg_reliso = utils.GetUnderOverHist(fbg17,"h_reliso_{}_ST_All_NM1".format(reg),"N_{Jet} for Signal and Background STs;N_{Jet};Fraction",ROOT.kGreen,norm=True)

    for sigfilename in sigfiles:
        fsig = ROOT.TFile.Open(sigfilename)
        sigtag = sigfilename[sigfilename.rfind("fastsim_")+8:sigfilename.rfind(".")].replace("-","_")
        os.system("mkdir -p pdfs_SvsBG/{}".format(sigtag))

        # Nlayer
        hsig = utils.GetUnderOverHist(fsig,"h_nlayer_{}_ST_All".format(reg),"N_{Layer} for Signal and Background STs;N_{Layer};Fraction",ROOT.kBlack,norm=True)
        tl.Clear()
        tl.AddEntry(hsig,"Matched Signal")
        tl.AddEntry(hbg_nlayer,"1-lep t#bar{t}")
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_nlayer.GetMinimum()]),1e-3]))
        hsig.Draw()
        hbg_nlayer.Draw("same")
        tl.Draw()
        simplecanvas.SaveAs("pdfs_SvsBG/{}/nlayer_{}.pdf".format(sigtag,reg))

        hsig = utils.GetUnderOverHist(fsig,"h_pt_{}_ST_All".format(reg),"p_{T} for Signal and Background STs;p_{T} (GeV);Fraction",ROOT.kBlack,norm=True)
        tl.Clear()
        tl.AddEntry(hsig,"Matched Signal")
        tl.AddEntry(hbg_pt,"1-lep t#bar{t}")
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_pt.GetMinimum()]),1e-3]))
        hsig.Draw()
        hbg_pt.Draw("same")
        tl.Draw()
        simplecanvas.SaveAs("pdfs_SvsBG/{}/pt_{}.pdf".format(sigtag,reg))

        hsig = utils.GetUnderOverHist(fsig,"h_mt2_{}_ST_All".format(reg),"M_{T2} for Signal and Background STs;M_{T2} (GeV);Fraction",ROOT.kBlack,norm=True)
        tl.Clear()
        tl.AddEntry(hsig,"Matched Signal")
        tl.AddEntry(hbg_mt2,"1-lep t#bar{t}")
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_mt2.GetMinimum()]),1e-3]))
        hsig.Draw()
        hbg_mt2.Draw("same")
        tl.Draw()
        simplecanvas.SaveAs("pdfs_SvsBG/{}/mt2_{}.pdf".format(sigtag,reg))

        hsig = utils.GetUnderOverHist(fsig,"h_met_{}_ST_All".format(reg),"MET for Signal and Background STs;MET (GeV);Fraction",ROOT.kBlack,norm=True)
        tl.Clear()
        tl.AddEntry(hsig,"Matched Signal")
        tl.AddEntry(hbg_met,"1-lep t#bar{t}")
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_met.GetMinimum()]),1e-3]))
        hsig.Draw()
        hbg_met.Draw("same")
        tl.Draw()
        simplecanvas.SaveAs("pdfs_SvsBG/{}/met_{}.pdf".format(sigtag,reg))

        hsig = utils.GetUnderOverHist(fsig,"h_ht_{}_ST_All".format(reg),"H_{T} for Signal and Background STs;H_{T} (GeV);Fraction",ROOT.kBlack,norm=True)
        tl.Clear()
        tl.AddEntry(hsig,"Matched Signal")
        tl.AddEntry(hbg_ht,"1-lep t#bar{t}")
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_ht.GetMinimum()]),1e-3]))
        hsig.Draw()
        hbg_ht.Draw("same")
        tl.Draw()
        simplecanvas.SaveAs("pdfs_SvsBG/{}/ht_{}.pdf".format(sigtag,reg))

        hsig = utils.GetUnderOverHist(fsig,"h_nj_{}_ST_All".format(reg),"N_{Jet} for Signal and Background STs;N_{Jet};Fraction",ROOT.kBlack,norm=True)
        tl.Clear()
        tl.AddEntry(hsig,"Matched Signal")
        tl.AddEntry(hbg_nj,"1-lep t#bar{t}")
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_nj.GetMinimum()]),1e-3]))
        hsig.Draw()
        hbg_nj.Draw("same")
        tl.Draw()
        simplecanvas.SaveAs("pdfs_SvsBG/{}/nj_{}.pdf".format(sigtag,reg))

        hsig = utils.GetUnderOverHist(fsig,"h_mt_{}_ST_All".format(reg),"M_{T} for Signal and Background STs;M_{T}(Track,MET);Fraction",ROOT.kBlack,norm=True)
        tl.Clear()
        tl.AddEntry(hsig,"Matched Signal")
        tl.AddEntry(hbg_mt,"1-lep t#bar{t}")
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_mt.GetMinimum()]),1e-3]))
        hsig.Draw()
        hbg_mt.Draw("same")
        tl.Draw()
        simplecanvas.SaveAs("pdfs_SvsBG/{}/mt_{}.pdf".format(sigtag,reg))

        hsig = utils.GetUnderOverHist(fsig,"h_reliso_{}_ST_All_NM1".format(reg),"RelIso for Signal and Background STs [N-1];RelIso;Fraction",ROOT.kBlack,norm=True)
        tl.Clear()
        tl.AddEntry(hsig,"Matched Signal")
        tl.AddEntry(hbg_reliso,"1-lep t#bar{t}")
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_reliso.GetMinimum()]),1e-3]))
        hsig.Draw()
        hbg_reliso.Draw("same")
        tl.Draw()
        simplecanvas.SaveAs("pdfs_SvsBG/{}/reliso_{}.pdf".format(sigtag,reg))

        fsig.Close()

    del hbg_nlayer,hbg_pt,hbg_mt2,hbg_met,hbg_ht,hbg_nj,hbg_mt,hbg_reliso

print "Done"
