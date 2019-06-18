# Rework of combine limit extraction

import ROOT
import re
from math import sqrt,log,exp
from sys import argv,exit
from array import array
import os
import ppmUtils as utils

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
ROOT.gStyle.SetPaintTextFormat("#2.2g")
simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(1000,1000)
simplecanvas.SetTicks(1,1)
simplecanvas.SetLeftMargin(0.16)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetRightMargin(0.16)
simplecanvas.SetLogx(True)
#simplecanvas.SetLogz(True)
#ROOT.gStyle.SetPalette(ROOT.kLightTemperature)

ROOT.gStyle.SetLegendTextSize(0.025)
ROOT.gStyle.SetPaintTextFormat("#2.2g")
ROOT.gStyle.SetPalette(ROOT.kLightTemperature)
ROOT.gStyle.SetLegendBorderSize(0)
tl = ROOT.TLegend(0.31,0.20,0.78,0.38)
tl.SetTextFont(42)
tl.SetTextSize(0.036)
tl.SetLineColor(ROOT.kWhite)
tl.SetShadowColor(ROOT.kWhite)
tl.SetFillColor(ROOT.kWhite)

verbose = False

printstyle = "colz"
#printstyle = "text"

model = argv[1]
intag = argv[2]
fixMglu = -1
if len(argv) > 3: 
    fixMglu = int(argv[3])
    print "Fixing Mglu to {}".format(fixMglu)

one_fold = True

#ctaus = [1,5,10,30,50,100,150,200,300,500,1000,2000]
# 300 is icky for T1qqqq
# 200 is icky for T2qq 8fold
# 150 and 500 are icky for T2qq 1-fold
if model.find("T1qqqq") >= 0:
    ctaus = [5,10,30,50,100,150,200,500,1000,2000] 
#    ctaus = [10,2000]
    ctaus_prepend = [1,3.4]
    one_fold = False
elif model.find("T2qq") >= 0:
    if one_fold:
        ctaus_prepend = [1,2.2]
        ctaus=[5,10,30,50,100,200,300,1000,2000]
    else:
        ctaus_prepend = [1,2.2]
        ctaus=[5,10,30,50,100,150,300,500,1000,2000]
elif model.find("T2bt") >= 0:
    one_fold = False
    ctaus_prepend = [1,3.2] #[1+0.1*x for x in range(23)]
#    ctaus=[5,10,30,50,100,150,200,300,500,1000,2000]
    ctaus=[5,10,30,50,100,200,300,500,1000,2000] # omit 150
else:
    print "Didn't recognize model",model
    exit()

ctaus_graph = ctaus_prepend + ctaus #if model.find("T1qqqq") >= 0 else [1]

obs_list = []
exp_list = []
expp1_list = []
expp2_list = []
expm1_list = []
expm2_list = []

for i in range(len(ctaus_prepend)):
    if model.find("T1qqqq") >= 0:
        obs_list.append(1193.26)
        exp_list.append(1132.32)
        expp1_list.append(1076.29)
        expp2_list.append(1020.65)
        expm1_list.append(1191.16)
        expm2_list.append(1197.10)
    elif model.find("T2qq") >= 0:
        if one_fold:
            obs_list.append(512.1055)
            exp_list.append(470.47)
            expp1_list.append(391.45)
            expp2_list.append(339.13)
            expm1_list.append(514.10)
            expm2_list.append(515.93)
        else:
            obs_list.append(849.52)
            exp_list.append(777.33)
            expp1_list.append(694.24)
            expp2_list.append(641.84)
            expm1_list.append(851.97)
            expm2_list.append(853.92)
    elif model.find("T2bt") >= 0:
            obs_list.append(574.105)
            exp_list.append(519.13)
            expp1_list.append(447.52)
            expp2_list.append(361.74)
            expm1_list.append(579.84)
            expm2_list.append(619.30)

rescale = 1
if model.find("T1") >= 0:
    primary = "gluino"
    inc = 25
    one_fold = False
