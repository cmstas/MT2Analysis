# plots comparing background and signal STs

import ROOT
from math import sqrt
import re
import sys
import os
from os.path import isfile,join
import subprocess
import ppmUtils as utils
from array import array

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

suppressHorizontal = False

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
if suppressHorizontal: 
    ROOT.gStyle.SetErrorX(0)
    errOpt = "E1"
    legOpt = "pE"
else:
    errOpt = ""
    legOpt = "pLE"
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

tl=ROOT.TLegend(0.18,0.65,0.8,0.85)
#tl=ROOT.TLegend(0.30,0.55,0.8,0.80)

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

ttext = ROOT.TLatex()
ttext.SetNDC(1)
ttext.SetTextFont(42)
ttext.SetTextAlign(22)
ttext.SetTextSize(0.030)
ttext.SetTextAngle(90)

for reg in ["SR","VR"]:
    print reg

    hbg_nlayer = utils.GetUnderOverHist(fbg17,"h_nlayer_{}_ST_All".format(reg),"N_{Layer} for Signal and Background STs;N_{Layer};Fraction",ROOT.kRed,norm=True)
    hbg_pt = utils.GetUnderOverHist(fbg17,"h_pt_{}_ST_All".format(reg),"p_{T} for Signal and Background STs;p_{T} (GeV);Fraction",ROOT.kRed,norm=True)
    hbg_mt2 = utils.GetUnderOverHist(fbg17,"h_mt2_{}_ST_All".format(reg),"M_{T2} for Signal and Background STs;M_{T2} (GeV);Fraction",ROOT.kRed,norm=True)
    hbg_met = utils.GetUnderOverHist(fbg17,"h_met_{}_ST_All".format(reg),"MET for Signal and Background STs;MET (GeV);Fraction",ROOT.kRed,norm=True)
    hbg_ht = utils.GetUnderOverHist(fbg17,"h_ht_{}_ST_All".format(reg),"H_{T} for Signal and Background STs;H_{T} (GeV);Fraction",ROOT.kRed,norm=True)
    hbg_nj = utils.GetUnderOverHist(fbg17,"h_nj_{}_ST_All".format(reg),"N_{Jet} for Signal and Background STs;N_{Jet};Fraction",ROOT.kRed,norm=True)
    hbg_mt = utils.GetUnderOverHist(fbg17,"h_mt_{}_ST_All".format(reg),"N_{Jet} for Signal and Background STs;N_{Jet};Fraction",ROOT.kRed,norm=True)
    hbg_reliso_raw = utils.GetUnderOverHist(fbg17,"h_reliso_{}_ST_All_NM1".format(reg),"N_{Jet} for Signal and Background STs;N_{Jet};Fraction",ROOT.kRed,over=False,norm=False)

    for sigfilename in sigfiles:
        fsig = ROOT.TFile.Open(sigfilename)
        sigtag = sigfilename[sigfilename.rfind("fastsim_")+8:sigfilename.rfind(".")].replace("-","_")
        signame = sigtag.replace("_"," ").replace("90cm","c#tau_{0}(#tilde{#chi}_{1}^{#pm}) = 90 cm,").replace("1800","M_{#tilde{g}} = 1800 GeV, M_{#tilde{#chi}_{1}^{0}} = ") + " GeV"
        sigmass = signame[signame.find("M"):]
        sigctau = signame[:signame.find("M")]
        os.system("mkdir -p pdfs_SvsBG/{}".format(sigtag))

        # Nlayer
        hsig = utils.GetUnderOverHist(fsig,"h_nlayer_{}_ST_All".format(reg),"N_{Layer} for Signal and Background STs;N_{Layer};Fraction",ROOT.kBlack,norm=True)
        tl.Clear()
        tl.AddEntry(hsig,"#splitline{pp #rightarrow #tilde{g}#tilde{g}, #tilde{g} #rightarrow q#bar{q}'#tilde{#chi}_{1}^{#pm}, #tilde{#chi}_{1}^{#pm} #rightarrow #tilde{#chi}_{1}^{0}#pi^{#pm}, "+sigmass+"}{"+sigctau+"#DeltaR(track,#tilde{#chi}_{1}^{#pm}) < 0.01}")
        tl.AddEntry(hbg_nlayer,"SM Background")
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_nlayer.GetMinimum()]),1e-3]))
        hsig.SetTitle(";N_{Layer};Fraction of events")
        hsig.Draw()
        hbg_nlayer.Draw("same")
        tl.Draw()
