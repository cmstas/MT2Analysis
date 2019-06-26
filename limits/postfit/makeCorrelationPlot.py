from math import sqrt
import ROOT as r
r.gROOT.SetBatch(1)
r.gStyle.SetOptStat(0)
r.gStyle.SetNumberContours(255)
r.gStyle.SetPalette(r.kLightTemperature)

fin = r.TFile("FullRunII_17MCfor18_ttbbWeights_v3/fitDiagnostics_all.root")
# fin = r.TFile("FullRunII_17MCfor18_ttbbWeights_v3/fitDiagnostics_t0_all.root")

h_cov = fin.Get("shapes_prefit/overall_total_covar")
# h_cov = fin.Get("shapes_fit_b/overall_total_covar")
h_corr = h_cov.Clone("h_corr")

for i in range(1,h_cov.GetNbinsX()+1):
    for j in range(1,h_cov.GetNbinsY()+1):
        corr = h_cov.GetBinContent(i,j) / sqrt(h_cov.GetBinContent(i,i)*h_cov.GetBinContent(j,j))
        h_corr.SetBinContent(i, j, corr)


c = r.TCanvas("c","c",1000,1000)

c.SetLeftMargin(0.08)
c.SetRightMargin(0.12)
c.SetTopMargin(0.07)
c.SetBottomMargin(0.08)

h_corr.SetTitle("Bin Correlation (b-only)")
h_corr.GetXaxis().SetTickSize(0)
h_corr.GetXaxis().SetLabelSize(0.005)
h_corr.GetXaxis().SetTitle("")
h_corr.GetYaxis().SetTickSize(0)
h_corr.GetYaxis().SetLabelSize(0.005)
h_corr.GetYaxis().SetTitle("")

h_corr.GetZaxis().SetRangeUser(-1,1)

h_corr.Draw("colz")

c.SaveAs("~/public_html/test.pdf")
