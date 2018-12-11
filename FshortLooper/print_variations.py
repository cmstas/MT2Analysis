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
ROOT.gStyle.SetOptStat(False)
ROOT.gStyle.SetLegendTextSize(0.03)
ROOT.gStyle.SetPaintTextFormat("#3.3g")
ROOT.gStyle.SetLegendBorderSize(0)
simplecanvas = ROOT.TCanvas("plotter")
simplecanvas.SetCanvasSize(600,600)
simplecanvas.SetTicks(1,1)
simplecanvas.SetLeftMargin(0.16)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetRightMargin(0.16)
simplecanvas.SetBottomMargin(0.35)

tl=ROOT.TLegend(0.2,0.7,0.4,0.8)

tag=sys.argv[1]

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
    d17=ROOT.TFile.Open("output/Fshort_data_2017_{}.root".format(tag)) if data else ROOT.TFile.Open("output/Fshort_mc_2017_{}.root".format(tag))
    d16=ROOT.TFile.Open("output/Fshort_data_2016_{}.root".format(tag)) if data else ROOT.TFile.Open("output/Fshort_mc_2016_{}.root".format(tag))
    for variation in ["Baseline","HT250","HT450","HT450MET100","MET30","MET100","MET250"]:
        hname = "h_fs"+fileregion+"_"+variation

        h = d17.Get(hname)
        vals17[variation + " STC"] = h.GetBinContent(cat,3)
        vals17[variation + " ST"] = h.GetBinContent(cat,2)
        vals17[variation + " FS"] = h.GetBinContent(cat,1)
        errs17[variation + " STC"] = h.GetBinError(cat,3)
        errs17[variation + " ST"] = h.GetBinError(cat,2)
        errs17[variation + " FS"] = h.GetBinError(cat,1)

        h = d16.Get(hname)
        vals16[variation + " STC"] = h.GetBinContent(cat,3)
        vals16[variation + " ST"] = h.GetBinContent(cat,2)
        vals16[variation + " FS"] = h.GetBinContent(cat,1)
        errs16[variation + " STC"] = h.GetBinError(cat,3)
        errs16[variation + " ST"] = h.GetBinError(cat,2)
        errs16[variation + " FS"] = h.GetBinError(cat,1)

    d17.Close()
    d16.Close()

    return vals17,errs17,vals16,errs16

writeabletag = { "Baseline" : "Baseline",
                 "HT250"   : "$250 < H_{T} < 450$ GeV",
                 "HT450"   : "$H_{T} > 450$ GeV",
                 "HT450MET100" : "$H_{T} > 450$ and $\\text{MET} > 100$ GeV",
                 "MET30"   : "$30 < \\text{MET} < 100$ GeV",
                 "MET100" : "$ 100 < \\text{MET} < 250$ GeV",
                 "MET250"  : "$\\text{MET} > 250$ GeV"
                 }

roottag = { "Baseline" : "Baseline",
                 "HT250"   : "250 < H_{T} < 450 GeV",
                 "HT450"   : "H_{T} > 450 GeV",
                 "HT450MET100" : "H_{T} > 450 and MET > 100 GeV",
                 "MET30"   : "30 < MET < 100 GeV",
                 "MET100" : " 100 < MET < 250 GeV",
                 "MET250"  : "MET > 250 GeV"
                 }

def getLineData(variation,D17,eD17,D16,eD16):
    
    return "{} & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(writeabletag[variation],
                                                                                                                                                                                                                                    D17[variation+" STC"], D17[variation+" ST"], D17[variation+" FS"], eD17[variation+" FS"],                               
                                                                                                                                                                                                                                    D16[variation+" STC"], D16[variation+" ST"], D16[variation+" FS"], eD16[variation+" FS"])

def getLineMC(variation,M17,eM17,M16,eM16):
    
    return "{} & {:.2f} $\pm$ {:.2f} & {:.2f} $\pm$ {:.2f} & {:.3f} $\pm$ {:.3f} & {:.2f} $\pm$ {:.2f} & {:.2f} $\pm$ {:.2f} & {:.3f} $\pm$ {:.3f}  \\\\ \n".format(writeabletag[variation],
                                                                                                                                                                                                                                    M17[variation+" STC"], eM17[variation+" STC"], M17[variation+" ST"], eM17[variation+" ST"], M17[variation+" FS"], eM17[variation+" FS"],
                                                 M16[variation+" STC"], eM16[variation+" STC"], M16[variation+" ST"], eM16[variation+" ST"], M16[variation+" FS"], eM16[variation+" FS"])