elif model.find("T2bt") >= 0:
    primary = "stop"
    inc = 50
    one_fold = False
else:
    primary = "squark"
    inc = 25
    rescale = 10 if one_fold else 1/0.8

ymin = 100
ymax = 2500

print primary
print "inc",inc
print "ymin",ymin
print "ymax",ymax
print ""

h_obs = ROOT.TH2D("h_obs_{}".format(model),"Observed Exclusion;c#tau_{0} (cm);M_{#tilde{#chi}^{0}_{1}} (GeV)", 12000, 0.5, 3000.5, int(round(ymax-ymin)/inc)+1, ymin - inc/2, ymax + inc/2)
h_exp = ROOT.TH2D("h_exp_{}".format(model),"Expected Exclusion;c#tau_{0} (cm);M_{#tilde{#chi}^{0}_{1}} (GeV)", 12000, 0.5, 3000.5, int(round(ymax-ymin)/inc)+1, ymin - inc/2, ymax + inc/2)
h_expp1 = ROOT.TH2D("h_expp1_{}".format(model),"Expected Exclusion +1#sigma;c#tau_{0} (cm);M_{LSP} (GeV)", 12000, 0.5, 3000.5, int(round(ymax-ymin)/inc)+1, ymin - inc/2, ymax + inc/2)
h_expp2 = ROOT.TH2D("h_expp2_{}".format(model),"Expected Exclusion +2#sigma;c#tau_{0} (cm);M_{LSP} (GeV)", 12000, 0.5, 3000.5, int(round(ymax-ymin)/inc)+1, ymin - inc/2, ymax + inc/2)
h_expm1 = ROOT.TH2D("h_expm1_{}".format(model),"Expected Exclusion -1#sigma;c#tau_{0} (cm);M_{LSP} (GeV)", 12000, 0.5, 3000.5, int(round(ymax-ymin)/inc)+1, ymin - inc/2, ymax + inc/2)
h_expm2 = ROOT.TH2D("h_expm2_{}".format(model),"Expected Exclusion -2#sigma;c#tau_{0} (cm);M_{LSP} (GeV)", 12000, 0.5, 3000.5, int(round(ymax-ymin)/inc)+1, ymin - inc/2, ymax + inc/2)

def interpolate_limits(m1, m2, limit, ctau, index, recursed = False):
    if recursed: print m1,m2
    indir="/hadoop/cms/store/user/dpgilber/ShortTrackScanLimits/{}/{}_{}".format(intag,model,ctau) if ctau != 2000 else "/hadoop/cms/store/user/dpgilber/ShortTrackScanLimits/{}/{}_{}".format(intag,model,1000)
    files = os.listdir(indir)
    exclusion = 1 if ctau != 2000 else 0.5
