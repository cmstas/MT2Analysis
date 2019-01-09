# STCImplementation background estimates and plots

import ROOT
from math import sqrt
import re
import sys
import os
from os.path import isfile,join
import subprocess

sys.path.append("../Nifty")

import NiftyUtils

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

if len(sys.argv) < 3: 
    print "Use stat? syst? syststat?"
    exit(1)

tag = sys.argv[1]

syststring = sys.argv[2]

d18 = "output_merged/data_2018_{}_{}.root".format(tag,syststring)
d17 = "output_merged/data_2017_{}_{}.root".format(tag,syststring)
d16 = "output_merged/data_2016_{}_{}.root".format(tag,syststring)
m18 = "output_merged/mc_2018_{}_{}.root".format(tag,syststring)
m17 = "output_merged/mc_2017_{}_{}.root".format(tag,syststring)
m16 = "output_merged/mc_2016_{}_{}.root".format(tag,syststring)

d1718 = "output_merged/data_2017and2018_{}_{}.root".format(tag,syststring)

#filenames = [d18, m18, d17, m17, d16, m16]
#filenames = [d18, d17, m17, d16, m16]
filenames = [d1718, m17, d16, m16]
#filenames = [d17, m17, d16]
#filenames = [m17]
#filenames = [d17]
#filenames = [d1718]

def ErrorTuple(h):
    return (h.GetBinError(1,1),h.GetBinError(2,1),h.GetBinError(3,1),h.GetBinError(4,1),h.GetBinError(5,1))
def ContentTuple(h):
    return (h.GetBinContent(1,1),h.GetBinContent(2,1),h.GetBinContent(3,1),h.GetBinContent(4,1),h.GetBinContent(5,1))

