# STCImplementation background estimates and plots

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

def printHeader(outfile):
    outfile.write("\documentclass[10pt]{article}\n\n")

    outfile.write("\usepackage{amsmath,graphicx,fancyhdr,setspace,multirow,booktabs}\n")
    outfile.write("\usepackage[landscape]{geometry}\n")
    outfile.write("\usepackage[table]{xcolor}\n\n")

    outfile.write("\\addtolength{\\topmargin}{-.875in}\n")
    outfile.write("\\addtolength{\\textheight}{1.75in}\n\n")

    outfile.write("\\addtolength{\oddsidemargin}{-.875in}\n")
    outfile.write("\\addtolength{\evensidemargin}{-.875in}\n")
    outfile.write("\\addtolength{\\textwidth}{1.25in}\n\n")

    outfile.write("\\nofiles\n\n")

    outfile.write("\pagestyle{empty}\n\n")

    outfile.write("\setlength{\parindent}{0in}\n")
    outfile.write("\setlength{\columnsep}{1cm}\n\n")

    outfile.write("\pagestyle{fancy}\n")
    outfile.write("\\fancyhf{}\n")
    outfile.write("\lhead{Dylan Gilbert}\n")
    outfile.write("\\rhead{\\thepage}\n\n")

    outfile.write("\onehalfspacing\n\n")

    outfile.write("\\newcolumntype{g}{>{\\columncolor{blue!25}}c}\n\n")


def startFshortTableData(outfile):
    outfile.write("\chead{Fshort Table}\n")
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *2c g | *2c g}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Variation} & \multicolumn{3}{c}{2017 Data} & \multicolumn{3}{c}{2016 Data}\\\\ \n")
    outfile.write(" & STC & ST & $f_{short}$ & STC & ST & $f_{short}$\\\\ \n ")
    outfile.write("\hline\n")

def startFshortTableMC(outfile):
    outfile.write("\chead{Fshort Table}\n")
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *2c g | *2c g}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Variation} & \multicolumn{3}{c}{2017 MC} &  \multicolumn{3}{c}{2016 MC} \\\\ \n")
    outfile.write(" & STC & ST & $f_{short}$ & STC & ST & $f_{short}$\\\\ \n ")
    outfile.write("\hline\n")

def printFooter(outfile):
    outfile.write("\\bottomrule\n")
    outfile.write("\end{tabular}\n\n")
    outfile.write("\\bigskip\n\n")

    outfile.write("\end{document}\n\n")

def getFshort(region,data):
    vals17 = {}
    vals16 = {}
    errs17 = {}
    errs16 = {}
    fileregion = region[2:]
    if region[0] == "P":
        cat = 2
    elif region[0] == "M":
        cat = 3
    else:
        cat = 4
    for tag in ["Default","HT250","HT450","HT450MET100","MET30","MET100","MET250"]:
        d17=ROOT.TFile.Open("output/Fshort_data_2017_fsvar{}_weight1.root".format(tag)) if data else ROOT.TFile.Open("output/Fshort_mc_2017_fsvar{}_weight1.root".format(tag))
        d16=ROOT.TFile.Open("output/Fshort_data_2016_fsvar{}_weight1.root".format(tag)) if data else ROOT.TFile.Open("output/Fshort_mc_2016_fsvar{}_weight1.root".format(tag))

        h = d17.Get("h_fs"+fileregion)
        vals17[tag + " STC"] = h.GetBinContent(cat,3)
        vals17[tag + " ST"] = h.GetBinContent(cat,2)
        vals17[tag + " FS"] = h.GetBinContent(cat,1)
        errs17[tag + " STC"] = h.GetBinError(cat,3)
        errs17[tag + " ST"] = h.GetBinError(cat,2)
        errs17[tag + " FS"] = h.GetBinError(cat,1)

        h = d16.Get("h_fs"+fileregion)
        vals16[tag + " STC"] = h.GetBinContent(cat,3)
        vals16[tag + " ST"] = h.GetBinContent(cat,2)
        vals16[tag + " FS"] = h.GetBinContent(cat,1)
        errs16[tag + " STC"] = h.GetBinError(cat,3)
        errs16[tag + " ST"] = h.GetBinError(cat,2)
        errs16[tag + " FS"] = h.GetBinError(cat,1)

        d17.Close()
        d16.Close()

    return vals17,errs17,vals16,errs16

