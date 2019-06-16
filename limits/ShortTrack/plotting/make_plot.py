# Rework of combine limit extraction

import ROOT
import re
from math import sqrt
from sys import argv,exit
import os

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
ROOT.gStyle.SetLegendTextSize(0.1)
ROOT.gStyle.SetPaintTextFormat("#2.2g")
simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(1000,1000)
simplecanvas.SetTicks(1,1)
simplecanvas.SetLeftMargin(0.16)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetRightMargin(0.16)
simplecanvas.SetLogz(True)

ROOT.gStyle.SetPalette(ROOT.kLightTemperature)

verbose = True

printstyle = "colz"
#printstyle = "text"

model = argv[1]
intag = argv[2]

remake = model.find(".root") < 0

if not remake:
    model = model[model.find("/")+1:model.find(".root")]

indir="/hadoop/cms/store/user/dpgilber/ShortTrackScanLimits/{}/{}".format(intag,model)

tokens = model.split("_")
particles = tokens[0]
ctau = tokens[1]

if model.find("T1") >= 0:
    primary = "gluino"
    xmin = 600
    xmax = 3000
    inc = 25
elif model.find("T2bt") >= 0:
    primary = "stop"
    xmin = 300
    xmax = 2600
    inc = 16.6667
else:
    primary = "squark"
    xmin = 600
    xmax = 3000
    inc = 25

ymin = 0
ymax = xmax

print primary
print "xmin",xmin
print "xmax",xmax
print "inc",inc
print "ymin",ymin
print "ymax",ymax
print ""

def GetLeft(th2, m1, m2):
    if m1 == xmin:
        return -1,-1
    testm1 = m1 - inc
    trialbin = th2.GetBinContent(th2.GetXaxis().FindBin(testm1), th2.GetYaxis().FindBin(m2))
    if trialbin > 0: 
        return testm1,trialbin
    if testm1 == xmin:
        return -1,-1
    testm1 -= inc
    trialbin = th2.GetBinContent(th2.GetXaxis().FindBin(testm1), th2.GetYaxis().FindBin(m2))
    if trialbin > 0: 
        return testm1,trialbin
    return -1,-1

def GetRight(th2, m1, m2):
    if m1 == xmax:
        return -1,-1
    testm1 = m1 + inc
    trialbin = th2.GetBinContent(th2.GetXaxis().FindBin(testm1), th2.GetYaxis().FindBin(m2))
    if trialbin > 0: 
        return testm1,trialbin
    if testm1 == xmax:
        return -1,-1
    testm1 += inc
    trialbin = th2.GetBinContent(th2.GetXaxis().FindBin(testm1), th2.GetYaxis().FindBin(m2))
    if trialbin > 0: 
        return testm1,trialbin
    return -1,-1
        
def GetUp(th2, m1, m2):
    if m2 == m1:
        return -1,-1
    testm2 = m2 + inc
    trialbin = th2.GetBinContent(th2.GetXaxis().FindBin(m1), th2.GetYaxis().FindBin(testm2))
    if trialbin > 0: 
        return testm2,trialbin
    if testm2 == m1:
        return -1,-1
    testm2 += inc
    trialbin = th2.GetBinContent(th2.GetXaxis().FindBin(m1), th2.GetYaxis().FindBin(testm2))
    if trialbin > 0: 
        return testm2,trialbin
    return -1,-1

def GetDown(th2, m1, m2):
    if m2 == 0:
        return -1,-1
    testm2 = m2 - inc
    trialbin = th2.GetBinContent(th2.GetXaxis().FindBin(m1), th2.GetYaxis().FindBin(testm2))
    if trialbin > 0: 
        return testm2,trialbin
    if testm2 == 0:
        return -1,-1
    testm2 -= inc
    trialbin = th2.GetBinContent(th2.GetXaxis().FindBin(m1), th2.GetYaxis().FindBin(testm2))
    if trialbin > 0: 
        return testm2,trialbin
    return -1,-1



