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
simplecanvas.SetBottomMargin(0.12)

tl=ROOT.TLegend(0.2,0.7,0.4,0.8)

if len(sys.argv) < 2: 
    print "Which tag?"
    exit()

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


def startRegionTableData(outfile):
    outfile.write("\chead{Background Estimation Table}\n")
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *2c | *2c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region} & \multicolumn{2}{c}{2017 Data} & \multicolumn{2}{c}{2016 Data}\\\\ \n")
    outfile.write(" & Pred ST & Obs ST & Pred ST & Obs ST\\\\ \n ")
    outfile.write("\hline\n")

def startRegionTableMC(outfile):
    outfile.write("\chead{Background Estimation Table}\n")
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *2c | *2c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region} & \multicolumn{2}{c}{2017 MC} & \multicolumn{2}{c}{2016 MC}\\\\ \n")
    outfile.write(" & Pred ST & Obs ST & Pred ST & Obs ST \\\\ \n ")
    outfile.write("\hline\n")

def startRegionTableDataSTC(outfile):
    outfile.write("\chead{Background Estimation Table}\n")
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *3c | *3c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region} & \multicolumn{3}{c}{2017 Data} & \multicolumn{3}{c}{2016 Data}\\\\ \n")
    outfile.write(" & STC & Pred ST & Obs ST & STC & Pred ST & Obs ST\\\\ \n ")
    outfile.write("\hline\n")

def startRegionTableMCSTC(outfile):
    outfile.write("\chead{Background Estimation Table}\n")
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *3c | *3c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region} & \multicolumn{3}{c}{2017 MC} & \multicolumn{3}{c}{2016 MC}\\\\ \n")
    outfile.write(" & STC & Pred ST & Obs ST & STC & Pred ST & Obs ST \\\\ \n ")
    outfile.write("\hline\n")

def startFshortTableData(outfile):
    outfile.write("\chead{f$_{\text{short}}$ Table}\n")
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | c | c}\n")
    outfile.write("\\toprule\n")
    outfile.write("Category & 2017 Data & 2016 Data\\\\ \n")
    outfile.write("\hline\n")

def startFshortTableMC(outfile):
    outfile.write("\chead{f$_{\text{short}}$ Table}\n")
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | c | c}\n")
    outfile.write("\\toprule\n")
    outfile.write("Category & 2017 MC & 2016 MC\\\\ \n")
    outfile.write("\hline\n")

def printFooter(outfile):
    outfile.write("\\bottomrule\n")
    outfile.write("\end{tabular}\n\n")
    outfile.write("\\bigskip\n\n")

    outfile.write("\end{document}\n\n")

def getFshorts(f):
    h_FS = f.Get("h_FS")
    h_FS_23 = f.Get("h_FS_23")
    h_FS_4 = f.Get("h_FS_4")
    vals = {}
    errs = {}
    vals["P"] = h_FS.GetBinContent(1)
    vals["M"] = h_FS.GetBinContent(2)
    vals["L"] = h_FS.GetBinContent(3)

    vals["P 23"] = h_FS_23.GetBinContent(1)
    vals["M 23"] = h_FS_23.GetBinContent(2)
    vals["L 23"] = h_FS_23.GetBinContent(3)

    vals["P 4"] = h_FS_4.GetBinContent(1)
    vals["M 4"] = h_FS_4.GetBinContent(2)
    vals["L 4"] = h_FS_4.GetBinContent(3)

    errs["P"] = h_FS.GetBinError(1)
    errs["M"] = h_FS.GetBinError(2)
    errs["L"] = h_FS.GetBinError(3)

    errs["P 23"] = h_FS_23.GetBinError(1)
    errs["M 23"] = h_FS_23.GetBinError(2)
    errs["L 23"] = h_FS_23.GetBinError(3)

    errs["P 4"] = h_FS_4.GetBinError(1)
    errs["M 4"] = h_FS_4.GetBinError(2)
    errs["L 4"] = h_FS_4.GetBinError(3)

    return vals,errs

