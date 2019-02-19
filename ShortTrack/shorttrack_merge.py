# Combine and process histograms produced by ShortTrackLooper

import ROOT
from math import sqrt
import re
import sys
import os
from os.path import isfile,join
import subprocess

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

verbose = False # Print more status messages
doRmtpt = False
fullPropagate = True


ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
ROOT.gStyle.SetLegendTextSize(0.1)
ROOT.gStyle.SetPaintTextFormat("#3.3g")
simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(600,600)
simplecanvas.SetTicks(1,2)
simplecanvas.SetLeftMargin(0.2)
simplecanvas.SetRightMargin(0.2)
simplecanvas.SetTopMargin(0.12)

if len(sys.argv) < 2: 
    print "Which tag?"
    exit(1)

tag = sys.argv[1]

# Can give the output a new name using the second argument, if you wish.
outtag = sys.argv[2] if len(sys.argv) > 2 else tag

os.system("mkdir -p output_merged")

d18 = "output_merged/data_2018_{}.root".format(outtag)
d17 = "output_merged/data_2017_{}.root".format(outtag)
d16 = "output_merged/data_2016_{}.root".format(outtag)
m18 = "output_merged/mc_2018_{}.root".format(outtag)
m17 = "output_merged/mc_2017_{}.root".format(outtag)
m16 = "output_merged/mc_2016_{}.root".format(outtag)

d1718 = "output_merged/data_2017and2018_{}.root".format(outtag)
m1718 = "output_merged/mc_2017and2018_{}.root".format(outtag)

#filenames = [d18, d17, m18, m17]
#filenames = [d1718, m1718, d16, m16]
#filenames = [d1718, d16]
filenames = [d1718, m1718, d18, m18, d17, m17, d16, m16]
#filenames = [m16]

def ErrorTuple(h):
    return (h.GetBinError(1,1),h.GetBinError(2,1),h.GetBinError(3,1),h.GetBinError(4,1),h.GetBinError(5,1))
def ContentTuple(h):
    return (h.GetBinContent(1,1),h.GetBinContent(2,1),h.GetBinContent(3,1),h.GetBinContent(4,1),h.GetBinContent(5,1))
def getAsymmetricErrors(y):
    yp=0.0
    ym=0.0
    yerr=[]
    alpha=1 - 0.6827
    if(y==0):
        ym = 0.0
    else:
        ym = ROOT.Math.gamma_quantile(alpha/2,y,1);
    yp =  ROOT.Math.gamma_quantile_c(alpha/2,y+1,1)
    ym=y-ym
    yp=yp-y
    yerr.append(yp)
    yerr.append(ym)
    return yerr

