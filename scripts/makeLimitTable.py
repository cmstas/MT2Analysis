# Make tables containing limits on benchmark points.

import ROOT
from math import sqrt
import re
import sys
import os
import subprocess

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

verbose = False # Print more status messages

ROOT.gROOT.SetBatch(True)

if len(sys.argv) < 2: 
    print "Which tag?"
    exit()

tag = sys.argv[1]
os.system("mkdir -p limit_tables")

def printHeader(outfile):
    outfile.write("\documentclass[10pt]{article}\n\n")

    outfile.write("\usepackage{amsmath,graphicx,setspace,multirow,booktabs}\n")
    outfile.write("\usepackage[paperheight=16in]{geometry}\n")
    outfile.write("\usepackage[table]{xcolor}\n")
    outfile.write("\usepackage[T1]{fontenc}\n")
    outfile.write("\usepackage{inconsolata}\n\n")

    outfile.write("\\addtolength{\\topmargin}{-.875in}\n")
    outfile.write("\\addtolength{\\textheight}{1.75in}\n\n")

    outfile.write("\\addtolength{\oddsidemargin}{-.875in}\n")
    outfile.write("\\addtolength{\evensidemargin}{-.875in}\n")
    outfile.write("\\addtolength{\\textwidth}{1.25in}\n\n")

    outfile.write("\\nofiles\n\n")

    outfile.write("\pagestyle{empty}\n\n")

    outfile.write("\setlength{\parindent}{0in}\n")
    outfile.write("\setlength{\columnsep}{1cm}\n\n")

    outfile.write("\onehalfspacing\n\n")


def startLimitTable(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *3c | *3c | *2c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region(s)} & \multicolumn{3}{c}{10 cm} & \multicolumn{3}{c}{90 cm} & \multicolumn{2}{c}{Prompt}\\\\ \n")
    outfile.write(" & Data VR & MC VR & MC SR & Data VR & MC VR & MC SR & 2016-18 MC SR & 2016 MC SR\\\\ \n ")
    outfile.write("\hline\n")

def startSRLimitTable(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | c | c | *3c}\n")
    outfile.write("\\toprule\n")
    outfile.write("Region(s) & 10 cm & 90 cm & 2016-18 T1qqqq Exp & 2016 T1qqqq Exp & 2016 T1qqqq Obs \\\\ \n ")
    outfile.write("\hline\n")

def startSRLimitTableObs(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *2c | *2c | *3c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region(s)} & \multicolumn{2}{c}{10 cm} & \multicolumn{2}{c}{90 cm} & 2016-18 T1qqqq     & \multicolumn{2}{c}{2016 T1qqqq} \\\\ \n ")
    outfile.write("                           & Exp      & Obs            & Exp       & Obs           & Exp                & Exp         & Obs               \\\\ \n ")
    outfile.write("\hline\n")

def printFooter(outfile):
    outfile.write("\\bottomrule\n")
    outfile.write("\end{tabular}\n\n")
    outfile.write("\\bigskip\n\n")

    outfile.write("\end{document}\n\n")

# 2017-18 exp (data), 2016 published exp (data), 2016 published obs (data)
main_limits = { (1800, 1400) : ("4.7", "8.7", "6.1"),
                (1800, 1600) : ("16.8", ">21", ">13"),
                (1800, 1700) : (">16.8", ">>21", ">>13"),
                (1700, 1600) : ("10.9", ">21",">13")
                }