def getCounts(f):
    h_LL_VR_23 = f.Get("h_LL_VR_23")
    h_LH_VR_23 = f.Get("h_LH_VR_23")
    h_HL_VR_4 = f.Get("h_HL_VR_4")
    h_HH_VR_4 = f.Get("h_HH_VR_4")
    vals = {}
    errs = {}
    vals["P LL VR STC"] = h_LL_VR_23.GetBinContent(1,3)
    vals["P LL VR pre"] = h_LL_VR_23.GetBinContent(1,2)
    vals["P LL VR obs"] = h_LL_VR_23.GetBinContent(1,1)
    vals["M LL VR STC"] = h_LL_VR_23.GetBinContent(2,3)
    vals["M LL VR pre"] = h_LL_VR_23.GetBinContent(2,2)
    vals["M LL VR obs"] = h_LL_VR_23.GetBinContent(2,1)
    vals["L LL VR STC"] = h_LL_VR_23.GetBinContent(3,3)
    vals["L LL VR pre"] = h_LL_VR_23.GetBinContent(3,2)
    vals["L LL VR obs"] = h_LL_VR_23.GetBinContent(3,1)

    vals["P LH VR STC"] = h_LH_VR_23.GetBinContent(1,3)
    vals["P LH VR pre"] = h_LH_VR_23.GetBinContent(1,2)
    vals["P LH VR obs"] = h_LH_VR_23.GetBinContent(1,1)
    vals["M LH VR STC"] = h_LH_VR_23.GetBinContent(2,3)
    vals["M LH VR pre"] = h_LH_VR_23.GetBinContent(2,2)
    vals["M LH VR obs"] = h_LH_VR_23.GetBinContent(2,1)
    vals["L LH VR STC"] = h_LH_VR_23.GetBinContent(3,3)
    vals["L LH VR pre"] = h_LH_VR_23.GetBinContent(3,2)
    vals["L LH VR obs"] = h_LH_VR_23.GetBinContent(3,1)

    vals["P HL VR STC"] = h_HL_VR_4.GetBinContent(1,3)
    vals["P HL VR pre"] = h_HL_VR_4.GetBinContent(1,2)
    vals["P HL VR obs"] = h_HL_VR_4.GetBinContent(1,1)
    vals["M HL VR STC"] = h_HL_VR_4.GetBinContent(2,3)
    vals["M HL VR pre"] = h_HL_VR_4.GetBinContent(2,2)
    vals["M HL VR obs"] = h_HL_VR_4.GetBinContent(2,1)
    vals["L HL VR STC"] = h_HL_VR_4.GetBinContent(3,3)
    vals["L HL VR pre"] = h_HL_VR_4.GetBinContent(3,2)
    vals["L HL VR obs"] = h_HL_VR_4.GetBinContent(3,1)

    vals["P HH VR STC"] = h_HH_VR_4.GetBinContent(1,3)
    vals["P HH VR pre"] = h_HH_VR_4.GetBinContent(1,2)
    vals["P HH VR obs"] = h_HH_VR_4.GetBinContent(1,1)
    vals["M HH VR STC"] = h_HH_VR_4.GetBinContent(2,3)
    vals["M HH VR pre"] = h_HH_VR_4.GetBinContent(2,2)
    vals["M HH VR obs"] = h_HH_VR_4.GetBinContent(2,1)
    vals["L HH VR STC"] = h_HH_VR_4.GetBinContent(3,3)
    vals["L HH VR pre"] = h_HH_VR_4.GetBinContent(3,2)
    vals["L HH VR obs"] = h_HH_VR_4.GetBinContent(3,1)

    #Errors
    errs["P LL VR STC"] = h_LL_VR_23.GetBinError(1,3)
    errs["P LL VR pre"] = h_LL_VR_23.GetBinError(1,2)
    errs["P LL VR obs"] = h_LL_VR_23.GetBinError(1,1)
    errs["M LL VR STC"] = h_LL_VR_23.GetBinError(2,3)
    errs["M LL VR pre"] = h_LL_VR_23.GetBinError(2,2)
    errs["M LL VR obs"] = h_LL_VR_23.GetBinError(2,1)
    errs["L LL VR STC"] = h_LL_VR_23.GetBinError(3,3)
    errs["L LL VR pre"] = h_LL_VR_23.GetBinError(3,2)
    errs["L LL VR obs"] = h_LL_VR_23.GetBinError(3,1)

    errs["P LH VR STC"] = h_LH_VR_23.GetBinError(1,3)
    errs["P LH VR pre"] = h_LH_VR_23.GetBinError(1,2)
    errs["P LH VR obs"] = h_LH_VR_23.GetBinError(1,1)
    errs["M LH VR STC"] = h_LH_VR_23.GetBinError(2,3)
    errs["M LH VR pre"] = h_LH_VR_23.GetBinError(2,2)
    errs["M LH VR obs"] = h_LH_VR_23.GetBinError(2,1)
    errs["L LH VR STC"] = h_LH_VR_23.GetBinError(3,3)
    errs["L LH VR pre"] = h_LH_VR_23.GetBinError(3,2)
    errs["L LH VR obs"] = h_LH_VR_23.GetBinError(3,1)

    errs["P HL VR STC"] = h_HL_VR_4.GetBinError(1,3)
    errs["P HL VR pre"] = h_HL_VR_4.GetBinError(1,2)
    errs["P HL VR obs"] = h_HL_VR_4.GetBinError(1,1)
    errs["M HL VR STC"] = h_HL_VR_4.GetBinError(2,3)
    errs["M HL VR pre"] = h_HL_VR_4.GetBinError(2,2)
    errs["M HL VR obs"] = h_HL_VR_4.GetBinError(2,1)
    errs["L HL VR STC"] = h_HL_VR_4.GetBinError(3,3)
    errs["L HL VR pre"] = h_HL_VR_4.GetBinError(3,2)
    errs["L HL VR obs"] = h_HL_VR_4.GetBinError(3,1)

    errs["P HH VR STC"] = h_HH_VR_4.GetBinError(1,3)
    errs["P HH VR pre"] = h_HH_VR_4.GetBinError(1,2)
    errs["P HH VR obs"] = h_HH_VR_4.GetBinError(1,1)
    errs["M HH VR STC"] = h_HH_VR_4.GetBinError(2,3)
    errs["M HH VR pre"] = h_HH_VR_4.GetBinError(2,2)
    errs["M HH VR obs"] = h_HH_VR_4.GetBinError(2,1)
    errs["L HH VR STC"] = h_HH_VR_4.GetBinError(3,3)
    errs["L HH VR pre"] = h_HH_VR_4.GetBinError(3,2)
    errs["L HH VR obs"] = h_HH_VR_4.GetBinError(3,1)

    #
    # SR
    #
    h_LL_SR_23 = f.Get("h_LL_SR_23")
    h_LH_SR_23 = f.Get("h_LH_SR_23")
    h_HL_SR_4 = f.Get("h_HL_SR_4")
    h_HH_SR_4 = f.Get("h_HH_SR_4")

    vals["P LL SR STC"] = h_LL_SR_23.GetBinContent(1,3)
    vals["P LL SR pre"] = h_LL_SR_23.GetBinContent(1,2)
    vals["P LL SR obs"] = h_LL_SR_23.GetBinContent(1,1)
    vals["M LL SR STC"] = h_LL_SR_23.GetBinContent(2,3)
    vals["M LL SR pre"] = h_LL_SR_23.GetBinContent(2,2)
    vals["M LL SR obs"] = h_LL_SR_23.GetBinContent(2,1)
    vals["L LL SR STC"] = h_LL_SR_23.GetBinContent(3,3)
    vals["L LL SR pre"] = h_LL_SR_23.GetBinContent(3,2)
    vals["L LL SR obs"] = h_LL_SR_23.GetBinContent(3,1)

    vals["P LH SR STC"] = h_LH_SR_23.GetBinContent(1,3)
    vals["P LH SR pre"] = h_LH_SR_23.GetBinContent(1,2)
    vals["P LH SR obs"] = h_LH_SR_23.GetBinContent(1,1)
    vals["M LH SR STC"] = h_LH_SR_23.GetBinContent(2,3)
    vals["M LH SR pre"] = h_LH_SR_23.GetBinContent(2,2)
    vals["M LH SR obs"] = h_LH_SR_23.GetBinContent(2,1)
    vals["L LH SR STC"] = h_LH_SR_23.GetBinContent(3,3)
    vals["L LH SR pre"] = h_LH_SR_23.GetBinContent(3,2)
    vals["L LH SR obs"] = h_LH_SR_23.GetBinContent(3,1)

    vals["P HL SR STC"] = h_HL_SR_4.GetBinContent(1,3)
    vals["P HL SR pre"] = h_HL_SR_4.GetBinContent(1,2)
    vals["P HL SR obs"] = h_HL_SR_4.GetBinContent(1,1)
    vals["M HL SR STC"] = h_HL_SR_4.GetBinContent(2,3)
    vals["M HL SR pre"] = h_HL_SR_4.GetBinContent(2,2)
    vals["M HL SR obs"] = h_HL_SR_4.GetBinContent(2,1)
    vals["L HL SR STC"] = h_HL_SR_4.GetBinContent(3,3)
    vals["L HL SR pre"] = h_HL_SR_4.GetBinContent(3,2)
    vals["L HL SR obs"] = h_HL_SR_4.GetBinContent(3,1)

    vals["P HH SR STC"] = h_HH_SR_4.GetBinContent(1,3)
    vals["P HH SR pre"] = h_HH_SR_4.GetBinContent(1,2)
    vals["P HH SR obs"] = h_HH_SR_4.GetBinContent(1,1)
    vals["M HH SR STC"] = h_HH_SR_4.GetBinContent(2,3)
    vals["M HH SR pre"] = h_HH_SR_4.GetBinContent(2,2)
    vals["M HH SR obs"] = h_HH_SR_4.GetBinContent(2,1)
    vals["L HH SR STC"] = h_HH_SR_4.GetBinContent(3,3)
    vals["L HH SR pre"] = h_HH_SR_4.GetBinContent(3,2)
    vals["L HH SR obs"] = h_HH_SR_4.GetBinContent(3,1)

    #Errors
    errs["P LL SR STC"] = h_LL_SR_23.GetBinError(1,3)
    errs["P LL SR pre"] = h_LL_SR_23.GetBinError(1,2)
    errs["P LL SR obs"] = h_LL_SR_23.GetBinError(1,1)
    errs["M LL SR STC"] = h_LL_SR_23.GetBinError(2,3)
    errs["M LL SR pre"] = h_LL_SR_23.GetBinError(2,2)
    errs["M LL SR obs"] = h_LL_SR_23.GetBinError(2,1)
    errs["L LL SR STC"] = h_LL_SR_23.GetBinError(3,3)
    errs["L LL SR pre"] = h_LL_SR_23.GetBinError(3,2)
    errs["L LL SR obs"] = h_LL_SR_23.GetBinError(3,1)

    errs["P LH SR STC"] = h_LH_SR_23.GetBinError(1,3)
    errs["P LH SR pre"] = h_LH_SR_23.GetBinError(1,2)
    errs["P LH SR obs"] = h_LH_SR_23.GetBinError(1,1)
    errs["M LH SR STC"] = h_LH_SR_23.GetBinError(2,3)
    errs["M LH SR pre"] = h_LH_SR_23.GetBinError(2,2)
    errs["M LH SR obs"] = h_LH_SR_23.GetBinError(2,1)
    errs["L LH SR STC"] = h_LH_SR_23.GetBinError(3,3)
    errs["L LH SR pre"] = h_LH_SR_23.GetBinError(3,2)
    errs["L LH SR obs"] = h_LH_SR_23.GetBinError(3,1)

    errs["P HL SR STC"] = h_HL_SR_4.GetBinError(1,3)
    errs["P HL SR pre"] = h_HL_SR_4.GetBinError(1,2)
    errs["P HL SR obs"] = h_HL_SR_4.GetBinError(1,1)
    errs["M HL SR STC"] = h_HL_SR_4.GetBinError(2,3)
    errs["M HL SR pre"] = h_HL_SR_4.GetBinError(2,2)
    errs["M HL SR obs"] = h_HL_SR_4.GetBinError(2,1)
    errs["L HL SR STC"] = h_HL_SR_4.GetBinError(3,3)
    errs["L HL SR pre"] = h_HL_SR_4.GetBinError(3,2)
    errs["L HL SR obs"] = h_HL_SR_4.GetBinError(3,1)

    errs["P HH SR STC"] = h_HH_SR_4.GetBinError(1,3)
    errs["P HH SR pre"] = h_HH_SR_4.GetBinError(1,2)
    errs["P HH SR obs"] = h_HH_SR_4.GetBinError(1,1)
    errs["M HH SR STC"] = h_HH_SR_4.GetBinError(2,3)
    errs["M HH SR pre"] = h_HH_SR_4.GetBinError(2,2)
    errs["M HH SR obs"] = h_HH_SR_4.GetBinError(2,1)
    errs["L HH SR STC"] = h_HH_SR_4.GetBinError(3,3)
    errs["L HH SR pre"] = h_HH_SR_4.GetBinError(3,2)
    errs["L HH SR obs"] = h_HH_SR_4.GetBinError(3,1)

    return vals,errs

