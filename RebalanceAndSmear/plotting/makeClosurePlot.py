import numpy as np
import ROOT
import glob
import os

ROOT.gROOT.SetBatch(1)

# tag = "V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_ecalDeadCell_tail0p66_noJERsmear"
tag = "V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_noJERsmear"
# tag = "V00-10-01_31Mar2018_2016JRTs_try2"

# prefix = "sr"
# prefix = "crRSMT2SideBand"
# prefix = "crRSInvertDPhi"
prefix = "crRSDPhiMT2"

dir = "looper_output/{0}/qcd/".format(tag)
dir_noRS = "../SmearLooper/output/V00-10-04_94x_2017_noRS/"
# dir_noRS = "../../MT2Looper/output/V00-10-04_94x_2016_2017/"

username = os.environ["USER"]
os.system("mkdir -p /home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/".format(username, tag))
os.system("cp ~/scripts/index.php /home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/".format(username, tag))

hrs = ROOT.TH1D("hrs","",51,0,51)
hnrs = ROOT.TH1D("hnrs","",51,0,51)

h_evts_rs = ROOT.TH1D("h_evts_rs","",1,0,2)
h_evts_nrs = ROOT.TH1D("h_evts_nrs","",1,0,2)

frs = ROOT.TFile(os.path.join(dir,"merged_hists.root"))
fnrs = ROOT.TFile(os.path.join(dir_noRS,"qcd_ht.root"))

top_regs_vl=[1,2,3,12,13,14,15]

ibin = 0
for ht_reg in ["VL","L","M","H","UH"]:
  sum_rs = 0
  sum_nrs = 0
  top_regs = []
  if ht_reg == "VL":
    top_regs = top_regs_vl
  else:
    top_regs.extend(range(1,12))  
  for top_reg in top_regs:
    ibin+=1
    print ibin, top_reg, ht_reg

    if h_evts_rs:
      h_evts_rs.Reset()
    if h_evts_nrs:
      h_evts_nrs.Reset()
    try:
      h_evts_rs = frs.Get("{0}{1}{2}/h_Events_w".format(prefix,top_reg,ht_reg))
    except:
      pass
    try:
      h_evts_nrs = fnrs.Get("{0}{1}{2}/h_Events_w".format(prefix,top_reg,ht_reg))
      # h_evts_nrs.Scale(1.0/41.4)
    except:
      pass

    if h_evts_rs:
      hrs.SetBinContent(ibin, h_evts_rs.GetBinContent(1))
      hrs.SetBinError(ibin, h_evts_rs.GetBinError(1))
    else:
      hrs.SetBinContent(ibin, 0)
      hrs.SetBinError(ibin, 0)

    if h_evts_nrs:
      hnrs.SetBinContent(ibin, h_evts_nrs.GetBinContent(1))
      hnrs.SetBinError(ibin, h_evts_nrs.GetBinError(1))
    else:
      hnrs.SetBinContent(ibin, 0)
      hnrs.SetBinError(ibin, 0)

    sum_rs += hrs.GetBinContent(ibin)
    sum_nrs += hnrs.GetBinContent(ibin)

  ratio = sum_rs/sum_nrs if sum_nrs > 0 else 0
  print "{0} HT: rs = {1}, nrs = {2}, ratio = {3}".format(ht_reg, sum_rs, sum_nrs, ratio)
        
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

hnrs.SetLineColor(401)
hnrs.SetMarkerColor(401)
hnrs.SetMarkerStyle(20)

hnrs.GetYaxis().SetRangeUser(1e-3, 1e3 if prefix=="sr" else 1e5)
hnrs.GetXaxis().SetLabelSize(0)

hnrs.Draw("PE")
hrs.Draw("PE SAME")

NBINS = hrs.GetNbinsX()
bindivs = [7,18,29,40]
binwidth = (1-pads[0].GetLeftMargin()-pads[0].GetRightMargin()) / NBINS

line = ROOT.TLine()
line.SetLineStyle(2)
for ix in bindivs:
  x = pads[0].GetLeftMargin() + ix/51.0 * (1-pads[0].GetLeftMargin()-pads[0].GetRightMargin())
  line.DrawLineNDC(x,1-pads[0].GetTopMargin(),x,pads[0].GetBottomMargin())

leg = ROOT.TLegend(0.815,0.78,0.94,0.9)
leg.AddEntry(hrs, "R&S from MC")
leg.AddEntry(hnrs, "QCD MC")
leg.Draw()

