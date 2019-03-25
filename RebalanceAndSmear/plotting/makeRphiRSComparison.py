import ROOT as r
import glob
import os
import math
from common import *

doRatioToTotal = False

def getRatioAndError (h1, h2):
  err1 = r.Double(0)
  val1 = h1.IntegralAndError(1,-1,err1)
  err2 = r.Double(0)
  val2 = h2.IntegralAndError(1,-1,err2)
  val = val1/val2
  err = val * math.sqrt((err1/val1)*(err1/val1) + (err2/val2)*(err2/val2))
  return {'val':val, 'err':err}
  
# r.gROOT.SetBatch(1)

rphi_file = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/scripts/qcdEstimate/output/FullRunII/qcdEstimate.root"
# rphi_file = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/scripts/qcdEstimate/output/V00-10-09_2018fullYear/qcdEstimate.root"
rs_file = "../estimate/qcdFromRS.root"
data_file = "../../MT2Looper/output/V00-10-10_combined_17MCfor18_ttbbWeights/data_RunAll.root"

NBINS = sum([len(x[1]) for x in topo_reg_defs.items()])

hrphi = r.TH1D("hrphi","",NBINS,0,NBINS)
hrs = r.TH1D("hrs","",NBINS,0,NBINS)
hdata = r.TH1D("hdata","",NBINS,0,NBINS)

h_evts_rphi = r.TH1D("h_evts_rphi","",1,0,2)
h_evts_rs = r.TH1D("h_evts_rs","",1,0,2)
h_evts_data = r.TH1D("h_evts_data","",1,0,2)

top_regs_vl=[1,2,3,12,13,14,15]

ht_reg_names = ["HT250to450", "HT450to575", "HT575to1200", "HT1200to1500", "HT1500toInf"]
vll_top_reg_names = ["j2to3_b0", "j2to3_b1", "j2to3_b2", "j4to6_b0", "j4to6_b1", "j4to6_b2", "j7toInf_b0", "j7toInf_b1", "j7toInf_b2", "j2to6_b3toInf", "j7toInf_b3toInf"]
mhuh_top_reg_names = ["j2to3_b0", "j2to3_b1", "j2to3_b2", "j4to6_b0", "j4to6_b1", "j4to6_b2", "j2to6_b3toInf", "j7to9_b0", "j7to9_b1", "j7to9_b2", "j7to9_b3", "j7to9_b4toInf", "j10toInf_b0", "j10toInf_b1", "j10toInf_b2", "j10toInf_b3", "j10toInf_b4toInf"]

frphi = r.TFile(rphi_file)
frs = r.TFile(rs_file)
fdata = r.TFile(data_file)
ibin = 0
ht_regs = ["VL","L","M","H","UH"]
for iht,ht_reg in enumerate(ht_regs):
  sum_rphi = 0
  sum_rs = 0
  top_regs = topo_reg_defs[ht_reg]
  topo_reg_names = vll_top_reg_names
  if ht_reg in ["M", "H", "UH"]:
    topo_reg_names = mhuh_top_reg_names
  for itop,top_reg in enumerate(top_regs):
    ibin+=1

    rphi_err = r.Double(0)
    if h_evts_rs:
      h_evts_rs.Reset()
    if h_evts_rphi:
      h_evts_rphi.Reset()
    if h_evts_data:
      h_evts_data.Reset()
    try:
      h_evts_rphi = frphi.Get("qcdEstimate/{0}_{1}/yield_qcdEstimate_{0}_{1}".format(ht_reg_names[iht], topo_reg_names[itop]))
    except:
      pass
    try:
      h_evts_rs = frs.Get("sr{0}{1}/h_mt2bins".format(top_reg,ht_reg))
    except:
      pass
    try:
      h_evts_data = fdata.Get("sr{0}{1}/h_mt2bins".format(top_reg,ht_reg))
    except:
      pass      

    if doRatioToTotal:
      if h_evts_rphi and h_evts_data:
        vals = getRatioAndError(h_evts_rphi, h_evts_data)
        hrphi.SetBinContent(ibin, vals['val'])
        hrphi.SetBinError(ibin, vals['err'])
      else:
        hrphi.SetBinContent(ibin, 0)
        hrphi.SetBinError(ibin, 0)
      if h_evts_rs and h_evts_data:
        vals = getRatioAndError(h_evts_rs, h_evts_data)
        hrs.SetBinContent(ibin, vals['val'])
        hrs.SetBinError(ibin, vals['err'])
      else:
        hrs.SetBinContent(ibin, 0)
        hrs.SetBinError(ibin, 0)        
    else:
      if h_evts_rphi:
        hrphi.SetBinContent(ibin, h_evts_rphi.IntegralAndError(1,-1,rphi_err))
        hrphi.SetBinError(ibin, rphi_err)
      else:
        hrphi.SetBinContent(ibin, 0)
        hrphi.SetBinError(ibin, 0)
            
      if h_evts_rs:
        hrs.SetBinContent(ibin, h_evts_rs.GetBinContent(1))
        hrs.SetBinError(ibin, h_evts_rs.GetBinError(1))
      else:
        hrs.SetBinContent(ibin, 0)
        hrs.SetBinError(ibin, 0)

    sum_rphi += hrphi.GetBinContent(ibin)            
    sum_rs += hrs.GetBinContent(ibin)

  ratio = sum_rs/sum_rphi if sum_rphi > 0 else 0
  print "{0} HT: rs = {1}, rphi = {2}, ratio = {3}".format(ht_reg, sum_rs, sum_rphi, ratio)
    