def makePlot(regions,vals,errs,desc): 
    simplecanvas.cd()
    tl.Clear()
    nregions=len(regions)
    hobs=ROOT.TH1D(desc,desc+";;Normalized Short Track Counts",nregions,0,nregions)
    hobs.SetLineWidth(3)
    hpred=hobs.Clone(hobs.GetName()+"_prediction")
    tag_index = 1
    for region in regions:
        hpred.GetXaxis().SetBinLabel(tag_index,region)
        pred = vals[region+" pre"]
        perr = errs[region+" pre"]
        obs = vals[region+" obs"]
        oerr = 1.83 if obs == 0 else errs[region+" obs"]
        if obs == 0:
            print "In region",region,"of",desc,", observation was 0, so setting oerr to 1.83"
        if pred > 0:
            hpred.SetBinContent(tag_index,1) # Set pred to 1 and rescale error below
            hpred.SetBinError(tag_index,perr/pred)
            hobs.SetBinContent(tag_index,obs/pred)
            hobs.SetBinError(tag_index,oerr/pred)
        else: # if prediction is 0, just plot the raw counts
            hpred.SetBinContent(tag_index,0)
            hpred.SetBinError(tag_index,perr)
            hobs.SetBinContent(tag_index,obs)
            hobs.SetBinError(tag_index,oerr)
        tag_index += 1    
    hpred.GetXaxis().LabelsOption("v")
    hpred.GetXaxis().SetTitleOffset(4.8)
    hpred.SetMinimum(-0.001)
    hpred.SetMaximum(3)
    hobs.SetLineColor(ROOT.kRed)
    hpred.SetLineColor(ROOT.kBlack)
    hpred_nofill = hpred.Clone(hpred.GetName()+"nofill")
    hpred.SetFillColor(ROOT.kGray)
    tl.AddEntry(hobs,"Observation")
    tl.AddEntry(hpred,"Prediction")
    hpred.Draw("E2")
    hpred_nofill.Draw("hist same")
    hobs.Draw("same E0")
    tl.Draw()
    simplecanvas.SaveAs("region_plots/{}.png".format(desc.replace(" ","_")))