#    if index < 2 or index == 5:
    if True:       
        for trial_m2 in [int(round(m2+(inc*x))) for x in range( 1,int(round(((m1-m2)/inc))))]:            
            trial_filename = "limit_{}_{}.root".format(m1,trial_m2)
            if trial_filename not in files: continue
            trial_file = ROOT.TFile.Open(indir+"/"+trial_filename)
            tree = trial_file.Get("limit")
            if tree.GetEntries() < 6:
                print index,"Next file tree with (m1,m2)=({},{})".format(m1,trial_m2),"is malformed, just using m2 = ",m2,"directly with limit",limit
                trial_file.Close()
                return m2
            else:
                tree.GetEntry(index)
                next_limit = tree.limit * rescale                     
                if next_limit < exclusion:
                    print "WARNING: next_limit < exclusion, recursing with",trial_m2,"and",next_limit
                    return interpolate_limits(m1,trial_m2,next_limit,ctau,index,True)
                trial_file.Close()
                slope = (next_limit-limit)/(trial_m2-m2)
                # exclusion_val = slope * (output_mass-max_mass) + max_limit => output_mass = (1 - max_limit)/slope + max_mass
                result = (exclusion-limit)/slope + m2
                print index,"Obtained",result,"starting from (m1,m2)=({},{})".format(m1,m2),"with limit",limit,"and extrapolating to",trial_m2,"with limit",next_limit
                if result > m1:
                    print "WARNING: result > m1, recursing with",trial_m2,"and",next_limit
                    return interpolate_limits(m1,trial_m2,next_limit,ctau,index,True)
                return result
        print index,"No file beyond (m1,m2)=({},{})".format(m1,m2),"available, instead extrapolating slope from below if there's space"
        if m1 - m2 == inc:
            print index,"No space, extrapolating with m1 instead"
            trial_m2 = m1+75
            trial_filename = "limit_{}_{}.root".format(m1+100,trial_m2)
            if trial_filename not in files: 
                print "Couldn't, just use m2",m2,"directly, limit =",limit
                return m2
            trial_file = ROOT.TFile.Open(indir+"/"+trial_filename)
            tree = trial_file.Get("limit")
            if tree.GetEntries() < 6:
                print "Couldn't (bad tree), just use m2",m2,"directly, limit =",limit
                trial_file.Close()
                return m2
            else:
                tree.GetEntry(index)
                next_limit = tree.limit * rescale                     
                trial_file.Close()
                slope = (next_limit-limit)/(trial_m2-m2)
                # exclusion_val = slope * (output_mass-max_mass) + max_limit => output_mass = (1 - max_limit)/slope + max_mass
                if slope <= 0:
                    print index,"Bad extrapolated slope",slope,"from m1 extrapolation,just using m2 =",m2,"directly with limit",limit
                    return m2
                print "Succesfully extrapolated using m1 direction as well.",(exclusion-limit)/slope + m2,"from",m2,limit,"and next_limit",next_limit
                return (exclusion-limit)/slope + m2
    if False:
        for trial_m2 in reversed(range(100,m2,inc)):
            trial_filename = "limit_{}_{}.root".format(m1,trial_m2)
            if trial_filename not in files: continue
            trial_file = ROOT.TFile.Open(indir+"/"+trial_filename)
            tree = trial_file.Get("limit")
            if tree.GetEntries() < 6:
                print index,"Preceding file tree with (m1,m2)=({},{})".format(m1,trial_m2),"is malformed, just using m2 =",m2,"directly with limit",limit
                trial_file.Close()
                return m2
            else:
                tree.GetEntry(index)
                next_limit = tree.limit * rescale                     
                if index > 2 and next_limit < 1:
                    print "WARNING: next_limit < exclusion, recursing with",trial_m2,"and",next_limit
                trial_file.Close()
                slope = (next_limit-limit)/(trial_m2-m2)
                # exclusion_val = slope * (output_mass-max_mass) + max_limit => output_mass = (1 - max_limit)/slope + max_mas
                print index,"From limit",limit,"and mass ",m2,"extrapolation with preceding point",trial_m2,next_limit,"gives",((1 if ctau != 2000 else 0.5)-limit)/slope + m2
                if slope <= 0:
                    print index,"Bad extrapolated slope",slope,"just using m2 =",m2,"directly with limit",limit
                    return m2
                return (exclusion-limit)/slope + m2
    print index,"Interpolation could not find a valid file along",m1,"so just applying m2 =",m2,"directly with limit",limit
    return m2

