import numpy as np
import ROOT
import glob
import os
from common import *

ROOT.gROOT.SetBatch(1)

# tag = "V00-10-16_newJER_ptBinned_XXX_JetID_PUID_BTagSFs_core2sigma"
tag = "V00-10-09_ptBinned_XXX_JetID_PUID_BTagSFs_core2sigma"

year = "All"
lumi = 137

# year = 2018
# lumi = 59.8

# year = 2017
# lumi = 41.5

# year = 2016
# lumi = 35.9

dir = "looper_output/{0}/data{1}_2017V00-10-09/".format(tag,year)
dir_UP = "looper_output/{0}_jerUP/data{1}/".format(tag,year)
dir_DOWN = "looper_output/{0}_jerDOWN/data{1}/".format(tag,year)

NBINS = sum([len(x[1]) for x in topo_reg_defs.items()])

hrs = ROOT.TH1D("hrs","",NBINS,0,NBINS)
hUP = ROOT.TH1D("hUP","",NBINS,0,NBINS)
hDOWN = ROOT.TH1D("hDOWN","",NBINS,0,NBINS)

h_evts_rs = ROOT.TH1D("h_evts_rs","",1,0,2)
h_evts_UP = ROOT.TH1D("h_evts_UP","",1,0,2)
h_evts_DOWN = ROOT.TH1D("h_evts_DOWN","",1,0,2)

frs = ROOT.TFile(os.path.join(dir,"merged_hists.root"))
fUP = ROOT.TFile(os.path.join(dir_UP,"merged_hists.root"))
fDOWN = ROOT.TFile(os.path.join(dir_DOWN,"merged_hists.root"))
ibin = 0
ht_regs = ["VL","L","M","H","UH"]
for ht_reg in ht_regs:
  sum_rs = 0
  sum_UP = 0
  sum_DOWN = 0
  top_regs = topo_reg_defs[ht_reg]
  for top_reg in top_regs:
    ibin+=1

    if h_evts_rs:
      h_evts_rs.Reset()
    if h_evts_UP:
      h_evts_UP.Reset()
    if h_evts_DOWN:
      h_evts_DOWN.Reset()
    try:
      h_evts_rs = frs.Get("sr{0}{1}/h_Events_w".format(top_reg,ht_reg))
    except:
      pass
    try:
      h_evts_UP = fUP.Get("sr{0}{1}/h_Events_w".format(top_reg,ht_reg))
    except:
      pass
    try:
      h_evts_DOWN = fDOWN.Get("sr{0}{1}/h_Events_w".format(top_reg,ht_reg))
    except:
      pass

    if h_evts_rs:
      hrs.SetBinContent(ibin, h_evts_rs.GetBinContent(1))
      hrs.SetBinError(ibin, h_evts_rs.GetBinError(1))
    else:
      hrs.SetBinContent(ibin, 0)
      hrs.SetBinError(ibin, 0)

    if h_evts_UP:
      hUP.SetBinContent(ibin, h_evts_UP.GetBinContent(1))
      hUP.SetBinError(ibin, h_evts_UP.GetBinError(1))
    else:
      hUP.SetBinContent(ibin, 0)
      hUP.SetBinError(ibin, 0)
    if h_evts_DOWN:
      hDOWN.SetBinContent(ibin, h_evts_DOWN.GetBinContent(1))
      hDOWN.SetBinError(ibin, h_evts_DOWN.GetBinError(1))
    else:
      hDOWN.SetBinContent(ibin, 0)
      hDOWN.SetBinError(ibin, 0)

    sum_rs += hrs.GetBinContent(ibin)
    sum_UP += hUP.GetBinContent(ibin)
    sum_DOWN += hDOWN.GetBinContent(ibin)
      
        
ROOT.gStyle.SetOptStat(0)

c = ROOT.TCanvas("c","c",900,600)

pads = []
pads.append(ROOT.TPad("1","1",0.0,0.18,1.0,1.0))
pads.append(ROOT.TPad("2","2",0.0,0.0,1.0,0.19))

pads[0].SetTopMargin(0.08)
pads[0].SetBottomMargin(0.13)
pads[0].SetRightMargin(0.05)
pads[0].SetLeftMargin(0.10)

pads[1].SetRightMargin(0.05)
pads[1].SetLeftMargin(0.10)

pads[0].Draw()
pads[1].Draw()
pads[0].cd()

pads[0].SetLogy(1)
pads[1].SetTickx(1)
pads[0].SetTicky(1)
pads[1].SetTicky(1)

hrs.SetLineColor(ROOT.kBlack)
hrs.SetMarkerColor(ROOT.kBlack)
hrs.SetMarkerStyle(20)

hUP.SetLineColor(ROOT.kGreen+2)
hUP.SetMarkerColor(ROOT.kGreen+2)
hUP.SetMarkerStyle(20)

hDOWN.SetLineColor(ROOT.kRed)
hDOWN.SetMarkerColor(ROOT.kRed)
hDOWN.SetMarkerStyle(20)

hrs.GetYaxis().SetRangeUser(1e-1,1e5)
hrs.GetXaxis().SetLabelSize(0)

hrs.Draw("PE")
hUP.Draw("PE SAME")
hDOWN.Draw("PE SAME")

bin_width = (1-pads[0].GetLeftMargin()-pads[0].GetRightMargin()) / NBINS
bin_divisions = [0]
for i in range(len(ht_regs)):
    bin_divisions.append(bin_divisions[i]+len(topo_reg_defs[ht_regs[i]]))