def makePlotRaw(regions,vals,errs,desc): # Don't rescale counts
    simplecanvas.cd()
    tl.Clear()
    nregions=len(regions)
    hobs=ROOT.TH1D(desc,desc+";;Short Track Counts",nregions,0,nregions)
    hobs.SetLineWidth(3)
    hpred=hobs.Clone(hobs.GetName()+"_prediction")
    tag_index = 1
    for region in regions:
        hpred.GetXaxis().SetBinLabel(tag_index,region)
        pred = vals[region+" pre"]
        perr = errs[region+" pre"]
        obs = vals[region+" obs"]
        oerr = 1.83 if obs == 0 else errs[region+" obs"]
        if obs == 0:
            print "In region",region,"of",desc,", observation was 0, so setting oerr to 1.83"
        hpred.SetBinContent(tag_index,pred)
        hpred.SetBinError(tag_index,perr)
        hobs.SetBinContent(tag_index,obs)
        hobs.SetBinError(tag_index,oerr)
        tag_index += 1    
    hpred.GetXaxis().LabelsOption("v")
    hpred.GetXaxis().SetTitleOffset(4.8)
    hpred.SetMinimum(-0.001)
    hpred.SetMaximum(1.5*max(hpred.GetMaximum(),hobs.GetMaximum()))
    hobs.SetLineColor(ROOT.kRed)
    hpred.SetLineColor(ROOT.kBlack)
    hpred_nofill = hpred.Clone(hpred.GetName()+"nofill")
    hpred.SetFillColor(ROOT.kGray)
    tl.AddEntry(hobs,"Observation")
    tl.AddEntry(hpred,"Prediction")
    hpred.Draw("E2")
    hpred_nofill.Draw("hist same")
    hobs.Draw("same E0")
    tl.Draw()
    simplecanvas.SaveAs("region_plots/{}_raw.png".format(desc.replace(" ","_")))