for ctau in ctaus:
    print "ctau: ",ctau
    binX = h_obs.GetXaxis().FindBin(ctau)
    indir="/hadoop/cms/store/user/dpgilber/ShortTrackScanLimits/{}/{}_{}".format(intag,model,ctau) if ctau != 2000 else "/hadoop/cms/store/user/dpgilber/ShortTrackScanLimits/{}/{}_{}".format(intag,model,1000)
    exclusion = 1 if ctau != 2000 else 0.5 # take a limit of 0.5 at 1000 cm as an exclusion also at 2000 cm
    files = os.listdir(indir)
    obs_max = 0
    exp_max = 0
    expp1_max = 0
    expp2_max = 0
    expm1_max = 0
    expm2_max = 0
    obs_m1 = 0
    exp_m1 = 0
    expp1_m1 = 0
    expp2_m1 = 0
    expm1_m1 = 0
    expm2_m1 = 0
    m2 = ymin
    while m2 <= ymax-200:
        binY = h_obs.GetYaxis().FindBin(m2)
        m2_round = int(round(m2))
        for m1 in [int(round(m2+inc*x)) for x in range( int (round((3000-m2)/inc)) ) ] if fixMglu < 0 else [fixMglu]:
            filename = "limit_{}_{}.root".format(m1,m2)

            if filename not in files: continue

            f = ROOT.TFile(indir+"/"+filename,"READ")
            tree = f.Get("limit")

            if tree == None: 
                if verbose: print "No tree for "+indir+"/"+filename+", removing"
                os.remove(indir+"/"+filename)
                f.Close()
                continue 
            elif tree.GetEntries() < 6:
                if verbose: print "Missing entries for "+indir+"/"+filename+", removing"
                os.remove(indir+"/"+filename)
                f.Close()
                continue

            tree.GetEntry(5)
            obs_limit = tree.limit
            if verbose: print "Obs", obs_limit

            if obs_limit > 0 and rescale * obs_limit < exclusion:
                obs_max = m2
                obs_m1 = m1
                obs_max_limit = rescale * obs_limit
                h_obs.SetBinContent(binX,binY,1)

            tree.GetEntry(2)
            exp_limit = tree.limit
            if verbose: print "Exp", exp_limit

            if exp_limit > 0 and rescale*exp_limit < exclusion:
                exp_max = m2
                exp_m1 = m1
                exp_max_limit = rescale*exp_limit
                h_exp.SetBinContent(binX,binY,1)

                tree.GetEntry(0)
                exp_limit_m2 = tree.limit
                expm2_max_limit = rescale*exp_limit_m2
                                
                tree.GetEntry(1)
                exp_limit_m1 = tree.limit
                expm1_max_limit = rescale*exp_limit_m1

                tree.GetEntry(3)
                exp_limit_p1 = tree.limit
                expp1_max_limit = rescale*exp_limit_p1

                tree.GetEntry(4)
                exp_limit_p2 = tree.limit
                expp2_max_limit = rescale*exp_limit_p2

            f.Close()
        m2 += inc
    obs_list.append( interpolate_limits(obs_m1, obs_max, obs_max_limit, ctau, 5) )
    exp_list.append( interpolate_limits(exp_m1, exp_max, exp_max_limit, ctau, 2) )
    expm2_list.append( interpolate_limits(exp_m1, exp_max, expm2_max_limit, ctau, 0) )
    expm1_list.append( interpolate_limits(exp_m1, exp_max, expm1_max_limit, ctau, 1) )
    expp1_list.append( interpolate_limits(exp_m1, exp_max, expp1_max_limit, ctau, 3) )
    expp2_list.append( interpolate_limits(exp_m1, exp_max, expp2_max_limit, ctau, 4) )

print "ctaus",ctaus_graph
print "obs  ",obs_list
print "exp-2",expm2_list
print "exp-1",expm1_list
print "exp  ",exp_list
print "exp+1",expp1_list
print "exp+2",expp2_list


h_obs.SetMinimum(0)
h_obs.SetMaximum(1)
h_exp.SetMinimum(0)
h_exp.SetMaximum(1)

if one_fold: 
    model += "_1fold"

outfile = ROOT.TFile.Open("{}_2d_anyM{}.root".format(model,primary),"RECREATE") if fixMglu < 0 else ROOT.TFile.Open("{}_2d_M{}_{}.root".format(model,primary,fixMglu),"RECREATE")
outfile.cd()
h_obs.Write()
h_exp.Write()
h_expp1.Write()
h_expp2.Write()
h_expm1.Write()
h_expm2.Write()

