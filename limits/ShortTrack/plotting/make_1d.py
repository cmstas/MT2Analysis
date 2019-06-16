# Rework of combine limit extraction

import ROOT
import re
from math import sqrt
from sys import argv,exit
import os
from array import array
import ppmUtils as utils

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(1000,1000)
simplecanvas.SetTicks(1,1)
simplecanvas.SetLeftMargin(0.16)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetRightMargin(0.16)
simplecanvas.SetLogz(True)

ROOT.gStyle.SetLegendTextSize(0.02)
ROOT.gStyle.SetPaintTextFormat("#2.2g")
ROOT.gStyle.SetPalette(ROOT.kLightTemperature)
ROOT.gStyle.SetLegendBorderSize(0)

verbose = True

printstyle = "colz"
#printstyle = "text"

model = argv[1]
intag = argv[2]

include_classic = False
one_fold = False

tclassic = ROOT.TLegend(0.22,0.12,0.46,0.26)
if include_classic:
    tl = ROOT.TLegend(0.54,0.12,0.78,0.26)
else:
    tl = ROOT.TLegend(0.22,0.12,0.78,0.26)
    tl.SetNColumns(2)


index_1fold = model.find("_1fold")
if index_1fold >= 0:
    model = model[:index_1fold]
    one_fold = True

mPrimary = 2500
mLSP = 1500
if len(argv) >= 5:
    mPrimary = int(argv[3])
    mLSP = int(argv[4])

tokens = model.split("_")
particles = tokens[0]

rescale = 1.0
if model.find("T1") >= 0:
    primary = "gluino"
    f_classic = ROOT.TFile.Open("/home/users/bemarsh/public_html/mt2/limits/V00-10-16_FullRunII_newQCD/T1qqqq_results.root")    
elif model.find("T2bt") >= 0:
    primary = "stop"
    f_classic = ROOT.TFile.Open("/home/users/bemarsh/public_html/mt2/limits/V00-10-16_FullRunII_newQCD/T2tt_results.root")
else:
    primary = "squark"
    f_classic = ROOT.TFile.Open("/home/users/bemarsh/public_html/mt2/limits/V00-10-16_FullRunII_newQCD/"+("T2qq_results.root" if not one_fold else "T2qq_results_OneFold.root"))
    rescale = 1/0.8 if not one_fold else 10

f_xsec = ROOT.TFile.Open("~/2018/MT2AnalysisPreapproval/babymaker/data/xsec_susy_13tev_run2.root")
h_xsec = f_xsec.Get("h_xsec_"+primary)
xsec_bin = h_xsec.GetXaxis().FindBin(mPrimary)
xsec = "%#.2g" % (h_xsec.GetBinContent(xsec_bin) * 1000/rescale)
xsec_err = "%#.2g" % (h_xsec.GetBinError(xsec_bin) * 1000/rescale)
f_xsec.Close()

h_obs_classic = f_classic.Get("obs_mu")
h_exp_classic = f_classic.Get("exp_mu")
h_ep1_classic = f_classic.Get("ep1s_mu")
h_ep2_classic = f_classic.Get("ep2s_mu")
h_em1_classic = f_classic.Get("em1s_mu")
h_em2_classic = f_classic.Get("em2s_mu")
binX = h_obs_classic.GetXaxis().FindBin(mPrimary)
binY = h_obs_classic.GetYaxis().FindBin(mLSP)
classic_obs = h_obs_classic.GetBinContent(binX,binY)
classic_exp = h_exp_classic.GetBinContent(binX,binY)
classic_ep2 = h_ep2_classic.GetBinContent(binX,binY)
classic_ep1 = h_ep1_classic.GetBinContent(binX,binY)
classic_em1 = h_em1_classic.GetBinContent(binX,binY)
classic_em2 = h_em2_classic.GetBinContent(binX,binY)
f_classic.Close()