tag = sys.argv[1]

d17=ROOT.TFile.Open("output_merged/data_2017_{}.root".format(tag))
d16=ROOT.TFile.Open("output_merged/data_2016_{}.root".format(tag))
m17=ROOT.TFile.Open("output_merged/mc_2017_{}.root".format(tag))
m16=ROOT.TFile.Open("output_merged/mc_2016_{}.root".format(tag))

D17,eD17=getCounts(d17)
D16,eD16=getCounts(d16)
M17,eM17=getCounts(m17)
M16,eM16=getCounts(m16)

D17f,eD17f=getFshorts(d17)
D16f,eD16f=getFshorts(d16)
M17f,eM17f=getFshorts(m17)
M16f,eM16f=getFshorts(m16)

d17.Close()
d16.Close()
m17.Close()
m16.Close()

def getLineData(region):
    return "{} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f}\\\\ \n".format(region,
                                                                                                                                                                                                                                    D17[region+" pre"], eD17[region+" pre"], D17[region+" obs"],
                                                                                                                                                                                                                                    D16[region+" pre"], eD16[region+" pre"], D16[region+" obs"])

def getLineMC(region):
    return "{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} \\\\ \n".format(region,
                                                                                                                                                                                                                                    M17[region+" pre"], eM17[region+" pre"], M17[region+" obs"], eM17[region+" obs"],
                                  M16[region+" pre"], eM16[region+" pre"], M16[region+" obs"], eM16[region+" obs"])