def getLine(njht,pt="",cat="",year="",m1="",m2=""):
    if njht == "All" or njht == "hi" or njht == "lo" or njht == "Long":
        printable_region = njht
        if njht != "All" and njht != "Long": printable_region = "All " + njht +" $p_{T}$"
        elif njht == "Long": printable_region = "All L"
        filename = "limits_{}_10cm_{}-{}_MC_SR/higgsCombine{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht)
        f_MC_SR_10=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename = "limits_{}_90cm_{}-{}_MC_SR/higgsCombine{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht)
        f_MC_SR_90=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename = "limits_{}_10cm_{}-{}_Data_VR/higgsCombine{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht)
        f_Data_VR_10=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename = "limits_{}_90cm_{}-{}_Data_VR/higgsCombine{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht)
        f_Data_VR_90=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename = "limits_{}_10cm_{}-{}_MC_VR/higgsCombine{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht)
        f_MC_VR_10=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename = "limits_{}_90cm_{}-{}_MC_VR/higgsCombine{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht)
        f_MC_VR_90=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
    else:
        nj = "4" if njht[0] == "H" else "23"
        post_region = "{}_{}_{}_{}_fastsim_ctaucm_{}-{}_{}_{}".format(nj,pt,cat,year,m1,m2,m1,m2) if len(pt) > 0 else "{}_{}_{}_fastsim_ctaucm_{}-{}_{}_{}".format(nj,cat,year,m1,m2,m1,m2)
        printable_region = cat + " " + njht + " "+pt+" "+year
        filename="limits_{}_10cm_{}-{}_MC_SR/higgsCombine{}_SR_{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht,post_region.replace("ctau","10")) 
        f_MC_SR_10=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename="limits_{}_90cm_{}-{}_MC_SR/higgsCombine{}_SR_{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht,post_region.replace("ctau","90")) 
        f_MC_SR_90=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename="limits_{}_10cm_{}-{}_Data_VR/higgsCombine{}_VR_{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht,post_region.replace("ctau","10")) 
        f_Data_VR_10=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename="limits_{}_90cm_{}-{}_Data_VR/higgsCombine{}_VR_{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht,post_region.replace("ctau","90")) 
        f_Data_VR_90=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename="limits_{}_10cm_{}-{}_MC_VR/higgsCombine{}_VR_{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht,post_region.replace("ctau","10")) 
        f_MC_VR_10=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename="limits_{}_90cm_{}-{}_MC_VR/higgsCombine{}_VR_{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht,post_region.replace("ctau","90")) 
        f_MC_VR_90=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None

    limit_MC_SR_10 = -1
    if f_MC_SR_10 != None:
        t_MC_SR_10 = f_MC_SR_10.Get("limit")
        if t_MC_SR_10 != None:
            if t_MC_SR_10.GetEntries() == 6:
                t_MC_SR_10.GetEntry(2)
                exp_limit_50 = t_MC_SR_10.limit
                #obs_limit from t.GetEntry(5)
                limit_MC_SR_10 = float(exp_limit_50)
                if verbose: print "limit_MC_SR_10 exists:",f_MC_SR_10.GetName(),limit_MC_SR_10
                f_MC_SR_10.Close()
    limit_MC_SR_90 = -1
    if f_MC_SR_90 != None:
        t_MC_SR_90 = f_MC_SR_90.Get("limit")
        if t_MC_SR_90 != None:
            if t_MC_SR_90.GetEntries() == 6:
                t_MC_SR_90.GetEntry(2)
                exp_limit_50 = t_MC_SR_90.limit
                #obs_limit from t.GetEntry(5)
                limit_MC_SR_90 = float(exp_limit_50)
                if verbose: print "limit_MC_SR_90 exists:",f_MC_SR_90.GetName(),limit_MC_SR_90
                f_MC_SR_90.Close()
    limit_MC_VR_10 = -1
    if f_MC_VR_10 != None:
        t_MC_VR_10 = f_MC_VR_10.Get("limit")
        if t_MC_VR_10 != None:
            if t_MC_VR_10.GetEntries() == 6:
                t_MC_VR_10.GetEntry(2)
                exp_limit_50 = t_MC_VR_10.limit
                #obs_limit from t.GetEntry(5)
                limit_MC_VR_10 = float(exp_limit_50)
                f_MC_VR_10.Close()
                if verbose: print "limit_MC_VR_10 exists:",f_MC_VR_10.GetName(),limit_MC_VR_10
    limit_MC_VR_90 = -1
    if f_MC_VR_90 != None:
        t_MC_VR_90 = f_MC_VR_90.Get("limit")
        if t_MC_VR_90 != None:
            if t_MC_VR_90.GetEntries() == 6:
                t_MC_VR_90.GetEntry(2)
                exp_limit_50 = t_MC_VR_90.limit
                #obs_limit from t.GetEntry(5)
                limit_MC_VR_90 = float(exp_limit_50)
                if verbose: print "limit_MC_VR_90 exists:",f_MC_VR_90.GetName(),limit_MC_VR_90
                f_MC_VR_90.Close()
    limit_Data_VR_10 = -1
    if f_Data_VR_10 != None:
        t_Data_VR_10 = f_Data_VR_10.Get("limit")
        if t_Data_VR_10 != None:
            if t_Data_VR_10.GetEntries() == 6:
                t_Data_VR_10.GetEntry(2)
                exp_limit_50 = t_Data_VR_10.limit
                #obs_limit from t.GetEntry(5)
                limit_Data_VR_10 = float(exp_limit_50)
                if verbose: print "limit_Data_VR_10 exists:",f_Data_VR_10.GetName(),limit_Data_VR_10
                f_Data_VR_10.Close()
    limit_Data_VR_90 = -1
    if f_Data_VR_90 != None:
        t_Data_VR_90 = f_Data_VR_90.Get("limit")
        if t_Data_VR_90 != None:
            if t_Data_VR_90.GetEntries() == 6:
                t_Data_VR_90.GetEntry(2)
                exp_limit_50 = t_Data_VR_90.limit
                #obs_limit from t.GetEntry(5)
                limit_Data_VR_90 = float(exp_limit_50)
                if verbose: print "limit_Data_VR_90 exists:",f_Data_VR_90.GetName(),limit_Data_VR_90
                f_Data_VR_90.Close()

    main_limit = main_limits[(int(m1),int(m2))]
    limit_16to18 = main_limit[0] if njht == "All" else " "
    limit_16     = main_limit[1] if njht == "All" else " "

    line_to_return = "{} & {:.2f} & {:.2f} & {:.2f} & {:.2f} & {:.2f} & {:.2f} & {} & {}\\\\ \n".format(printable_region.replace("and","\&"),limit_Data_VR_10,limit_MC_VR_10,limit_MC_SR_10,limit_Data_VR_90,limit_MC_VR_90,limit_MC_SR_90,limit_16to18,limit_16)
    return line_to_return.replace("-1.00","--")