ctaus=[1, 5, 10, 30, 50, 100, 150, 200, 300, 500, 1000]
#ctaus=[1, 5, 10, 30, 50, 100, 150, 200, 300, 500] if model.find("T1qqqq") >= 0 else [1, 5, 10, 30, 50, 100, 150, 200, 300, 500, 1000] # no 1000, for T1qqqq
ctaus_loop = list(ctaus)
exp=[]
expm1=[]
expp1=[]
expm2=[]
expp2=[]
obs=[]

for ctau in ctaus_loop:
    indir="/hadoop/cms/store/user/dpgilber/ShortTrackScanLimits/{}/{}_{}".format(intag,model,ctau)
    path = "{}/limit_{}_{}.root".format(indir,mPrimary,mLSP)

    if ctau == 1 and model.find("T2bt") >= 0 and mPrimary == 1300 and mLSP == 700:
        ctaus.remove(1)
        continue

    if ROOT.gSystem.AccessPathName(path):
        if model.find("T1qqqq") >= 0: 
            if False and mPrimary == 1600 and mLSP == 1575 and ctau == 1:
                obs.append(6105)
                expm1.append(1930)
                exp.append(4000)
                expp1.append(8050)
                expm2.append(1125)
                expp2.append(12131)
            elif mPrimary == 1600 and mLSP == 1575 and ctau == 5:
                obs.append(6.08)
                expm1.append(4.85)
                exp.append(7.62)
                expp1.append(12.4)
                expm2.append(3.30)
                expp2.append(19.7)
            elif mPrimary == 1600 and mLSP == 1575 and ctau == 30:
                obs.append(0.2024)
                expm1.append(0.2062)
                exp.append(0.3340)
                expp1.append(0.5523)
                expm2.append(0.137)
                expp2.append(0.874)
            elif mPrimary == 1600 and mLSP == 1575 and ctau == 70:
                obs.append(0.106)
                expm1.append(0.0976)
                exp.append(0.1641)
                expp1.append(0.2831)
                expm2.append(0.0628)
                expp2.append(0.4635)
            elif mPrimary == 1600 and mLSP == 1575 and ctau == 500:
                obs.append(0.153)
                expm1.append(0.1273)
                exp.append(0.2158)
                expp1.append(0.3793)
                expm2.append(0.0809)
                expp2.append(0.6411)
            elif mPrimary == 1800 and mLSP == 200 and ctau == 100:
                obs.append(0.0531)
                expm1.append(0.0279)
                exp.append(0.0508)
                expp1.append(0.0949)
                expm2.append(0.0163)
                expp2.append(0.154)
            else:
                if verbose: print "skipping {0},{1},{2}".format(mPrimary,mLSP,ctau)
                ctaus.remove(ctau)
        else:
            if verbose: print "skipping {0},{1},{2}".format(mPrimary,mLSP,ctau)
            ctaus.remove(ctau)
        continue

    f = ROOT.TFile(path,"READ")

    tree = f.Get("limit")

    if verbose:
        print "ctau:",ctau

    if tree == None: 
        if verbose: print "No tree for "+path
        f.Close()
        continue 

    tree.GetEntry(5)
    obs_limit = rescale*tree.limit
    if verbose: print "Obs", obs_limit
    obs.append(obs_limit)

    tree.GetEntry(2)
    exp_limit = rescale*tree.limit
    if verbose: print "Exp", exp_limit
    exp.append(exp_limit)

    tree.GetEntry(0)
    exp_limit_m2 = rescale*tree.limit
    expm2.append(exp_limit_m2)

    tree.GetEntry(1)
    exp_limit_m1 = rescale*tree.limit
    expm1.append(exp_limit_m1)
    
    tree.GetEntry(3)
    exp_limit_p1 = rescale*tree.limit
    expp1.append(exp_limit_p1)

    tree.GetEntry(4)
    exp_limit_p2 = rescale*tree.limit
    expp2.append(exp_limit_p2)

    if obs_limit == 0 or exp_limit == 0 or exp_limit_p1 == 0 or exp_limit_m1 == 0 or exp_limit > 100:
        print "Removing limit for ctau",ctau
        ctaus.remove(ctau)
        obs.pop()
        exp.pop()
        expp1.pop()
        expm1.pop()
        expp2.pop()
        expm2.pop()
    f.Close()