for filename in filenames:

    outfile = ROOT.TFile.Open(filename,"RECREATE")

    isData = False
    if filename == d18:
        print "2018 Data"
        filepath = "output_unmerged/2018_{}/data/".format(tag)
        inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
        isData = True
    elif filename == d17:
        print "2017 Data"
        filepath = "output_unmerged/2017_{}/data/".format(tag)
        inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
        isData = True
    elif filename == d16:
        print "2016 Data"
        filepath = "output_unmerged/2016_{}/data/".format(tag)
        inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
        isData = True
    elif filename == m18:
        print "2018 MC"
        filepath = "output_unmerged/2018_{}/".format(tag)
        inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
    elif filename == m17:
        print "2017 MC"
        filepath = "output_unmerged/2017_{}/".format(tag)
        inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
    elif filename == m16:
        print "2016 MC"
        filepath = "output_unmerged/2016_{}/".format(tag)
        inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
    elif filename == d1718:
        print "2017 and 2018 Data"
        filepath = "output_unmerged/2017_{}/data/".format(tag)
        inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
        filepath = "output_unmerged/2018_{}/data/".format(tag)
        inputlist += [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
        isData = True
    elif filename == m1718:
        print "2017 and 2018 MC"
        filepath = "output_unmerged/2017_{}/".format(tag)
        inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
        filepath = "output_unmerged/2018_{}/".format(tag)
        inputlist += [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
        isData = False


    filelist = [ROOT.TFile.Open(infile) for infile in inputlist]

    h_fsMR_stat = filelist[0].Get("h_FS")
    h_fsMR_Nj23_stat = filelist[0].Get("h_FS_23")
    h_fsMR_Nj4_stat = filelist[0].Get("h_FS_4")
    h_fsMR_hi_stat = filelist[0].Get("h_FS_hi")
    h_fsMR_Nj23_hi_stat = filelist[0].Get("h_FS_23_hi")
    h_fsMR_Nj4_hi_stat = filelist[0].Get("h_FS_4_hi")
    h_fsMR_lo_stat = filelist[0].Get("h_FS_lo")
    h_fsMR_Nj23_lo_stat = filelist[0].Get("h_FS_23_lo")
    h_fsMR_Nj4_lo_stat = filelist[0].Get("h_FS_4_lo")

    h_fsMR_stat_up = filelist[0].Get("h_FS_up")
    h_fsMR_Nj23_stat_up = filelist[0].Get("h_FS_23_up")
    h_fsMR_Nj4_stat_up = filelist[0].Get("h_FS_4_up")
    h_fsMR_hi_stat_up = filelist[0].Get("h_FS_hi_up")
    h_fsMR_Nj23_hi_stat_up = filelist[0].Get("h_FS_23_hi_up")
    h_fsMR_Nj4_hi_stat_up = filelist[0].Get("h_FS_4_hi_up")
    h_fsMR_lo_stat_up = filelist[0].Get("h_FS_lo_up")
    h_fsMR_Nj23_lo_stat_up = filelist[0].Get("h_FS_23_lo_up")
    h_fsMR_Nj4_lo_stat_up = filelist[0].Get("h_FS_4_lo_up")

    h_fsMR_stat_dn = filelist[0].Get("h_FS_dn")
    h_fsMR_Nj23_stat_dn = filelist[0].Get("h_FS_23_dn")
    h_fsMR_Nj4_stat_dn = filelist[0].Get("h_FS_4_dn")
    h_fsMR_hi_stat_dn = filelist[0].Get("h_FS_hi_dn")
    h_fsMR_Nj23_hi_stat_dn = filelist[0].Get("h_FS_23_hi_dn")
    h_fsMR_Nj4_hi_stat_dn = filelist[0].Get("h_FS_4_hi_dn")
    h_fsMR_lo_stat_dn = filelist[0].Get("h_FS_lo_dn")
    h_fsMR_Nj23_lo_stat_dn = filelist[0].Get("h_FS_23_lo_dn")
    h_fsMR_Nj4_lo_stat_dn = filelist[0].Get("h_FS_4_lo_dn")

    h_fsMR_syst = filelist[0].Get("h_FS_syst")
    h_fsMR_Nj23_syst = filelist[0].Get("h_FS_23_syst")
    h_fsMR_Nj4_syst = filelist[0].Get("h_FS_4_syst")
    h_fsMR_hi_syst = filelist[0].Get("h_FS_hi_syst")
    h_fsMR_Nj23_hi_syst = filelist[0].Get("h_FS_23_hi_syst")
    h_fsMR_Nj4_hi_syst = filelist[0].Get("h_FS_4_hi_syst")
    h_fsMR_lo_syst = filelist[0].Get("h_FS_lo_syst")
    h_fsMR_Nj23_lo_syst = filelist[0].Get("h_FS_23_lo_syst")
    h_fsMR_Nj4_lo_syst = filelist[0].Get("h_FS_4_lo_syst")

    fserr_stat_up = { "h_LL_VR" : ErrorTuple(h_fsMR_stat_up),
              "h_LLM_VR" : ErrorTuple(h_fsMR_stat_up),
              "h_LM_VR" : ErrorTuple(h_fsMR_stat_up),
              "h_LH_VR" : ErrorTuple(h_fsMR_stat_up),
              "h_HL_VR" : ErrorTuple(h_fsMR_stat_up),
              "h_HLM_VR" : ErrorTuple(h_fsMR_stat_up),
              "h_HM_VR" : ErrorTuple(h_fsMR_stat_up),
              "h_HH_VR" : ErrorTuple(h_fsMR_stat_up),
              "h_LL_VR_23" : ErrorTuple(h_fsMR_Nj23_stat_up),
              "h_LLM_VR_23" : ErrorTuple(h_fsMR_Nj23_stat_up),
              "h_LM_VR_23" : ErrorTuple(h_fsMR_Nj23_stat_up),
              "h_LH_VR_23" : ErrorTuple(h_fsMR_Nj23_stat_up),
              "h_HL_VR_4" : ErrorTuple(h_fsMR_Nj4_stat_up),
              "h_HLM_VR_4" : ErrorTuple(h_fsMR_Nj4_stat_up),
              "h_HM_VR_4" : ErrorTuple(h_fsMR_Nj4_stat_up),
              "h_HH_VR_4" : ErrorTuple(h_fsMR_Nj4_stat_up),
              
              "h_LL_SR" : ErrorTuple(h_fsMR_stat_up),
              "h_LLM_SR" : ErrorTuple(h_fsMR_stat_up),
              "h_LM_SR" : ErrorTuple(h_fsMR_stat_up),
              "h_LH_SR" : ErrorTuple(h_fsMR_stat_up),
              "h_HL_SR" : ErrorTuple(h_fsMR_stat_up),
              "h_HLM_SR" : ErrorTuple(h_fsMR_stat_up),
              "h_HM_SR" : ErrorTuple(h_fsMR_stat_up),
              "h_HH_SR" : ErrorTuple(h_fsMR_stat_up),
              "h_LL_SR_23" : ErrorTuple(h_fsMR_Nj23_stat_up),
              "h_LLM_SR_23" : ErrorTuple(h_fsMR_Nj23_stat_up),
              "h_LM_SR_23" : ErrorTuple(h_fsMR_Nj23_stat_up),
              "h_LH_SR_23" : ErrorTuple(h_fsMR_Nj23_stat_up),
              "h_HL_SR_4" : ErrorTuple(h_fsMR_Nj4_stat_up),
              "h_HLM_SR_4" : ErrorTuple(h_fsMR_Nj4_stat_up),
              "h_HM_SR_4" : ErrorTuple(h_fsMR_Nj4_stat_up),
              "h_HH_SR_4" : ErrorTuple(h_fsMR_Nj4_stat_up),
              
              # hi pt
              "h_LL_VR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_LLM_VR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_LM_VR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_LH_VR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_HL_VR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_HLM_VR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_HM_VR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_HH_VR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_LL_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_up),
              "h_LLM_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_up),
              "h_LM_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_up),
              "h_LH_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_up),
              "h_HL_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_up),
              "h_HLM_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_up),
              "h_HM_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_up),
              "h_HH_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_up),

              "h_LL_SR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_LLM_SR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_LM_SR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_LH_SR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_HL_SR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_HLM_SR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_HM_SR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_HH_SR_hi" : ErrorTuple(h_fsMR_hi_stat_up),
              "h_LL_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_up),
              "h_LLM_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_up),
              "h_LM_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_up),
              "h_LH_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_up),
              "h_HL_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_up),
              "h_HLM_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_up),
              "h_HM_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_up),
              "h_HH_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_up),

              # lo pt
              "h_LL_VR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_LLM_VR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_LM_VR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_LH_VR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_HL_VR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_HLM_VR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_HM_VR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_HH_VR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_LL_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_up),
              "h_LLM_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_up),
              "h_LM_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_up),
              "h_LH_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_up),
              "h_HL_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_up),
              "h_HLM_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_up),
              "h_HM_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_up),
              "h_HH_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_up),

              "h_LL_SR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_LLM_SR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_LM_SR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_LH_SR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_HL_SR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_HLM_SR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_HM_SR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_HH_SR_lo" : ErrorTuple(h_fsMR_lo_stat_up),
              "h_LL_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_up),
              "h_LLM_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_up),
              "h_LM_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_up),
              "h_LH_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_up),
              "h_HL_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_up),
              "h_HLM_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_up),
              "h_HM_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_up),
              "h_HH_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_up)
              }

    fserr_stat_dn = { "h_LL_VR" : ErrorTuple(h_fsMR_stat_dn),
              "h_LLM_VR" : ErrorTuple(h_fsMR_stat_dn),
              "h_LM_VR" : ErrorTuple(h_fsMR_stat_dn),
              "h_LH_VR" : ErrorTuple(h_fsMR_stat_dn),
              "h_HL_VR" : ErrorTuple(h_fsMR_stat_dn),
              "h_HLM_VR" : ErrorTuple(h_fsMR_stat_dn),
              "h_HM_VR" : ErrorTuple(h_fsMR_stat_dn),
              "h_HH_VR" : ErrorTuple(h_fsMR_stat_dn),
              "h_LL_VR_23" : ErrorTuple(h_fsMR_Nj23_stat_dn),
              "h_LLM_VR_23" : ErrorTuple(h_fsMR_Nj23_stat_dn),
              "h_LM_VR_23" : ErrorTuple(h_fsMR_Nj23_stat_dn),
              "h_LH_VR_23" : ErrorTuple(h_fsMR_Nj23_stat_dn),
              "h_HL_VR_4" : ErrorTuple(h_fsMR_Nj4_stat_dn),
              "h_HLM_VR_4" : ErrorTuple(h_fsMR_Nj4_stat_dn),
              "h_HM_VR_4" : ErrorTuple(h_fsMR_Nj4_stat_dn),
              "h_HH_VR_4" : ErrorTuple(h_fsMR_Nj4_stat_dn),
              
              "h_LL_SR" : ErrorTuple(h_fsMR_stat_dn),
              "h_LLM_SR" : ErrorTuple(h_fsMR_stat_dn),
              "h_LM_SR" : ErrorTuple(h_fsMR_stat_dn),
              "h_LH_SR" : ErrorTuple(h_fsMR_stat_dn),
              "h_HL_SR" : ErrorTuple(h_fsMR_stat_dn),
              "h_HLM_SR" : ErrorTuple(h_fsMR_stat_dn),
              "h_HM_SR" : ErrorTuple(h_fsMR_stat_dn),
              "h_HH_SR" : ErrorTuple(h_fsMR_stat_dn),
              "h_LL_SR_23" : ErrorTuple(h_fsMR_Nj23_stat_dn),
              "h_LLM_SR_23" : ErrorTuple(h_fsMR_Nj23_stat_dn),
              "h_LM_SR_23" : ErrorTuple(h_fsMR_Nj23_stat_dn),
              "h_LH_SR_23" : ErrorTuple(h_fsMR_Nj23_stat_dn),
              "h_HL_SR_4" : ErrorTuple(h_fsMR_Nj4_stat_dn),
              "h_HLM_SR_4" : ErrorTuple(h_fsMR_Nj4_stat_dn),
              "h_HM_SR_4" : ErrorTuple(h_fsMR_Nj4_stat_dn),
              "h_HH_SR_4" : ErrorTuple(h_fsMR_Nj4_stat_dn),
              
              # hi pt
              "h_LL_VR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_LLM_VR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_LM_VR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_LH_VR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_HL_VR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_HLM_VR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_HM_VR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_HH_VR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_LL_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_dn),
              "h_LLM_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_dn),
              "h_LM_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_dn),
              "h_LH_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_dn),
              "h_HL_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_dn),
              "h_HLM_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_dn),
              "h_HM_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_dn),
              "h_HH_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_dn),

              "h_LL_SR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_LLM_SR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_LM_SR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_LH_SR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_HL_SR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_HLM_SR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_HM_SR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_HH_SR_hi" : ErrorTuple(h_fsMR_hi_stat_dn),
              "h_LL_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_dn),
              "h_LLM_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_dn),
              "h_LM_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_dn),
              "h_LH_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_stat_dn),
              "h_HL_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_dn),
              "h_HLM_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_dn),
              "h_HM_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_dn),
              "h_HH_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_stat_dn),

              # lo pt
              "h_LL_VR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_LLM_VR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_LM_VR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_LH_VR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_HL_VR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_HLM_VR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_HM_VR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_HH_VR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_LL_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_dn),
              "h_LLM_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_dn),
              "h_LM_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_dn),
              "h_LH_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_dn),
              "h_HL_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_dn),
              "h_HLM_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_dn),
              "h_HM_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_dn),
              "h_HH_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_dn),

              "h_LL_SR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_LLM_SR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_LM_SR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_LH_SR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_HL_SR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_HLM_SR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_HM_SR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_HH_SR_lo" : ErrorTuple(h_fsMR_lo_stat_dn),
              "h_LL_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_dn),
              "h_LLM_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_dn),
              "h_LM_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_dn),
              "h_LH_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_stat_dn),
              "h_HL_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_dn),
              "h_HLM_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_dn),
              "h_HM_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_dn),
              "h_HH_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_stat_dn)
              }

    fserr_syst = { "h_LL_VR" : ErrorTuple(h_fsMR_syst),
              "h_LLM_VR" : ErrorTuple(h_fsMR_syst),
              "h_LM_VR" : ErrorTuple(h_fsMR_syst),
              "h_LH_VR" : ErrorTuple(h_fsMR_syst),
              "h_HL_VR" : ErrorTuple(h_fsMR_syst),
              "h_HLM_VR" : ErrorTuple(h_fsMR_syst),
              "h_HM_VR" : ErrorTuple(h_fsMR_syst),
              "h_HH_VR" : ErrorTuple(h_fsMR_syst),
              "h_LL_VR_23" : ErrorTuple(h_fsMR_Nj23_syst),
              "h_LLM_VR_23" : ErrorTuple(h_fsMR_Nj23_syst),
              "h_LM_VR_23" : ErrorTuple(h_fsMR_Nj23_syst),
              "h_LH_VR_23" : ErrorTuple(h_fsMR_Nj23_syst),
              "h_HL_VR_4" : ErrorTuple(h_fsMR_Nj4_syst),
              "h_HLM_VR_4" : ErrorTuple(h_fsMR_Nj4_syst),
              "h_HM_VR_4" : ErrorTuple(h_fsMR_Nj4_syst),
              "h_HH_VR_4" : ErrorTuple(h_fsMR_Nj4_syst),
              
              "h_LL_SR" : ErrorTuple(h_fsMR_syst),
              "h_LLM_SR" : ErrorTuple(h_fsMR_syst),
              "h_LM_SR" : ErrorTuple(h_fsMR_syst),
              "h_LH_SR" : ErrorTuple(h_fsMR_syst),
              "h_HL_SR" : ErrorTuple(h_fsMR_syst),
              "h_HLM_SR" : ErrorTuple(h_fsMR_syst),
              "h_HM_SR" : ErrorTuple(h_fsMR_syst),
              "h_HH_SR" : ErrorTuple(h_fsMR_syst),
              "h_LL_SR_23" : ErrorTuple(h_fsMR_Nj23_syst),
              "h_LLM_SR_23" : ErrorTuple(h_fsMR_Nj23_syst),
              "h_LM_SR_23" : ErrorTuple(h_fsMR_Nj23_syst),
              "h_LH_SR_23" : ErrorTuple(h_fsMR_Nj23_syst),
              "h_HL_SR_4" : ErrorTuple(h_fsMR_Nj4_syst),
              "h_HLM_SR_4" : ErrorTuple(h_fsMR_Nj4_syst),
              "h_HM_SR_4" : ErrorTuple(h_fsMR_Nj4_syst),
              "h_HH_SR_4" : ErrorTuple(h_fsMR_Nj4_syst),
              
              # hi pt
              "h_LL_VR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_LLM_VR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_LM_VR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_LH_VR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_HL_VR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_HLM_VR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_HM_VR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_HH_VR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_LL_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_syst),
              "h_LLM_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_syst),
              "h_LM_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_syst),
              "h_LH_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_syst),
              "h_HL_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_syst),
              "h_HLM_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_syst),
              "h_HM_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_syst),
              "h_HH_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_syst),

              "h_LL_SR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_LLM_SR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_LM_SR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_LH_SR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_HL_SR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_HLM_SR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_HM_SR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_HH_SR_hi" : ErrorTuple(h_fsMR_hi_syst),
              "h_LL_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_syst),
              "h_LLM_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_syst),
              "h_LM_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_syst),
              "h_LH_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi_syst),
              "h_HL_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_syst),
              "h_HLM_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_syst),
              "h_HM_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_syst),
              "h_HH_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi_syst),

              # lo pt
              "h_LL_VR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_LLM_VR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_LM_VR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_LH_VR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_HL_VR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_HLM_VR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_HM_VR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_HH_VR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_LL_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_syst),
              "h_LLM_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_syst),
              "h_LM_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_syst),
              "h_LH_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_syst),
              "h_HL_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_syst),
              "h_HLM_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_syst),
              "h_HM_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_syst),
              "h_HH_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_syst),

              "h_LL_SR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_LLM_SR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_LM_SR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_LH_SR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_HL_SR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_HLM_SR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_HM_SR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_HH_SR_lo" : ErrorTuple(h_fsMR_lo_syst),
              "h_LL_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_syst),
              "h_LLM_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_syst),
              "h_LM_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_syst),
              "h_LH_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo_syst),
              "h_HL_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_syst),
              "h_HLM_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_syst),
              "h_HM_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_syst),
              "h_HH_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo_syst)
              }

    fs = { "h_LL_VR" : ContentTuple(h_fsMR_stat),
              "h_LLM_VR" : ContentTuple(h_fsMR_stat),
              "h_LM_VR" : ContentTuple(h_fsMR_stat),
              "h_LH_VR" : ContentTuple(h_fsMR_stat),
              "h_HL_VR" : ContentTuple(h_fsMR_stat),
              "h_HLM_VR" : ContentTuple(h_fsMR_stat),
              "h_HM_VR" : ContentTuple(h_fsMR_stat),
              "h_HH_VR" : ContentTuple(h_fsMR_stat),
              "h_LL_VR_23" : ContentTuple(h_fsMR_Nj23_stat),
              "h_LLM_VR_23" : ContentTuple(h_fsMR_Nj23_stat),
              "h_LM_VR_23" : ContentTuple(h_fsMR_Nj23_stat),
              "h_LH_VR_23" : ContentTuple(h_fsMR_Nj23_stat),
              "h_HL_VR_4" : ContentTuple(h_fsMR_Nj4_stat),
              "h_HLM_VR_4" : ContentTuple(h_fsMR_Nj4_stat),
              "h_HM_VR_4" : ContentTuple(h_fsMR_Nj4_stat),
              "h_HH_VR_4" : ContentTuple(h_fsMR_Nj4_stat),
              
              "h_LL_SR" : ContentTuple(h_fsMR_stat),
              "h_LLM_SR" : ContentTuple(h_fsMR_stat),
              "h_LM_SR" : ContentTuple(h_fsMR_stat),
              "h_LH_SR" : ContentTuple(h_fsMR_stat),
              "h_HL_SR" : ContentTuple(h_fsMR_stat),
              "h_HLM_SR" : ContentTuple(h_fsMR_stat),
              "h_HM_SR" : ContentTuple(h_fsMR_stat),
              "h_HH_SR" : ContentTuple(h_fsMR_stat),
              "h_LL_SR_23" : ContentTuple(h_fsMR_Nj23_stat),
              "h_LLM_SR_23" : ContentTuple(h_fsMR_Nj23_stat),
              "h_LM_SR_23" : ContentTuple(h_fsMR_Nj23_stat),
              "h_LH_SR_23" : ContentTuple(h_fsMR_Nj23_stat),
              "h_HL_SR_4" : ContentTuple(h_fsMR_Nj4_stat),
              "h_HLM_SR_4" : ContentTuple(h_fsMR_Nj4_stat),
              "h_HM_SR_4" : ContentTuple(h_fsMR_Nj4_stat),
              "h_HH_SR_4" : ContentTuple(h_fsMR_Nj4_stat),
              
              # hi pt
              "h_LL_VR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_LLM_VR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_LM_VR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_LH_VR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_HL_VR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_HLM_VR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_HM_VR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_HH_VR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_LL_VR_23_hi" : ContentTuple(h_fsMR_Nj23_hi_stat),
              "h_LLM_VR_23_hi" : ContentTuple(h_fsMR_Nj23_hi_stat),
              "h_LM_VR_23_hi" : ContentTuple(h_fsMR_Nj23_hi_stat),
              "h_LH_VR_23_hi" : ContentTuple(h_fsMR_Nj23_hi_stat),
              "h_HL_VR_4_hi" : ContentTuple(h_fsMR_Nj4_hi_stat),
              "h_HLM_VR_4_hi" : ContentTuple(h_fsMR_Nj4_hi_stat),
              "h_HM_VR_4_hi" : ContentTuple(h_fsMR_Nj4_hi_stat),
              "h_HH_VR_4_hi" : ContentTuple(h_fsMR_Nj4_hi_stat),

              "h_LL_SR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_LLM_SR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_LM_SR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_LH_SR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_HL_SR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_HLM_SR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_HM_SR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_HH_SR_hi" : ContentTuple(h_fsMR_hi_stat),
              "h_LL_SR_23_hi" : ContentTuple(h_fsMR_Nj23_hi_stat),
              "h_LLM_SR_23_hi" : ContentTuple(h_fsMR_Nj23_hi_stat),
              "h_LM_SR_23_hi" : ContentTuple(h_fsMR_Nj23_hi_stat),
              "h_LH_SR_23_hi" : ContentTuple(h_fsMR_Nj23_hi_stat),
              "h_HL_SR_4_hi" : ContentTuple(h_fsMR_Nj4_hi_stat),
              "h_HLM_SR_4_hi" : ContentTuple(h_fsMR_Nj4_hi_stat),
              "h_HM_SR_4_hi" : ContentTuple(h_fsMR_Nj4_hi_stat),
              "h_HH_SR_4_hi" : ContentTuple(h_fsMR_Nj4_hi_stat),

              # lo pt
              "h_LL_VR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_LLM_VR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_LM_VR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_LH_VR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_HL_VR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_HLM_VR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_HM_VR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_HH_VR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_LL_VR_23_lo" : ContentTuple(h_fsMR_Nj23_lo_stat),
              "h_LLM_VR_23_lo" : ContentTuple(h_fsMR_Nj23_lo_stat),
              "h_LM_VR_23_lo" : ContentTuple(h_fsMR_Nj23_lo_stat),
              "h_LH_VR_23_lo" : ContentTuple(h_fsMR_Nj23_lo_stat),
              "h_HL_VR_4_lo" : ContentTuple(h_fsMR_Nj4_lo_stat),
              "h_HLM_VR_4_lo" : ContentTuple(h_fsMR_Nj4_lo_stat),
              "h_HM_VR_4_lo" : ContentTuple(h_fsMR_Nj4_lo_stat),
              "h_HH_VR_4_lo" : ContentTuple(h_fsMR_Nj4_lo_stat),

              "h_LL_SR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_LLM_SR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_LM_SR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_LH_SR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_HL_SR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_HLM_SR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_HM_SR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_HH_SR_lo" : ContentTuple(h_fsMR_lo_stat),
              "h_LL_SR_23_lo" : ContentTuple(h_fsMR_Nj23_lo_stat),
              "h_LLM_SR_23_lo" : ContentTuple(h_fsMR_Nj23_lo_stat),
              "h_LM_SR_23_lo" : ContentTuple(h_fsMR_Nj23_lo_stat),
              "h_LH_SR_23_lo" : ContentTuple(h_fsMR_Nj23_lo_stat),
              "h_HL_SR_4_lo" : ContentTuple(h_fsMR_Nj4_lo_stat),
              "h_HLM_SR_4_lo" : ContentTuple(h_fsMR_Nj4_lo_stat),
              "h_HM_SR_4_lo" : ContentTuple(h_fsMR_Nj4_lo_stat),
              "h_HH_SR_4_lo" : ContentTuple(h_fsMR_Nj4_lo_stat)
              }

#    doubletracknames = ["h_2STC_{}".format(reg) for reg in ["MR","VR","SR"]]
#    for hname in doubletracknames:
#        print hname
#        h = filelist[0].Get(hname)
#        for f in filelist[1:]:
#            h.Add(f.Get(hname))
#        outfile.cd()
#        h.Write()

    histnames = ["h_" + nj + ht + "_" + reg + njsplit + ptstring for nj in ["H"] for ht in ["L","LM","M","H"] for reg in ["MR","VR","SR"] for njsplit in ["","_4"] for ptstring in ["","_lo","_hi"]]
    histnames += ["h_" + nj + ht + "_" + reg + njsplit + ptstring for nj in ["L"] for ht in ["L","LM","M","H"] for reg in ["MR","VR","SR"] for njsplit in ["","_23"] for ptstring in ["","_lo","_hi"]]

    for histname in histnames:
        h = filelist[0].Get(histname)
        if verbose:
            print filelist[0].GetName()
            print "start:",h.GetBinContent(2,3),h.GetBinContent(2,2),h.GetBinContent(2,1)
            if h.GetBinContent(2,3) > 0: print "inferred fs:",h.GetBinContent(2,2)/h.GetBinContent(2,3)
        # First, sum all region histograms
        for f in filelist[1:]:
            hminor = f.Get(histname)
            h.Add(hminor)
            if verbose:
                print f.GetName()
                print "adding:",hminor.GetBinContent(2,3),hminor.GetBinContent(2,2),hminor.GetBinContent(2,1)
                print "Running total:",h.GetBinContent(2,3),hminor.GetBinContent(2,2),h.GetBinContent(2,1)
                if hminor.GetBinContent(2,3) > 0: print "inferred fs:",hminor.GetBinContent(2,2)/hminor.GetBinContent(2,3)
                if h.GetBinContent(2,3) > 0: print "inferred total fs:",h.GetBinContent(2,2)/h.GetBinContent(2,3)
        outfile.cd()
        h.Write()
        h_up = h.Clone(histname+"_up")
        h_dn = h.Clone(histname+"_dn")
        for length in range(1,h.GetNbinsX()+1):
            # First STCs
            nstc = h.GetBinContent(length,3)
            errstc = getAsymmetricErrors(nstc) if isData else [h.GetBinError(length,3)]*2
            h_up.SetBinError(length,3,errstc[0])
            h_dn.SetBinError(length,3,errstc[1])
            if histname.find("MR") < 0: # only do the prediction for non-MR hists
                # Propagate the STC error to the ST prediction
                if nstc > 0:
                    nst_predicted = h.GetBinContent(length,2)
                    h_up.SetBinError(length,2,errstc[0]/nstc * nst_predicted) 
                    h_dn.SetBinError(length,2,errstc[1]/nstc * nst_predicted) 
                else: # prediction upper error is upper error of 0 count, multiplied by appropriate fshort
                    fs_to_use = fs[histname][length-1]
                    h.SetBinError(length,2,fs_to_use*errstc[0])
            # Now observed STs
            nst = h.GetBinContent(length,1)
            errst = getAsymmetricErrors(nst) if isData else [h.GetBinError(length,1)]*2
            h_up.SetBinError(length,1,errst[0])
            h_dn.SetBinError(length,1,errst[1])
        
        h_stc_stats_up = h_up.Clone(h.GetName()+"_stcstats_up")
        h_stc_stats_dn = h_dn.Clone(h.GetName()+"_stcstats_dn")
        h_stc_stats_up.Write()
        h_stc_stats_dn.Write()
        if histname.find("MR") >= 0:
            # set errors for Nstc and Nst properly, no need to continue for MR
            continue

        # Now, propagate fshort errors to the predicted counts of these histograms (y bin 2), starting with statistics

        h_stat_up = h_up.Clone(h.GetName()+"_allstats_up")
        h_stat_dn = h_dn.Clone(h.GetName()+"_allstats_dn")
        for length in range(h.GetNbinsX()):
            bin = length+1            
            stc_err_up = h_stc_stats_up.GetBinError(bin,3) # STC stat error
            stc_err_dn = h_stc_stats_dn.GetBinError(bin,3) # STC stat error
            nstc = h.GetBinContent(bin,3) 
            nst_pred = h.GetBinContent(bin,2) # prediction ST counts
            fs_to_use = fs[histname][length]
            fserr_to_use_up = fserr_stat_up[histname][length]
            fserr_to_use_dn = fserr_stat_dn[histname][length]
            if nstc > 0 and fs_to_use > 0: # fshort is zero only in cases where we don't use it; still need a protection
                new_err_up = sqrt( (stc_err_up/nstc)**2 + (fserr_to_use_up / fs_to_use)**2 ) * nst_pred
                new_err_dn = sqrt( (stc_err_dn/nstc)**2 + (fserr_to_use_dn / fs_to_use)**2 ) * nst_pred
            else:                
                # for nstc == 0, use (fshort+fshort_up)*stc_up
                fshort_up = fs_to_use + fserr_to_use_up
                new_err_up = stc_err_up * fshort_up
                new_err_dn = 0
            h_stat_up.SetBinError(bin,2,new_err_up)
            h_stat_dn.SetBinError(bin,2,new_err_dn)

        outfile.cd()
        h_stat_up.Write()
        h_stat_dn.Write()

        # Now the fshort systematic

        h_syst = h.Clone(h.GetName()+"_fshortsyst")
        for length in range(h.GetNbinsX()):
            bin = length+1            
            fs_to_use = fs[histname][length]
            if fs_to_use == 0: 
                h_syst.SetBinError(bin,2,0) # just set these cases to 0 and move on
                continue 
            fserr_to_use = fserr_syst[histname][length]
            relerr = fserr_to_use / fs_to_use
            nst_pred = h_syst.GetBinContent(bin,2)
            h_syst.SetBinError(bin,2,relerr*nst_pred)

        outfile.cd()
        h_syst.Write()
        
        # Now everything but nonclosure
        h_allButNC_up = h.Clone(h.GetName()+"_allButNC_up")
        h_allButNC_dn = h.Clone(h.GetName()+"_allButNC_dn")
        for length in range(h.GetNbinsX()):
            bin = length+1            
            err_stat = [h_stat_up.GetBinError(bin,2), h_stat_dn.GetBinError(bin,2)]
            err_syst = h_syst.GetBinError(bin,2)
            err_allButNC = [sqrt(err_stat[0]**2+err_syst**2), sqrt(err_stat[1]**2+err_syst**2)]
            h_allButNC_up.SetBinError(bin,err_allButNC[0])
            h_allButNC_dn.SetBinError(bin,err_allButNC[1])

        outfile.cd()
        h_allButNC_up.Write()
        h_allButNC_dn.Write()


    outfile.cd()
    h_fsMR_stat.Write()
    h_fsMR_Nj23_stat.Write()
    h_fsMR_Nj4_stat.Write()
    h_fsMR_hi_stat.Write()
    h_fsMR_Nj23_hi_stat.Write()
    h_fsMR_Nj4_hi_stat.Write()
    h_fsMR_lo_stat.Write()
    h_fsMR_Nj23_lo_stat.Write()
    h_fsMR_Nj4_lo_stat.Write()

    h_fsMR_stat_up.Write()
    h_fsMR_Nj23_stat_up.Write()
    h_fsMR_Nj4_stat_up.Write()
    h_fsMR_hi_stat_up.Write()
    h_fsMR_Nj23_hi_stat_up.Write()
    h_fsMR_Nj4_hi_stat_up.Write()
    h_fsMR_lo_stat_up.Write()
    h_fsMR_Nj23_lo_stat_up.Write()
    h_fsMR_Nj4_lo_stat_up.Write()

    h_fsMR_stat_dn.Write()
    h_fsMR_Nj23_stat_dn.Write()
    h_fsMR_Nj4_stat_dn.Write()
    h_fsMR_hi_stat_dn.Write()
    h_fsMR_Nj23_hi_stat_dn.Write()
    h_fsMR_Nj4_hi_stat_dn.Write()
    h_fsMR_lo_stat_dn.Write()
    h_fsMR_Nj23_lo_stat_dn.Write()
    h_fsMR_Nj4_lo_stat_dn.Write()

    h_fsMR_syst.Write()
    h_fsMR_Nj23_syst.Write()
    h_fsMR_Nj4_syst.Write()
    h_fsMR_hi_syst.Write()
    h_fsMR_Nj23_hi_syst.Write()
    h_fsMR_Nj4_hi_syst.Write()
    h_fsMR_lo_syst.Write()
    h_fsMR_Nj23_lo_syst.Write()
    h_fsMR_Nj4_lo_syst.Write()

    # Now find non-closure systematic

    print "Nonclosure systematics"

    for histname in histnames:
        if histname.find("VR") < 0: continue
        h = outfile.Get(histname+"_stcstats_up") # any works fine, just use stcstats_up arbitrarily
        h_allButNC_up = outfile.Get(histname+"_allButNC_up")
        h_allButNC_dn = outfile.Get(histname+"_allButNC_dn")
        h_stat_up = outfile.Get(histname+"_allstats_up")
        h_stat_dn = outfile.Get(histname+"_allstats_dn")
        h_nc = ROOT.TH1F(histname+"_nonclosure_systematic","Systematic from Non-closure in Validation Region",h.GetNbinsX(),0,h.GetNbinsX())
        for length in range(1,h.GetNbinsX()+1):
            pred = h.GetBinContent(length,2)
            obs = h.GetBinContent(length,1)            
            h_stat = h_stat_up if obs > pred else h_stat_dn
            h_allButNC = h_allButNC_up if obs > pred else h_allButNC_dn
            pred_err = h_allButNC.GetBinError(length,2)
            obs_err = getAsymmetricErrors(obs)[1 if obs > pred else 0] if isData else h.GetBinError(length,1)
            total_err = sqrt(pred_err**2 + obs_err**2)
            expr = (pred-obs)**2 - total_err**2
            if pred > 0:
                minimal_rel_error = h_stat.GetBinError(length,2)/pred # if the prediction is nonzero, nonclosure systematic can be no smaller than relative prediction stat error
            elif obs > 0:
                minimal_rel_error = h_stat.GetBinError(length,1)/obs # if prediction is zero but obs isn't, use obs relative error as minimum.
                # The calculated nonclosure error is probably always larger in this case, but why not.
            else:
                minimal_rel_error = 1.0 # In this case, both pred and obs are 0, so there is "perfect closure", but we can't quantify it that well. Assign 100% for SR to be conservative.

            if expr < 0:
                systematic_percent = 0
            elif pred > 0:
                systematic_percent = sqrt(expr)/pred
            elif obs > 0:
                systematic_percent = sqrt(expr)/obs # use the obs to quantify the relative error if prediction is 0
            else:
                # in this case, we're just taking 100%, see above
                systematic_percent = 0
            # Set systematic to either the calculated systematic, or the prediction statistical error if it is larger
            h_nc.SetBinContent(length,min(max(systematic_percent,minimal_rel_error),1.0)) # cap at 100%, can be larger if statistical error is gigantic
        outfile.cd()
        h_nc.Write()

    for f in filelist:
        f.Close()
    outfile.Close()

print "Done"