h_obs.Draw(printstyle)    
simplecanvas.SaveAs("pngs/{}_2d_obs.png".format(model)) if fixMglu < 0 else simplecanvas.SaveAs("pngs/{}_2d_M{}_{}_obs.png".format(model,primary,fixMglu))
simplecanvas.SaveAs("pdfs/{}_2d_obs.pdf".format(model)) if fixMglu < 0 else simplecanvas.SaveAs("pngs/{}_2d_M{}_{}_obs.pdf".format(model,primary,fixMglu))
h_exp.Draw(printstyle)
simplecanvas.SaveAs("pngs/{}_2d_exp.png".format(model)) if fixMglu < 0 else simplecanvas.SaveAs("pngs/{}_2d_M{}_{}_exp.png".format(model,primary,fixMglu))
simplecanvas.SaveAs("pdfs/{}_2d_exp.pdf".format(model)) if fixMglu < 0 else simplecanvas.SaveAs("pngs/{}_2d_M{}_{}_exp.pdf".format(model,primary,fixMglu))

gr_obs = ROOT.TGraph(len(ctaus_graph),array('d',ctaus_graph),array('d',obs_list))
gr_exp = ROOT.TGraph(len(ctaus_graph),array('d',ctaus_graph),array('d',exp_list))
#gr_expp1 = ROOT.TGraph(len(ctaus_graph),array('d',ctaus_graph),array('d',expp1_list))
#gr_expm1 = ROOT.TGraph(len(ctaus_graph),array('d',ctaus_graph),array('d',expm1_list))
#gr_expp2 = ROOT.TGraph(len(ctaus_graph),array('d',ctaus_graph),array('d',expp2_list))
#gr_expm2 = ROOT.TGraph(len(ctaus_graph),array('d',ctaus_graph),array('d',expm2_list))

yp_2s = array('d',[ exp_list[i] - expp2_list[i] for i in range(len(exp_list)) ])
yp_1s = array('d',[ exp_list[i] - expp1_list[i] for i in range(len(exp_list)) ])
ym_2s = array('d',[ expm2_list[i] - exp_list[i] for i in range(len(exp_list)) ])
ym_1s = array('d',[ expm1_list[i] - exp_list[i] for i in range(len(exp_list)) ])
x_errs = array('d',[0.0]*len(ctaus_graph))

gr_1s = ROOT.TGraphAsymmErrors(len(ctaus_graph),array('d',ctaus_graph),array('d',exp_list),x_errs,x_errs,ym_1s,yp_1s)
gr_1s.SetFillColor(ROOT.kGreen+1)
gr_1s.SetLineColor(0)

gr_2s = ROOT.TGraphAsymmErrors(len(ctaus_graph),array('d',ctaus_graph),array('d',exp_list),x_errs,x_errs,ym_2s,yp_2s)
gr_2s.SetFillColor(ROOT.kOrange-2)
gr_2s.SetLineColor(0)

ROOT.gStyle.SetMarkerSize(1.5)

gr_obs.SetMinimum(0)
if model.find("T1qqqq") >= 0:
    gr_obs.SetMaximum(3000)
elif model.find("T2qq") >= 0:
    if not one_fold:
        gr_obs.SetMaximum(2500)
    else:
        gr_obs.SetMaximum(2000)
elif model.find("T2bt") >= 0:
    gr_obs.SetMaximum(1800)
simplecanvas.SetLogx()

gr_obs.GetXaxis().SetLimits(ctaus_graph[0],ctaus_graph[len(ctaus_graph)-1])

#gr_obs.SetMarkerStyle(20)
#gr_obs.SetMarkerColor(ROOT.kBlack)
gr_obs.SetLineColor(ROOT.kBlack)
gr_obs.SetLineWidth(3)
#gr_exp.SetMarkerStyle(21)
#gr_exp.SetMarkerColor(ROOT.kRed)
#gr_exp.SetLineColor(ROOT.kRed)
#gr_exp.SetLineWidth(3)
#gr_expp1.SetLineColor(ROOT.kRed)
#gr_expp1.SetLineStyle(2)
#gr_expm1.SetLineColor(ROOT.kRed)
#gr_expm1.SetLineStyle(2)
#gr_expp2.SetLineColor(ROOT.kGreen)
#gr_expp2.SetLineStyle(2)
#gr_expm2.SetLineColor(ROOT.kGreen)
#gr_expm2.SetLineStyle(2)