def Smooth(th2):
    m1 = xmin
    while m1 < xmax+1:
        m1_round = int(round(m1))
        m2 = ymin
        while m2 < m1:
            m2_round = int(round(m2))
            binX = th2.GetXaxis().FindBin(m1)
            binY = th2.GetYaxis().FindBin(m2)
            if th2.GetBinContent(binX, binY) > 0: 
                m2 += inc
                continue
            left,leftval = GetLeft(th2,m1,m2)
            right,rightval = GetRight(th2,m1,m2)
            up,upval = GetUp(th2,m1,m2)
            down,downval = GetDown(th2,m1,m2)
            if left > 0 and right > 0 and up > 0 and down > 0:
                mX = (rightval - leftval)/(right - left)
                mY = (upval - downval)/(up - down)
                valX = mX * (m1-left) + leftval
                valY = mY * (m2-down) + downval
                val = (valX + valY) / 2
            elif left > 0 and right > 0:
                mX = (rightval - leftval)/(right - left)
                val = mX * (m1-left) + leftval
            elif up > 0 and down > 0:
                mY = (upval - downval)/(up - down)
                val = mY * (m2-down) + downval
            else:
                m2 += inc
                continue
            th2.SetBinContent(binX,binY,val)
            m2 += inc
        m1 += inc

h_obs = ROOT.TH2D("h_obs_{}".format(model),"Observed Limit ({}#; {} cm);M_{{{}}} (GeV);M_{{LSP}} (GeV)".format(particles,ctau,primary), int(round(xmax-xmin)/inc), xmin, xmax, int(round(ymax-ymin)/inc), ymin, ymax)
h_exp = ROOT.TH2D("h_exp_{}".format(model),"Expected Limit ({}#; {} cm);M_{{{}}} (GeV);M_{{LSP}} (GeV)".format(particles,ctau,primary), int(round(xmax-xmin)/inc), xmin, xmax, int(round(ymax-ymin)/inc), ymin, ymax)
h_expp1 = ROOT.TH2D("h_expp1_{}".format(model),"Expected Limit +1#sigma ({}#; {} cm);M_{{{}}} (GeV);M_{{LSP}} (GeV)".format(particles,ctau,primary), int(round(xmax-xmin)/inc), xmin, xmax, int(round(ymax-ymin)/inc), ymin, ymax)
h_expp2 = ROOT.TH2D("h_expp2_{}".format(model),"Expected Limit +2#sigma ({}#; {} cm);M_{{{}}} (GeV);M_{{LSP}} (GeV)".format(particles,ctau,primary), int(round(xmax-xmin)/inc), xmin, xmax, int(round(ymax-ymin)/inc), ymin, ymax)
h_expm1 = ROOT.TH2D("h_expm1_{}".format(model),"Expected Limit -1#sigma ({}#; {} cm);M_{{{}}} (GeV);M_{{LSP}} (GeV)".format(particles,ctau,primary), int(round(xmax-xmin)/inc), xmin, xmax, int(round(ymax-ymin)/inc), ymin, ymax)
h_expm2 = ROOT.TH2D("h_expm2_{}".format(model),"Expected Limit -2#sigma ({}#; {} cm);M_{{{}}} (GeV);M_{{LSP}} (GeV)".format(particles,ctau,primary), int(round(xmax-xmin)/inc), xmin, xmax, int(round(ymax-ymin)/inc), ymin, ymax)

f_txt = open("limits_{}.txt".format(model),"w+")

m1 = xmin
while m1 < xmax+1:
    m1_round = int(round(m1))
    m2 = ymin
    while m2 < m1:
        m2_round = int(round(m2))
        path = "{}/limit_{}_{}.root".format(indir,m1_round,m2_round)
        if ROOT.gSystem.AccessPathName(path): 
            if verbose: print "skipping {0},{1}".format(m1_round,m2_round)
            m2 += inc
            continue

        f = ROOT.TFile(path,"READ")

        tree = f.Get("limit")

        if tree == None: 
            if verbose: print "No tree for "+path
            f.Close()
            m2 += inc
            continue 
        elif tree.GetEntries() < 6:
            if verbose: print "Not enough limits in "+path+", removing"
            f.Close()
            os.remove(path)
            m2 += inc
            continue 