#        utils.DrawCmsText(simplecanvas,"CMS Simulation Preliminary")
#        ttext.DrawLatex(1.0-simplecanvas.GetRightMargin()-0.01, 1.0-simplecanvas.GetTopMargin()+0.01, "(13 TeV)")
        simplecanvas.SaveAs("pdfs_SvsBG/{}/nlayer_{}.pdf".format(sigtag,reg))

        hsig = utils.GetUnderOverHist(fsig,"h_pt_{}_ST_All".format(reg),"p_{T} for Signal and Background STs;p_{T} (GeV);Fraction",ROOT.kBlack,norm=True)
        tl.Clear()
        tl.AddEntry(hsig,"#splitline{pp #rightarrow #tilde{g}#tilde{g}, #tilde{g} #rightarrow q#bar{q}'#tilde{#chi}_{1}^{#pm}, #tilde{#chi}_{1}^{#pm} #rightarrow #tilde{#chi}_{1}^{0}#pi^{#pm}, "+sigmass+"}{"+sigctau+"#DeltaR(track,#tilde{#chi}_{1}^{#pm}) < 0.01}")
        tl.AddEntry(hbg_pt,"SM Background")
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_pt.GetMinimum()]),1e-3]))
        hsig.SetTitle(";Track p_{T} (GeV);Fraction of events")
        hsig.Draw()
        hbg_pt.Draw("same")
        tl.Draw()
        utils.DrawCmsText(simplecanvas,"CMS Simulation Preliminary")
        simplecanvas.SaveAs("pdfs_SvsBG/{}/pt_{}.pdf".format(sigtag,reg))

        hsig = utils.GetUnderOverHist(fsig,"h_mt2_{}_ST_All".format(reg),"M_{T2} for Signal and Background STs;M_{T2} (GeV);Fraction",ROOT.kBlack,norm=True)
        tl.Clear()
        tl.AddEntry(hsig,"#splitline{pp #rightarrow #tilde{g}#tilde{g}, #tilde{g} #rightarrow q#bar{q}'#tilde{#chi}_{1}^{#pm}, #tilde{#chi}_{1}^{#pm} #rightarrow #tilde{#chi}_{1}^{0}#pi^{#pm}, "+sigmass+"}{"+sigctau+"#DeltaR(track,#tilde{#chi}_{1}^{#pm}) < 0.01}")
        tl.AddEntry(hbg_mt2,"SM Background")
        hsig.SetTitle(";M_{T2} (GeV);Fraction of events")
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_mt2.GetMinimum()]),1e-3]))
        hsig.Draw()
        hbg_mt2.Draw("same")
        tl.Draw()
        utils.DrawCmsText(simplecanvas,"CMS Simulation Preliminary")
        simplecanvas.SaveAs("pdfs_SvsBG/{}/mt2_{}.pdf".format(sigtag,reg))

        hsig = utils.GetUnderOverHist(fsig,"h_met_{}_ST_All".format(reg),"MET for Signal and Background STs;MET (GeV);Fraction",ROOT.kBlack,norm=True)
        tl.Clear()
        tl.AddEntry(hsig,"#splitline{pp #rightarrow #tilde{g}#tilde{g}, #tilde{g} #rightarrow q#bar{q}'#tilde{#chi}_{1}^{#pm}, #tilde{#chi}_{1}^{#pm} #rightarrow #tilde{#chi}_{1}^{0}#pi^{#pm}, "+sigmass+"}{"+sigctau+"#DeltaR(track,#tilde{#chi}_{1}^{#pm}) < 0.01}")
        tl.AddEntry(hbg_met,"SM Background")
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_met.GetMinimum()]),1e-3]))
        hsig.SetTitle(";MET (GeV);Fraction of events")
        hsig.Draw()
        hbg_met.Draw("same")
        tl.Draw()
        utils.DrawCmsText(simplecanvas,"CMS Simulation Preliminary")
        simplecanvas.SaveAs("pdfs_SvsBG/{}/met_{}.pdf".format(sigtag,reg))

        hsig = utils.GetUnderOverHist(fsig,"h_ht_{}_ST_All".format(reg),"H_{T} for Signal and Background STs;H_{T} (GeV);Fraction",ROOT.kBlack,norm=True)
        tl.Clear()
        tl.AddEntry(hsig,"#splitline{pp #rightarrow #tilde{g}#tilde{g}, #tilde{g} #rightarrow q#bar{q}'#tilde{#chi}_{1}^{#pm}, #tilde{#chi}_{1}^{#pm} #rightarrow #tilde{#chi}_{1}^{0}#pi^{#pm}, "+sigmass+"}{"+sigctau+"#DeltaR(track,#tilde{#chi}_{1}^{#pm}) < 0.01}")
        tl.AddEntry(hbg_ht,"SM Background")
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_ht.GetMinimum()]),1e-3]))
        hsig.SetTitle(";H_{T} (GeV);Fraction of events")
        hsig.Draw()
        hbg_ht.Draw("same")
        tl.Draw()
        utils.DrawCmsText(simplecanvas,"CMS Simulation Preliminary")
        simplecanvas.SaveAs("pdfs_SvsBG/{}/ht_{}.pdf".format(sigtag,reg))

        hsig = utils.GetUnderOverHist(fsig,"h_nj_{}_ST_All".format(reg),"N_{Jet} for Signal and Background STs;N_{Jet};Fraction",ROOT.kBlack,norm=True)
        tl.Clear()
        tl.AddEntry(hsig,"#splitline{pp #rightarrow #tilde{g}#tilde{g}, #tilde{g} #rightarrow q#bar{q}'#tilde{#chi}_{1}^{#pm}, #tilde{#chi}_{1}^{#pm} #rightarrow #tilde{#chi}_{1}^{0}#pi^{#pm}, "+sigmass+"}{"+sigctau+"#DeltaR(track,#tilde{#chi}_{1}^{#pm}) < 0.01}")
        tl.AddEntry(hbg_nj,"SM Background")
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_nj.GetMinimum()]),1e-3]))
        hsig.SetTitle(";N_{Jet} (GeV);Fraction of events")
        hsig.Draw()
        hbg_nj.Draw("same")
        tl.Draw()
        utils.DrawCmsText(simplecanvas,"CMS Simulation Preliminary")
        simplecanvas.SaveAs("pdfs_SvsBG/{}/nj_{}.pdf".format(sigtag,reg))

        hsig = utils.GetUnderOverHist(fsig,"h_mt_{}_ST_All".format(reg),"M_{T} for Signal and Background STs;M_{T}(Track,MET);Fraction",ROOT.kBlack,norm=True)
        tl.Clear()
        tl.AddEntry(hsig,"#splitline{pp #rightarrow #tilde{g}#tilde{g}, #tilde{g} #rightarrow q#bar{q}'#tilde{#chi}_{1}^{#pm}, #tilde{#chi}_{1}^{#pm} #rightarrow #tilde{#chi}_{1}^{0}#pi^{#pm}, "+sigmass+"}{"+sigctau+"#DeltaR(track,#tilde{#chi}_{1}^{#pm}) < 0.01}")
        tl.AddEntry(hbg_mt,"SM Background")
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_mt.GetMinimum()]),1e-3]))
        hsig.SetTitle(";M_{T}(MET,Track) (GeV);Fraction of events")
        hsig.Draw()
        hbg_mt.Draw("same")
        tl.Draw()
        utils.DrawCmsText(simplecanvas,"CMS Simulation Preliminary")
        simplecanvas.SaveAs("pdfs_SvsBG/{}/mt_{}.pdf".format(sigtag,reg))

        hsig_raw = utils.GetUnderOverHist(fsig,"h_reliso_{}_ST_All_NM1".format(reg),"RelIso for Signal and Background STs [N-1];RelIso;Fraction",ROOT.kBlack,over=False,norm=False)
        bins = array('d',[0.01*i for i in range(21)]+[0.23])