ratios = []
ratioerrs = []
modbindivs = [0] + bindivs + [hrs.GetNbinsX()]
for i in range(len(modbindivs)-1):
  err_rs = ROOT.Double(0.0)
  yield_rs = hrs.IntegralAndError(modbindivs[i]+1, modbindivs[i+1], err_rs)
  err_nrs = ROOT.Double(0.0)
  yield_nrs = hnrs.IntegralAndError(modbindivs[i]+1, modbindivs[i+1], err_nrs)
  ratios.append(yield_nrs / yield_rs)
  ratioerrs.append(ratios[-1] * np.sqrt((err_rs/yield_rs)**2 + (err_nrs/yield_nrs)**2))
  
text = ROOT.TLatex()
text.SetNDC(1)
text.SetTextSize(0.03)
text.SetTextAlign(22)
names = ["Very Low", "Low", "Medium", "High", "Extreme"]
xposs = [pads[0].GetLeftMargin() + 0.5*(modbindivs[i]+modbindivs[i+1])*binwidth for i in range(len(modbindivs)-1)]
for i in range(len(modbindivs)-1):
  ypos = 0.79
  if names[i]=="Extreme":
    ypos = 0.69
  text.DrawLatex(xposs[i],ypos,"{0} H_{{T}}".format(names[i]))
  text.DrawLatex(xposs[i], ypos-0.05, "{0:.2f} #pm {1:.2f}".format(ratios[i], ratioerrs[i]))
text.SetTextFont(42)
text.SetTextSize(0.04)
text.DrawLatex(0.8,0.93,"1 fb^{-1} (13 TeV)")


binWidth = (1-pads[0].GetLeftMargin()-pads[1].GetRightMargin())/51.0
binLabels_vl = ["2-3j, 0b", "2-3j, 1b", "2-3j, 2b", "#geq4j, 0b", "#geq4j, 1b", "#geq4j, 2b", "#geq2j, #geq3b"] 
binLabels = ["2-3j, 0b", "2-3j, 1b", "2-3j, 2b", "4-6j, 0b", "4-6j, 1b", "4-6j, 2b", "#geq7j, 0b", "#geq7j, 1b", "#geq7j, 2b", "2-6j, #geq3b", "#geq7j, #geq3b"]
text = ROOT.TLatex()
text.SetNDC(1)
text.SetTextAlign(32)
text.SetTextAngle(90)
text.SetTextSize(min(binWidth * 1.3,0.027))
text.SetTextFont(42)
for ibin in range(11):
  x = pads[0].GetLeftMargin() + (ibin+0.5)*binWidth
  y = pads[0].GetBottomMargin()-0.009
  if ibin < 7:
    text.DrawLatex(x,y,binLabels_vl[ibin])
  text.DrawLatex(x+7*binWidth,y,binLabels[ibin])    
  text.DrawLatex(x+18*binWidth,y,binLabels[ibin])
  text.DrawLatex(x+29*binWidth,y,binLabels[ibin])
  text.DrawLatex(x+40*binWidth,y,binLabels[ibin])


## ratio
pads[1].cd()

h_ratio = hrs.Clone("h_ratio")
h_ratio.Divide(hnrs)

h_ratio.GetYaxis().SetRangeUser(0,2)
h_ratio.GetYaxis().SetNdivisions(505)
h_ratio.GetYaxis().SetTitle("R&S/MC")
h_ratio.GetYaxis().SetTitleSize(0.16)
h_ratio.GetYaxis().SetTitleOffset(0.18)
h_ratio.GetYaxis().SetLabelSize(0.13)
h_ratio.GetYaxis().CenterTitle()
h_ratio.GetYaxis().SetTickLength(0.02)
h_ratio.GetXaxis().SetLabelSize(0)
h_ratio.GetXaxis().SetTitle("")
h_ratio.GetXaxis().SetNdivisions(51,0,0)
h_ratio.GetXaxis().SetTickSize(0.06)
h_ratio.SetMarkerStyle(20)
h_ratio.SetMarkerSize(1.0)
h_ratio.SetLineWidth(1)

h_ratio.Draw("PE")

line = ROOT.TLine()
line.DrawLine(0,1,51,1)

c.SaveAs("/home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/mc_closure_{2}.pdf".format(username, tag, prefix))
c.SaveAs("/home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/mc_closure_{2}.png".format(username, tag, prefix))

raw_input()
