import numpy as np
import ROOT
import glob
import os

ROOT.gROOT.SetBatch(1)

tag = "V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_noJERsmear"

dir = "looper_output/V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_noJERsmear/qcd"
dir_mean04 = "looper_output/V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_mean04/qcd"
dir_noRS = "../SmearLooper/output/V00-10-04_94x_2017_noRS/"

hrs = ROOT.TH1D("hrs","",51,0,51)
h04 = ROOT.TH1D("h04","",51,0,51)
hnrs = ROOT.TH1D("hnrs","",51,0,51)

h_evts_rs = ROOT.TH1D("h_evts_rs","",1,0,2)
h_evts_04 = ROOT.TH1D("h_evts_04","",1,0,2)
h_evts_nrs = ROOT.TH1D("h_evts_nrs","",1,0,2)

ibin = 0
for ht_reg in ["VL","L","M","H","UH"]:
    sum_rs = 0
    sum_rs04 = 0
    top_regs = range(1,12)
    if ht_reg=="VL":
        top_regs = [1,2,3,12,13,14,15]
    for top_reg in top_regs:
        ibin+=1
        # print ibin

        h_evts_rs.Reset()
        h_evts_04.Reset()
        h_evts_nrs.Reset()
        bn = "merged_hists.root"
        frs = ROOT.TFile(os.path.join(dir,bn))
        f04 = ROOT.TFile(os.path.join(dir_mean04,bn))
        fnrs = ROOT.TFile(os.path.join(dir_noRS,"qcd_ht.root"))
        try:
            h_evts_rs.Add(frs.Get("sr{0}{1}/h_Events_w".format(top_reg,ht_reg)))
        except:
            pass
        try:
            h_evts_04.Add(f04.Get("sr{0}{1}/h_Events_w".format(top_reg,ht_reg)))
        except:
            pass
        try:
            h_evts_nrs.Add(fnrs.Get("sr{0}{1}/h_Events_w".format(top_reg,ht_reg)))
        except:
            pass

        hrs.SetBinContent(ibin, h_evts_rs.GetBinContent(1))
        hrs.SetBinError(ibin, h_evts_rs.GetBinError(1))

        h04.SetBinContent(ibin, h_evts_04.GetBinContent(1))
        h04.SetBinError(ibin, h_evts_04.GetBinError(1))

        hnrs.SetBinContent(ibin, h_evts_nrs.GetBinContent(1))
        hnrs.SetBinError(ibin, h_evts_nrs.GetBinError(1))
        
        sum_rs += h_evts_rs.GetBinContent(1)
        sum_rs04 += h_evts_04.GetBinContent(1)
        
    print "{0} HT: rs = {1}, rs04 = {2}, ratio = {3}".format(ht_reg, sum_rs, sum_rs04, sum_rs04/sum_rs)

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

h04.SetLineColor(ROOT.kBlue)
h04.SetMarkerColor(ROOT.kBlue)
h04.SetMarkerStyle(20)

hnrs.SetLineColor(401)
hnrs.SetMarkerColor(401)
hnrs.SetMarkerStyle(20)

hnrs.GetYaxis().SetRangeUser(1e-3,1e3)
hnrs.GetXaxis().SetLabelSize(0)

hnrs.Draw("PE")
hrs.Draw("PE SAME")
h04.Draw("PE SAME")

NBINS = hrs.GetNbinsX()
bindivs = [7,18,29,40]
binwidth = (1-pads[0].GetLeftMargin()-pads[0].GetRightMargin()) / NBINS

line = ROOT.TLine()
line.SetLineStyle(2)
for ix in bindivs:
  x = pads[0].GetLeftMargin() + ix/51.0 * (1-pads[0].GetLeftMargin()-pads[0].GetRightMargin())
  line.DrawLineNDC(x,1-pads[0].GetTopMargin(),x,pads[0].GetBottomMargin())

leg = ROOT.TLegend(0.78,0.75,0.94,0.9)
leg.AddEntry(hnrs, "QCD MC")
leg.AddEntry(hrs, "R&S from MC")
leg.AddEntry(h04, "Template Mean +4%")
leg.Draw()