def getLineDataSTC(region):
    return "{} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f}\\\\ \n".format(region,
                                                                                                                                                                                                                                    D17[region+" STC"], D17[region+" pre"], eD17[region+" pre"], D17[region+" obs"],
                                                                                                                                                                                                                                    D16[region+" STC"], D16[region+" pre"], eD16[region+" pre"], D16[region+" obs"])

def getLineMCSTC(region):
    return "{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} \\\\ \n".format(region,
                                                                                                                                                                                                                                    M17[region+" STC"], eM17[region+" STC"], M17[region+" pre"], eM17[region+" pre"], M17[region+" obs"], eM17[region+" obs"],
                                  M16[region+" STC"], eM16[region+" STC"], M16[region+" pre"], eM16[region+" pre"], M16[region+" obs"], eM16[region+" obs"])


def getFSLineData(cat):
    return "{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(cat,D17f[cat],eD17f[cat],D16f[cat],eD16f[cat])

def getFSLineMC(cat):
    return "{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(cat,M17f[cat],eM17f[cat],M16f[cat],eM16f[cat])

regionsData = [cat + " " + kin + " VR" for cat in ["P","M","L"] for kin in ["LL","LH","HL","HH"]]
regionsDataNoL = [cat + " " + kin + " VR" for cat in ["P","M"] for kin in ["LL","LH","HL","HH"]]
regionsDataL = ["L " + kin + " VR" for kin in ["LL","LH","HL","HH"]]