def getSRLine(njht,pt="",cat="",year="",m1="",m2=""):
    if njht == "All" or njht == "hi" or njht == "lo" or njht == "Long":
        printable_region = "All"
        if njht != "All" and njht != "Long": printable_region = "All " + njht +" $p_{T}$"
        elif njht == "Long": printable_region = "All L"
        filename = "limits_{}_10cm_{}-{}_Data_SR/higgsCombine{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht)
        f_Data_SR_10=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename = "limits_{}_90cm_{}-{}_Data_SR/higgsCombine{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht)
        f_Data_SR_90=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
    else:
        nj = "4" if njht[0] == "H" else "23"
        post_region = "{}_{}_{}_{}_fastsim_ctaucm_{}-{}_{}_{}".format(nj,pt,cat,year,m1,m2,m1,m2) if len(pt) > 0 else "{}_{}_{}_fastsim_ctaucm_{}-{}_{}_{}".format(nj,cat,year,m1,m2,m1,m2)
        printable_region = cat+ " " + njht + " "+pt+" "+year
        filename="limits_{}_10cm_{}-{}_Data_SR/higgsCombine{}_SR_{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht,post_region.replace("ctau","10")) 
        f_Data_SR_10=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename="limits_{}_90cm_{}-{}_Data_SR/higgsCombine{}_SR_{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht,post_region.replace("ctau","90")) 
        f_Data_SR_90=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None

    limit_Data_SR_10 = -1
    limit_Data_SR_10_p1 = -1
    limit_Data_SR_10_m1 = -1
    if f_Data_SR_10 != None:
        t_Data_SR_10 = f_Data_SR_10.Get("limit")
        if t_Data_SR_10 != None:
            if t_Data_SR_10.GetEntries() == 6:
                t_Data_SR_10.GetEntry(1)
                exp_limit_m1 = t_Data_SR_10.limit
                limit_Data_SR_10_m1 = float(exp_limit_m1)
                t_Data_SR_10.GetEntry(2)
                exp_limit_50 = t_Data_SR_10.limit
                #obs_limit from t.GetEntry(5)
                limit_Data_SR_10 = float(exp_limit_50)
                t_Data_SR_10.GetEntry(3)
                exp_limit_p1 = t_Data_SR_10.limit
                limit_Data_SR_10_p1 = float(exp_limit_p1)
                limit_Data_SR_10_m1 = limit_Data_SR_10 - limit_Data_SR_10_m1
                limit_Data_SR_10_p1 = limit_Data_SR_10_p1 - limit_Data_SR_10
                if verbose: print "limit_Data_SR_10 exists:",f_Data_SR_10.GetName(),limit_Data_SR_10
                f_Data_SR_10.Close()
    limit_Data_SR_90 = -1
    limit_Data_SR_90_p1 = -1
    limit_Data_SR_90_m1 = -1
    if f_Data_SR_90 != None:
        t_Data_SR_90 = f_Data_SR_90.Get("limit")
        if t_Data_SR_90 != None:
            if t_Data_SR_90.GetEntries() == 6:
                t_Data_SR_90.GetEntry(1)
                exp_limit_m1 = t_Data_SR_90.limit
                limit_Data_SR_90_m1 = float(exp_limit_m1)
                t_Data_SR_90.GetEntry(2)
                exp_limit_50 = t_Data_SR_90.limit
                #obs_limit from t.GetEntry(5)
                limit_Data_SR_90 = float(exp_limit_50)
                t_Data_SR_90.GetEntry(3)
                exp_limit_p1 = t_Data_SR_90.limit
                limit_Data_SR_90_p1 = float(exp_limit_p1)
                limit_Data_SR_90_m1 = limit_Data_SR_90 - limit_Data_SR_90_m1
                limit_Data_SR_90_p1 = limit_Data_SR_90_p1 - limit_Data_SR_90
                if verbose: print "limit_Data_SR_90 exists:",f_Data_SR_90.GetName(),limit_Data_SR_90
                f_Data_SR_90.Close()

    main_limit = main_limits[(int(m1),int(m2))]
    limit_16to18 = main_limit[0] if njht == "All" else " "
    limit_16_exp     = main_limit[1] if njht == "All" else " "
    limit_16_obs     = main_limit[2] if njht == "All" else " "

    line_to_return = "{} & \\textbf{{{:2.2g}}} & \\textbf{{{:2.2g}}} & {} & {} & {}\\\\ \n".format(printable_region.replace("and","\&"),limit_Data_SR_10,limit_Data_SR_90,limit_16to18,limit_16_exp,limit_16_obs)
    return line_to_return.replace("-1","--").replace("e+01","0")

