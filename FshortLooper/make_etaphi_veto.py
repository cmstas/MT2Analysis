import ROOT
import os

ROOT.gROOT.SetBatch(True)
ROOT.gErrorIgnoreLevel = ROOT.kError
ROOT.gStyle.SetOptStat(False)

simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(600,600)
simplecanvas.SetTicks(2,2)
simplecanvas.SetLeftMargin(0.14)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetRightMargin(0.12)
simplecanvas.cd()

os.system("mkdir -p pngs_vetohist")

def SetVetoBins(th1, rectangles):
    for rectangle in rectangles:
        left = rectangle[0]
        right = rectangle[1]
        down = rectangle[2]
        up = rectangle[3]
        leftbin = th1.GetXaxis().FindBin(left)
        rightbin = th1.GetXaxis().FindBin(right)
        downbin = th1.GetYaxis().FindBin(down)
        upbin = th1.GetYaxis().FindBin(up)
        for xbin in range(leftbin,rightbin+1):
            for ybin in range(downbin,upbin+1):
                th1.SetBinContent(xbin,ybin,1)
    return th1


veto_hist_16 = ROOT.TH2D("etaphi_veto_16","Vetoed (#eta,#phi) Patches;#eta;#phi",100,-2.4,2.4,100,-ROOT.TMath.Pi(),ROOT.TMath.Pi())

# etamin etamax phimin phimax
veto_xy_16 = [ (-2.4,-2.16,-1.6,-1.35),
               (-2.15,-1.95,-2.62,-2.35),
               (-2.15,-2.0,2.95,ROOT.TMath.Pi()),
               (-1.8,-1.6,-0.55,-0.3),
               (-1.7,-1.5,0.55,0.75),
               (-1.6,-1.4,-ROOT.TMath.Pi(),ROOT.TMath.Pi()),
               (-1.4,-1.28,-0.3,-0.1),
               (-1.38,-1.22,-2.75,-2.6),
               (-1.4,-1.25,-0.25,-0.05),
               (-1.3,-1.1,-1.3,-1.1),
               (-1.18,-1.05,-2.05,-1.85),
               (-0.92,-0.7,-1.8,-1.6),
               (-0.85,-0.65,-2.35,-2.15),
               (-0.95,-0.8,-0.08,0.1),
               (-1.05,-0.85,2.9,3.1),
               (-1.0,-0.8,1.95,2.1),
               (-0.85,-0.65,-0.85,-0.65),
               (-0.6,-0.38,2.65,2.85),
               (-0.4,-0.08,0.05,0.3),
               (-0.35,-0.08,1.6,1.8),
               (-0.22,-0.02,-2.7,-2.5),
               (-0.25,0.0,0.65,0.85),
               (-0.15,0.08,0.8,1.0),
               (0.0,0.22,1.7,1.9),
               (0.8,1.0,0.3,0.5),
               (0.82,1.08,1.62,1.78),
               (0.82,1.0,2.78,2.95),
               (0.9,1.12,0.58,0.75),
               (1.0,1.15,-ROOT.TMath.Pi(),-2.95),
               (1.2,1.35,-0.6,-0.4),
               (1.35,1.5,-2.65,-2.5),
               (1.38,1.5,-0.35,-0.1),
               (1.35,1.5,2.65,2.85),
               (1.4,1.6,-ROOT.TMath.Pi(),ROOT.TMath.Pi()),
               (1.6,1.82,-0.75,-0.48),
               (1.62,1.85,0.65,0.9),
               (1.7,1.9,-2.2,-1.92),

               # extra from Mario
               (0.35,0.6,0.75,0.9), # moved from 2017
               (-1.2,-0.7,-2.9,-2.6),
               (-0.75,-0.6,-1.0,-0.85),
               (1.6,1.75,2.5,2.65),
               (2.3,2.4,-0.5,-0.3),
               (2.3,2.4,0.3,0.55)
               ]

extra_veto_xy_17 = [ (-2.1,-1.9,0.3,0.5),
                     (-0.95,-0.74,1.55,1.72),
                     # moved to 2016
                     (0.0,0.32,2.2,2.52),
                     (0.38,0.8,2.5,2.68),

                     # extra from Mario
                     (0.4,0.55,-0.75,-0.55),
                     (0.55,0.72,-1.1,-0.92),
                     (0.1,0.25,1.95,2.1)
                     ]

#extra_veto_xy_18 = [ (2.0, 2.45, 1.7, 1.9) ]
extra_veto_xy_18 = [ (-1.3,-1.1,2.7,2.9),
                     # HEM
                     (-2.4,-1.4,-1.6,-0.9),
                     # Tag and Probe
                     (-0.82,-0.65,-2.05,-1.8),
                     (-0.55,-0.4,-2.05,-1.8),
                     (-0.4,-0.2,-0.35,-0.1),
                     (-0.5,-0.3,0.5,0.7),
                     (-0.15,0.1,-0.6,-0.4),
                     (0.8,1.0,2.55,2.8)
                     ]

veto_hist_16 = SetVetoBins(veto_hist_16,veto_xy_16)

outfile = ROOT.TFile("veto_etaphi.root","RECREATE")
outfile.cd()

veto_hist_16.Write()
simplecanvas.cd()
veto_hist_16.Draw("col")
simplecanvas.SaveAs("pngs_vetohist/etaphi_veto_16.png")

veto_hist_17 = veto_hist_16.Clone("etaphi_veto_17")
veto_hist_17 = SetVetoBins(veto_hist_17,extra_veto_xy_17)
outfile.cd()
veto_hist_17.Write()
veto_hist_17.Draw("col")
simplecanvas.SaveAs("pngs_vetohist/etaphi_veto_17.png")

veto_hist_18 = veto_hist_17.Clone("etaphi_veto_18")
veto_hist_18 = SetVetoBins(veto_hist_18,extra_veto_xy_18)
outfile.cd()
veto_hist_18.Write()
veto_hist_18.Draw("col")
simplecanvas.SaveAs("pngs_vetohist/etaphi_veto_18.png")
