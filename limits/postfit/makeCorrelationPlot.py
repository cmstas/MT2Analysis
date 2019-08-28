from math import *
import ROOT as r
r.gROOT.SetBatch(1)
r.gStyle.SetOptStat(0)
r.gStyle.SetNumberContours(255)

DO_CORR = False
TAG = "V00-10-17_FullRunII"

def sortkey(bin):
    htlo = int(bin.split("HT")[-1].split("to")[0])
    jlo = int(bin.split("_j")[-1].split("_")[0].split("to")[0])
    if "j2to6" in bin:
        jlo = 5
    blo = int(bin.split("_b")[-1].split("_")[0].split("to")[0])
    mlo = int(bin.split("_m")[-1].split("_")[0].split("to")[0])
    if "j1_" in bin:
        return (0, blo, htlo, 0)
    else:
        return(htlo, jlo, blo, mlo)


fin = r.TFile("{0}/fitDiagnostics_all.root".format(TAG))
# fin = r.TFile("FullRunII_17MCfor18_ttbbWeights_v3/fitDiagnostics_t0_all.root")

h_cov = fin.Get("shapes_prefit/overall_total_covar")
# h_cov = fin.Get("shapes_fit_b/overall_total_covar")
h_corr = h_cov.Clone("h_corr")

vals = {}
for i in range(1,h_cov.GetNbinsX()+1):
    for j in range(1,h_cov.GetNbinsY()+1):
        binx = h_cov.GetXaxis().GetBinLabel(i)
        biny = h_cov.GetYaxis().GetBinLabel(j)
        if binx not in vals:
            vals[binx] = {}
        cov = h_cov.GetBinContent(i,j)
        corr = h_cov.GetBinContent(i,j) / sqrt(h_cov.GetBinContent(i,i)*h_cov.GetBinContent(j,j))
        vals[binx][biny] = (abs(cov), corr)

bins = sorted(vals.keys(), key=sortkey)

current_reg = -1
thresh_bins = []
h = h_corr if DO_CORR else h_cov
for i,binx in enumerate(bins):
    if "_j1" in binx and current_reg < 0:
        thresh_bins.append(i+1)
        current_reg += 1
    if "HT250" in binx and "_j1" not in binx and current_reg < 1:
        thresh_bins.append(i+1)
        current_reg += 1
    if "HT450" in binx and "_j1" not in binx and current_reg < 2:
        thresh_bins.append(i+1)
        current_reg += 1
    if "HT575" in binx and "_j1" not in binx and current_reg < 3:
        thresh_bins.append(i+1)
        current_reg += 1
    if "HT1200" in binx and "_j1" not in binx and current_reg < 4:
        thresh_bins.append(i+1)
        current_reg += 1
    if "HT1500" in binx and "_j1" not in binx and current_reg < 5:
        thresh_bins.append(i+1)
        current_reg += 1
    for j,biny in enumerate(bins):
        h.GetXaxis().SetBinLabel(i+1, "")
        h.GetYaxis().SetBinLabel(j+1, "")
        h.SetBinContent(i+1, j+1, vals[binx][biny][1 if DO_CORR else 0])

reg_names = ["1 jet", "VL H_{T}", "L H_{T}", "M H_{T}", "H H_{T}", "UH H_{T}"]
for ibin, name in zip(thresh_bins, reg_names):
    h.GetXaxis().SetBinLabel(ibin, name)
    h.GetYaxis().SetBinLabel(ibin, name)

c = r.TCanvas("c","c",1000,1000)
if DO_CORR:
    r.gStyle.SetPalette(r.kLightTemperature)
else:
    c.SetLogz()
    r.gStyle.SetPalette(r.kRainBow)

c.SetLeftMargin(0.10)
c.SetRightMargin(0.14)
c.SetTopMargin(0.06)
c.SetBottomMargin(0.10)
left = c.GetLeftMargin()
right = c.GetRightMargin()
bottom = c.GetBottomMargin()
top = c.GetTopMargin()

h.SetTitle("")
h.GetXaxis().SetTickSize(0)
h.GetXaxis().SetLabelSize(0.035)
h.GetXaxis().SetTitle("")
h.GetYaxis().SetTickSize(0)
h.GetYaxis().SetLabelSize(0.035)
h.GetYaxis().SetTitle("")
h.GetZaxis().SetLabelSize(0.023)
title = "Correlation (#rho_{xy}#kern[0.4]{)}" if DO_CORR else "Covariance (|#kern[0.1]{#sigma}_{xy}#kern[0.2]{|}#kern[0.4]{)}"
h.GetZaxis().SetTitle(title)
h.GetZaxis().SetTitleOffset(1.3)

if DO_CORR:
    h.GetZaxis().SetRangeUser(-1,1)
else:
    min_cov = min([min([vals[b1][b2][0] for b2 in bins]) for b1 in bins])
    max_cov = max([max([vals[b1][b2][0] for b2 in bins]) for b1 in bins])
    min_cov = 10**floor(log10(min_cov))
    max_cov = 10**ceil(log10(max_cov))
    h.GetZaxis().SetRangeUser(min_cov, max_cov)

h.Draw("colz")

c.Update()
palette = h.GetListOfFunctions().FindObject("palette")
palette.SetX2NDC(1.0 - right + 0.04)

line = r.TLine()
line.SetLineWidth(2)
for ibin in thresh_bins[1:]:
    x = left + (1.0 - left - right)/len(bins)*(ibin-1)
    line.DrawLineNDC(x, bottom, x, bottom+0.025)
    y = bottom + (1.0 - bottom - top)/len(bins)*(ibin-1)
    line.DrawLineNDC(left, y, left+0.025, y)

text = r.TLatex()
text.SetNDC(1)
text.SetTextFont(62)
text.SetTextAlign(11)
text.SetTextSize(0.035)
text.DrawLatex(left+0.005, 1.0-top+0.01, "CMS #font[52]{Supplementary}")
text.SetTextAlign(31)
text.SetTextFont(42)
text.DrawLatex(1.0-right-0.005, 1.0-top+0.01, "137 fb^{-1} (13 TeV)")

c.SaveAs("~/public_html/mt2/result_plots/2019_FSR/{0}/sr_{1}.png".format(TAG, "correlation" if DO_CORR else "covariance"))
c.SaveAs("~/public_html/mt2/result_plots/2019_FSR/{0}/sr_{1}.pdf".format(TAG, "correlation" if DO_CORR else "covariance"))