def getSRLineWithErrors(njht,pt="",cat="",year="",m1="",m2=""):
    if njht == "All" or njht == "hi" or njht == "lo" or njht == "Long":
        printable_region = "All"
        if njht != "All" and njht != "Long": printable_region = "All " + njht +" $p_{T}$"
        elif njht == "Long": printable_region = "All L"
        filename = "limits_{}_10cm_{}-{}_Data_SR/higgsCombine{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht)
        f_Data_SR_10=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename = "limits_{}_90cm_{}-{}_Data_SR/higgsCombine{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht)
        f_Data_SR_90=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
    else:
        nj = "4" if njht[0] == "H" else "23"
        post_region = "{}_{}_{}_{}_fastsim_ctaucm_{}-{}_{}_{}".format(nj,pt,cat,year,m1,m2,m1,m2) if len(pt) > 0 else "{}_{}_{}_fastsim_ctaucm_{}-{}_{}_{}".format(nj,cat,year,m1,m2,m1,m2)
        printable_region = cat+ " " + njht + " "+pt+" "+year
        filename="limits_{}_10cm_{}-{}_Data_SR/higgsCombine{}_SR_{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht,post_region.replace("ctau","10")) 
        f_Data_SR_10=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename="limits_{}_90cm_{}-{}_Data_SR/higgsCombine{}_SR_{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht,post_region.replace("ctau","90")) 
        f_Data_SR_90=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None

    limit_Data_SR_10 = -1
    limit_Data_SR_10_p1 = -1
    limit_Data_SR_10_m1 = -1
    if f_Data_SR_10 != None:
        t_Data_SR_10 = f_Data_SR_10.Get("limit")
        if t_Data_SR_10 != None:
            if t_Data_SR_10.GetEntries() == 6:
                t_Data_SR_10.GetEntry(1)
                exp_limit_m1 = t_Data_SR_10.limit
                limit_Data_SR_10_m1 = float(exp_limit_m1)
                t_Data_SR_10.GetEntry(2)
                exp_limit_50 = t_Data_SR_10.limit
                #obs_limit from t.GetEntry(5)
                limit_Data_SR_10 = float(exp_limit_50)
                t_Data_SR_10.GetEntry(3)
                exp_limit_p1 = t_Data_SR_10.limit
                limit_Data_SR_10_p1 = float(exp_limit_p1)
                limit_Data_SR_10_m1 = limit_Data_SR_10 - limit_Data_SR_10_m1
                limit_Data_SR_10_p1 = limit_Data_SR_10_p1 - limit_Data_SR_10
                if verbose: print "limit_Data_SR_10 exists:",f_Data_SR_10.GetName(),limit_Data_SR_10
                f_Data_SR_10.Close()
    limit_Data_SR_90 = -1
    limit_Data_SR_90_p1 = -1
    limit_Data_SR_90_m1 = -1
    if f_Data_SR_90 != None:
        t_Data_SR_90 = f_Data_SR_90.Get("limit")
        if t_Data_SR_90 != None:
            if t_Data_SR_90.GetEntries() == 6:
                t_Data_SR_90.GetEntry(1)
                exp_limit_m1 = t_Data_SR_90.limit
                limit_Data_SR_90_m1 = float(exp_limit_m1)
                t_Data_SR_90.GetEntry(2)
                exp_limit_50 = t_Data_SR_90.limit
                #obs_limit from t.GetEntry(5)
                limit_Data_SR_90 = float(exp_limit_50)
                t_Data_SR_90.GetEntry(3)
                exp_limit_p1 = t_Data_SR_90.limit
                limit_Data_SR_90_p1 = float(exp_limit_p1)
                limit_Data_SR_90_m1 = limit_Data_SR_90 - limit_Data_SR_90_m1
                limit_Data_SR_90_p1 = limit_Data_SR_90_p1 - limit_Data_SR_90
                if verbose: print "limit_Data_SR_90 exists:",f_Data_SR_90.GetName(),limit_Data_SR_90
                f_Data_SR_90.Close()

    main_limit = main_limits[(int(m1),int(m2))]
    limit_16to18 = main_limit[0] if njht == "All" else " "
    limit_16_exp     = main_limit[1] if njht == "All" else " "
    limit_16_obs     = main_limit[2] if njht == "All" else " "

    line_to_return = "{} & \\textbf{{{:2.2g}}} (+ {:2.2g} - {:2.2g})  & \\textbf{{{:2.2g}}} (+ {:2.2g} - {:2.2g}) & {} & {} & {}\\\\ \n".format(printable_region.replace("and","\&"),limit_Data_SR_10,limit_Data_SR_10_p1,limit_Data_SR_10_m1,limit_Data_SR_90,limit_Data_SR_90_p1,limit_Data_SR_90_m1,limit_16to18,limit_16_exp,limit_16_obs)
    return line_to_return.replace("-1","--").replace("e+01","0")