for filename in filenames:

    outfile = ROOT.TFile.Open(filename,"RECREATE")

    if filename == d18:
        print "2018 Data"
        filepath = "output_unmerged/2018_{}/data/".format(tag)
        inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
    elif filename == d17:
        print "2017 Data"
        filepath = "output_unmerged/2017_{}/data/".format(tag)
        inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
    elif filename == d16:
        print "2016 Data"
        filepath = "output_unmerged/2016_{}/data/".format(tag)
        inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
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

    h_fsMR_syst = filelist[0].Get("h_FS_syst")
    h_fsMR_Nj23_syst = filelist[0].Get("h_FS_23_syst")
    h_fsMR_Nj4_syst = filelist[0].Get("h_FS_4_syst")
    h_fsMR_hi_syst = filelist[0].Get("h_FS_hi_syst")
    h_fsMR_Nj23_hi_syst = filelist[0].Get("h_FS_23_hi_syst")
    h_fsMR_Nj4_hi_syst = filelist[0].Get("h_FS_4_hi_syst")
    h_fsMR_lo_syst = filelist[0].Get("h_FS_lo_syst")
    h_fsMR_Nj23_lo_syst = filelist[0].Get("h_FS_23_lo_syst")
    h_fsMR_Nj4_lo_syst = filelist[0].Get("h_FS_4_lo_syst")

    h_fsMR_syststat = filelist[0].Get("h_FS_syststat")
    h_fsMR_Nj23_syststat = filelist[0].Get("h_FS_23_syststat")
    h_fsMR_Nj4_syststat = filelist[0].Get("h_FS_4_syststat")
    h_fsMR_hi_syststat = filelist[0].Get("h_FS_hi_syststat")
    h_fsMR_Nj23_hi_syststat = filelist[0].Get("h_FS_23_hi_syststat")
    h_fsMR_Nj4_hi_syststat = filelist[0].Get("h_FS_4_hi_syststat")
    h_fsMR_lo_syststat = filelist[0].Get("h_FS_lo_syststat")
    h_fsMR_Nj23_lo_syststat = filelist[0].Get("h_FS_23_lo_syststat")
    h_fsMR_Nj4_lo_syststat = filelist[0].Get("h_FS_4_lo_syststat")

    if syststring == "stat":
        h_fsMR = h_fsMR_stat
        h_fsMR_Nj23 = h_fsMR_Nj23_stat
        h_fsMR_Nj4 = h_fsMR_Nj4_stat
        h_fsMR_hi = h_fsMR_hi_stat
        h_fsMR_Nj23_hi = h_fsMR_Nj23_hi_stat
        h_fsMR_Nj4_hi = h_fsMR_Nj4_hi_stat
        h_fsMR_lo = h_fsMR_lo_stat
        h_fsMR_Nj23_lo = h_fsMR_Nj23_lo_stat
        h_fsMR_Nj4_lo = h_fsMR_Nj4_lo_stat
    elif syststring == "syst":
        h_fsMR = h_fsMR_syst
        h_fsMR_Nj23 = h_fsMR_Nj23_syst
        h_fsMR_Nj4 = h_fsMR_Nj4_syst
        h_fsMR_hi = h_fsMR_hi_syst
        h_fsMR_Nj23_hi = h_fsMR_Nj23_hi_syst
        h_fsMR_Nj4_hi = h_fsMR_Nj4_hi_syst
        h_fsMR_lo = h_fsMR_lo_syst
        h_fsMR_Nj23_lo = h_fsMR_Nj23_lo_syst
        h_fsMR_Nj4_lo = h_fsMR_Nj4_lo_syst
    elif syststring == "syststat":
        h_fsMR = h_fsMR_syststat
        h_fsMR_Nj23 = h_fsMR_Nj23_syststat
        h_fsMR_Nj4 = h_fsMR_Nj4_syststat
        h_fsMR_hi = h_fsMR_hi_syststat
        h_fsMR_Nj23_hi = h_fsMR_Nj23_hi_syststat
        h_fsMR_Nj4_hi = h_fsMR_Nj4_hi_syststat
        h_fsMR_lo = h_fsMR_lo_syststat
        h_fsMR_Nj23_lo = h_fsMR_Nj23_lo_syststat
        h_fsMR_Nj4_lo = h_fsMR_Nj4_lo_syststat
    else:
        print "Did not recognize syststring", syststring
        exit(1)


    if verbose:    
        h_fsMR.Print("all")
        h_fsMR_Nj23.Print("all")
        h_fsMR_Nj4.Print("all")
        h_fsMR_hi.Print("all")
        h_fsMR_Nj23_hi.Print("all")
        h_fsMR_Nj4_hi.Print("all")
        h_fsMR_lo.Print("all")
        h_fsMR_Nj23_lo.Print("all")
        h_fsMR_Nj4_lo.Print("all")

    fserr = { "h_LL_VR" : ErrorTuple(h_fsMR),
              "h_LLM_VR" : ErrorTuple(h_fsMR),
              "h_LM_VR" : ErrorTuple(h_fsMR),
              "h_LH_VR" : ErrorTuple(h_fsMR),
              "h_HL_VR" : ErrorTuple(h_fsMR),
              "h_HLM_VR" : ErrorTuple(h_fsMR),
              "h_HM_VR" : ErrorTuple(h_fsMR),
              "h_HH_VR" : ErrorTuple(h_fsMR),
              "h_LL_VR_23" : ErrorTuple(h_fsMR_Nj23),
              "h_LLM_VR_23" : ErrorTuple(h_fsMR_Nj23),
              "h_LM_VR_23" : ErrorTuple(h_fsMR_Nj23),
              "h_LH_VR_23" : ErrorTuple(h_fsMR_Nj23),
              "h_HL_VR_4" : ErrorTuple(h_fsMR_Nj4),
              "h_HLM_VR_4" : ErrorTuple(h_fsMR_Nj4),
              "h_HM_VR_4" : ErrorTuple(h_fsMR_Nj4),
              "h_HH_VR_4" : ErrorTuple(h_fsMR_Nj4),
              
              "h_LL_SR" : ErrorTuple(h_fsMR),
              "h_LLM_SR" : ErrorTuple(h_fsMR),
              "h_LM_SR" : ErrorTuple(h_fsMR),
              "h_LH_SR" : ErrorTuple(h_fsMR),
              "h_HL_SR" : ErrorTuple(h_fsMR),
              "h_HLM_SR" : ErrorTuple(h_fsMR),
              "h_HM_SR" : ErrorTuple(h_fsMR),
              "h_HH_SR" : ErrorTuple(h_fsMR),
              "h_LL_SR_23" : ErrorTuple(h_fsMR_Nj23),
              "h_LLM_SR_23" : ErrorTuple(h_fsMR_Nj23),
              "h_LM_SR_23" : ErrorTuple(h_fsMR_Nj23),
              "h_LH_SR_23" : ErrorTuple(h_fsMR_Nj23),
              "h_HL_SR_4" : ErrorTuple(h_fsMR_Nj4),
              "h_HLM_SR_4" : ErrorTuple(h_fsMR_Nj4),
              "h_HM_SR_4" : ErrorTuple(h_fsMR_Nj4),
              "h_HH_SR_4" : ErrorTuple(h_fsMR_Nj4),
              
              # hi pt
              "h_LL_VR_hi" : ErrorTuple(h_fsMR_hi),
              "h_LLM_VR_hi" : ErrorTuple(h_fsMR_hi),
              "h_LM_VR_hi" : ErrorTuple(h_fsMR_hi),
              "h_LH_VR_hi" : ErrorTuple(h_fsMR_hi),
              "h_HL_VR_hi" : ErrorTuple(h_fsMR_hi),
              "h_HLM_VR_hi" : ErrorTuple(h_fsMR_hi),
              "h_HM_VR_hi" : ErrorTuple(h_fsMR_hi),
              "h_HH_VR_hi" : ErrorTuple(h_fsMR_hi),
              "h_LL_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi),
              "h_LLM_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi),
              "h_LM_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi),
              "h_LH_VR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi),
              "h_HL_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi),
              "h_HLM_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi),
              "h_HM_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi),
              "h_HH_VR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi),

              "h_LL_SR_hi" : ErrorTuple(h_fsMR_hi),
              "h_LLM_SR_hi" : ErrorTuple(h_fsMR_hi),
              "h_LM_SR_hi" : ErrorTuple(h_fsMR_hi),
              "h_LH_SR_hi" : ErrorTuple(h_fsMR_hi),
              "h_HL_SR_hi" : ErrorTuple(h_fsMR_hi),
              "h_HLM_SR_hi" : ErrorTuple(h_fsMR_hi),
              "h_HM_SR_hi" : ErrorTuple(h_fsMR_hi),
              "h_HH_SR_hi" : ErrorTuple(h_fsMR_hi),
              "h_LL_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi),
              "h_LLM_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi),
              "h_LM_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi),
              "h_LH_SR_23_hi" : ErrorTuple(h_fsMR_Nj23_hi),
              "h_HL_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi),
              "h_HLM_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi),
              "h_HM_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi),
              "h_HH_SR_4_hi" : ErrorTuple(h_fsMR_Nj4_hi),

              # lo pt
              "h_LL_VR_lo" : ErrorTuple(h_fsMR_lo),
              "h_LLM_VR_lo" : ErrorTuple(h_fsMR_lo),
              "h_LM_VR_lo" : ErrorTuple(h_fsMR_lo),
              "h_LH_VR_lo" : ErrorTuple(h_fsMR_lo),
              "h_HL_VR_lo" : ErrorTuple(h_fsMR_lo),
              "h_HLM_VR_lo" : ErrorTuple(h_fsMR_lo),
              "h_HM_VR_lo" : ErrorTuple(h_fsMR_lo),
              "h_HH_VR_lo" : ErrorTuple(h_fsMR_lo),
              "h_LL_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo),
              "h_LLM_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo),
              "h_LM_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo),
              "h_LH_VR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo),
              "h_HL_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo),
              "h_HLM_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo),
              "h_HM_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo),
              "h_HH_VR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo),

              "h_LL_SR_lo" : ErrorTuple(h_fsMR_lo),
              "h_LLM_SR_lo" : ErrorTuple(h_fsMR_lo),
              "h_LM_SR_lo" : ErrorTuple(h_fsMR_lo),
              "h_LH_SR_lo" : ErrorTuple(h_fsMR_lo),
              "h_HL_SR_lo" : ErrorTuple(h_fsMR_lo),
              "h_HLM_SR_lo" : ErrorTuple(h_fsMR_lo),
              "h_HM_SR_lo" : ErrorTuple(h_fsMR_lo),
              "h_HH_SR_lo" : ErrorTuple(h_fsMR_lo),
              "h_LL_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo),
              "h_LLM_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo),
              "h_LM_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo),
              "h_LH_SR_23_lo" : ErrorTuple(h_fsMR_Nj23_lo),
              "h_HL_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo),
              "h_HLM_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo),
              "h_HM_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo),
              "h_HH_SR_4_lo" : ErrorTuple(h_fsMR_Nj4_lo)
              }

    fs = { "h_LL_VR" : ContentTuple(h_fsMR),
              "h_LLM_VR" : ContentTuple(h_fsMR),
              "h_LM_VR" : ContentTuple(h_fsMR),
              "h_LH_VR" : ContentTuple(h_fsMR),
              "h_HL_VR" : ContentTuple(h_fsMR),
              "h_HLM_VR" : ContentTuple(h_fsMR),
              "h_HM_VR" : ContentTuple(h_fsMR),
              "h_HH_VR" : ContentTuple(h_fsMR),
              "h_LL_VR_23" : ContentTuple(h_fsMR_Nj23),
              "h_LLM_VR_23" : ContentTuple(h_fsMR_Nj23),
              "h_LM_VR_23" : ContentTuple(h_fsMR_Nj23),
              "h_LH_VR_23" : ContentTuple(h_fsMR_Nj23),
              "h_HL_VR_4" : ContentTuple(h_fsMR_Nj4),
              "h_HLM_VR_4" : ContentTuple(h_fsMR_Nj4),
              "h_HM_VR_4" : ContentTuple(h_fsMR_Nj4),
              "h_HH_VR_4" : ContentTuple(h_fsMR_Nj4),
              
              "h_LL_SR" : ContentTuple(h_fsMR),
              "h_LLM_SR" : ContentTuple(h_fsMR),
              "h_LM_SR" : ContentTuple(h_fsMR),
              "h_LH_SR" : ContentTuple(h_fsMR),
              "h_HL_SR" : ContentTuple(h_fsMR),
              "h_HLM_SR" : ContentTuple(h_fsMR),
              "h_HM_SR" : ContentTuple(h_fsMR),
              "h_HH_SR" : ContentTuple(h_fsMR),
              "h_LL_SR_23" : ContentTuple(h_fsMR_Nj23),
              "h_LLM_SR_23" : ContentTuple(h_fsMR_Nj23),
              "h_LM_SR_23" : ContentTuple(h_fsMR_Nj23),
              "h_LH_SR_23" : ContentTuple(h_fsMR_Nj23),
              "h_HL_SR_4" : ContentTuple(h_fsMR_Nj4),
              "h_HLM_SR_4" : ContentTuple(h_fsMR_Nj4),
              "h_HM_SR_4" : ContentTuple(h_fsMR_Nj4),
              "h_HH_SR_4" : ContentTuple(h_fsMR_Nj4),
              
              # hi pt
              "h_LL_VR_hi" : ContentTuple(h_fsMR_hi),
              "h_LLM_VR_hi" : ContentTuple(h_fsMR_hi),
              "h_LM_VR_hi" : ContentTuple(h_fsMR_hi),
              "h_LH_VR_hi" : ContentTuple(h_fsMR_hi),
              "h_HL_VR_hi" : ContentTuple(h_fsMR_hi),
              "h_HLM_VR_hi" : ContentTuple(h_fsMR_hi),
              "h_HM_VR_hi" : ContentTuple(h_fsMR_hi),
              "h_HH_VR_hi" : ContentTuple(h_fsMR_hi),
              "h_LL_VR_23_hi" : ContentTuple(h_fsMR_Nj23_hi),
              "h_LLM_VR_23_hi" : ContentTuple(h_fsMR_Nj23_hi),
              "h_LM_VR_23_hi" : ContentTuple(h_fsMR_Nj23_hi),
              "h_LH_VR_23_hi" : ContentTuple(h_fsMR_Nj23_hi),
              "h_HL_VR_4_hi" : ContentTuple(h_fsMR_Nj4_hi),
              "h_HLM_VR_4_hi" : ContentTuple(h_fsMR_Nj4_hi),
              "h_HM_VR_4_hi" : ContentTuple(h_fsMR_Nj4_hi),
              "h_HH_VR_4_hi" : ContentTuple(h_fsMR_Nj4_hi),

              "h_LL_SR_hi" : ContentTuple(h_fsMR_hi),
              "h_LLM_SR_hi" : ContentTuple(h_fsMR_hi),
              "h_LM_SR_hi" : ContentTuple(h_fsMR_hi),
              "h_LH_SR_hi" : ContentTuple(h_fsMR_hi),
              "h_HL_SR_hi" : ContentTuple(h_fsMR_hi),
              "h_HLM_SR_hi" : ContentTuple(h_fsMR_hi),
              "h_HM_SR_hi" : ContentTuple(h_fsMR_hi),
              "h_HH_SR_hi" : ContentTuple(h_fsMR_hi),
              "h_LL_SR_23_hi" : ContentTuple(h_fsMR_Nj23_hi),
              "h_LLM_SR_23_hi" : ContentTuple(h_fsMR_Nj23_hi),
              "h_LM_SR_23_hi" : ContentTuple(h_fsMR_Nj23_hi),
              "h_LH_SR_23_hi" : ContentTuple(h_fsMR_Nj23_hi),
              "h_HL_SR_4_hi" : ContentTuple(h_fsMR_Nj4_hi),
              "h_HLM_SR_4_hi" : ContentTuple(h_fsMR_Nj4_hi),
              "h_HM_SR_4_hi" : ContentTuple(h_fsMR_Nj4_hi),
              "h_HH_SR_4_hi" : ContentTuple(h_fsMR_Nj4_hi),

              # lo pt
              "h_LL_VR_lo" : ContentTuple(h_fsMR_lo),
              "h_LLM_VR_lo" : ContentTuple(h_fsMR_lo),
              "h_LM_VR_lo" : ContentTuple(h_fsMR_lo),
              "h_LH_VR_lo" : ContentTuple(h_fsMR_lo),
              "h_HL_VR_lo" : ContentTuple(h_fsMR_lo),
              "h_HLM_VR_lo" : ContentTuple(h_fsMR_lo),
              "h_HM_VR_lo" : ContentTuple(h_fsMR_lo),
              "h_HH_VR_lo" : ContentTuple(h_fsMR_lo),
              "h_LL_VR_23_lo" : ContentTuple(h_fsMR_Nj23_lo),
              "h_LLM_VR_23_lo" : ContentTuple(h_fsMR_Nj23_lo),
              "h_LM_VR_23_lo" : ContentTuple(h_fsMR_Nj23_lo),
              "h_LH_VR_23_lo" : ContentTuple(h_fsMR_Nj23_lo),
              "h_HL_VR_4_lo" : ContentTuple(h_fsMR_Nj4_lo),
              "h_HLM_VR_4_lo" : ContentTuple(h_fsMR_Nj4_lo),
              "h_HM_VR_4_lo" : ContentTuple(h_fsMR_Nj4_lo),
              "h_HH_VR_4_lo" : ContentTuple(h_fsMR_Nj4_lo),

              "h_LL_SR_lo" : ContentTuple(h_fsMR_lo),
              "h_LLM_SR_lo" : ContentTuple(h_fsMR_lo),
              "h_LM_SR_lo" : ContentTuple(h_fsMR_lo),
              "h_LH_SR_lo" : ContentTuple(h_fsMR_lo),
              "h_HL_SR_lo" : ContentTuple(h_fsMR_lo),
              "h_HLM_SR_lo" : ContentTuple(h_fsMR_lo),
              "h_HM_SR_lo" : ContentTuple(h_fsMR_lo),
              "h_HH_SR_lo" : ContentTuple(h_fsMR_lo),
              "h_LL_SR_23_lo" : ContentTuple(h_fsMR_Nj23_lo),
              "h_LLM_SR_23_lo" : ContentTuple(h_fsMR_Nj23_lo),
              "h_LM_SR_23_lo" : ContentTuple(h_fsMR_Nj23_lo),
              "h_LH_SR_23_lo" : ContentTuple(h_fsMR_Nj23_lo),
              "h_HL_SR_4_lo" : ContentTuple(h_fsMR_Nj4_lo),
              "h_HLM_SR_4_lo" : ContentTuple(h_fsMR_Nj4_lo),
              "h_HM_SR_4_lo" : ContentTuple(h_fsMR_Nj4_lo),
              "h_HH_SR_4_lo" : ContentTuple(h_fsMR_Nj4_lo)
              }