def makePlot(region,variations,vals,errs,desc):
    simplecanvas.cd()
    tl.Clear()
    nvariations=len(variations)
    title = desc + " " + region
    title = title.replace("_"," ")
    title = title.replace("23","N_{Jet}=2-3")
    title = title.replace("4","N_{Jet}>3")
    hist=ROOT.TH1D(region,title + " Kinematical Variations;;f_{short}",nvariations,0,nvariations)
    hist.SetLineWidth(3)
    hdef=hist.Clone(hist.GetName()+"_Baseline")
    variation_index = 1
    for variation in variations:
        hdef.GetXaxis().SetBinLabel(variation_index,roottag[variation])
        hist.SetBinContent(variation_index,vals[variation+" FS"])
        hist.SetBinError(variation_index,errs[variation+" FS"])
        hdef.SetBinContent(variation_index,vals["Baseline FS"])
        hdef.SetBinError(variation_index,errs["Baseline FS"])
        variation_index += 1    
    hdef.GetXaxis().LabelsOption("v")
    hdef.SetMaximum(hist.GetMaximum() * 2)
    hdef.GetXaxis().SetTitleOffset(4.8)
    hdef.SetMinimum(0)
    hist.SetLineColor(ROOT.kRed)
    hdef.SetLineColor(ROOT.kBlack)
    hdef_nofill = hdef.Clone(hdef.GetName()+"nofill")
    hdef.SetFillColor(ROOT.kGray)
    tl.AddEntry(hist,"Variations")
    tl.AddEntry(hdef,"Baseline")
    hdef.Draw("E2")
    hdef_nofill.Draw("hist same")
    hist.Draw("same")
    tl.Draw()
    simplecanvas.SaveAs("fshort_plots/{}.png".format(hist.GetName()+"_"+desc))

for region in ["P_MR_23","M_MR_23","L_MR_23","P_MR_4","M_MR_4","L_MR_4","P_VR_23","M_VR_23","L_VR_23","P_VR_4","M_VR_4","L_VR_4"]:
    print region

    D17,eD17,D16,eD16=getFshort(region,True)

    output = open("variation_tables/fshort_data_{}.tex".format(region),"w")
    printHeader(output)
    startFshortTableData(output)
    for variation in ["Baseline","HT250","HT450","HT450MET100","MET30","MET100","MET250"]:
        output.write(getLineData(variation,D17,eD17,D16,eD16))
    printFooter(output)
    output.close()

    makePlot(region,["HT250","HT450","MET30","MET100","MET250","HT450MET100"],D17,eD17,"2017_DATA")
    makePlot(region,["HT250","HT450","MET30","MET100","MET250","HT450MET100"],D16,eD16,"2016_DATA")

    D17,eD17,D16,eD16=getFshort(region,False)

    output = open("variation_tables/fshort_mc_{}.tex".format(region),"w")
    printHeader(output)
    startFshortTableMC(output)
    for variation in ["Baseline","HT250","HT450","HT450MET100","MET30","MET100","MET250"]:
        output.write(getLineMC(variation,D17,eD17,D16,eD16))
    printFooter(output)
    output.close()

    makePlot(region,["HT250","HT450","MET30","MET100","MET250","HT450MET100"],D17,eD17,"2017_MC")
    makePlot(region,["HT250","HT450","MET30","MET100","MET250","HT450MET100"],D16,eD16,"2016_MC")

for region in ["P_SR_23","M_SR_23","L_SR_23","P_SR_4","M_SR_4","L_SR_4"]:
    print region

    M17,eM17,M16,eM16=getFshort(region,False)

    output = open("variation_tables/fshort_mc_{}.tex".format(region),"w")
    printHeader(output)
    startFshortTableMC(output)
    for variation in ["Baseline","HT250","HT450","HT450MET100","MET30","MET100","MET250"]:
        output.write(getLineMC(variation,M17,eM17,M16,eM16))
    printFooter(output)
    output.close()

    makePlot(region,["HT250","HT450","MET30","MET100","MET250","HT450MET100"],M17,eM17,"2017_MC")
    makePlot(region,["HT250","HT450","MET30","MET100","MET250","HT450MET100"],M16,eM16,"2016_MC")

print "Done"
