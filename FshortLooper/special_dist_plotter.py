# Makes certain special plots of short track properties

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
simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(800,600)
simplecanvas.SetTicks(1,1)
simplecanvas.SetLeftMargin(0.14)
simplecanvas.SetRightMargin(0.1)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetBottomMargin(0.16)

simplecanvas.SetLogy()

ROOT.gStyle.SetLegendBorderSize(0)
tl=ROOT.TLegend(0.6,0.65,0.9,0.85)

ROOT.TH1.SetDefaultSumw2()

if len(sys.argv) < 2: 
    print "Which tag?"
    exit(1)

tag = sys.argv[1]

plotdir="special_distributions/{}".format(tag)
os.system("mkdir -p {}".format(plotdir))

f16 = ROOT.TFile.Open("output_merged/data_2016_{}.root".format(tag))
f1718 = ROOT.TFile.Open("output_merged/data_2017and2018_{}.root".format(tag))

histname_met = "h_met_Incl_ST_P"
histname_adj = "h_adjMet_Incl_ST_P"

h16_met_P = utils.GetUnderOverHist(f16,histname_met,"P ST and PF MET;MET (GeV);Count",ROOT.kRed)
h16_adj_P = utils.GetUnderOverHist(f16,histname_adj,"P ST and PF MET;MET (GeV);Count",ROOT.kMagenta)
histname_met = "h_met_Incl_ST_P3"
histname_adj = "h_adjMet_Incl_ST_P3"
h1718_met_P3 = utils.GetUnderOverHist(f1718,histname_met,"P3 ST and PF MET;MET (GeV);Count",ROOT.kBlue)
h1718_adj_P3 = utils.GetUnderOverHist(f1718,histname_adj,"P3 ST and PF MET;MET (GeV);Count",ROOT.kCyan)
histname_met = "h_met_Incl_ST_P4"
histname_adj = "h_adjMet_Incl_ST_P4"
h1718_met_P4 = utils.GetUnderOverHist(f1718,histname_met,"P4 ST and PF MET;MET (GeV);Count",ROOT.kGreen)
h1718_adj_P4 = utils.GetUnderOverHist(f1718,histname_adj,"P4 ST and PF MET;MET (GeV);Count",ROOT.kGreen-2)

tl.AddEntry(h16_met_P,"PF MET in P ST Events (2016)")
tl.AddEntry(h16_adj_P,"ST-subtracted MET in P ST Events (2016)")
tl.AddEntry(h1718_met_P3,"PF MET in P3 ST Events (2017-18)")
tl.AddEntry(h1718_adj_P3,"ST-subtracted MET in P3 ST Events (2017-18)")
tl.AddEntry(h1718_met_P4,"PF MET in P4 ST Events (2017-18)")
tl.AddEntry(h1718_adj_P4,"ST-subtracted MET in P4 ST Events (2017-18)")

h16_adj_P.SetMinimum(0.1)
h16_adj_P.SetMaximum(20*max([h16_met_P.GetMaximum(),h16_adj_P.GetMaximum(),h1718_met_P3.GetMaximum(),h1718_adj_P3.GetMaximum(),h1718_met_P4.GetMaximum(),h1718_adj_P4.GetMaximum()]))

h16_adj_P.Draw()
h16_met_P.Draw("same")
h1718_met_P3.Draw("same")
h1718_adj_P3.Draw("same")
h1718_met_P4.Draw("same")
h1718_adj_P4.Draw("same")
tl.Draw()
simplecanvas.SaveAs("{}/adjmet_P.png".format(plotdir))

tl.Clear()

histname_met = "h_met_Incl_ST_M"
histname_adj = "h_adjMet_Incl_ST_M"

h16_met_M = utils.GetUnderOverHist(f16,histname_met,"M ST and PF MET;MET (GeV);Count",ROOT.kRed)
h16_adj_M = utils.GetUnderOverHist(f16,histname_adj,"M ST and PF MET;MET (GeV);Count",ROOT.kMagenta)
h1718_met_M = utils.GetUnderOverHist(f1718,histname_met,"M ST and PF MET;MET (GeV);Count",ROOT.kBlue)
h1718_adj_M = utils.GetUnderOverHist(f1718,histname_adj,"M ST and PF MET;MET (GeV);Count",ROOT.kCyan)

tl.AddEntry(h16_met_M,"PF MET in M ST Events (2016)")
tl.AddEntry(h16_adj_M,"ST-subtracted MET in M ST Events (2016)")
tl.AddEntry(h1718_met_M,"PF MET in M ST Events (2017-18)")
tl.AddEntry(h1718_adj_M,"ST-subtracted MET in M ST Events (2017-18)")

h16_adj_M.SetMinimum(0.1)
h16_adj_M.SetMaximum(20*max([h16_met_M.GetMaximum(),h16_adj_M.GetMaximum(),h1718_met_M.GetMaximum(),h1718_adj_M.GetMaximum()]))

h16_adj_M.Draw()
h16_met_M.Draw("same")
h1718_met_M.Draw("same")
h1718_adj_M.Draw("same")
tl.Draw()
simplecanvas.SaveAs("{}/adjmet_M.png".format(plotdir))

tl.Clear()

histname_met = "h_met_Incl_ST_L"
histname_adj = "h_adjMet_Incl_ST_L"