r.gStyle.SetOptStat(0)

c = r.TCanvas("c","c",900,600)

pads = []
pads.append(r.TPad("1","1",0.0,0.18,1.0,1.0))
pads.append(r.TPad("2","2",0.0,0.0,1.0,0.19))

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

hrs.SetLineColor(r.kBlack)
hrs.SetMarkerColor(r.kBlack)
hrs.SetMarkerStyle(20)

hrphi.SetLineColor(401)
hrphi.SetMarkerColor(401)
hrphi.SetMarkerStyle(20)

hrphi.GetYaxis().SetRangeUser(1e-3,1e7)
if doRatioToTotal:
  hrphi.GetYaxis().SetRangeUser(1e-3,5)
hrphi.GetXaxis().SetLabelSize(0)

hrphi.Draw("PE")
hrs.Draw("PE SAME")

bin_width = (1-pads[0].GetLeftMargin()-pads[0].GetRightMargin()) / NBINS
bin_divisions = [0]
for i in range(len(ht_regs)):
    bin_divisions.append(bin_divisions[i]+len(topo_reg_defs[ht_regs[i]]))
bin_divisions = bin_divisions[1:-1]
line = r.TLine()
line.SetLineStyle(2)
for ix in bin_divisions:
    x = pads[0].GetLeftMargin() + ix * bin_width
    line.DrawLineNDC(x,1-pads[0].GetTopMargin(),x,pads[0].GetBottomMargin())

leg = r.TLegend(0.815,0.78,0.94,0.9)
leg.AddEntry(hrs, "R&S estimate")
leg.AddEntry(hrphi, "rphi estimate")
leg.Draw()


text = r.TLatex()
text.SetNDC(1)
text.SetTextSize(0.03)
text.DrawLatex(0.12,0.79,"Very Low H_{T}")
text.DrawLatex(0.27,0.79,"Low H_{T}")
text.DrawLatex(0.42,0.79,"Medium H_{T}")
text.DrawLatex(0.62,0.79,"High H_{T}")
text.DrawLatex(0.8,0.73,"Extreme H_{T}")
text.SetTextFont(42)
text.SetTextSize(0.04)
text.DrawLatex(0.8,0.93,"136.3 fb^{-1} (13 TeV)")


binLabels_all = []
for ht_reg in ht_regs:
    binLabels_all += [binLabels[tr] for tr in topo_reg_defs[ht_reg]]
text = r.TLatex()
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
pads[1].SetLogy(1)

h_ratio = hrphi.Clone("h_ratio")
h_ratio.Divide(hrs)

h_ratio.GetYaxis().SetRangeUser(0.1,30)
h_ratio.GetYaxis().SetNdivisions(505)
h_ratio.GetYaxis().SetTitle("R&S/rphi")
h_ratio.GetYaxis().SetTitleSize(0.16)
h_ratio.GetYaxis().SetTitleOffset(0.18)
h_ratio.GetYaxis().SetLabelSize(0.13)
h_ratio.GetYaxis().CenterTitle()
h_ratio.GetYaxis().SetTickLength(0.02)
h_ratio.GetXaxis().SetLabelSize(0)
h_ratio.GetXaxis().SetTitle("")
h_ratio.GetXaxis().SetNdivisions(NBINS,0,0)
h_ratio.GetXaxis().SetTickSize(0.06)
h_ratio.SetMarkerStyle(20)
h_ratio.SetMarkerSize(1.0)
h_ratio.SetMarkerColor(r.kBlack)
h_ratio.SetLineWidth(1)

h_ratio.Draw("PE")

line = r.TLine()
line.DrawLine(0,1,NBINS,1)

username = os.environ["USER"]
suffix = "_rat" if doRatioToTotal else ""
# c.SaveAs("/home/users/{0}/public_html/mt2/RebalanceAndSmear/data_tests2/comparison{1}.pdf".format(username,suffix))
# c.SaveAs("/home/users/{0}/public_html/mt2/RebalanceAndSmear/data_tests2/comparison{1}.png".format(username,suffix))
# c.SaveAs("/home/users/{0}/public_html/mt2/RebalanceAndSmear/data_tests2/comparison{1}.root".format(username,suffix))

raw_input()