#    histnames = [hname + ptstring for hname in ["h_LL_VR","h_LH_VR","h_HL_VR","h_HH_VR","h_LL_SR","h_LH_SR","h_HL_SR","h_HH_SR","h_LL_VR_23","h_LH_VR_23","h_HL_VR_4","h_HH_VR_4","h_LL_SR_23","h_LH_SR_23","h_HL_SR_4","h_HH_SR_4"] for ptstring in ["","_lo","_hi"]]
    histnames = ["h_" + nj + ht + "_" + reg + njsplit + ptstring for nj in ["H"] for ht in ["L","LM","M","H"] for reg in ["VR","SR"] for njsplit in ["","_4"] for ptstring in ["","_lo","_hi"]]
    histnames += ["h_" + nj + ht + "_" + reg + njsplit + ptstring for nj in ["L"] for ht in ["L","LM","M","H"] for reg in ["VR","SR"] for njsplit in ["","_23"] for ptstring in ["","_lo","_hi"]]

    for histname in histnames:
#    for histname in ["h_LH_VR_23"]:
        print histname
        h = filelist[0].Get(histname)
        # First, sum all region histograms
        for f in filelist[1:]:
            h.Add(f.Get(histname))
        # Now, propagate fshort errors to the predicted counts of these histograms (y bin 2)
        # ROOT takes care of observed STC and ST counts for us, and tracks the error in bin 2 coming from STC statistics, but not that coming from fshort
        # if CR count is 0, then take upper STC count as 1.83, and upper end of predicted error as 1.83 * (fshort+fshorterr)
        fs_P = fs[histname][0]
        fs_P_err = fserr[histname][0]
        fs_P3 = fs[histname][1]
        fs_P3_err = fserr[histname][1]
        fs_P4 = fs[histname][2]
        fs_P4_err = fserr[histname][2]
        fs_M = fs[histname][3]
        fs_M_err = fserr[histname][3]
        fs_L = fs[histname][4]
        fs_L_err = fserr[histname][4]

        Perr = h.GetBinError(1,2) # pred ST err for P, currently based only on STC stats
        Pcount = h.GetBinContent(1,2) # pred ST counts for P
        new_Perr = 1.83 * (fs_P + fs_P_err) if Pcount == 0 or fs_P == 0 else sqrt( (Perr/Pcount)**2 + (fs_P_err / fs_P)**2 ) * Pcount # total relative error is rel error from STC counts and rel error from fshort, in quadrature
        h.SetBinError(1,2,new_Perr) 
        P3err = h.GetBinError(2,2) # pred ST err for P3, currently based only on STC stats
        P3count = h.GetBinContent(2,2) # pred ST counts for P3
        new_P3err = 1.83 * (fs_P3 + fs_P3_err) if P3count == 0 or fs_P3 == 0 else sqrt( (P3err/P3count)**2 + (fs_P3_err / fs_P3)**2 ) * P3count # total relative error is rel error from STC counts and rel error from fshort, in quadrature
        h.SetBinError(2,2,new_P3err) 
        P4err = h.GetBinError(3,2) # pred ST err for P4, currently based only on STC stats
        P4count = h.GetBinContent(3,2) # pred ST counts for P4
        new_P4err = 1.83 * (fs_P4 + fs_P4_err) if P4count == 0 or fs_P4 == 0 else sqrt( (P4err/P4count)**2 + (fs_P4_err / fs_P4)**2 ) * P4count # total relative error is rel error from STC counts and rel error from fshort, in quadrature
        h.SetBinError(3,2,new_P4err) 
        Merr = h.GetBinError(4,2) # pred ST err for M, currently based only on STC stats
        Mcount = h.GetBinContent(4,2) # pred ST counts for M
        new_Merr = 1.83 * (fs_M + fs_M_err) if Mcount == 0 or fs_M == 0 else sqrt( (Merr/Mcount)**2 + (fs_M_err / fs_M)**2 ) * Mcount # total relative error is rel error from STC counts and rel error from fshort, in quadrature
        h.SetBinError(4,2,new_Merr)
        Lerr = h.GetBinError(5,2) # pred ST err for L, currently based only on STC stats
        Lcount = h.GetBinContent(5,2) # pred ST counts for L
        new_Lerr = 1.83 * (fs_L + fs_L_err) if Lcount == 0 or fs_L == 0 else sqrt( (Lerr/Lcount)**2 + (fs_L_err / fs_L)**2 ) * Lcount # total relative error is rel error from STC counts and rel error from fshort, in quadrature
        h.SetBinError(5,2,new_Lerr )
        outfile.cd()
        h.Write()

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
    h_fsMR_syst.Write()
    h_fsMR_Nj23_syst.Write()
    h_fsMR_Nj4_syst.Write()
    h_fsMR_hi_syst.Write()
    h_fsMR_Nj23_hi_syst.Write()
    h_fsMR_Nj4_hi_syst.Write()
    h_fsMR_lo_syst.Write()
    h_fsMR_Nj23_lo_syst.Write()
    h_fsMR_Nj4_lo_syst.Write()
    h_fsMR_syststat.Write()
    h_fsMR_Nj23_syststat.Write()
    h_fsMR_Nj4_syststat.Write()
    h_fsMR_hi_syststat.Write()
    h_fsMR_Nj23_hi_syststat.Write()
    h_fsMR_Nj4_hi_syststat.Write()
    h_fsMR_lo_syststat.Write()
    h_fsMR_Nj23_lo_syststat.Write()
    h_fsMR_Nj4_lo_syststat.Write()

    # Now find non-closure systematic