text = ROOT.TLatex()
text.SetNDC(1)
text.SetTextSize(0.03)
text.SetTextAlign(22)
modbindivs = [0] + bindivs + [hrs.GetNbinsX()]
names = ["Very Low", "Low", "Medium", "High", "Extreme"]
xposs = [pads[0].GetLeftMargin() + 0.5*(modbindivs[i]+modbindivs[i+1])*binwidth for i in range(len(modbindivs)-1)]
for i in range(len(modbindivs)-1):
  ypos = 0.79
  if names[i]=="Extreme":
    ypos = 0.72
  text.SetTextColor(ROOT.kBlack)
  text.DrawLatex(xposs[i],ypos,"{0} H_{{T}}".format(names[i]))
  yrs, ers = 0.0, ROOT.Double(0.0)
  y04, e04 = 0.0, ROOT.Double(0.0)
  yrs = hrs.IntegralAndError(modbindivs[i]+1, modbindivs[i+1], ers)
  y04 = h04.IntegralAndError(modbindivs[i]+1, modbindivs[i+1], e04)
  r04 = y04 / yrs
  e04 = r04 * np.sqrt((e04/y04)**2 + (ers/yrs)**2)
  text.SetTextColor(ROOT.kBlue)
  text.DrawLatex(xposs[i], ypos-0.03, "{0:.2f} #pm {1:.2f}".format(r04,e04))

text.SetTextFont(42)
text.SetTextSize(0.04)
text.SetTextAlign(31)
text.SetTextColor(ROOT.kBlack)
text.DrawLatex(1.0-pads[0].GetRightMargin(), 1.0-pads[0].GetTopMargin()+0.01,"1 fb^{-1} (13 TeV)")
text.SetTextAlign(11)
text.SetTextFont(62)
text.DrawLatex(pads[0].GetLeftMargin()+0.01, 1.0-pads[0].GetTopMargin()+0.01,"CMS Preliminary")

binLabels_vl = ["2-3j, 0b", "2-3j, 1b", "2-3j, 2b", "#geq4j, 0b", "#geq4j, 1b", "#geq4j, 2b", "#geq2j, #geq3b"] 
binLabels = ["2-3j, 0b", "2-3j, 1b", "2-3j, 2b", "4-6j, 0b", "4-6j, 1b", "4-6j, 2b", "#geq7j, 0b", "#geq7j, 1b", "#geq7j, 2b", "2-6j, #geq3b", "#geq7j, #geq3b"]
text = ROOT.TLatex()
text.SetNDC(1)
text.SetTextAlign(32)
text.SetTextAngle(90)
text.SetTextSize(min(binwidth * 1.3,0.027))
text.SetTextFont(42)
for ibin in range(11):
  x = pads[0].GetLeftMargin() + (ibin+0.5)*binwidth
  y = pads[0].GetBottomMargin()-0.009
  if ibin < 7:
    text.DrawLatex(x,y,binLabels_vl[ibin])
  text.DrawLatex(x+7*binwidth,y,binLabels[ibin])    
  text.DrawLatex(x+18*binwidth,y,binLabels[ibin])
  text.DrawLatex(x+29*binwidth,y,binLabels[ibin])
  text.DrawLatex(x+40*binwidth,y,binLabels[ibin])



## ratio
pads[1].cd()

h_ratio04 = h04.Clone("h_ratio")
h_ratio04.Divide(hrs)

h_ratio04.GetYaxis().SetRangeUser(0,2)
h_ratio04.GetYaxis().SetNdivisions(505)
h_ratio04.GetYaxis().SetTitle("Shift/R&S")
h_ratio04.GetYaxis().SetTitleSize(0.16)
h_ratio04.GetYaxis().SetTitleOffset(0.18)
h_ratio04.GetYaxis().SetLabelSize(0.13)
h_ratio04.GetYaxis().CenterTitle()
h_ratio04.GetYaxis().SetTickLength(0.02)
h_ratio04.GetXaxis().SetLabelSize(0)
h_ratio04.GetXaxis().SetTitle("")
h_ratio04.GetXaxis().SetNdivisions(51,0,0)
h_ratio04.GetXaxis().SetTickSize(0.06)
h_ratio04.SetMarkerStyle(20)
h_ratio04.SetMarkerSize(1.0)
h_ratio04.SetLineWidth(1)

h_ratio04.Draw("PE")

line = ROOT.TLine()
line.DrawLine(0,1,51,1)

c.SaveAs("/home/users/bemarsh/public_html/mt2/RebalanceAndSmear/{0}/mc_mean_var.pdf".format(tag))
c.SaveAs("/home/users/bemarsh/public_html/mt2/RebalanceAndSmear/{0}/mc_mean_var.png".format(tag))