h16_met_L = utils.GetUnderOverHist(f16,histname_met,"L ST and PF MET;MET (GeV);Count",ROOT.kRed)
h16_adj_L = utils.GetUnderOverHist(f16,histname_adj,"L ST and PF MET;MET (GeV);Count",ROOT.kMagenta)
h1718_met_L = utils.GetUnderOverHist(f1718,histname_met,"L ST and PF MET;MET (GeV);Count",ROOT.kBlue)
h1718_adj_L = utils.GetUnderOverHist(f1718,histname_adj,"L ST and PF MET;MET (GeV);Count",ROOT.kCyan)

tl.AddEntry(h16_met_L,"PF MET in L ST Events (2016)")
tl.AddEntry(h16_adj_L,"ST-subtracted MET in L ST Events (2016)")
tl.AddEntry(h1718_met_L,"PF MET in L ST Events (2017-18)")
tl.AddEntry(h1718_adj_L,"ST-subtracted MET in L ST Events (2017-18)")

h16_adj_L.SetMinimum(0.1)
h16_adj_L.SetMaximum(20*max([h16_met_L.GetMaximum(),h16_adj_L.GetMaximum(),h1718_met_L.GetMaximum(),h1718_adj_L.GetMaximum()]))

h16_adj_L.Draw()
h16_met_L.Draw("same")
h1718_met_L.Draw("same")
h1718_adj_L.Draw("same")
tl.Draw()
simplecanvas.SaveAs("{}/adjmet_L.png".format(plotdir))

tl.Clear()

# Delta Met

simplecanvas.SetLogy(False)


for track_type in ["ST","STC"]:
    histname_delta = "h_deltaMet_Incl_{}_P".format(track_type)
    h16_deltamet_P = utils.GetUnderOverHist(f16,histname_delta,"P {0};#frac{{PF MET - {0}-subtracted MET}}{{PF MET}};Fraction".format(track_type),ROOT.kRed,norm=True)
    histname_delta = "h_deltaMet_Incl_{}_P3".format(track_type)
    h1718_deltamet_P3 = utils.GetUnderOverHist(f1718,histname_delta,color=ROOT.kBlue,norm=True)
    histname_delta = "h_deltaMet_Incl_{}_P4".format(track_type)
    h1718_deltamet_P4 = utils.GetUnderOverHist(f1718,histname_delta,color=ROOT.kGreen,norm=True)
    tl.AddEntry(h16_deltamet_P,"2016 P {}s".format(track_type))
    tl.AddEntry(h1718_deltamet_P3,"2017-18 P3 {}s".format(track_type))
    tl.AddEntry(h1718_deltamet_P4,"2017-18 P4 {}s".format(track_type))

    h16_deltamet_P.GetXaxis().SetTitleOffset(1.6)
    h16_deltamet_P.SetMinimum(0)
    h16_deltamet_P.SetMaximum(1.35*max([h16_deltamet_P.GetMaximum(),h1718_deltamet_P3.GetMaximum(),h1718_deltamet_P4.GetMaximum()]))

    h16_deltamet_P.Draw()
    h1718_deltamet_P3.Draw("same")
    h1718_deltamet_P4.Draw("same")
    tl.Draw()
    simplecanvas.SaveAs("{}/deltamet_P_{}.png".format(plotdir,track_type))

    tl.Clear()

    histname_delta = "h_deltaMet_Incl_{}_M".format(track_type)
    h16_deltamet_M = utils.GetUnderOverHist(f16,histname_delta,"M {0};#frac{{PF MET - {0}-subtracted MET}}{{PF MET}};Fraction".format(track_type),ROOT.kRed,norm=True)
    h1718_deltamet_M = utils.GetUnderOverHist(f1718,histname_delta,color=ROOT.kBlue,norm=True)
    tl.AddEntry(h16_deltamet_M,"2016 M {}s".format(track_type))
    tl.AddEntry(h1718_deltamet_M,"2017-18 M {}s".format(track_type))

    h16_deltamet_M.GetXaxis().SetTitleOffset(1.6)
    h16_deltamet_M.SetMinimum(0)
    h16_deltamet_M.SetMaximum(1.35*max([h16_deltamet_M.GetMaximum(),h1718_deltamet_M.GetMaximum()]))

    h16_deltamet_M.Draw()
    h1718_deltamet_M.Draw("same")
    tl.Draw()
    simplecanvas.SaveAs("{}/deltamet_M_{}.png".format(plotdir,track_type))

    tl.Clear()

    if track_type == "ST": continue

    histname_delta = "h_deltaMet_Incl_{}_L".format(track_type)
    h16_deltamet_L = utils.GetUnderOverHist(f16,histname_delta,"L {0};#frac{{PF MET - {0}-subtracted MET}}{{PF MET}};Fraction".format(track_type),ROOT.kRed,norm=True)
    h1718_deltamet_L = utils.GetUnderOverHist(f1718,histname_delta,color=ROOT.kGreen,norm=True)
    tl.AddEntry(h16_deltamet_L,"2016 L {}s".format(track_type))
    tl.AddEntry(h1718_deltamet_L,"2017-18 L {}s".format(track_type))

    h16_deltamet_L.GetXaxis().SetTitleOffset(1.6)
    h16_deltamet_L.SetMinimum(0)
    h16_deltamet_L.SetMaximum(1.35*max([h16_deltamet_L.GetMaximum(),h1718_deltamet_L.GetMaximum()]))

    h16_deltamet_L.Draw()
    h1718_deltamet_L.Draw("same")
    tl.Draw()
    simplecanvas.SaveAs("{}/deltamet_L_{}.png".format(plotdir,track_type))

    tl.Clear()



f16.Close()
f1718.Close()


print "Done"