#        hsig = ROOT.TH1D("hsig_reliso_"+sigtag,hsig_raw.GetTitle(),21,0,0.21)
        hsig = ROOT.TH1D("hsig_reliso_"+sigtag,hsig_raw.GetTitle(),len(bins)-1,bins)
        hbg_reliso = hsig.Clone("hbg_reliso_"+sigtag)
        hsig.SetMarkerStyle(22)
        hbg_reliso.SetMarkerStyle(21)
        hsig.SetLineColor(ROOT.kBlack)
        hsig.SetMarkerColor(ROOT.kBlack)
        hbg_reliso.SetLineColor(ROOT.kRed)        
        hbg_reliso.SetMarkerColor(ROOT.kRed)        
        for i in range(1,21):
            hsig.SetBinContent(i,hsig_raw.GetBinContent(i))
            hsig.SetBinError(i,hsig_raw.GetBinError(i))
            hbg_reliso.SetBinContent(i,hbg_reliso_raw.GetBinContent(i))
            hbg_reliso.SetBinError(i,hbg_reliso_raw.GetBinError(i))
        hsig.SetBinContent(21,hsig_raw.GetBinContent(21))
        hsig.SetBinError(21,hsig_raw.GetBinError(21))
        hsig.Scale(1/hsig.Integral())
        hbg_reliso.SetBinContent(21,hbg_reliso_raw.GetBinContent(21))
        hbg_reliso.SetBinError(21,hbg_reliso_raw.GetBinError(21))
        hbg_reliso.Scale(1/hbg_reliso.Integral())
        tl.Clear()
        tl.AddEntry(hsig,"#splitline{pp #rightarrow #tilde{g}#tilde{g}, #tilde{g} #rightarrow q#bar{q}'#tilde{#chi}_{1}^{#pm}, #tilde{#chi}_{1}^{#pm} #rightarrow #tilde{#chi}_{1}^{0}#pi^{#pm}, "+sigmass+"}{"+sigctau+"#DeltaR(track, #tilde{#chi}_{1}^{#pm}) < 0.01}",legOpt)
        tl.AddEntry(hbg_reliso,"SM Background",legOpt)
        hsig.SetMaximum(1.1)
        hsig.SetMinimum(max([min([0.1*hsig.GetMinimum(),0.1*hbg_reliso.GetMinimum()]),1e-3]))
        hsig.SetTitle(";Isolation / track p_{T};Fraction of events")        