def getSRLineObs(njht,pt="",cat="",year="",m1="",m2=""):
    if njht == "All" or njht == "hi" or njht == "lo" or njht == "Long":
        printable_region = "All"
        if njht != "All" and njht != "Long": printable_region = "All " + njht +" $p_{T}$"
        elif njht == "Long": printable_region = "All L"
        filename = "limits_{}_10cm_{}-{}_Data_SR/higgsCombine{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht)
        f_Data_SR_10=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename = "limits_{}_90cm_{}-{}_Data_SR/higgsCombine{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht)
        f_Data_SR_90=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
    else:
        nj = "4" if njht[0] == "H" else "23"
        post_region = "{}_{}_{}_{}_fastsim_ctaucm_{}-{}_{}_{}".format(nj,pt,cat,year,m1,m2,m1,m2) if len(pt) > 0 else "{}_{}_{}_fastsim_ctaucm_{}-{}_{}_{}".format(nj,cat,year,m1,m2,m1,m2)
        printable_region = cat+ " " + njht + " "+pt+" "+year
        filename="limits_{}_10cm_{}-{}_Data_SR/higgsCombine{}_SR_{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht,post_region.replace("ctau","10")) 
        f_Data_SR_10=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None
        filename="limits_{}_90cm_{}-{}_Data_SR/higgsCombine{}_SR_{}.AsymptoticLimits.mH120.root".format(tag,m1,m2,njht,post_region.replace("ctau","90")) 
        f_Data_SR_90=ROOT.TFile.Open(filename) if os.path.isfile(filename) else None

    exp_Data_SR_10 = -1
    obs_Data_SR_10 = -1
    if f_Data_SR_10 != None:
        t_Data_SR_10 = f_Data_SR_10.Get("limit")
        if t_Data_SR_10 != None:
            if t_Data_SR_10.GetEntries() == 6:
                t_Data_SR_10.GetEntry(5)
                obs_limit = t_Data_SR_10.limit
                obs_Data_SR_10 = float(obs_limit)
                t_Data_SR_10.GetEntry(2)
                exp_limit_50 = t_Data_SR_10.limit
                exp_Data_SR_10 = float(exp_limit_50)
    exp_Data_SR_90 = -1
    obs_Data_SR_90 = -1
    if f_Data_SR_90 != None:
        t_Data_SR_90 = f_Data_SR_90.Get("limit")
        if t_Data_SR_90 != None:
            if t_Data_SR_90.GetEntries() == 6:
                t_Data_SR_90.GetEntry(5)
                obs_limit = t_Data_SR_90.limit
                obs_Data_SR_90 = float(obs_limit)
                t_Data_SR_90.GetEntry(2)
                exp_limit_50 = t_Data_SR_90.limit
                exp_Data_SR_90 = float(exp_limit_50)

    main_limit = main_limits[(int(m1),int(m2))]
    limit_16to18 = main_limit[0] if njht == "All" else " "
    limit_16_exp     = main_limit[1] if njht == "All" else " "
    limit_16_obs     = main_limit[2] if njht == "All" else " "

    exp_10 = "\\textbf{{{:2.2g}}}".format(exp_Data_SR_10) if exp_Data_SR_10 > 0 else "--"
    obs_10 = "\\textbf{{{:2.2g}}}".format(obs_Data_SR_10) if obs_Data_SR_10 > 0 else "--"
    exp_90 = "\\textbf{{{:2.2g}}}".format(exp_Data_SR_90) if exp_Data_SR_90 > 0 else "--"
    obs_90 = "\\textbf{{{:2.2g}}}".format(obs_Data_SR_90) if obs_Data_SR_90 > 0 else "--"

    line_to_return = "{} & {} & {} & {} & {} & {} & {} & {}\\\\ \n".format(printable_region.replace("and","\&"),exp_10,obs_10,exp_90,obs_90,limit_16to18,limit_16_exp,limit_16_obs)
    return line_to_return