#    regions = [("L",nj,ht,"") for nj in ["L","H"] for ht in ["LM","H"]] # L regions are common to 2016 and 2017-2018
    regions = [("M",nj,ht) for nj in ["L","H"] for ht in ["LM","H"]]
    if filename == d18 or filename == d17 or filename == d1718 or filename == m17 or filename == m18:
        regions += [("P3",nj,ht) for nj in ["L","H"] for ht in ["L","M","H"]] # P3 tracks have an extra Ht split
        regions += [("P4",nj,ht) for nj in ["L","H"] for ht in ["LM","H"]] # tracks are like M
    elif filename == d16 or filename == m16:
        regions += [("P",nj,ht) for nj in ["L","H"] for ht in ["L","M","H"]] # P tracks have an extra Ht split
    

    for ptstring in ["_lo","_hi"]:
        max_nonclosure_sigma = 0
        max_nonclosure_error = 0
        max_nonclosure_delta = 0
        max_nonclosure_pred = 0
        max_nonclosure_obs = 0
        max_nonclosure_region = ""
        for region in regions:
            cat = region[0]
            if cat == "P":
                index = 1
            elif cat == "P3":
                index = 2
            elif cat == "P4":
                index = 3
            elif cat == "M":
                index = 4
            nj = region[1]
            njsplit = "23" if nj == "L" else "4"
            ht = region[2]
            histname = "h_" + nj + ht + "_VR_" + njsplit + ptstring
            h = outfile.Get(histname)
            pred = h.GetBinContent(index,2)
            obs = h.GetBinContent(index,1)
            if pred == 0 or obs == 0: continue
            pred_err = h.GetBinError(index,2)
            obs_err = h.GetBinError(index,1)
            total_err = sqrt(pred_err**2 + obs_err**2)
            delta_in_sigma = abs(pred-obs) / total_err
            if delta_in_sigma > max_nonclosure_sigma:
                max_nonclosure_sigma = delta_in_sigma
                max_nonclosure_error = total_err
                max_nonclosure_delta = abs(pred-obs)
                max_nonclosure_region = region
                max_nonclosure_pred = pred
                max_nonclosure_obs = obs
        print "Maximum nonclosure region is:","{} {}{} for {}".format(max_nonclosure_region[0],max_nonclosure_region[1],max_nonclosure_region[2],ptstring)
        print "With Delta (in sigma):",max_nonclosure_sigma
        print "Delta:",max_nonclosure_delta
        print "Pred:",max_nonclosure_pred
        print "Total Error:",max_nonclosure_error
        # require that even max nonclosure region be within 1 sigma, total
        # (delta)**2 = syst**2 + total_err**2 => syst = sqrt((delta)**2 - total_err**2)
        expr = max_nonclosure_delta**2 - max_nonclosure_error**2
        if expr < 0:
            systematic = 0
        else:
            systematic = sqrt(expr)/sqrt(max_nonclosure_pred)

        print "Systematic:",systematic

        h_syst = ROOT.TH1F("h_nonclosure_systematic"+ptstring,"Systematic from Non-closure in Validation Region",1,0,1)
        h_syst.SetBinContent(1,systematic)
        outfile.cd()
        h_syst.Write()

    for f in filelist:
        f.Close()
    outfile.Close()

print "Done"