print "ctaus:",ctaus,"length",len(ctaus)
print "obs:",obs,"length",len(obs)
print "exp:",exp,"length",len(exp)
print "exp+1:",expp1,"length",len(expp1)
print "exp-1:",expm1,"length",len(expm1)
print "exp+2:",expp2,"length",len(expp2)
print "exp-2:",expm2,"length",len(expm2)

gr_obs = ROOT.TGraph(len(ctaus),array('d',ctaus),array('d',obs))
gr_exp = ROOT.TGraph(len(ctaus),array('d',ctaus),array('d',exp))

gr_obs_classic = ROOT.TGraph(len(ctaus),array('d',ctaus),array('d',[classic_obs]*len(ctaus)))
gr_exp_classic = ROOT.TGraph(len(ctaus),array('d',ctaus),array('d',[classic_exp]*len(ctaus)))

yp_2s_classic = array('d',[classic_ep2 - classic_exp] * len(ctaus))
yp_1s_classic = array('d',[classic_ep1 - classic_exp] * len(ctaus))
ym_2s_classic = array('d',[classic_exp - classic_em2] * len(ctaus))
ym_1s_classic = array('d',[classic_exp - classic_em1] * len(ctaus))
x_errs = array('d',[0.0]*len(ctaus))

gr_1s_classic = ROOT.TGraphAsymmErrors(len(ctaus),array('d',ctaus),array('d',[classic_exp]*len(ctaus)),x_errs,x_errs,ym_1s_classic,yp_1s_classic)
gr_1s_classic.SetFillColor(ROOT.kBlue-2)
gr_1s_classic.SetLineColor(0)
gr_2s_classic = ROOT.TGraphAsymmErrors(len(ctaus),array('d',ctaus),array('d',[classic_exp]*len(ctaus)),x_errs,x_errs,ym_2s_classic,yp_2s_classic)
gr_2s_classic.SetFillColor(ROOT.kCyan)
gr_2s_classic.SetLineColor(0)

gr_exp.SetLineStyle(7)
gr_exp.SetLineWidth(3)

gr_exp_classic.SetLineStyle(7)
gr_exp_classic.SetLineWidth(3)

yp_2s = array('d',[ expp2[i] - exp[i] for i in range(len(exp)) ])
yp_1s = array('d',[ expp1[i] - exp[i] for i in range(len(exp)) ])
ym_2s = array('d',[ exp[i] - expm2[i] for i in range(len(exp)) ])
ym_1s = array('d',[ exp[i] - expm1[i] for i in range(len(exp)) ])

gr_1s = ROOT.TGraphAsymmErrors(len(ctaus),array('d',ctaus),array('d',exp),x_errs,x_errs,ym_1s,yp_1s)
gr_1s.SetFillColor(ROOT.kGreen+1)
gr_1s.SetLineColor(0)
gr_2s = ROOT.TGraphAsymmErrors(len(ctaus),array('d',ctaus),array('d',exp),x_errs,x_errs,ym_2s,yp_2s)
gr_2s.SetFillColor(ROOT.kOrange-2)
gr_2s.SetLineColor(0)

gr_expp1 = ROOT.TGraph(len(ctaus),array('d',ctaus),array('d',expp1))
gr_expm1 = ROOT.TGraph(len(ctaus),array('d',ctaus),array('d',expm1))

simplecanvas.SetLogy()
simplecanvas.SetLogx()
gr_obs.SetMinimum(min(obs+expm2+[classic_em2])/20)
gr_obs.SetMaximum(max([classic_ep2*50,max(expp2+obs)*20,3]))

gr_obs.GetXaxis().SetLimits(ctaus[0],ctaus[len(ctaus)-1])

gr_obs.SetLineColor(ROOT.kBlack)
gr_obs.SetLineWidth(3)
gr_exp.SetLineColor(ROOT.kBlack)
gr_exp.SetLineWidth(3)