#gr_obs.SetTitle("{} Max Limit".format(model))
gr_obs.SetTitle("")
gr_obs.GetXaxis().SetTitle("c#tau_{0}(#tilde{#chi}_{1}^{#pm}) [cm]")
gr_obs.GetXaxis().SetTitleOffset(1.25)
gr_obs.GetXaxis().SetLabelOffset(0.0015)
gr_obs.GetYaxis().SetTitle("m_{#tilde{#chi}_{1}^{0}} [GeV]")

gr_exp.SetLineStyle(7)
gr_exp.SetLineWidth(3)

tl.AddEntry(gr_obs,"Observed limit (95% CL)", "l")
tl.AddEntry(gr_exp,"Median expected limit", "l")
tl.AddEntry(gr_1s,"68% expected","f")
tl.AddEntry(gr_2s,"95% expected","f")

gr_obs.Draw("AL")
tl.Draw()
gr_2s.Draw("3")
gr_1s.Draw("3")
gr_exp.Draw("L")
#gr_expp1.Draw("L same")
#gr_expm1.Draw("L same")
#gr_expp2.Draw("L same")
#gr_expm2.Draw("L same")
diagX = array('f',[2000, 2000, 1, 1])
diagY = array('f',[0, 91.9, 91.9, 0])
gdiagonal = ROOT.TGraph(4, diagX, diagY)
gdiagonal.SetName("BlankLEP")
gdiagonal.SetFillColorAlpha(ROOT.kGray+2, 1) # transparency = 1
gdiagonal.SetFillStyle(3007)
gdiagonalw = ROOT.TGraph(4, diagX, diagY)
gdiagonalw.SetName("BlankLEPW")
gdiagonalw.SetFillColorAlpha(ROOT.kWhite, 1)                                                                                                                                                         
wino_text = ROOT.TLatex(0.55,0.8,"#splitline{Wino-like #tilde{#chi}^{#pm}_{1} - #tilde{#chi}^{0}_{1}}{m_{#tilde{#chi}^{#pm}_{1}} = m_{#tilde{#chi}^{0}_{1}} + O(100 MeV)}")
wino_text.SetNDC()
wino_text.SetLineWidth(3)
wino_text.SetTextSize(0.026)
wino_text.SetTextFont(42)
wino_text.Draw("same")

if model.find("T1qqqq") >= 0:
    process_text = ROOT.TLatex(0.2,0.83,"pp #rightarrow #tilde{g} #tilde{g}")
    branching_text = ROOT.TLatex(0.2,0.755,"#splitline{BR(#tilde{g} #rightarrow q #bar{q} #tilde{#chi}^{0}_{1}) = #frac{1}{3}}{#splitline{BR(#tilde{g} #rightarrow q_{d} #bar{q}_{u} #tilde{#chi}^{+}_{1}, #tilde{#chi}^{+}_{1} #rightarrow #pi^{+} #tilde{#chi}^{0}_{1}) = #frac{1}{3}}{BR(#tilde{g} #rightarrow q_{u} #bar{q}_{d} #tilde{#chi}^{-}_{1}, #tilde{#chi}^{-}_{1} #rightarrow #pi^{-} #tilde{#chi}^{0}_{1}) = #frac{1}{3}}}")
    q_text = ROOT.TLatex(0.2,0.64,"(q = u, d, s, or c)")
    q_text.SetNDC()
    q_text.SetLineWidth(2)
    q_text.SetTextSize(0.026)
    q_text.SetTextFont(42)
    q_text.Draw("same")