#        f.Close()
#        m2 += inc
#        continue

        tree.GetEntry(5)
        obs_limit = tree.limit
        if verbose: print "Obs", obs_limit

        if obs_limit > 0:
            h_obs.Fill(m1,m2,obs_limit)

        tree.GetEntry(2)
        exp_limit = tree.limit
        if verbose: print "Exp", exp_limit

        if exp_limit > 0:
            h_exp.Fill(m1,m2,exp_limit)

        tree.GetEntry(0)
        exp_limit_m2 = tree.limit
        if exp_limit_m2 > 0:
            h_expm2.Fill(m1,m2,exp_limit_m2)

        tree.GetEntry(1)
        exp_limit_m1 = tree.limit
        if exp_limit_m1 > 0:
            h_expm1.Fill(m1,m2,exp_limit_m1)

        tree.GetEntry(3)
        exp_limit_p1 = tree.limit
        if exp_limit_p1 > 0:
            h_expp1.Fill(m1,m2,exp_limit_p1)

        tree.GetEntry(4)
        exp_limit_p2 = tree.limit
        if exp_limit_p2 > 0:
            h_expp2.Fill(m1,m2,exp_limit_p2)

        if m2_round == 0: m2_round = 1
        f_txt.write(str(m1_round)+" "+str(m2_round)+" "+str(exp_limit)+" "+str(obs_limit)+" "+str(exp_limit_p1)+" "+str(exp_limit_m1)+" "+str(exp_limit_p2)+" "+str(exp_limit_m2)+"\n")

        f.Close()
        m2 += inc
    m1 += inc

f_txt.close()

outfile = ROOT.TFile.Open("{}.root".format(model),"RECREATE")
outfile.cd()

h_obs.SetMinimum(1e-3)
h_obs.SetMaximum(100)
h_obs.GetXaxis().SetLabelSize(2.0/h_obs.GetXaxis().GetNbins())
h_obs.GetYaxis().SetLabelSize(2.0/h_obs.GetYaxis().GetNbins())
h_obs.Write()

h_exp.SetMinimum(1e-3)
h_exp.SetMaximum(100)
h_exp.GetXaxis().SetLabelSize(2.0/h_exp.GetXaxis().GetNbins())
h_exp.GetYaxis().SetLabelSize(2.0/h_exp.GetYaxis().GetNbins())
h_exp.Write()

h_expm2.Write()
h_expm1.Write()
h_expp1.Write()
h_expp2.Write()

outfile.Close()

h_obs.Draw(printstyle)    
simplecanvas.SaveAs("pngs/{}_obs.png".format(model))
simplecanvas.SaveAs("pdfs/{}_obs.pdf".format(model))
print "Smoothing obs hist"
Smooth(h_obs)
Smooth(h_obs)
Smooth(h_obs)
Smooth(h_obs)
Smooth(h_obs)
h_obs.Draw(printstyle)    
simplecanvas.SaveAs("pngs/{}_obs_interp.png".format(model))
simplecanvas.SaveAs("pdfs/{}_obs_interp.pdf".format(model))
h_exp.Draw(printstyle)
simplecanvas.SaveAs("pngs/{}_exp.png".format(model))
simplecanvas.SaveAs("pdfs/{}_exp.pdf".format(model))
print "Smoothing exp hist"
Smooth(h_exp)
Smooth(h_exp)
Smooth(h_exp)
Smooth(h_exp)
Smooth(h_exp)
h_exp.Draw(printstyle)
simplecanvas.SaveAs("pngs/{}_exp_interp.png".format(model))
simplecanvas.SaveAs("pdfs/{}_exp_interp.pdf".format(model))

h_obs.SetMaximum(1)
h_exp.SetMaximum(1)
h_exp.Draw(printstyle)
simplecanvas.SaveAs("pngs/{}_exp_curve.png".format(model))
simplecanvas.SaveAs("pdfs/{}_exp_curve.pdf".format(model))
h_obs.Draw(printstyle)
simplecanvas.SaveAs("pngs/{}_obs_curve.png".format(model))
simplecanvas.SaveAs("pdfs/{}_obs_curve.pdf".format(model))