makePlot(regionsData,D17,eD17,"2017 DATA")
makePlot(regionsData,D16,eD16,"2016 DATA")

makePlot(regionsDataNoL,D17,eD17,"2017 DATA No L")
makePlot(regionsDataNoL,D16,eD16,"2016 DATA No L")

makePlotRaw(regionsDataL,D17,eD17,"2017 DATA L")
makePlotRaw(regionsDataL,D16,eD16,"2016 DATA L")

output = open("tables/regions_data_{}.tex".format(tag),"w")
printHeader(output)
startRegionTableData(output)
output.write("\\rowcolor{green!25}")
output.write(getLineData("P LL VR"))
output.write("\\rowcolor{green!25}")
output.write(getLineData("P LH VR"))
output.write("\\rowcolor{green!25}")
output.write(getLineData("P HL VR"))
output.write("\\rowcolor{green!25}")
output.write(getLineData("P HH VR"))
output.write("\\rowcolor{blue!25}")
output.write(getLineData("M LL VR"))
output.write("\\rowcolor{blue!25}")
output.write(getLineData("M LH VR"))
output.write("\\rowcolor{blue!25}")
output.write(getLineData("M HL VR"))
output.write("\\rowcolor{blue!25}")
output.write(getLineData("M HH VR"))
output.write("\\rowcolor{red!25}")
output.write(getLineData("L LL VR"))
output.write("\\rowcolor{red!25}")
output.write(getLineData("L LH VR"))
output.write("\\rowcolor{red!25}")
output.write(getLineData("L HL VR"))
output.write("\\rowcolor{red!25}")
output.write(getLineData("L HH VR"))
output.write("\\rowcolor{green!75}")
printFooter(output)
output.close()

output = open("tables/fshorts_data_{}.tex".format(tag),"w")
printHeader(output)
startFshortTableData(output)
output.write("\\rowcolor{green!10}")
output.write(getFSLineData("P 23"))
output.write("\\rowcolor{green!10}")
output.write(getFSLineData("P 4"))
output.write("\\rowcolor{blue!10}")
output.write(getFSLineData("M 23"))
output.write("\\rowcolor{blue!10}")
output.write(getFSLineData("M 4"))
output.write("\\rowcolor{red!10}")
output.write(getFSLineData("L 23"))
output.write("\\rowcolor{red!10}")
output.write(getFSLineData("L 4"))
printFooter(output)
output.close()