elif model.find("T2qq") >= 0:
    process_text = ROOT.TLatex(0.2,0.83,"pp #rightarrow #tilde{q} #bar{#tilde{q}}")
    if one_fold:
        nsquark_text = ROOT.TLatex(0.2,0.65,"One light #tilde{q}")
    else:
        nsquark_text = ROOT.TLatex(0.2,0.65,"#tilde{q} = #tilde{q}_{R} + #tilde{q}_{L} (#tilde{u}, #tilde{d}, #tilde{s}, #tilde{c})")
    nsquark_text.SetNDC()
    nsquark_text.SetLineWidth(2)
    nsquark_text.SetTextSize(0.026)
    nsquark_text.SetTextFont(42)
    nsquark_text.Draw("same")
    branching_text = ROOT.TLatex(0.2,0.755,"#splitline{BR(#tilde{q} #rightarrow q #tilde{#chi}^{0}_{1}) = #frac{1}{2}}{BR(#tilde{q} #rightarrow q' #tilde{#chi}^{#pm}_{1}, #tilde{#chi}^{#pm}_{1} #rightarrow #pi^{#pm} #tilde{#chi}^{0}_{1}) = #frac{1}{2}}")
    q_text = ROOT.TLatex(0.2,0.68,"(q = u, d, s, or c)")
    q_text.SetNDC()
    q_text.SetLineWidth(2)
    q_text.SetTextSize(0.026)
    q_text.SetTextFont(42)
    q_text.Draw("same")        
elif model.find("T2bt") >= 0:
    process_text = ROOT.TLatex(0.2,0.83,"pp #rightarrow #tilde{t} #bar{#tilde{t}}")
    branching_text = ROOT.TLatex(0.2,0.755,"#splitline{BR(#tilde{t} #rightarrow t #tilde{#chi}^{0}_{1}) = #frac{1}{2}}{BR(#tilde{t} #rightarrow b #tilde{#chi}^{#pm}_{1}, #tilde{#chi}^{#pm}_{1} #rightarrow #pi^{#pm} #tilde{#chi}^{0}_{1}) = #frac{1}{2}}")

process_text.SetNDC()
process_text.SetTextSize(0.026)
process_text.SetTextFont(42)
process_text.SetLineWidth(2)
branching_text.SetNDC()
branching_text.SetTextSize(0.026)
branching_text.SetTextFont(42)
branching_text.SetLineWidth(2)

process_text.Draw("SAME")
branching_text.Draw("SAME")


ldiagonal = ROOT.TLine(0,91.9,2000,91.9)
ldiagonal.SetLineColor(ROOT.kGray+2)
ldiagonal.SetLineStyle(2)
ldiagonal.SetLineWidth(3)
tdiagonal = ROOT.TLatex( 70,91.9/2-25,"Excluded 95% CL (LEP)")
tdiagonal.SetTextColor(ROOT.kBlack)
tdiagonal.SetTextAlign(21)
tdiagonal.SetTextSize(0.021)
gdiagonalw.Draw("FSAME")
ldiagonal.Draw("LSAME")
tdiagonal.Draw("SAME")

gr_obs.Draw("L same")

simplecanvas.RedrawAxis()

utils.CMS_Style(simplecanvas,lumi="137 fb^{-1}",cmsTextSize=0.30,lumiTextSize=0.30,relPosX=0.08,cmsTextOffset = 0.08, lumiTextOffset = 0.15)

simplecanvas.SaveAs("pngs/{}_2d_curve.png".format(model))
simplecanvas.SaveAs("pdfs/{}_2d_curve.pdf".format(model))

gr_obs.Write("contour_obs")
gr_exp.Write("contour_exp")
gr_1s.Write("exp_errs_1s")
gr_2s.Write("exp_errs_2s")
#gr_expp1.Write("contour_expp1")
#gr_expp2.Write("contour_expp2")
#gr_expm1.Write("contour_expm1")
#gr_expm2.Write("contour_expm2")
outfile.Close()