writeabletag = { "Default" : "Baseline",
                 "HT250"   : "$250 < H_{T} < 450$ GeV",
                 "HT450"   : "$H_{T} > 450$ GeV",
                 "HT450MET100" : "$H_{T} > 450$ and $\\text{MET} > 100$ GeV",
                 "MET30"   : "$30 < \\text{MET} < 100$ GeV",
                 "MET100" : "$ 100 < \\text{MET} < 250$ GeV",
                 "MET250"  : "$\\text{MET} > 250$ GeV"
                 }

def getLineData(tag,D17,eD17,D16,eD16):
    
    return "{} & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(writeabletag[tag],
                                                                                                                                                                                                                                    D17[tag+" STC"], D17[tag+" ST"], D17[tag+" FS"], eD17[tag+" FS"],                               
                                                                                                                                                                                                                                    D16[tag+" STC"], D16[tag+" ST"], D16[tag+" FS"], eD16[tag+" FS"])

def getLineMC(tag,M17,eM17,M16,eM16):
    
    return "{} & {:.2f} $\pm$ {:.2f} & {:.2f} $\pm$ {:.2f} & {:.3f} $\pm$ {:.3f} & {:.2f} $\pm$ {:.2f} & {:.2f} $\pm$ {:.2f} & {:.3f} $\pm$ {:.3f}  \\\\ \n".format(writeabletag[tag],
                                                                                                                                                                                                                                    M17[tag+" STC"], eM17[tag+" STC"], M17[tag+" ST"], eM17[tag+" ST"], M17[tag+" FS"], eM17[tag+" FS"],
                                                 M16[tag+" STC"], eM16[tag+" STC"], M16[tag+" ST"], eM16[tag+" ST"], M16[tag+" FS"], eM16[tag+" FS"])
for region in ["P_FSR_23","M_FSR_23","L_FSR_23","P_FSR_4","M_FSR_4","L_FSR_4","P_VR_23","M_VR_23","L_VR_23","P_VR_4","M_VR_4","L_VR_4"]:
    print region

    D17,eD17,D16,eD16=getFshort(region,True)

    output = open("variation_tables/fshort_data_{}.tex".format(region),"w")
    printHeader(output)
    startFshortTableData(output)
    for tag in ["Default","HT250","HT450","HT450MET100","MET30","MET100","MET250"]:
        output.write(getLineData(tag,D17,eD17,D16,eD16))
    printFooter(output)
    output.close()

    D17,eD17,D16,eD16=getFshort(region,False)

    output = open("variation_tables/fshort_mc_{}.tex".format(region),"w")
    printHeader(output)
    startFshortTableMC(output)
    for tag in ["Default","HT250","HT450","HT450MET100","MET30","MET100","MET250"]:
        output.write(getLineMC(tag,D17,eD17,D16,eD16))
    printFooter(output)
    output.close()

for region in ["P_SR_23","M_SR_23","L_SR_23","P_SR_4","M_SR_4","L_SR_4"]:
    print region

    M17,eM17,M16,eM16=getFshort(region,False)

    output = open("variation_tables/fshort_mc_{}.tex".format(region),"w")
    printHeader(output)
    startFshortTableMC(output)
    for tag in ["Default","HT250","HT450","HT450MET100","MET30","MET100","MET250"]:
        output.write(getLineMC(tag,M17,eM17,M16,eM16))
    printFooter(output)
    output.close()


print "Done"