regionsMC = [cat + " " + kin + " " + mt2 for mt2 in ["VR","SR"] for cat in ["P","M","L"] for kin in ["LL","LH","HL","HH"]]
regionsMCNoL = [cat + " " + kin + " " + mt2 for mt2 in ["VR","SR"] for cat in ["P","M"] for kin in ["LL","LH","HL","HH"]]
regionsMCL = ["L " + kin + " " + mt2 for mt2 in ["VR","SR"] for kin in ["LL","LH","HL","HH"]]

makePlot(regionsMC,M17,eM17,"2017 MC")
makePlot(regionsMC,M16,eM16,"2016 MC")

makePlot(regionsMCNoL,M17,eM17,"2017 MC No L")
makePlot(regionsMCNoL,M16,eM16,"2016 MC No L")

makePlotRaw(regionsMCL,M17,eM17,"2017 MC L")
makePlotRaw(regionsMCL,M16,eM16,"2016 MC L")

output = open("tables/regions_mc_{}.tex".format(tag),"w")
printHeader(output)
startRegionTableMC(output)
output.write("\\rowcolor{green!25}")
output.write(getLineMC("P LL VR"))
output.write("\\rowcolor{green!25}")
output.write(getLineMC("P LH VR"))
output.write("\\rowcolor{green!25}")
output.write(getLineMC("P HL VR"))
output.write("\\rowcolor{green!25}")
output.write(getLineMC("P HH VR"))
output.write("\\rowcolor{blue!25}")
output.write(getLineMC("M LL VR"))
output.write("\\rowcolor{blue!25}")
output.write(getLineMC("M LH VR"))
output.write("\\rowcolor{blue!25}")
output.write(getLineMC("M HL VR"))
output.write("\\rowcolor{blue!25}")
output.write(getLineMC("M HH VR"))
output.write("\\rowcolor{red!25}")
output.write(getLineMC("L LL VR"))
output.write("\\rowcolor{red!25}")
output.write(getLineMC("L LH VR"))
output.write("\\rowcolor{red!25}")
output.write(getLineMC("L HL VR"))
output.write("\\rowcolor{red!25}")
output.write(getLineMC("L HH VR"))
output.write("\\rowcolor{green!75}")
output.write(getLineMC("P LL SR"))
output.write("\\rowcolor{green!75}")
output.write(getLineMC("P LH SR"))
output.write("\\rowcolor{green!75}")
output.write(getLineMC("P HL SR"))
output.write("\\rowcolor{green!75}")
output.write(getLineMC("P HH SR"))
output.write("\\rowcolor{blue!50}")
output.write(getLineMC("M LL SR"))
output.write("\\rowcolor{blue!50}")
output.write(getLineMC("M LH SR"))
output.write("\\rowcolor{blue!50}")
output.write(getLineMC("M HL SR"))
output.write("\\rowcolor{blue!50}")
output.write(getLineMC("M HH SR"))
output.write("\\rowcolor{red!75}")
output.write(getLineMC("L LL SR"))
output.write("\\rowcolor{red!75}")
output.write(getLineMC("L LH SR"))
output.write("\\rowcolor{red!75}")
output.write(getLineMC("L HL SR"))
output.write("\\rowcolor{red!75}")
output.write(getLineMC("L HH SR"))
printFooter(output)
output.close()

output = open("tables/fshorts_mc_{}.tex".format(tag),"w")
printHeader(output)
startFshortTableMC(output)
output.write("\\rowcolor{green!10}")
output.write(getFSLineMC("P 23"))
output.write("\\rowcolor{green!10}")
output.write(getFSLineMC("P 4"))
output.write("\\rowcolor{blue!10}")
output.write(getFSLineMC("M 23"))
output.write("\\rowcolor{blue!10}")
output.write(getFSLineMC("M 4"))
output.write("\\rowcolor{red!10}")
output.write(getFSLineMC("L 23"))
output.write("\\rowcolor{red!10}")
output.write(getFSLineMC("L 4"))
printFooter(output)
output.close()


print "Done"
