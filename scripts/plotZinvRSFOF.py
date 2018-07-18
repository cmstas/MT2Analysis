import os
import ROOT
import numpy as np

ROOT.gROOT.SetBatch(1)
ROOT.gStyle.SetOptStat(0)

fin = ROOT.TFile("/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2looper/output/V00-09-04_41p96fb/data_Run2017F.root")
# fin = ROOT.TFile("/home/users/olivito/mt2_80x/MT2Analysis/MT2looper/output/V00-08-18_reminiaod/data_Run2016.root")
# fin = ROOT.TFile("/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2looper/output/full2016_data/data_Run2016.root")
# lumi = 42.0
# lumi = 4.79 + 9.79
# lumi = 4.32 + 9.42
lumi = 13.50
# lumi = 35.9
outdir = "/home/users/bemarsh/public_html/mt2/RSFOF/V00-09-04_41p96fb_v2_F"
# outdir = "/home/users/bemarsh/public_html/mt2/RSFOF/full2016"
# outdir = "/home/users/bemarsh/public_html/mt2/RSFOF/full2016_dom"

def makePlot(h_rsfof, rsfof, rsfoferr, outdir=".", name="ht", xaxis="H_{T} [GeV]"):
    global lumi

    c = ROOT.TCanvas("c","c",700, 600)
    c.SetTicky(1)

    h_rsfof.GetYaxis().SetRangeUser(0, 2.0)
    h_rsfof.GetYaxis().SetTitle("R(SF/OF)")
    h_rsfof.GetXaxis().SetTitle(xaxis)
    h_rsfof.SetMarkerStyle(4)
    h_rsfof.SetMarkerColor(ROOT.kBlue)
    h_rsfof.SetLineColor(ROOT.kBlue)
    h_rsfof.SetLineWidth(2)

    h_rsfof.Draw("PE")
    
    line = ROOT.TLine()
    line.SetLineColor(ROOT.kBlack)
    line.SetLineStyle(1)
    xmin = h_rsfof.GetXaxis().GetXmin()
    xmax = h_rsfof.GetXaxis().GetXmax()
    line.DrawLine(xmin, rSFOF, xmax, rSFOF)
    line.SetLineStyle(2)
    line.DrawLine(xmin, rSFOF-rSFOFerr, xmax, rSFOF-rSFOFerr)
    line.DrawLine(xmin, rSFOF+rSFOFerr, xmax, rSFOF+rSFOFerr)

    text = ROOT.TLatex()
    text.SetNDC(1)
    text.SetTextColor(ROOT.kRed)
    text.SetTextFont(42)
    text.SetTextSize(0.04)
    text.DrawLatex(0.5, 0.75, "R(SF/OF) = {0:.2f} #pm {1:.2f}".format(rsfof, rsfoferr))

    text.SetTextColor(ROOT.kBlack)
    text.SetTextSize(0.045)
    text.SetTextAlign(31)
    text.DrawLatex(0.9, 0.92, "{0:.1f} fb^{{-1}} (13 TeV)".format(lumi))

    h_rsfof.Draw("PE SAME")

    c.SaveAs(outdir+"/"+name+".pdf")
    c.SaveAs(outdir+"/"+name+".png")

os.system("mkdir -p "+outdir)
os.system("cp ~/scripts/index.php "+outdir)

## as a function of HT
h_ht_SF = fin.Get("crdybaseIncl/h_htLowPt")
h_ht_OF = fin.Get("crdybaseIncl/h_htLowPtemu")
ht_bins = np.array([250, 450, 575, 1000, 2000], dtype=float)
h_ht_SF = h_ht_SF.Rebin(ht_bins.size-1, "h_ht_RSFOF", ht_bins)
h_ht_OF = h_ht_OF.Rebin(ht_bins.size-1, "h_ht_OF", ht_bins)

nSFerr = ROOT.Double(0.)
nOFerr = ROOT.Double(0.)
nSF = h_ht_SF.IntegralAndError(0, -1, nSFerr)
nOF = h_ht_OF.IntegralAndError(0, -1, nOFerr)
rSFOF = nSF/nOF
rSFOFerr = rSFOF*np.sqrt((nSFerr/nSF)**2 + (nOFerr/nOF)**2)
print rSFOF,"+-",rSFOFerr

h_ht_SF.Divide(h_ht_OF)
makePlot(h_ht_SF, rSFOF, rSFOFerr, outdir=outdir, name="rsfof_ht", xaxis="H_{T} [GeV]")

## as a function of MT2
h_mt2_SF = fin.Get("crdybaseIncl/h_mt2LowPt")
h_mt2_OF = fin.Get("crdybaseIncl/h_mt2LowPtemu")
mt2_bins = np.array([200, 300, 400, 1500], dtype=float)
h_mt2_SF = h_mt2_SF.Rebin(mt2_bins.size-1, "h_mt2_RSFOF", mt2_bins)
h_mt2_OF = h_mt2_OF.Rebin(mt2_bins.size-1, "h_mt2_OF", mt2_bins)

h_mt2_SF.Divide(h_mt2_OF)
makePlot(h_mt2_SF, rSFOF, rSFOFerr, outdir=outdir, name="rsfof_mt2", xaxis="M_{T2} [GeV]")

## as a function of njets
h_nJet30_SF = fin.Get("crdybaseIncl/h_nJet30LowPt")
h_nJet30_OF = fin.Get("crdybaseIncl/h_nJet30LowPtemu")
nJet30_bins = np.array([1,2,4,7,10], dtype=float)
h_nJet30_SF = h_nJet30_SF.Rebin(nJet30_bins.size-1, "h_nJet30_RSFOF", nJet30_bins)
h_nJet30_OF = h_nJet30_OF.Rebin(nJet30_bins.size-1, "h_nJet30_OF", nJet30_bins)

h_nJet30_SF.Divide(h_nJet30_OF)
makePlot(h_nJet30_SF, rSFOF, rSFOFerr, outdir=outdir, name="rsfof_nJet30", xaxis="N_{j}")

## as a function of nbjets
h_nBJet20_SF = fin.Get("crdybaseIncl/h_nBJet20LowPt")
h_nBJet20_OF = fin.Get("crdybaseIncl/h_nBJet20LowPtemu")
nBJet20_bins = np.array([0,1,2,3,4], dtype=float)
h_nBJet20_SF = h_nBJet20_SF.Rebin(nBJet20_bins.size-1, "h_nBJet20_RSFOF", nBJet20_bins)
h_nBJet20_OF = h_nBJet20_OF.Rebin(nBJet20_bins.size-1, "h_nBJet20_OF", nBJet20_bins)

h_nBJet20_SF.Divide(h_nBJet20_OF)
makePlot(h_nBJet20_SF, rSFOF, rSFOFerr, outdir=outdir, name="rsfof_nBJet20", xaxis="N_{b}")