gr_obs_classic.SetLineColor(ROOT.kRed-7)
gr_obs_classic.SetLineWidth(3)
gr_exp_classic.SetLineColor(ROOT.kRed-7)
gr_exp_classic.SetLineWidth(3)

gr_1 = ROOT.TGraph(len(ctaus),array('d',ctaus),array('d',[1]*len(ctaus)))
gr_1.SetLineColor(ROOT.kGray+1)
gr_1.SetLineWidth(3)
gr_1.SetLineStyle(2)

#gr_obs.SetTitle("{} Limit, ({},{}) GeV".format(model,mPrimary,mLSP))
gr_obs.SetTitle("")
gr_obs.GetXaxis().SetTitle("c#tau_{0}(#tilde{#chi}_{1}^{#pm}) [cm]")
gr_obs.GetXaxis().SetTitleOffset(1.25)
gr_obs.GetXaxis().SetLabelOffset(0.0015)
gr_obs.GetYaxis().SetTitle("#sigma_{limit}/#sigma_{theory}")

if include_classic: 
    tl.SetHeader("Disappearing tracks","C")
    tl.AddEntry(gr_obs,"Observed limit (95% CL)", "l")
    tl.AddEntry(gr_exp,"Median expected limit", "l")
    tl.AddEntry(gr_1s,"68% expected","f")
    tl.AddEntry(gr_2s,"95% expected","f")
else:
    tl.AddEntry(gr_obs,"Observed limit (95% CL)", "l")
    tl.AddEntry(gr_1s,"68% expected","f")
    tl.AddEntry(gr_exp,"Median expected limit", "l")
    tl.AddEntry(gr_2s,"95% expected","f")

tclassic.SetHeader("Inclusive search","C")
tclassic.AddEntry(gr_obs_classic,"Observed limit (95% CL)", "l")
tclassic.AddEntry(gr_exp_classic,"Median expected limit", "l")
tclassic.AddEntry(gr_1s_classic,"68% expected", "f")
tclassic.AddEntry(gr_2s_classic,"95% expected", "f")

gr_obs.Draw("AL")
if include_classic:
    gr_2s_classic.Draw("3")
    gr_1s_classic.Draw("3")
gr_2s.Draw("3")
gr_1s.Draw("3")
if include_classic:
    gr_exp_classic.Draw("L")
    gr_obs_classic.Draw("L")
gr_exp.Draw("L")
tl.Draw()
if include_classic: tclassic.Draw()


wino_text = ROOT.TLatex(0.55,0.82,"#splitline{Wino-like #tilde{#chi}^{#pm}_{1} - #tilde{#chi}^{0}_{1}}{m_{#tilde{#chi}^{#pm}_{1}} = m_{#tilde{#chi}^{0}_{1}} + O(100 MeV)}")
wino_text.SetNDC()
wino_text.SetLineWidth(3)
wino_text.SetTextSize(0.022)
wino_text.SetTextFont(42)
wino_text.Draw("same")

if model.find("T1qqqq") >= 0:
    susy = "#tilde{g}"
elif model.find("T2qq") >= 0:
    susy = "#tilde{q}"
else:
    susy = "#tilde{t}"

mass_text = ROOT.TLatex(0.55,0.74,"#splitline{m_{"+susy+"} = "+str(mPrimary)+" GeV}{m_{#tilde{#chi}^{0}_{1}} = "+str(mLSP)+" GeV}")
mass_text.SetNDC()
mass_text.SetLineWidth(3)
mass_text.SetTextSize(0.022)
mass_text.SetTextFont(42)
mass_text.Draw("same")

xsec_text = ROOT.TLatex(0.55,0.70,"#sigma_{theory} = "+xsec+" #pm "+xsec_err+" fb")
xsec_text.SetNDC()
xsec_text.SetLineWidth(3)
xsec_text.SetTextSize(0.022)
xsec_text.SetTextFont(42)
xsec_text.Draw("same")