#############
# Make output
#############

#masses=[ ("1800","1400"), ("1800","1600"), ("1800","1700"), ("1700","1600") ]
masses=[ ("1800","1400"), ("1800","1600"), ("1800","1700") ]

for m1,m2 in masses:

    regions = [(njht,pt,"P","2016",m1,m2) for njht in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"]]
    regions += [(njht,pt,cat,"2017and2018",m1,m2) for njht in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"] for cat in ["P3"]]
    regions += [(njht,pt,cat,"2017and2018",m1,m2) for njht in ["LLM","LH","HLM","HH"] for pt in ["lo","hi"] for cat in ["P4"]]
    regions += [(njht,pt,"M",year,m1,m2) for njht in ["LLM","LH","HLM","HH"] for year in ["2016","2017and2018"] for pt in ["lo","hi"]]
    regions += [(njht,"","L",year,m1,m2) for njht in ["LLM","LH","HLM","HH"] for year in ["2016","2017and2018"]]

    output = open("limit_tables/limits_{0}_{1}_{2}_SR.tex".format(tag,m1,m2),"w")
    printHeader(output)
    startSRLimitTableObs(output)
    output.write("\\rowcolor{white}") 
    output.write(getSRLineObs("All",m1=m1,m2=m2))
    output.write("\hline\n")
    output.write("\\rowcolor{white}",) 
    output.write(getSRLineObs("hi",m1=m1,m2=m2))
    output.write("\\rowcolor{white}") 
    output.write(getSRLineObs("lo",m1=m1,m2=m2))
    output.write("\\rowcolor{white}") 
    output.write(getSRLineObs("Long",m1=m1,m2=m2))
    output.write("\hline\n")
    printFooter(output)
    output.close()

    output = open("limit_tables/limits_{0}_{1}_{2}_SR_fullset.tex".format(tag,m1,m2),"w")
    printHeader(output)
    startSRLimitTableObs(output)
    output.write("\\rowcolor{white}") 
    output.write(getSRLineObs("All",m1=m1,m2=m2))
    output.write("\hline\n")
    output.write("\\rowcolor{white}",) 
    output.write(getSRLineObs("hi",m1=m1,m2=m2))
    output.write("\\rowcolor{white}") 
    output.write(getSRLineObs("lo",m1=m1,m2=m2))
    output.write("\\rowcolor{white}") 
    output.write(getSRLineObs("Long",m1=m1,m2=m2))
    output.write("\hline\n")
    for region in regions: 
        if region[2] == "M":
            output.write("\\rowcolor{blue!25}") 
        elif region[2] == "L":
            output.write("\\rowcolor{red!25}") 
        else:
            output.write("\\rowcolor{green!25}") 
        output.write(getSRLineObs(region[0],region[1],region[2],region[3],region[4],region[5]))
    printFooter(output)
    output.close()


print "Done"