#        binlabels = [str(0.01 * (i+1)) for i in range(20)] + ["> 0.20"]
#        for bin in range(1,len(binlabels)+1):
#            hsig.GetXaxis().SetBinLabel(bin,binlabels[bin-1])
#        hsig.GetXaxis().LabelsOption("v")
#        hsig.GetXaxis().SetBinLabel(21,"> 0.20")
        hsig.Draw(errOpt)
        hbg_reliso.Draw("same"+errOpt)
        tl.Draw()
#        ttext.DrawLatex(1.0-simplecanvas.GetRightMargin()-0.025, simplecanvas.GetBottomMargin()+0.06, "> 0.20")
#        ttext.DrawLatex(1.0-simplecanvas.GetRightMargin()-0.025, simplecanvas.GetBottomMargin()+0.60, "> 0.20")
#        cut = ROOT.TLine(
        hsig.GetXaxis().SetTitleOffset(1.2)
        utils.CMS_Style(simplecanvas,extraText="Simulation Preliminary")
#        utils.DrawCmsText(simplecanvas,"CMS Simulation Preliminary")
#        ttext.DrawLatex(1.0-simplecanvas.GetRightMargin()-0.01, 1.0-simplecanvas.GetTopMargin()+0.01, "(13 TeV)")
        simplecanvas.SaveAs("pdfs_SvsBG/{}/reliso_{}.pdf".format(sigtag,reg))

        fsig.Close()

    del hbg_nlayer,hbg_pt,hbg_mt2,hbg_met,hbg_ht,hbg_nj,hbg_mt,hbg_reliso

print "Done"