bin_divisions = bin_divisions[1:-1]
line = ROOT.TLine()
line.SetLineStyle(2)
for ix in bin_divisions:
    x = pads[0].GetLeftMargin() + ix * bin_width
    line.DrawLineNDC(x,1-pads[0].GetTopMargin(),x,pads[0].GetBottomMargin())


leg = ROOT.TLegend(0.76,0.78,0.94,0.9)
leg.AddEntry(hrs, "R&S, JER central val")
leg.AddEntry(hUP, "R&S, JER UP")
leg.AddEntry(hDOWN, "R&S, JER DOWN")
leg.Draw()

text = ROOT.TLatex()
text.SetNDC(1)
text.SetTextSize(0.03)
text.SetTextAlign(22)
modbindivs = [0] + bin_divisions + [hrs.GetNbinsX()]
names = ["Very Low", "Low", "Medium", "High", "Extreme"]
xposs = [pads[0].GetLeftMargin() + 0.5*(modbindivs[i]+modbindivs[i+1])*bin_width for i in range(len(modbindivs)-1)]
for i in range(len(modbindivs)-1):
  ypos = 0.79
  if names[i]=="Extreme":
    ypos = 0.72
  text.SetTextColor(ROOT.kBlack)
  text.DrawLatex(xposs[i],ypos,"{0} H_{{T}}".format(names[i]))
  yrs, ers = 0.0, ROOT.Double(0.0)
  yUP, eUP = 0.0, ROOT.Double(0.0)
  yDOWN, eDOWN = 0.0, ROOT.Double(0.0)
  yrs = hrs.IntegralAndError(modbindivs[i]+1, modbindivs[i+1], ers)
  yUP = hUP.IntegralAndError(modbindivs[i]+1, modbindivs[i+1], eUP)
  yDOWN = hDOWN.IntegralAndError(modbindivs[i]+1, modbindivs[i+1], eDOWN)
  rUP = yUP/yrs
  eUP = rUP * np.sqrt((eUP/yUP)**2 + (ers/yrs)**2)
  rDOWN = yDOWN / yrs
  eDOWN = rDOWN * np.sqrt((eDOWN/yDOWN)**2 + (ers/yrs)**2)
  text.SetTextColor(ROOT.kGreen+2)
  text.DrawLatex(xposs[i], ypos-0.03, "{0:.2f} #pm {1:.2f}".format(rUP,eUP))
  text.SetTextColor(ROOT.kRed)
  text.DrawLatex(xposs[i], ypos-0.06, "{0:.2f} #pm {1:.2f}".format(rDOWN,eDOWN))

text.SetTextFont(42)
text.SetTextSize(0.04)
text.SetTextAlign(31)
text.SetTextColor(ROOT.kBlack)
text.DrawLatex(1.0-pads[0].GetRightMargin(), 1.0-pads[0].GetTopMargin()+0.01,"{0} fb^{{-1}} (13 TeV)".format(lumi))
text.SetTextAlign(11)
text.SetTextFont(62)
text.DrawLatex(pads[0].GetLeftMargin()+0.01, 1.0-pads[0].GetTopMargin()+0.01,"CMS")


binLabels_all = []
for ht_reg in ht_regs:
    binLabels_all += [binLabels[tr] for tr in topo_reg_defs[ht_reg]]
text = ROOT.TLatex()
text.SetNDC(1)
text.SetTextAlign(32)
text.SetTextAngle(90)
text.SetTextSize(min(bin_width * 1.5,0.027))
text.SetTextFont(42)
for ibin in range(NBINS):
  x = pads[0].GetLeftMargin() + (ibin+0.5)*bin_width
  y = pads[0].GetBottomMargin()-0.009
  text.DrawLatex(x, y, binLabels_all[ibin])    


## ratio
pads[1].cd()

h_ratioUP = hUP.Clone("h_ratio")
h_ratioUP.Divide(hrs)
h_ratioDOWN = hDOWN.Clone("h_ratio")
h_ratioDOWN.Divide(hrs)

for i in range(NBINS):
  h_ratioUP.SetBinError(i, 0)
  h_ratioDOWN.SetBinError(i, 0)

h_ratioUP.GetYaxis().SetRangeUser(0.5,1.5)
h_ratioUP.GetYaxis().SetNdivisions(505)
h_ratioUP.GetYaxis().SetTitle("R&S/Obs")
h_ratioUP.GetYaxis().SetTitleSize(0.16)
h_ratioUP.GetYaxis().SetTitleOffset(0.18)
h_ratioUP.GetYaxis().SetLabelSize(0.13)
h_ratioUP.GetYaxis().CenterTitle()
h_ratioUP.GetYaxis().SetTickLength(0.02)
h_ratioUP.GetXaxis().SetLabelSize(0)
h_ratioUP.GetXaxis().SetTitle("")
h_ratioUP.GetXaxis().SetNdivisions(NBINS,0,0)
h_ratioUP.GetXaxis().SetTickSize(0.06)
h_ratioUP.SetMarkerStyle(20)
h_ratioUP.SetMarkerSize(1.0)
h_ratioUP.SetLineWidth(1)

h_ratioUP.Draw("PE")
h_ratioDOWN.Draw("PE SAME")

line = ROOT.TLine()
line.DrawLine(0,1,NBINS,1)

username = os.environ["USER"]
c.SaveAs("/home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/data{2}_JER_systVar.pdf".format(username, tag, year))
c.SaveAs("/home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/data{2}_JER_systVar.png".format(username, tag, year))

# raw_input()