if model.find("T1qqqq") >= 0:
    process_text = ROOT.TLatex(0.2,0.845,"pp #rightarrow #tilde{g} #tilde{g}")
    branching_text = ROOT.TLatex(0.2,0.785,"#splitline{BR(#tilde{g} #rightarrow q #bar{q} #tilde{#chi}^{0}_{1}) = #frac{1}{3}}{#splitline{BR(#tilde{g} #rightarrow q_{d} #bar{q}_{u} #tilde{#chi}^{+}_{1}, #tilde{#chi}^{+}_{1} #rightarrow #pi^{+} #tilde{#chi}^{0}_{1}) = #frac{1}{3}}{BR(#tilde{g} #rightarrow q_{u} #bar{q}_{d} #tilde{#chi}^{-}_{1}, #tilde{#chi}^{-}_{1} #rightarrow #pi^{-} #tilde{#chi}^{0}_{1}) = #frac{1}{3}}}")
    q_text = ROOT.TLatex(0.2,0.68,"(q = u, d, s, or c)")
    q_text.SetNDC()
    q_text.SetLineWidth(2)
    q_text.SetTextSize(0.022)
    q_text.SetTextFont(42)
    q_text.Draw("same")
elif model.find("T2qq") >= 0:
    process_text = ROOT.TLatex(0.2,0.845,"pp #rightarrow #tilde{q} #bar{#tilde{q}}")
    if one_fold:
        nsquark_text = ROOT.TLatex(0.2,0.70,"One light #tilde{q}")
    else:
        nsquark_text = ROOT.TLatex(0.2,0.70,"#tilde{q} = #tilde{q}_{R} + #tilde{q}_{L} (#tilde{u}, #tilde{d}, #tilde{s}, #tilde{c})")
    nsquark_text.SetNDC()
    nsquark_text.SetLineWidth(2)
    nsquark_text.SetTextSize(0.022)
    nsquark_text.SetTextFont(42)
    nsquark_text.Draw("same")
    branching_text = ROOT.TLatex(0.2,0.785,"#splitline{BR(#tilde{q} #rightarrow q #tilde{#chi}^{0}_{1}) = #frac{1}{2}}{BR(#tilde{q} #rightarrow q' #tilde{#chi}^{#pm}_{1}, #tilde{#chi}^{#pm}_{1} #rightarrow #pi^{#pm} #tilde{#chi}^{0}_{1}) = #frac{1}{2}}")
    q_text = ROOT.TLatex(0.2,0.73,"(q = u, d, s, or c)")
    q_text.SetNDC()
    q_text.SetLineWidth(2)
    q_text.SetTextSize(0.022)
    q_text.SetTextFont(42)
    q_text.Draw("same")        
elif model.find("T2bt") >= 0:
    process_text = ROOT.TLatex(0.2,0.845,"pp #rightarrow #tilde{t} #bar{#tilde{t}}")
    branching_text = ROOT.TLatex(0.2,0.785,"#splitline{BR(#tilde{t} #rightarrow t #tilde{#chi}^{0}_{1}) = #frac{1}{2}}{BR(#tilde{t} #rightarrow b #tilde{#chi}^{#pm}_{1}, #tilde{#chi}^{#pm}_{1} #rightarrow #pi^{#pm} #tilde{#chi}^{0}_{1}) = #frac{1}{2}}")

process_text.SetNDC()
process_text.SetTextSize(0.022)
process_text.SetTextFont(42)
process_text.SetLineWidth(2)
branching_text.SetNDC()
branching_text.SetTextSize(0.022)
branching_text.SetTextFont(42)
branching_text.SetLineWidth(2)

process_text.Draw("SAME")
branching_text.Draw("SAME")

gr_obs.Draw("L same")
gr_1.Draw("SAME")

simplecanvas.RedrawAxis()

utils.CMS_Style(simplecanvas,lumi="137 fb^{-1}",cmsTextSize=0.30,lumiTextSize=0.30,relPosX=0.08,cmsTextOffset = 0.08, lumiTextOffset = 0.15)

simplecanvas.SaveAs("pngs/{}_{}_{}_1d.png".format(argv[1],mPrimary,mLSP))
simplecanvas.SaveAs("pdfs/{}_{}_{}_1d.pdf".format(argv[1],mPrimary,mLSP))
