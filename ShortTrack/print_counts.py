# Make nice tables and plots of results

import ROOT
from math import sqrt
import re
import sys
import os
import subprocess

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

verbose = False # Print more status messages
printTables = True
full_unblind = True

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
ROOT.gStyle.SetLegendTextSize(0.03)
ROOT.gStyle.SetPaintTextFormat("#3.3g")
ROOT.gStyle.SetLegendBorderSize(0)
simplecanvas = ROOT.TCanvas("plotter_simple")
simplecanvas.SetCanvasSize(1000,600)
simplecanvas.SetTicks(1,1)
simplecanvas.SetLeftMargin(0.16)
simplecanvas.SetTopMargin(0.12)
simplecanvas.SetRightMargin(0.16)
simplecanvas.SetBottomMargin(0.16)

ratiocanvas = ROOT.TCanvas("plotter_ratio")
ratiocanvas.SetCanvasSize(1000,1000)
ratiocanvas.SetTicks(1,0)
ratiocanvas.SetLeftMargin(0.16)
ratiocanvas.SetTopMargin(0.12)
ratiocanvas.SetRightMargin(0.16)
ratiocanvas.SetBottomMargin(0.16)
pads = []
pads.append(ROOT.TPad("1","1",0.0,0.26,1.0,1.0))
pads.append(ROOT.TPad("2","2",0.0,0.0,1.0,0.27))
pads[0].SetTopMargin(0.08)
pads[0].SetLeftMargin(0.12)
pads[0].SetBottomMargin(0.17)
pads[1].SetLeftMargin(0.12)
pads[0].Draw()
pads[1].Draw()


tl=ROOT.TLegend(0.2,0.60,0.4,0.85)

if len(sys.argv) < 2: 
    print "Which tag?"
    exit()

tag = sys.argv[1]
tabledir = "region_tables/{}".format(tag)
plotdir = "region_plots/{}".format(tag)
os.system("mkdir -p {}".format(tabledir))
os.system("mkdir -p {}".format(plotdir))

def printHeader(outfile):
    outfile.write("\documentclass[10pt]{article}\n\n")

    outfile.write("\usepackage{amsmath,graphicx,setspace,multirow,booktabs}\n")
    outfile.write("\usepackage[paperheight=20in,paperwidth=16in]{geometry}\n")
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

    outfile.write("\onehalfspacing\n\n")


def startRegionTableData(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *2c | *2c | *2c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region} & \multicolumn{2}{c}{2018 Data} & \multicolumn{2}{c}{2017 Data} & \multicolumn{2}{c}{2016 Data}\\\\ \n")
    outfile.write(" & Pred ST & Obs ST & Pred ST & Obs ST & Pred ST & Obs ST\\\\ \n ")
    outfile.write("\hline\n")

def startRegionTableMC(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *2c | *2c | *2c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region} & \multicolumn{2}{c}{2018 MC} & \multicolumn{2}{c}{2017 MC} & \multicolumn{2}{c}{2016 MC}\\\\ \n")
    outfile.write(" & Pred ST & Obs ST & Pred ST & Obs ST & Pred ST & Obs ST \\\\ \n ")
    outfile.write("\hline\n")

def startRegionTableDataSTC(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *3c | *3c | *3c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region} & \multicolumn{3}{c}{2018 Data} & \multicolumn{3}{c}{2017 Data} & \multicolumn{3}{c}{2016 Data}\\\\ \n")
    outfile.write(" & STC & Pred ST & Obs ST & STC & Pred ST & Obs ST & STC & Pred ST & Obs ST\\\\ \n ")
    outfile.write("\hline\n")

def startRegionTableMCSTC(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *3c | *3c | *3c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region} & \multicolumn{3}{c}{2018 MC} &  \multicolumn{3}{c}{2017 MC} & \multicolumn{3}{c}{2016 MC}\\\\ \n")
    outfile.write(" & STC & Pred ST & Obs ST & STC & Pred ST & Obs ST & STC & Pred ST & Obs ST \\\\ \n ")
    outfile.write("\hline\n")

def startFshortTableData(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | c | c | c}\n")
    outfile.write("\\toprule\n")
    outfile.write("Category & 2018 Data & 2017 Data & 2016 Data\\\\ \n")
    outfile.write("\hline\n")

def startFshortTableMC(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | c | c | c}\n")
    outfile.write("\\toprule\n")
    outfile.write("Category & 2018 MC & 2017 MC & 2016 MC\\\\ \n")
    outfile.write("\hline\n")

def startMergedRegionTableData(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *2c | *2c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region} & \multicolumn{2}{c}{2017-18 Data} & \multicolumn{2}{c}{2016 Data}\\\\ \n")
    outfile.write(" & Pred ST & Obs ST & Pred ST & Obs ST \\\\ \n ")
    outfile.write("\hline\n")

def startMergedRegionTableMC(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *2c | *2c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region} & \multicolumn{2}{c}{2017-18 MC} & \multicolumn{2}{c}{2016 MC}\\\\ \n")
    outfile.write(" & Pred ST & Obs ST & Pred ST & Obs ST \\\\ \n ")
    outfile.write("\hline\n")

def startMergedRegionTableDataSTC(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *3c | *3c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region} & \multicolumn{3}{c}{2017-18 Data} & \multicolumn{3}{c}{2016 Data}\\\\ \n")
    outfile.write(" & STC & Pred ST & Obs ST & STC & Pred ST & Obs ST \\\\ \n ")
    outfile.write("\hline\n")

def startMergedRegionTableMCSTC(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *3c | *3c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region} & \multicolumn{3}{c}{2017-18 MC} & \multicolumn{3}{c}{2016 MC}\\\\ \n")
    outfile.write(" & STC & Pred ST & Obs ST & STC & Pred ST & Obs ST \\\\ \n ")
    outfile.write("\hline\n")

def startMergedFshortTableData(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | c | c}\n")
    outfile.write("\\toprule\n")
    outfile.write("Category & 2017-18 Data & 2016 Data\\\\ \n")
    outfile.write("\hline\n")

def startMergedFshortTableMC(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | c | c}\n")
    outfile.write("\\toprule\n")
    outfile.write("Category & 2017-18 MC & 2016 MC\\\\ \n")
    outfile.write("\hline\n")

def startMergedCountsTable(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | c c | *3c| *3c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region} & 2017-18 Data & 2016 Data & \multicolumn{3}{c}{10 cm Signal} & \multicolumn{3}{c}{90 cm Signal} \\\\ \n")
    outfile.write(" & Pred Background & Pred Background & 1800, 1400 GeV & 1800, 1600 GeV & 1800, 1700 GeV & 1800, 1400 GeV & 1800, 1600 GeV & 1800, 1700 GeV\\\\ \n ")
    outfile.write("\hline\n")

def startMRvsSRTable(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *3c| *3c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{3}{*}{Track Length N$_{J}$,H$_{T}$,p$_{T}$ Region} & \multicolumn{6}{c}{MR / SR ST Counts} \\\\ \n")
    outfile.write(" & \multicolumn{3}{c}{90 cm Signal} & \multicolumn{3}{c}{10 cm Signal} \\\\ \n")
    outfile.write(" & 1800, 1400 GeV & 1800, 1600 GeV & 1800, 1700 GeV & 1800, 1400 GeV & 1800, 1600 GeV & 1800, 1700 GeV\\\\ \n ")
    outfile.write("\hline\n")

def printFooter(outfile):
    outfile.write("\\bottomrule\n")
    outfile.write("\end{tabular}\n\n")
    outfile.write("\\bigskip\n\n")

    outfile.write("\end{document}\n\n")

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

def getPoissonGraph( histo, errors = None, drawXline = True):

  nBins = histo.GetNbinsX()
  graph = ROOT.TGraphAsymmErrors()

  graph.SetMaximum(histo.GetMaximum())
  graph.SetMinimum(histo.GetMinimum())
  graph.SetLineWidth(histo.GetLineWidth())
  graph.SetLineColor(histo.GetLineColor())
  graph.SetFillColor(histo.GetFillColor())
  graph.GetYaxis().SetTitle(histo.GetYaxis().GetTitle())
  graph.GetXaxis().SetTitle(histo.GetXaxis().GetTitle())
  graph.SetTitle(histo.GetTitle())

  for iBin in xrange(1,nBins+1):
      x = histo.GetXaxis().GetBinCenter(iBin)
      xerr = histo.GetBinWidth(iBin)/2.0 if drawXline else 0
      y = histo.GetBinContent(iBin)
      
      yerrplus=0.0
      yerrminus=0.0

      yerr = getAsymmetricErrors(y) if errors is None else errors[iBin-1]
      yerrplus = yerr[0]
      yerrminus = yerr[1]

      thisPoint = graph.GetN();
      graph.SetPoint( thisPoint, x, y );
      graph.SetPointError( thisPoint, xerr, xerr, yerrminus, yerrplus );
      if verbose: print x, y, xerr, xerr, yerrminus, yerrplus

  return graph



def getRatioGraph( histo_data, histo_mc, data_errs = None, mc_errs = None ):

    if not histo_data or not histo_mc: 
        return 0
    
    graph  = ROOT.TGraphAsymmErrors()
    graph_data = getPoissonGraph(histo_data, data_errs)
    graph_mc = getPoissonGraph(histo_mc, mc_errs)
    for i in xrange(0,graph_data.GetN()):
        x_tmp=ROOT.Double(0.0)
        data=ROOT.Double(0.0)
        graph_data.GetPoint( i, x_tmp, data )
        
        data_errUp = graph_data.GetErrorYhigh(i)
        data_errDn = graph_data.GetErrorYlow(i)
        
        x_tmp_mc=ROOT.Double(0.0);
        mc=ROOT.Double(0.0)
        graph_mc.GetPoint( i, x_tmp_mc, mc )
        
        mc_errUp = graph_mc.GetErrorYhigh(i)
        mc_errDn = graph_mc.GetErrorYlow(i)
        
        if(mc>0):
            ratio = data/mc;
            ratio_errUp = ROOT.TMath.Sqrt( data_errUp*data_errUp/(mc*mc) + mc_errUp*mc_errUp*data*data/(mc*mc*mc*mc) )
            ratio_errDn = ROOT.TMath.Sqrt( data_errDn*data_errDn/(mc*mc) + mc_errDn*mc_errDn*data*data/(mc*mc*mc*mc) )
        else:
            ratio = -1.0;
            ratio_errUp = 0.0
            ratio_errDn = 0.0

        xerr = histo_mc.GetBinWidth(i)/2.;

        graph.SetPoint(i, x_tmp, ratio );
        graph.SetPointEYhigh(i, ratio_errUp );
        graph.SetPointEYlow(i, ratio_errDn );
        graph.SetPointEXhigh(i, xerr );
        graph.SetPointEXlow(i, xerr );
        
    return graph;



def getGraphRatioGraph( graph_data, graph_mc ):

    if not graph_data or not graph_mc: 
        return 0
    
    graph  = ROOT.TGraphAsymmErrors()
    #graph_data = getPoissonGraph(histo_data)
    #graph_mc = getPoissonGraph(histo_mc)
    for i in xrange(0,graph_data.GetN()):
        x_tmp=ROOT.Double(0.0) 
        data=ROOT.Double(0.0)
        graph_data.GetPoint( i, x_tmp, data )
        
        data_errUp = graph_data.GetErrorYhigh(i)
        data_errDn = graph_data.GetErrorYlow(i)
        
        x_tmp_mc=ROOT.Double(0.0);
        mc=ROOT.Double(0.0)
        graph_mc.GetPoint( i, x_tmp_mc, mc )
        
        mc_errUp = graph_mc.GetErrorYhigh(i)
        mc_errDn = graph_mc.GetErrorYlow(i)

        if(mc>0):
            ratio = ROOT.Double(float(data)/float(mc))
            ratio_errUp = ROOT.TMath.Sqrt( float(data_errUp*data_errUp)/float(mc*mc) + float(mc_errUp*mc_errUp*data*data)/float(mc*mc*mc*mc) )
            ratio_errDn = ROOT.TMath.Sqrt( float(data_errDn*data_errDn)/float(mc*mc) + float(mc_errDn*mc_errDn*data*data)/float(mc*mc*mc*mc) )
        else:
            ratio = -1.0;
            ratio_errUp = 0.0
            ratio_errDn = 0.0

        xerr = graph_mc.GetErrorXhigh(i)
        
        print ratio, data_errUp, data_errDn, mc_errUp, mc_errDn, ratio_errUp, ratio_errDn
        
        graph.SetPoint(i, x_tmp, ratio );
        graph.SetPointEYhigh(i, ratio_errUp );
        graph.SetPointEYlow(i, ratio_errDn );
        graph.SetPointEXhigh(i, xerr );
        graph.SetPointEXlow(i, xerr );
        
    #graph->SetLineColor(1);
    #graph->SetMarkerColor(1);
    #graph->SetMarkerStyle(20);

    return graph;

def correspondingFshort(region):
    if region[1] == " ":
        cat = region[0] # P M L
    else:
        cat = region[0:2] # P3 P4
    if region.find("4") >= 0:
        nj = " 4"
    elif region.find("23") >= 0:
        nj = " 23"
    else:
        nj = ""
    if region.find("lo") >= 0:
        pt = " lo"
    elif region.find("hi") >= 0:
        pt = " hi"
    else:
        pt = ""
    return cat+nj+pt

def getFshorts(f):
    h_FS = f.Get("h_FS")
    h_FS_23 = f.Get("h_FS_23")
    h_FS_4 = f.Get("h_FS_4")
    h_FS_hi = f.Get("h_FS_hi")
    h_FS_23_hi = f.Get("h_FS_23_hi")
    h_FS_4_hi = f.Get("h_FS_4_hi")
    h_FS_lo = f.Get("h_FS_lo")
    h_FS_23_lo = f.Get("h_FS_23_lo")
    h_FS_4_lo = f.Get("h_FS_4_lo")
    h_FS_up = f.Get("h_FS_up")
    h_FS_23_up = f.Get("h_FS_23_up")
    h_FS_4_up = f.Get("h_FS_4_up")
    h_FS_hi_up = f.Get("h_FS_hi_up")
    h_FS_23_hi_up = f.Get("h_FS_23_hi_up")
    h_FS_4_hi_up = f.Get("h_FS_4_hi_up")
    h_FS_lo_up = f.Get("h_FS_lo_up")
    h_FS_23_lo_up = f.Get("h_FS_23_lo_up")
    h_FS_4_lo_up = f.Get("h_FS_4_lo_up")
    h_FS_dn = f.Get("h_FS_dn")
    h_FS_23_dn = f.Get("h_FS_23_dn")
    h_FS_4_dn = f.Get("h_FS_4_dn")
    h_FS_hi_dn = f.Get("h_FS_hi_dn")
    h_FS_23_hi_dn = f.Get("h_FS_23_hi_dn")
    h_FS_4_hi_dn = f.Get("h_FS_4_hi_dn")
    h_FS_lo_dn = f.Get("h_FS_lo_dn")
    h_FS_23_lo_dn = f.Get("h_FS_23_lo_dn")
    h_FS_4_lo_dn = f.Get("h_FS_4_lo_dn")
    h_FS_syst = f.Get("h_FS_syst")
    h_FS_23_syst = f.Get("h_FS_23_syst")
    h_FS_4_syst = f.Get("h_FS_4_syst")
    h_FS_hi_syst = f.Get("h_FS_hi_syst")
    h_FS_23_hi_syst = f.Get("h_FS_23_hi_syst")
    h_FS_4_hi_syst = f.Get("h_FS_4_hi_syst")
    h_FS_lo_syst = f.Get("h_FS_lo_syst")
    h_FS_23_lo_syst = f.Get("h_FS_23_lo_syst")
    h_FS_4_lo_syst = f.Get("h_FS_4_lo_syst")
    vals = {}
    errs = {}
    systs = {}
    vals["P"] = h_FS.GetBinContent(1)
    vals["P3"] = h_FS.GetBinContent(2)
    vals["P4"] = h_FS.GetBinContent(3)
    vals["M"] = h_FS.GetBinContent(4)
    vals["L"] = h_FS.GetBinContent(5)

    vals["P 23"] = h_FS_23.GetBinContent(1)
    vals["P3 23"] = h_FS_23.GetBinContent(2)
    vals["P4 23"] = h_FS_23.GetBinContent(3)
    vals["M 23"] = h_FS_23.GetBinContent(4)
    vals["L 23"] = h_FS_23.GetBinContent(5)

    vals["P 4"] = h_FS_4.GetBinContent(1)
    vals["P3 4"] = h_FS_4.GetBinContent(2)
    vals["P4 4"] = h_FS_4.GetBinContent(3)
    vals["M 4"] = h_FS_4.GetBinContent(4)
    vals["L 4"] = h_FS_4.GetBinContent(5)

    errs["P"] = [h_FS_up.GetBinError(1), h_FS_dn.GetBinError(1)]
    errs["P3"] = [h_FS_up.GetBinError(2), h_FS_dn.GetBinError(1)]
    errs["P4"] = [h_FS_up.GetBinError(3), h_FS_dn.GetBinError(1)]
    errs["M"] = [h_FS_up.GetBinError(4), h_FS_dn.GetBinError(1)]
    errs["L"] = [h_FS_up.GetBinError(5), h_FS_dn.GetBinError(1)]

    errs["P 23"] = [h_FS_23_up.GetBinError(1), h_FS_23_dn.GetBinError(1)]
    errs["P3 23"] = [h_FS_23_up.GetBinError(2), h_FS_23_dn.GetBinError(2)]
    errs["P4 23"] = [h_FS_23_up.GetBinError(3), h_FS_23_dn.GetBinError(3)]
    errs["M 23"] = [h_FS_23_up.GetBinError(4), h_FS_23_dn.GetBinError(4)]
    errs["L 23"] = [h_FS_23_up.GetBinError(5), h_FS_23_dn.GetBinError(5)]

    errs["P 4"] = [h_FS_4_up.GetBinError(1), h_FS_4_dn.GetBinError(1)]
    errs["P3 4"] = [h_FS_4_up.GetBinError(2), h_FS_4_dn.GetBinError(2)]
    errs["P4 4"] = [h_FS_4_up.GetBinError(3), h_FS_4_dn.GetBinError(3)]
    errs["M 4"] = [h_FS_4_up.GetBinError(4), h_FS_4_dn.GetBinError(4)]
    errs["L 4"] = [h_FS_4_up.GetBinError(5), h_FS_4_dn.GetBinError(5)]

    systs["P"] = h_FS_syst.GetBinError(1)
    systs["P3"] = h_FS_syst.GetBinError(2)
    systs["P4"] = h_FS_syst.GetBinError(3)
    systs["M"] = h_FS_syst.GetBinError(4)
    systs["L"] = h_FS_syst.GetBinError(5)

    systs["P 23"] = h_FS_23_syst.GetBinError(1)
    systs["P3 23"] = h_FS_23_syst.GetBinError(2)
    systs["P4 23"] = h_FS_23_syst.GetBinError(3)
    systs["M 23"] = h_FS_23_syst.GetBinError(4)
    systs["L 23"] = h_FS_23_syst.GetBinError(5)

    systs["P 4"] = h_FS_4_syst.GetBinError(1)
    systs["P3 4"] = h_FS_4_syst.GetBinError(2)
    systs["P4 4"] = h_FS_4_syst.GetBinError(3)
    systs["M 4"] = h_FS_4_syst.GetBinError(4)
    systs["L 4"] = h_FS_4_syst.GetBinError(5)

    # hi pt
    vals["P hi"] = h_FS_hi.GetBinContent(1)
    vals["P3 hi"] = h_FS_hi.GetBinContent(2)
    vals["P4 hi"] = h_FS_hi.GetBinContent(3)
    vals["M hi"] = h_FS_hi.GetBinContent(4)
    vals["L hi"] = h_FS_hi.GetBinContent(5)

    vals["P 23 hi"] = h_FS_23_hi.GetBinContent(1)
    vals["P3 23 hi"] = h_FS_23_hi.GetBinContent(2)
    vals["P4 23 hi"] = h_FS_23_hi.GetBinContent(3)
    vals["M 23 hi"] = h_FS_23_hi.GetBinContent(4)
    vals["L 23 hi"] = h_FS_23_hi.GetBinContent(5)

    vals["P 4 hi"] = h_FS_4_hi.GetBinContent(1)
    vals["P3 4 hi"] = h_FS_4_hi.GetBinContent(2)
    vals["P4 4 hi"] = h_FS_4_hi.GetBinContent(3)
    vals["M 4 hi"] = h_FS_4_hi.GetBinContent(4)
    vals["L 4 hi"] = h_FS_4_hi.GetBinContent(5)

    errs["P hi"] = [h_FS_hi_up.GetBinError(1), h_FS_hi_dn.GetBinError(1)]
    errs["P3 hi"] = [h_FS_hi_up.GetBinError(2), h_FS_hi_dn.GetBinError(2)]
    errs["P4 hi"] = [h_FS_hi_up.GetBinError(3), h_FS_hi_dn.GetBinError(3)]
    errs["M hi"] = [h_FS_hi_up.GetBinError(4), h_FS_hi_dn.GetBinError(4)]
    errs["L hi"] = [h_FS_hi_up.GetBinError(5), h_FS_hi_dn.GetBinError(5)]

    errs["P 23 hi"] = [h_FS_23_hi_up.GetBinError(1), h_FS_23_dn.GetBinError(1)]
    errs["P3 23 hi"] = [h_FS_23_hi_up.GetBinError(2), h_FS_23_hi_dn.GetBinError(2)]
    errs["P4 23 hi"] = [h_FS_23_hi_up.GetBinError(3), h_FS_23_hi_dn.GetBinError(3)]
    errs["M 23 hi"] = [h_FS_23_hi_up.GetBinError(4), h_FS_23_hi_dn.GetBinError(4)]
    errs["L 23 hi"] = [h_FS_23_hi_up.GetBinError(5), h_FS_23_hi_dn.GetBinError(5)]

    errs["P 4 hi"] = [h_FS_4_hi_up.GetBinError(1), h_FS_4_hi_dn.GetBinError(1)]
    errs["P3 4 hi"] = [h_FS_4_hi_up.GetBinError(2), h_FS_4_hi_dn.GetBinError(2)]
    errs["P4 4 hi"] = [h_FS_4_hi_up.GetBinError(3), h_FS_4_hi_dn.GetBinError(3)]
    errs["M 4 hi"] = [h_FS_4_hi_up.GetBinError(4), h_FS_4_hi_dn.GetBinError(4)]
    errs["L 4 hi"] = [h_FS_4_hi_up.GetBinError(5), h_FS_4_hi_dn.GetBinError(5)]

    systs["P hi"] = h_FS_hi_syst.GetBinError(1)
    systs["P3 hi"] = h_FS_hi_syst.GetBinError(2)
    systs["P4 hi"] = h_FS_hi_syst.GetBinError(3)
    systs["M hi"] = h_FS_hi_syst.GetBinError(4)
    systs["L hi"] = h_FS_hi_syst.GetBinError(5)

    systs["P 23 hi"] = h_FS_23_hi_syst.GetBinError(1)
    systs["P3 23 hi"] = h_FS_23_hi_syst.GetBinError(2)
    systs["P4 23 hi"] = h_FS_23_hi_syst.GetBinError(3)
    systs["M 23 hi"] = h_FS_23_hi_syst.GetBinError(4)
    systs["L 23 hi"] = h_FS_23_hi_syst.GetBinError(5)

    systs["P 4 hi"] = h_FS_4_hi_syst.GetBinError(1)
    systs["P3 4 hi"] = h_FS_4_hi_syst.GetBinError(2)
    systs["P4 4 hi"] = h_FS_4_hi_syst.GetBinError(3)
    systs["M 4 hi"] = h_FS_4_hi_syst.GetBinError(4)
    systs["L 4 hi"] = h_FS_4_hi_syst.GetBinError(5)


    # lo pt
    vals["P lo"] = h_FS_lo.GetBinContent(1)
    vals["P3 lo"] = h_FS_lo.GetBinContent(2)
    vals["P4 lo"] = h_FS_lo.GetBinContent(3)
    vals["M lo"] = h_FS_lo.GetBinContent(4)
    vals["L lo"] = h_FS_lo.GetBinContent(5)

    vals["P 23 lo"] = h_FS_23_lo.GetBinContent(1)
    vals["P3 23 lo"] = h_FS_23_lo.GetBinContent(2)
    vals["P4 23 lo"] = h_FS_23_lo.GetBinContent(3)
    vals["M 23 lo"] = h_FS_23_lo.GetBinContent(4)
    vals["L 23 lo"] = h_FS_23_lo.GetBinContent(5)

    vals["P 4 lo"] = h_FS_4_lo.GetBinContent(1)
    vals["P3 4 lo"] = h_FS_4_lo.GetBinContent(2)
    vals["P4 4 lo"] = h_FS_4_lo.GetBinContent(3)
    vals["M 4 lo"] = h_FS_4_lo.GetBinContent(4)
    vals["L 4 lo"] = h_FS_4_lo.GetBinContent(5)

    errs["P lo"] = [h_FS_lo_up.GetBinError(1), h_FS_lo_dn.GetBinError(1)]
    errs["P3 lo"] = [h_FS_lo_up.GetBinError(2), h_FS_lo_dn.GetBinError(2)]
    errs["P4 lo"] = [h_FS_lo_up.GetBinError(3), h_FS_lo_dn.GetBinError(3)]
    errs["M lo"] = [h_FS_lo_up.GetBinError(4), h_FS_lo_dn.GetBinError(4)]
    errs["L lo"] = [h_FS_lo_up.GetBinError(5), h_FS_lo_dn.GetBinError(5)]

    errs["P 23 lo"] = [h_FS_23_lo_up.GetBinError(1), h_FS_23_dn.GetBinError(1)]
    errs["P3 23 lo"] = [h_FS_23_lo_up.GetBinError(2), h_FS_23_lo_dn.GetBinError(2)]
    errs["P4 23 lo"] = [h_FS_23_lo_up.GetBinError(3), h_FS_23_lo_dn.GetBinError(3)]
    errs["M 23 lo"] = [h_FS_23_lo_up.GetBinError(4), h_FS_23_lo_dn.GetBinError(4)]
    errs["L 23 lo"] = [h_FS_23_lo_up.GetBinError(5), h_FS_23_lo_dn.GetBinError(5)]

    errs["P 4 lo"] = [h_FS_4_lo_up.GetBinError(1), h_FS_4_lo_dn.GetBinError(1)]
    errs["P3 4 lo"] = [h_FS_4_lo_up.GetBinError(2), h_FS_4_lo_dn.GetBinError(2)]
    errs["P4 4 lo"] = [h_FS_4_lo_up.GetBinError(3), h_FS_4_lo_dn.GetBinError(3)]
    errs["M 4 lo"] = [h_FS_4_lo_up.GetBinError(4), h_FS_4_lo_dn.GetBinError(4)]
    errs["L 4 lo"] = [h_FS_4_lo_up.GetBinError(5), h_FS_4_lo_dn.GetBinError(5)]

    systs["P lo"] = h_FS_lo_syst.GetBinError(1)
    systs["P3 lo"] = h_FS_lo_syst.GetBinError(2)
    systs["P4 lo"] = h_FS_lo_syst.GetBinError(3)
    systs["M lo"] = h_FS_lo_syst.GetBinError(4)
    systs["L lo"] = h_FS_lo_syst.GetBinError(5)

    systs["P 23 lo"] = h_FS_23_lo_syst.GetBinError(1)
    systs["P3 23 lo"] = h_FS_23_lo_syst.GetBinError(2)
    systs["P4 23 lo"] = h_FS_23_lo_syst.GetBinError(3)
    systs["M 23 lo"] = h_FS_23_lo_syst.GetBinError(4)
    systs["L 23 lo"] = h_FS_23_lo_syst.GetBinError(5)

    systs["P 4 lo"] = h_FS_4_lo_syst.GetBinError(1)
    systs["P3 4 lo"] = h_FS_4_lo_syst.GetBinError(2)
    systs["P4 4 lo"] = h_FS_4_lo_syst.GetBinError(3)
    systs["M 4 lo"] = h_FS_4_lo_syst.GetBinError(4)
    systs["L 4 lo"] = h_FS_4_lo_syst.GetBinError(5)

    return vals,errs,systs

def getCounts(f,fshorts,fshort_systs,isMC = False,isSig = False):
    vals = {}
    stats = {}
    systs = {}
    njhts = ["LL","LLM","LM","LH","HL","HLM","HM","HH"]
    regions = ["MR","VR","SR"]
    pts = ["","_hi","_lo"]
    histnames = ["h_{}_{}_{}{}".format(njht,region,"23" if njht[0] == "L" else "4",pt) for njht in njhts for region in regions for pt in pts]
    for histname in histnames:
        tokens = histname.split("_")
        njht = tokens[1]
        region = tokens[2]
        pt = " "+tokens[4] if len(tokens) > 4 else ""
        core = "{} {}{}".format(njht,region,pt)
        hist_stcstats = f.Get(histname+"_stcstats_up" if not isSig else histname) # we only care about the central value, so just use the one that exists also for MR
        hist_allstats_up = f.Get(histname+"_allstats_up")
        hist_allstats_dn = f.Get(histname+"_allstats_dn")
        hist_fs = f.Get(histname+"_fshortsyst")
        hist_nc = f.Get(histname.replace("SR","VR")+"_nonclosure_systematic") # use VR-derived syst in SR
        for index,track_length in enumerate(["P","P3","P4","M","L"]):
            bin = index+1
            prefix = track_length + " " + core
            try:
                vals[prefix + " STC"] = hist_stcstats.GetBinContent(bin,3)
            except:
                print histname
                print f.GetName()
                exit(1)
            stats[prefix + " STC"] = getAsymmetricErrors(hist_stcstats.GetBinContent(bin,3)) if not isMC else [hist_stcstats.GetBinError(bin,3)]*2
            vals[prefix + " obs"] = hist_stcstats.GetBinContent(bin,1)
            stats[prefix + " obs"] = getAsymmetricErrors(hist_stcstats.GetBinContent(bin,1)) if not isMC else [hist_stcstats.GetBinError(bin,1)]*2
            if isSig or histname.find("MR") >= 0: continue
            vals[prefix + " pre"] = hist_stcstats.GetBinContent(bin,2)
            stats[prefix + " pre"] = [hist_allstats_up.GetBinError(bin,2),hist_allstats_dn.GetBinError(bin,2)]
            systs[prefix+" fs"] = hist_fs.GetBinError(bin,2)
            if vals[prefix+" pre"] > 0:
                systs[prefix+" fsrel"] = hist_fs.GetBinError(bin,2) / vals[prefix+" pre"]
            else:
                fsregion = prefix.replace(" MR","").replace(" VR","").replace(" SR","").replace("HLM","4").replace("LLM","23").replace("LL","23").replace("LM","23").replace("LH","23").replace("HL","4").replace("HM","4").replace("HH","4")                
                systs[prefix+" fsrel"] = fshort_systs[fsregion] / fshorts[fsregion] if fshorts[fsregion] > 0 else 0
            systs[prefix+" nc"] = hist_nc.GetBinContent(bin)*hist_stcstats.GetBinContent(bin,2) # syst is relative to prediction
            systs[prefix+" ncrel"] = hist_nc.GetBinContent(bin) # syst is relative to prediction

    return vals,stats,systs

def makePlotFshort(regions,dvals,derrs,dsysts,mvals,merrs,msysts,desc):
    simplecanvas.cd()
    tlfs=ROOT.TLegend(0.5,0.65,0.7,0.85)
    tlfs.Clear()
    nregions=len(regions)
    hdata=ROOT.TH1D(desc+"_fshort_data",desc+" f_{short};;f_{short}",nregions,0,nregions)
    hdata.SetLineWidth(3)
    hdata_syst=hdata.Clone(hdata.GetName()+"_syst")
    hmc=hdata.Clone(hdata.GetName().replace("data","mc"))
    hmc_syst=hmc.Clone(hmc.GetName()+"_syst")
    merr_list = []
    derr_list = []
    msyst_list = []
    dsyst_list = []
    for index,region in enumerate(regions):
        bin_index = index+1
        hmc_syst.GetXaxis().SetBinLabel(bin_index,region)
        mval = mvals[region]
        merr_list.append(merrs[region])
        msyst_list.append((sqrt(merrs[region][0]**2+msysts[region]**2), sqrt(merrs[region][1]**2+msysts[region]**2)))
        dval = dvals[region]
        derr_list.append(derrs[region])
        dsyst_list.append( ( sqrt(dsysts[region]**2 + derrs[region][0]**2), sqrt(derrs[region][1]**2+dsysts[region]**2)) )
        hmc.SetBinContent(bin_index,mval)
        hmc_syst.SetBinContent(bin_index,mval)
        hdata.SetBinContent(bin_index,dval)
        hdata_syst.SetBinContent(bin_index,dval)
    hmc_syst.GetXaxis().LabelsOption("v")
    hmc_syst.GetXaxis().SetTitleOffset(4.8)
    hmc_syst.SetMinimum(0)
    hmc_syst.SetMaximum(1.5*max(hmc.GetMaximum(),hdata.GetMaximum()))
    hdata.SetFillColor(ROOT.kRed)
    hmc.SetFillColor(ROOT.kBlue)
    hdata_syst.SetLineColor(ROOT.kRed)
    hmc_syst.SetLineColor(ROOT.kBlue)
    gdata = getPoissonGraph( hdata, derr_list )
    gmc = getPoissonGraph( hmc, merr_list )
    gdata_syst = getPoissonGraph( hdata_syst, dsyst_list )
    gmc_syst = getPoissonGraph( hmc_syst, msyst_list )
#    tlfs.AddEntry(hdata,desc+" Data")
    tlfs.AddEntry(hdata_syst,desc+" Data, with Syst")
#    tlfs.AddEntry(hmc,desc+" MC")
    tlfs.AddEntry(hmc_syst,desc+" MC, with Syst")
#    hmc.Draw("E2")
    hmc_syst.Draw("AXIS")
    gdata_syst.Draw("p same")
#    hdata.Draw("same E2")
#    hdata_syst.Draw("same E")
    gmc_syst.Draw("p same")
    tlfs.Draw()
    simplecanvas.SaveAs("{0}/{1}_fshort.png".format(plotdir,desc.replace(" ","_")))

def makePlotRaw(regions,vals,stats,systs,desc,rescale=1.0, combineSysts = True): # Raw means non-normalized. "rescale" multiplies prediction, to enable partial unblinding.
    ratiocanvas.cd()
    tl.Clear()
    nregions=len(regions)
    hobs=ROOT.TH1D(desc,desc+";;Short Track Counts",nregions,0,nregions)
    hobs.SetLineWidth(3)
    hpred=hobs.Clone(hobs.GetName()+"_prediction")
    perrs = []
    oerrs = []
    perrs_withfs = []
    perrs_all = []
    for index,region in enumerate(regions):
        bin_index = index+1
        hpred.GetXaxis().SetBinLabel(bin_index,region)
        pred = vals[region+" pre"]*rescale
        perr = [stats[region+" pre"][i] * rescale for i in [0,1]]
        obs = vals[region+" obs"]
        oerr = stats[region+" obs"]
        perr_fs = systs[region+" fs"] * rescale
        perr_nc = systs[region+" nc"] * rescale
        hpred.SetBinContent(bin_index,pred)
        hpred.SetBinError(bin_index,1e-9) # Need to explictly set bin error to epsilon so lines aren't connected when drawing later (as for "hist" style)
        perrs.append(perr)
        perrs_withfs.append( [sqrt(perr[i]**2 + perr_fs**2) for i in [0,1]]  )
        perrs_all.append( [sqrt(perr[i]**2 + perr_fs**2 + perr_nc**2) for i in [0,1]] )
        hobs.SetBinContent(bin_index,obs)
        oerrs.append(oerr)
    hpred.GetXaxis().LabelsOption("v")
    hpred.GetXaxis().SetTitleOffset(4.8)
    hpred.SetMinimum(-0.001)
    hpred.SetMaximum(2.0*max(hpred.GetMaximum(),hobs.GetMaximum()))
    hobs.SetLineColor(ROOT.kBlack)
    hobs.SetMinimum(hpred.GetMinimum())
    hobs.SetMaximum(hpred.GetMaximum())
    hpred.SetLineColor(ROOT.kRed)
    pads[0].cd()    
    gpred_stat = getPoissonGraph( hpred, perrs )
    gpred_stat.SetName("stat"+desc.replace("-","_").replace(" ","_"))
    gpred_withfs = getPoissonGraph( hpred, perrs_withfs )
    gpred_withfs.SetName("withfs"+desc.replace("-","_").replace(" ","_"))
    gpred_all = getPoissonGraph( hpred, perrs_all )
    gpred_all.SetName("all"+desc.replace("-","_").replace(" ","_"))
    gobs = getPoissonGraph( hobs, oerrs, False )
    gobs.SetName("obs"+desc.replace("-","_").replace(" ","_"))
    gpred_stat.SetFillColor(ROOT.kCyan-8)
    gpred_withfs.SetFillColor(ROOT.kGray)
    gpred_all.SetFillColor(ROOT.kGray+2)
    hpred.Draw("") # only want the axis from the histogram, for bin titles
    gpred_all.Draw("2 same") # 2 means draw filled rectangles for errors
    if not combineSysts:
        gpred_withfs.Draw("2 same")
    gpred_stat.Draw("2 same")
    hpred.Draw("same") # drawn without errors, and without connecting the lines, see above
    gobs.SetMarkerStyle(20)
    gobs.SetMarkerSize(2)
    gobs.SetMarkerColor(ROOT.kBlack)
    gobs.Draw("p same") # p draws in a typical histogram style, with markers
    hpred.Draw("AXIS same") # make tick marks show above fill areas
    tl.AddEntry(gobs,"Observation","pLE")
    tl.AddEntry(gpred_stat,"Prediction, Statistical Errors")
    if not combineSysts:
        tl.AddEntry(gpred_withfs,"Prediction, with f_{short} Syst")
    tl.AddEntry(gpred_all,"Prediction, with Total Error" if combineSysts else "Prediction, with also VR Syst")
    tl.Draw()
    pads[1].cd()
    h1=ROOT.TH1D("hratio"+desc,";;Obs / Pred",len(perrs),0,len(perrs))
    h1.SetLineWidth(3)
    h1.SetLineColor(ROOT.kRed)
    hobs_norm = hobs.Clone(hobs.GetName()+"_norm")
    perrs_all_norm = []
    perrs_withfs_norm = []
    perrs_stat_norm = []
    oerrs_norm = []
    maxval = 0.0
    for bin in range(1,len(perrs)+1):
        h1.GetXaxis().SetBinLabel(bin,"")
        this_pred = hpred.GetBinContent(bin)
        h1.SetBinContent(bin,1)
        if this_pred == 0:
            hobs_norm.SetBinContent(bin,-1)
            perrs_all_norm.append( [0,0] )
            perrs_withfs_norm.append( [0,0] )
            perrs_stat_norm.append( [0,0] )
            oerrs_norm.append( [0,0] )
        else: 
            hobs_norm.SetBinContent(bin,hobs.GetBinContent(bin) / this_pred)
            this_perr_all = perrs_all[bin-1]
            this_perr_withfs = perrs_withfs[bin-1]
            this_perr_stat = perrs[bin-1]
            this_oerr = oerrs[bin-1]
            perrs_all_norm.append( [this_perr_all[i]/this_pred for i in [0,1]] )
            perrs_withfs_norm.append( [this_perr_withfs[i]/this_pred for i in [0,1]] )
            perrs_stat_norm.append( [this_perr_stat[i]/this_pred for i in [0,1]] )
            oerrs_norm.append( [this_oerr[i]/this_pred for i in [0,1]] )
    h1.SetMaximum(min(round(hobs_norm.GetMaximum()+1),5))
    h1.SetMinimum(-0.001)
    gall_norm = getPoissonGraph( h1, perrs_all_norm )
    gall_norm.SetFillColor(ROOT.kGray+2)
    gwithfs_norm = getPoissonGraph( h1, perrs_withfs_norm )
    gwithfs_norm.SetFillColor(ROOT.kGray)
    gstat_norm = getPoissonGraph( h1, perrs_stat_norm)
    gstat_norm.SetFillColor(ROOT.kCyan-8)
    gobs_norm = getPoissonGraph(hobs_norm, oerrs_norm, False)
    gobs_norm.SetMarkerStyle(20)
    gobs_norm.SetMarkerSize(2)
    gobs_norm.SetMarkerColor(ROOT.kBlack)
    h1.GetYaxis().SetLabelSize(hpred.GetYaxis().GetLabelSize()*.83/.16/2)
    h1.GetYaxis().SetTitleSize(hpred.GetYaxis().GetTitleSize()*.83/.16/2)
    h1.GetYaxis().SetTitleOffset(0.35)
    h1.Draw("AXIS")
    # "0" option forces the drawing of error bars even if the central value is off-scale
    gall_norm.Draw("0 2 same")
    if not combineSysts:
        gwithfs_norm.Draw("0 2 same")
    gstat_norm.Draw("0 2 same")
    gobs_norm.Draw("0 p same")
    h1.Draw("same")
    ratiocanvas.SaveAs("{0}/{1}_raw.png".format(plotdir,desc.replace(" ","_")))

def makeSignalPlot(regions,vals_bg,stats_bg,systs_bg,list_of_vals_sig,list_of_errs_sig, rescale_lumi, desc, sig_tags, sig_colors, rescale_unblind, combineErrors = True): 
    ratiocanvas.cd()
    tl.Clear()
    nregions=len(regions)
    hobs=ROOT.TH1D(desc,desc+";;Short Track Counts",nregions,0,nregions)
    hobs.SetLineWidth(3)
    hpred=hobs.Clone(hobs.GetName()+"_prediction")
    list_of_hsigs = []
    for sig_tag in sig_tags:
        list_of_hsigs.append(hpred.Clone("hsig_"+desc+"_"+sig_tag))
    perrs = []
    oerrs = []
    perrs_withfs = []
    perrs_all = []
    for index,region in enumerate(regions):
        bin_index = index+1
        hpred.GetXaxis().SetBinLabel(bin_index,region)
        pred = vals_bg[region+" pre"] * rescale_unblind
        for sig_index,hsig in enumerate(list_of_hsigs):
            vals_sig = list_of_vals_sig[sig_index]
            val_sig = vals_sig[region+" obs"] * rescale_lumi * rescale_unblind
            hsig.SetBinContent(bin_index,val_sig + pred)
            hsig.SetBinError(bin_index,list_of_errs_sig[sig_index][region+" obs"][0] * rescale_lumi * rescale_unblind) # sig errors are symmetric (MC)
        perr = [stats_bg[region+" pre"][i] * rescale_unblind for i in [0,1]]
        obs = vals_bg[region+" obs"]
        oerr = stats_bg[region+" obs"]
        perr_fs = systs_bg[region+" fs"] * rescale_unblind
        perr_nc = systs_bg[region+" nc"] * rescale_unblind
        hpred.SetBinContent(bin_index,pred)
        hpred.SetBinError(bin_index,1e-9) # Need to explictly set bin error to epsilon so lines aren't connected when drawing later (as for "hist" style)
        perrs.append(perr)
        perrs_withfs.append( [sqrt(perr[i]**2 + perr_fs**2) for i in [0,1]]  )
        perrs_all.append( [sqrt(perr[i]**2 + perr_fs**2 + perr_nc**2) for i in [0,1]] )
        hobs.SetBinContent(bin_index,obs)
        oerrs.append(oerr)
    hpred.GetXaxis().LabelsOption("v")
    hpred.GetXaxis().SetTitleOffset(4.8)
    hpred.SetMinimum(-0.001)
    hpred.SetMaximum(2.0*max(hpred.GetMaximum(),hobs.GetMaximum()))
    hobs.SetLineColor(ROOT.kBlack)
    hobs.SetMinimum(hpred.GetMinimum())
    hobs.SetMaximum(hpred.GetMaximum())
    hpred.SetLineColor(ROOT.kRed)
    simplecanvas.cd()
    gpred_stat = getPoissonGraph( hpred, perrs )
    gpred_stat.SetName("stat"+desc.replace("-","_").replace(" ","_"))
    gpred_withfs = getPoissonGraph( hpred, perrs_withfs )
    gpred_withfs.SetName("withfs"+desc.replace("-","_").replace(" ","_"))
    gpred_all = getPoissonGraph( hpred, perrs_all )
    gpred_all.SetName("all"+desc.replace("-","_").replace(" ","_"))
    gobs = getPoissonGraph( hobs, oerrs, False )
    gobs.SetName("obs"+desc.replace("-","_").replace(" ","_"))
    gpred_stat.SetFillColor(ROOT.kCyan-8)
    gpred_withfs.SetFillColor(ROOT.kGray)
    gpred_all.SetFillColor(ROOT.kGray+2)
    hpred.Draw("") # only want the axis from the histogram, for bin titles
    gpred_all.Draw("2 same") # 2 means draw filled rectangles for errors
    if not combineErrors:
        gpred_withfs.Draw("2 same")
        gpred_stat.Draw("2 same")
    hpred.Draw("same") # drawn without errors, and without connecting the lines, see above
    gobs.SetMarkerStyle(20)
    gobs.SetMarkerSize(2)
    gobs.SetMarkerColor(ROOT.kBlack)
    gobs.Draw("p same") # p draws in a typical histogram style, with markers
    hpred.Draw("AXIS same") # make tick marks show above fill areas
    tl.AddEntry(gobs,"Observation","pLE")
    if not combineErrors:
        tl.AddEntry(gpred_stat,"Prediction, Statistical Errors")
        tl.AddEntry(gpred_withfs,"Prediction, with f_{short} Syst")
    tl.AddEntry(gpred_all,("Prediction, with also VR Syst" if not combineErrors else "Prediction, with Total Error"))
    for sig_index,hsig in enumerate(list_of_hsigs):
        hsig.SetLineColor(sig_colors[sig_index])
        hsig.Draw("E same")
        tl.AddEntry(hsig,sig_tags[sig_index]+" Pred BG (MC Stat Error)")
    tl.Draw()
    unblind = "partialunblind" if rescale_unblind < 1.0 else "fullunblind"
    simplecanvas.SaveAs("{}/{}_counts_{}.png".format(plotdir,desc.replace(" ","_").replace("(","").replace(")","").replace(",",""),unblind))

def makePlotDiscrepancies(regions_sets,vals_sets,errs_sets,systs_sets,desc,onlyNonMin=False,rescale=[1.0,1.0]): # Raw means non-normalized. "rescale" multiplies prediction, to enable partial unblinding.
    simplecanvas.cd()
    tl.Clear()
    # X.001 so slightly over X.0 is not in overlow, specifically for floating point weirdness in VR, which is +/- 1.0 by construction
    maxsigma = 1.001 if desc.find("VR") >= 0 else 2.001
    nbins = 10 if desc.find("VR") >= 0 else 20
    hsigma=ROOT.TH1D(desc,"N_{Pred} - N_{Obs}, "+desc+" Regions;(N_{Pred}-N_{Obs})/#sqrt{#sigma_{pred}^{2}+#sigma_{obs}^{2}};Region Count",nbins+1,-maxsigma,maxsigma)
    hsigma.SetLineWidth(3)
    hno0 = hsigma.Clone(hsigma.GetName()+"_no0")
    print "begin mpd"
    for set_index in range(len(regions_sets)):
        regions = regions_sets[set_index]
        vals = vals_sets[set_index]
        errs = errs_sets[set_index]
        systs = systs_sets[set_index]
        for index,region in enumerate(regions):
            bin_index = index+1
            pred = vals[region+" pre"]*rescale[set_index]
            obs = vals[region+" obs"]
            perr = errs[region+" pre"][1 if obs < pred else 0]*rescale[set_index] # take upper error if obs < pred, else lower
            oerr = errs[region+" obs"][1 if obs > pred else 0] # take lower error if pred < obs, upper if pred > obs
            perr_syst = sqrt(perr**2 + systs[region+" fs"]**2 + systs[region+" nc"]**2)
            delta = pred - obs
            total_err = sqrt( oerr**2 + perr_syst**2 )
            if total_err > 0:
                sigma = delta/total_err 
            elif delta == 0:
                sigma = 0
            elif delta < 0:
                sigma = -maxsigma+0.001
            elif delta > 0:
                sigma = maxsigma-0.001
            if hsigma.FindBin(sigma) == 0 or hsigma.FindBin(sigma) == hsigma.GetNbinsX()+1:
                print "Sigma =", sigma, "is overflow", desc, "Setting to fall in max bin"
                sigma = maxsigma - 0.001 if sigma > 0 else maxsigma + 0.001 # Automatic overflow bins
            hsigma.Fill(sigma)
            if not (obs == 0 and pred < 0.5): hno0.Fill(sigma)
    print "end mpd"
    hsigma.SetMinimum(0)
    hsigma.SetMaximum(1.5*hsigma.GetMaximum())
    hsigma.SetLineColor(ROOT.kBlack)
    hsigma.Draw("hist")
    hno0.SetLineColor(ROOT.kRed)
    hno0.Draw("hist same")
    tl.AddEntry(hsigma,"(N_{{Pred}}-N_{{Obs}})/#sigma, Mean = {:.2f}, Deviation = {:.2f}".format(hsigma.GetMean(),hsigma.GetStdDev()))
    tl.AddEntry(hno0,"Suppressed for Obs = 0 and Pred < 0.5")
    tl.Draw()
    simplecanvas.SaveAs("{0}/{1}_sigma.png".format(plotdir,desc.replace(" ","_")))

# compare signal yields at limiting mu, and systematic error, in VR
def makePlotSigErr(regions,vals,nonncerrs,ncsysts,list_of_sigvals,sigtags,sigcolors,limits,rescale_lumi,desc):
    simplecanvas.cd()
    tl.Clear()
    nbins=len(regions)
    hsig=ROOT.TH1D(desc+"_sig",desc+" Signal Yields Compared to Systematics;;Count",nbins,0,nbins)
    hsig.SetLineWidth(3)
    hsigs = []
    for i_sig in range(len(list_of_sigvals)):
        hsigs.append(hsig.Clone(hsig.GetName()+str(i_sig)))        
        hsigs[i_sig].SetLineColor(sigcolors[i_sig])
    herr=hsig.Clone(desc+"_err")
    herr.SetLineColor(ROOT.kGray)
    hobserr=herr.Clone(desc+"_errobs")
    hobserr.SetLineColor(ROOT.kYellow+2)
    hnc=hsig.Clone(desc+"_ncerr")
    hnc.SetLineColor(ROOT.kGray+2)
    for index,region in enumerate(regions):
        isVR = region.find("VR") >= 0
        bin_index = index+1
        pred = vals[region+" pre"]
        obs = vals[region+" obs"] if isVR else pred  # in the SR, currently have obs = pred, with partially blinded true obs
        obserr = sqrt(obs) if obs > 0 else 1.83
        perr = nonncerrs[region+" pre"]
        herr.GetXaxis().SetBinLabel(bin_index,region)
        ncerr = ncsysts[region] * pred
        hnc.SetBinContent(bin_index,ncerr)
        herr.SetBinContent(bin_index,sqrt(perr**2 + ncerr**2))
        hobserr.SetBinContent(bin_index,sqrt(obserr**2 + perr**2 + ncerr**2))
        for sigindex,sigvals in enumerate(list_of_sigvals):
            this_hsig = hsigs[sigindex]
            sig_count = sigvals[region+" obs"] * rescale_lumi * limits[sigindex]
            this_hsig.SetBinContent(bin_index,sig_count)
    herr.SetMinimum(1e-3)
    herr.SetMaximum(100*herr.GetMaximum())
    herr.GetXaxis().LabelsOption("v")
    herr.Draw("hist")
    for i_sig,h in enumerate(hsigs):
        h.Draw("hist same")
        tl.AddEntry(h,sigtags[i_sig]+" x {}".format(limits[i_sig]))
    hnc.Draw("hist same")
    hobserr.Draw("hist same")
    tl.AddEntry(hnc,"VR Systematic")
    tl.AddEntry(herr,"Total Error on Prediction")
    tl.AddEntry(hobserr,"Total Error, incl Obs Stat")
    tl.Draw()
    simplecanvas.SetLogy()
    simplecanvas.SaveAs("{0}/{1}_systcontam.png".format(plotdir,desc.replace(" ","_")))
    simplecanvas.SetLogy(False)

#d18=ROOT.TFile.Open("output_merged/data_2018_{}.root".format(tag))
#d17=ROOT.TFile.Open("output_merged/data_2017_{}.root".format(tag))
d1718=ROOT.TFile.Open("output_merged/data_2017and2018_{}.root".format(tag))
d16=ROOT.TFile.Open("output_merged/data_2016_{}.root".format(tag))
m1718=ROOT.TFile.Open("output_merged/mc_2017and2018_{}.root".format(tag))
m16=ROOT.TFile.Open("output_merged/mc_2016_{}.root".format(tag))

sig1718 = {}
sig1718[(1800,1400,10)]=ROOT.TFile("output_unmerged/2017_{0}/signal/fastsim_10cm_1800-1400.root".format(tag))
sig1718[(1800,1600,10)]=ROOT.TFile("output_unmerged/2017_{0}/signal/fastsim_10cm_1800-1600.root".format(tag))
sig1718[(1800,1700,10)]=ROOT.TFile("output_unmerged/2017_{0}/signal/fastsim_10cm_1800-1700.root".format(tag))
sig1718[(1800,1400,90)]=ROOT.TFile("output_unmerged/2017_{0}/signal/fastsim_90cm_1800-1400.root".format(tag))
sig1718[(1800,1600,90)]=ROOT.TFile("output_unmerged/2017_{0}/signal/fastsim_90cm_1800-1600.root".format(tag))
sig1718[(1800,1700,90)]=ROOT.TFile("output_unmerged/2017_{0}/signal/fastsim_90cm_1800-1700.root".format(tag))

siglimits = {}
siglimits[(1800,1400,10)] = 0.56
siglimits[(1800,1600,10)] = 1.3
siglimits[(1800,1700,10)] = 3.3
siglimits[(1800,1400,90)] = 0.058
siglimits[(1800,1600,90)] = 0.14
siglimits[(1800,1700,90)] = 0.28

#D18f,eD18f=getFshorts(d18)
#D17f,eD17f=getFshorts(d17)
D1718f,eD1718f,sD1718f=getFshorts(d1718)
D16f,eD16f,sD16f=getFshorts(d16)
M1718f,eM1718f,sM1718f=getFshorts(m1718)
M16f,eM16f,sM16f=getFshorts(m16)

print D1718f["P 23 hi"], sD1718f["P 23 hi"]

# Mglu, Mlsp, ctau, limit mu
signal_points=[(1800,1400,10),(1800,1600,10),(1800,1700,10),(1800,1400,90),(1800,1600,90),(1800,1700,90)]
signal_points_10=[(1800,1400,10),(1800,1600,10),(1800,1700,10)]
signal_points_90=[(1800,1400,90),(1800,1600,90),(1800,1700,90)]
#D18,eD18=getCounts(d18)
#D17,eD17=getCounts(d17)
D1718,eD1718,sD1718=getCounts(d1718,D1718f,sD1718f)
D16,eD16,sD16=getCounts(d16,D16f,sD16f)
M1718,eM1718,sM1718=getCounts(m1718,M1718f,sM1718f,True)
M16,eM16,sM16=getCounts(m16,M16f,sM16f,True)

S1718 = {}
eS1718 = {}
for sp in signal_points:
    S1718[sp],eS1718[sp],dummy_systs = getCounts(sig1718[sp],{},{},True,True) # sig doesn't care about fshort, just feed dummies
    sig1718[sp].Close()

#d18.Close()
#d17.Close()
d1718.Close()
d16.Close()
m1718.Close()
m16.Close()

def getMergedCountsLine(region,year=None):
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]
    if cat == "P " or year == 2016: # don't return 2017-2018
        lumi = 35.9/41.97
        return colorline+"{} & - & {:.3f} +{:.3f}-{:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR","") + " (2016)",
                                                                                                                                                                                                                                    D16[region+" pre"], sqrt(eD16[region+" pre"][0]**2 + sD16[region+" nc"]**2 + sD16[region+" fs"]**2), sqrt(eD16[region+" pre"][1]**2 + sD16[region+" nc"]**2 + sD16[region+" fs"]**2), S1718[(1800,1400,10)][region+" obs"]*lumi, eS1718[(1800,1400,10)][region+" obs"][0]*lumi, S1718[(1800,1600,10)][region+" obs"]*lumi, eS1718[(1800,1600,10)][region+" obs"][0]*lumi, S1718[(1800,1700,10)][region+" obs"]*lumi, eS1718[(1800,1700,10)][region+" obs"][0]*lumi, S1718[(1800,1400,90)][region+" obs"]*lumi, eS1718[(1800,1400,90)][region+" obs"][0]*lumi, S1718[(1800,1600,90)][region+" obs"]*lumi, eS1718[(1800,1600,90)][region+" obs"][0]*lumi, S1718[(1800,1700,90)][region+" obs"]*lumi, eS1718[(1800,1700,90)][region+" obs"][0]*lumi)
    elif (cat == "P3" or cat == "P4") or year == 2017 or year == 2018: # don't return 2016
        lumi = 1+(58.83/41.97)
        return colorline+"{} & {:.3f} +{:.3f}-{:.3f} & - & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR","")+" (2017-18)",
                                                                                                                                                                                                                                    D1718[region+" pre"], sqrt(eD1718[region+" pre"][0]**2+sD1718[region+" nc"]**2+sD1718[region+" fs"]**2), sqrt(eD1718[region+" pre"][1]**2+sD1718[region+" nc"]**2+sD1718[region+" fs"]**2), S1718[(1800,1400,10)][region+" obs"]*lumi, eS1718[(1800,1400,10)][region+" obs"][0]*lumi, S1718[(1800,1600,10)][region+" obs"]*lumi, eS1718[(1800,1600,10)][region+" obs"][0]*lumi, S1718[(1800,1700,10)][region+" obs"]*lumi, eS1718[(1800,1700,10)][region+" obs"][0]*lumi, S1718[(1800,1400,90)][region+" obs"]*lumi, eS1718[(1800,1400,90)][region+" obs"][0]*lumi, S1718[(1800,1600,90)][region+" obs"]*lumi, eS1718[(1800,1600,90)][region+" obs"][0]*lumi, S1718[(1800,1700,90)][region+" obs"]*lumi, eS1718[(1800,1700,90)][region+" obs"][0]*lumi)
    else:
        lumi = (35.9/41.97)
        line_for_2016 = colorline+"{} & - & {:.3f} +{:.3f}-{:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR","") + " (2016)",
                                                                                                                                                                                                                                    D16[region+" pre"], sqrt(eD16[region+" pre"][0]**2+sD16[region+" fs"]**2+sD16[region+" nc"]**2), sqrt(eD16[region+" pre"][1]**2+sD16[region+" fs"]**2+sD16[region+" nc"]**2), S1718[(1800,1400,10)][region+" obs"]*lumi, eS1718[(1800,1400,10)][region+" obs"][0]*lumi, S1718[(1800,1600,10)][region+" obs"]*lumi, eS1718[(1800,1600,10)][region+" obs"][0]*lumi, S1718[(1800,1700,10)][region+" obs"]*lumi, eS1718[(1800,1700,10)][region+" obs"][0]*lumi, S1718[(1800,1400,90)][region+" obs"]*lumi, eS1718[(1800,1400,90)][region+" obs"][0]*lumi, S1718[(1800,1600,90)][region+" obs"]*lumi, eS1718[(1800,1600,90)][region+" obs"][0]*lumi, S1718[(1800,1700,90)][region+" obs"]*lumi, eS1718[(1800,1700,90)][region+" obs"][0]*lumi)
        lumi = 1+(58.83/41.97)
        line_for_2017and2018 = colorline+"{} & {:.3f} +{:.3f}-{:.3f} & - & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR","")+" (2017-18)",
                                                                                                                                                                                                                                    D1718[region+" pre"], sqrt(eD1718[region+" pre"][0]**2+sD1718[region+" fs"]**2+sD1718[region+" nc"]**2), sqrt(eD1718[region+" pre"][1]**2+sD1718[region+" fs"]**2+sD1718[region+" nc"]**2), S1718[(1800,1400,10)][region+" obs"]*lumi, eS1718[(1800,1400,10)][region+" obs"][0]*lumi, S1718[(1800,1600,10)][region+" obs"]*lumi, eS1718[(1800,1600,10)][region+" obs"][0]*lumi, S1718[(1800,1700,10)][region+" obs"]*lumi, eS1718[(1800,1700,10)][region+" obs"][0]*lumi, S1718[(1800,1400,90)][region+" obs"]*lumi, eS1718[(1800,1400,90)][region+" obs"][0]*lumi, S1718[(1800,1600,90)][region+" obs"]*lumi, eS1718[(1800,1600,90)][region+" obs"][0]*lumi, S1718[(1800,1700,90)][region+" obs"]*lumi, eS1718[(1800,1700,90)][region+" obs"][0]*lumi)
        return line_for_2016 + line_for_2017and2018


def getMRvsSRLine(region_SR,year=None):
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    region_MR = region_SR.replace("SR","MR")    
    num1 = S1718[(1800,1400,90)][region_MR+" obs"]
    num1err = eS1718[(1800,1400,90)][region_MR+" obs"]
    num2 = S1718[(1800,1600,90)][region_MR+" obs"]
    num2err = eS1718[(1800,1600,90)][region_MR+" obs"]
    num3 = S1718[(1800,1700,90)][region_MR+" obs"]
    num3err = eS1718[(1800,1700,90)][region_MR+" obs"]
    num4 = S1718[(1800,1400,10)][region_MR+" obs"]
    num4err = eS1718[(1800,1400,10)][region_MR+" obs"]
    num5 = S1718[(1800,1600,10)][region_MR+" obs"]
    num5err = eS1718[(1800,1600,10)][region_MR+" obs"]
    num6 = S1718[(1800,1700,10)][region_MR+" obs"]
    num6err = eS1718[(1800,1700,10)][region_MR+" obs"]

    den1 = S1718[(1800,1400,90)][region_SR+" obs"]
    den1err = eS1718[(1800,1400,90)][region_SR+" obs"]
    den2 = S1718[(1800,1600,90)][region_SR+" obs"]
    den2err = eS1718[(1800,1600,90)][region_SR+" obs"]
    den3 = S1718[(1800,1700,90)][region_SR+" obs"]
    den3err = eS1718[(1800,1700,90)][region_SR+" obs"]
    den4 = S1718[(1800,1400,10)][region_SR+" obs"]
    den4err = eS1718[(1800,1400,10)][region_SR+" obs"]
    den5 = S1718[(1800,1600,10)][region_SR+" obs"]
    den5err = eS1718[(1800,1600,10)][region_SR+" obs"]
    den6 = S1718[(1800,1700,10)][region_SR+" obs"]
    den6err = eS1718[(1800,1700,10)][region_SR+" obs"]

    if den1 > 0 and num1 > 0:
        rat1 = num1/den1
        rat1err = sqrt((num1err/num1)**2+(den1err/den1)**2)*rat1
        rat1str = "{:.2f} $\pm$ {:.2f}".format(rat1,rat1err)
    elif num1 > 0:
        rat1str = "{:.2f} $\pm$ {:.2f} / 0".format(num1,num1err)
    elif den1 > 0:
        rat1str = "0 / {:.2f} $\pm$ {:.2f}".format(den1,den1err)
    else: rat1str = " -- "
    if den2 > 0 and num2 > 0:
        rat2 = num2/den2
        rat2err = sqrt((num2err/num2)**2+(den2err/den2)**2)*rat2
        rat2str = "{:.2f} $\pm$ {:.2f}".format(rat2,rat2err)
    elif num2 > 0:
        rat2str = "{:.2f} $\pm$ {:.2f} / 0".format(num2,num2err)
    elif den2 > 0:
        rat2str = "0 / {:.2f} $\pm$ {:.2f}".format(den2,den2err)
    else: rat2str = " -- "
    if den3 > 0 and num3 > 0:
        rat3 = num3/den3
        rat3err = sqrt((num3err/num3)**2+(den3err/den3)**2)*rat3
        rat3str = "{:.2f} $\pm$ {:.2f}".format(rat3,rat3err)
    elif num3 > 0:
        rat3str = "{:.2f} $\pm$ {:.2f} / 0".format(num3,num3err)
    elif den3 > 0:
        rat3str = "0 / {:.2f} $\pm$ {:.2f}".format(den3,den3err)
    else: rat3str = " -- "
    if den4 > 0 and num4 > 0:
        rat4 = num4/den4
        rat4err = sqrt((num4err/num4)**2+(den4err/den4)**2)*rat4
        rat4str = "{:.2f} $\pm$ {:.2f}".format(rat4,rat4err)
    elif num4 > 0:
        rat4str = "{:.2f} $\pm$ {:.2f} / 0".format(num4,num4err)
    elif den4 > 0:
        rat4str = "0 / {:.2f} $\pm$ {:.2f}".format(den4,den4err)
    else: rat4str = " -- "
    if den5 > 0 and num5 > 0:
        rat5 = num5/den5
        rat5err = sqrt((num5err/num5)**2+(den5err/den5)**2)*rat5
        rat5str = "{:.2f} $\pm$ {:.2f}".format(rat5,rat5err)
    elif num5 > 0:
        rat5str = "{:.2f} $\pm$ {:.2f} / 0".format(num5,num5err)
    elif den5 > 0:
        rat5str = "0 / {:.2f} $\pm$ {:.2f}".format(den5,den5err)
    else: rat5str = " -- "
    if den6 > 0 and num6 > 0:
        rat6 = num6/den6
        rat6err = sqrt((num6err/num6)**2+(den6err/den6)**2)*rat6
        rat6str = "{:.2f} $\pm$ {:.2f}".format(rat6,rat6err)
    elif num6 > 0:
        rat6str = "{:.2f} $\pm$ {:.2f} / 0".format(num6,num6err)
    elif den6 > 0:
        rat6str = "0 / {:.2f} $\pm$ {:.2f}".format(den6,den6err)
    else: rat6str = " -- "

    return colorline+"{} & {} & {} & {} & {} & {} & {} \\\\\n".format(region_SR.replace(" SR",""), rat1str, rat2str, rat3str, rat4str, rat5str, rat6str)


def getMergedLineDataSTC(region,rescale16=1.0,rescale1718=1.0): # rescale multiplies prediction to enable partial unblinding
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]    
    pred16 = D16[region+" pre"]*rescale16
    pred1718 = D1718[region+" pre"]*rescale1718
    if pred16 > 0:
        systFS16 = "{:.3f}".format(sD16[region+" fs"])
        systNC16 = "{:.3f}".format(sD16[region+" nc"])
    else: 
        systFS16 = "$<${:.1f}\%$>$".format(100*sD16[region+" fsrel"])
        systNC16 = "$<${:.1f}\%$>$".format(100*sD16[region+" ncrel"])
    if pred1718 > 0:
        systFS1718 = "{:.3f}".format(sD1718[region+" fs"])
        systNC1718 = "{:.3f}".format(sD1718[region+" nc"])
    else: 
        systFS1718 = "$<${:.1f}\%$>$".format(100*sD1718[region+" fsrel"])
        systNC1718 = "$<${:.1f}\%$>$".format(100*sD1718[region+" ncrel"])
    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & - & {:.0f} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D16[region+" STC"], pred16, eD16[region+" pre"][0]*rescale16, eD16[region+" pre"][1]*rescale16, systFS16, systNC16, D16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & {:.0f} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.0f} & - & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D1718[region+" STC"], pred1718, eD1718[region+" pre"][0]*rescale1718, eD1718[region+" pre"][1]*rescale1718, systFS1718, systNC1718, D1718[region+" obs"])

    else:
        return colorline+"{} & {:.0f} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (f$_{{short}}$) $\pm$ {} (VR syst) & {:.0f} & {:.0f} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D1718[region+" STC"], pred1718, eD1718[region+" pre"][0]*rescale1718, eD1718[region+" pre"][1]*rescale1718, systFS1718, systNC1718, D1718[region+" obs"],
                           D16[region+" STC"], pred16, eD16[region+" pre"][0]*rescale16, eD16[region+" pre"][1]*rescale16, systFS16, systNC16, D16[region+" obs"])

def getMergedLineDataSTC_Combined(region,rescale16=1.0,rescale1718=1.0): # rescale multiplies prediction to enable partial unblinding
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]    
    pred16 = D16[region+" pre"]*rescale16
    pred1718 = D1718[region+" pre"]*rescale1718
    if pred16 > 0:
        syst16 = "{:.3f}".format(sqrt(sD16[region+" fs"]**2 + sD16[region+" nc"]**2))
    else: 
        systFS16 = "$<${:.1f}\%$>$".format(100*sqrt(sD16[region+" fsrel"]**2 + sD16[region+" ncrel"]**2))
    if pred1718 > 0:
        systFS1718 = "{:.3f}".format(sqrt(sD1718[region+" fs"]**2 + sD1718[region+" nc"]**2))
    else: 
        systFS1718 = "$<${:.1f}\%$>$".format(100*sqrt(sD1718[region+" fsrel"]**2 + sD1718[region+" ncrel"]**2))

    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & - & {:.0f} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (syst) & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D16[region+" STC"], pred16, eD16[region+" pre"][0]*rescale16, eD16[region+" pre"][1]*rescale16, syst16, D16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & {:.0f} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (syst) & {:.0f} & - & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D1718[region+" STC"], pred1718, eD1718[region+" pre"][0]*rescale1718, eD1718[region+" pre"][1]*rescale1718, syst1718, D1718[region+" obs"])

    else:
        return colorline+"{} & {:.0f} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (VR syst) & {:.0f} & {:.0f} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (VR syst) & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D1718[region+" STC"], pred1718, eD1718[region+" pre"][0]*rescale1718, eD1718[region+" pre"][1]*rescale1718, syst1718, D1718[region+" obs"],
                           D16[region+" STC"], pred16, eD16[region+" pre"][0]*rescale16, eD16[region+" pre"][1]*rescale16, syst16, D16[region+" obs"])

def getMergedLineData(region,rescale16=1.0,rescale1718=1.0): # rescale multiplies prediction to enable partial unblinding
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]    
    pred16 = D16[region+" pre"]*rescale16
    pred1718 = D1718[region+" pre"]*rescale1718
    if pred16 > 0:
        systFS16 = "{:.3f}".format(sD16[region+" fs"])
        systNC16 = "{:.3f}".format(sD16[region+" nc"])
    else: 
        systFS16 = "$<${:.1f}\%$>$".format(100*sD16[region+" fsrel"])
        systNC16 = "$<${:.1f}\%$>$".format(100*sD16[region+" ncrel"])
    if pred1718 > 0:
        systFS1718 = "{:.3f}".format(sD1718[region+" fs"])
        systNC1718 = "{:.3f}".format(sD1718[region+" nc"])
    else: 
        systFS1718 = "$<${:.1f}\%$>$".format(100*sD1718[region+" fsrel"])
        systNC1718 = "$<${:.1f}\%$>$".format(100*sD1718[region+" ncrel"])
    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred16, eD16[region+" pre"][0]*rescale16, eD16[region+" pre"][1]*rescale16, systFS16, systNC16, D16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & - & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred1718, eD1718[region+" pre"][0]*rescale1718, eD1718[region+" pre"][1]*rescale1718, systFS1718, systNC1718, D1718[region+" obs"])

    else:
        return colorline+"{} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (f$_{{short}}$) $\pm$ {} (VR syst) & {:.0f} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred1718, eD1718[region+" pre"][0]*rescale1718, eD1718[region+" pre"][1]*rescale1718, systFS1718, systNC1718, D1718[region+" obs"],
                           pred16, eD16[region+" pre"][0]*rescale16, eD16[region+" pre"][1]*rescale16, systFS16, systNC16, D16[region+" obs"])

def getMergedLineData_Combined(region,rescale16=1.0,rescale1718=1.0): # rescale multiplies prediction to enable partial unblinding
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]    
    pred16 = D16[region+" pre"]*rescale16
    pred1718 = D1718[region+" pre"]*rescale1718
    if pred16 > 0:
        syst16 = "{:.3f}".format(sqrt(sD16[region+" fs"]**2 + sD16[region+" nc"]**2))
    else: 
        systFS16 = "$<${:.1f}\%$>$".format(100*sqrt(sD16[region+" fsrel"]**2 + sD16[region+" ncrel"]**2))
    if pred1718 > 0:
        systFS1718 = "{:.3f}".format(sqrt(sD1718[region+" fs"]**2 + sD1718[region+" nc"]**2))
    else: 
        systFS1718 = "$<${:.1f}\%$>$".format(100*sqrt(sD1718[region+" fsrel"]**2 + sD1718[region+" ncrel"]**2))

    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (syst) & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred16, eD16[region+" pre"][0]*rescale16, eD16[region+" pre"][1]*rescale16, syst16, D16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (syst) & {:.0f} & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred1718, eD1718[region+" pre"][0]*rescale1718, eD1718[region+" pre"][1]*rescale1718, syst1718, D1718[region+" obs"])

    else:
        return colorline+"{} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (VR syst) & {:.0f} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (VR syst) & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred1718, eD1718[region+" pre"][0]*rescale1718, eD1718[region+" pre"][1]*rescale1718, syst1718, D1718[region+" obs"],
                           pred16, eD16[region+" pre"][0]*rescale16, eD16[region+" pre"][1]*rescale16, syst16, D16[region+" obs"])

def getMergedLineMCSTC(region,rescale16=1.0,rescale17=1.0): # rescale multiplies prediction to enable partial unblinding
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]    
    pred16 = M16[region+" pre"]*rescale16
    pred17 = M1718[region+" pre"]*rescale17
    if pred16 > 0:
        systFS16 = "{:.3f}".format(sM16[region+" fs"])
        systNC16 = "{:.3f}".format(sM16[region+" nc"])
    else: 
        systFS16 = "$<${:.1f}\%$>$".format(100*sM16[region+" fsrel"])
        systNC16 = "$<${:.1f}\%$>$".format(100*sM16[region+" ncrel"])
    if pred17 > 0:
        systFS17 = "{:.3f}".format(sM1718[region+" fs"])
        systNC17 = "{:.3f}".format(sM1718[region+" nc"])
    else: 
        systFS17 = "$<${:.1f}\%$>$".format(100*sM1718[region+" fsrel"])
        systNC17 = "$<${:.1f}\%$>$".format(100*sM1718[region+" ncrel"])
    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & - & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M16[region+" STC"], pred16, eM16[region+" pre"][0]*rescale16, systFS16, systNC16, M16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.3f} & - & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M1718[region+" STC"], pred17, eM1718[region+" pre"][0]*rescale17, systFS17, systNC17, M1718[region+" obs"])

    else:
        return colorline+"{} & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (f$_{{short}}$) $\pm$ {} (VR syst) & {:.3f} & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M1718[region+" STC"], pred17, eM1718[region+" pre"][0]*rescale17, systFS17, systNC17, M1718[region+" obs"],
                           M16[region+" STC"], pred16, eM16[region+" pre"][0]*rescale16, systFS16, systNC16, M16[region+" obs"])

def getMergedLineMCSTC_Combined(region,rescale16=1.0,rescale17=1.0): # rescale multiplies prediction to enable partial unblinding
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]    
    pred16 = M16[region+" pre"]*rescale16
    pred17 = M1718[region+" pre"]*rescale17
    if pred16 > 0:
        syst16 = "{:.3f}".format(sqrt(sM16[region+" fs"]**2 + sM16[region+" nc"]**2))
    else: 
        systFS16 = "$<${:.1f}\%$>$".format(100*sqrt(sM16[region+" fsrel"]**2 + sM16[region+" ncrel"]**2))
    if pred17 > 0:
        systFS17 = "{:.3f}".format(sqrt(sM1718[region+" fs"]**2 + sM1718[region+" nc"]**2))
    else: 
        systFS17 = "$<${:.1f}\%$>$".format(100*sqrt(sM1718[region+" fsrel"]**2 + sM1718[region+" ncrel"]**2))

    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & - & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (syst) & {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M16[region+" STC"], pred16, eM16[region+" pre"][0]*rescale16, syst16, M16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (syst) & {:.3f} & - & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M1718[region+" STC"], pred17, eM1718[region+" pre"][0]*rescale17, syst17, M1718[region+" obs"])

    else:
        return colorline+"{} & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (VR syst) & {:.3f} & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (VR syst) & {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M1718[region+" STC"], pred17, eM1718[region+" pre"][0]*rescale17, syst17, M1718[region+" obs"],
                           M16[region+" STC"], pred16, eM16[region+" pre"][0]*rescale16, syst16, M16[region+" obs"])


def getMergedLineMC(region,rescale16=1.0,rescale17=1.0): # rescale multiplies prediction to enable partial unblinding
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]    
    pred16 = M16[region+" pre"]*rescale16
    pred17 = M1718[region+" pre"]*rescale17
    if pred16 > 0:
        systFS16 = "{:.3f}".format(sM16[region+" fs"])
        systNC16 = "{:.3f}".format(sM16[region+" nc"])
    else: 
        systFS16 = "$<${:.1f}\%$>$".format(100*sM16[region+" fsrel"])
        systNC16 = "$<${:.1f}\%$>$".format(100*sM16[region+" ncrel"])
    if pred17 > 0:
        systFS17 = "{:.3f}".format(sM1718[region+" fs"])
        systNC17 = "{:.3f}".format(sM1718[region+" nc"])
    else: 
        systFS17 = "$<${:.1f}\%$>$".format(100*sM1718[region+" fsrel"])
        systNC17 = "$<${:.1f}\%$>$".format(100*sM1718[region+" ncrel"])
    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred16, eM16[region+" pre"][0]*rescale16, systFS16, systNC16, M16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & - & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred17, eM1718[region+" pre"][0]*rescale17, systFS17, systNC17, M1718[region+" obs"])

    else:
        return colorline+"{} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (f$_{{short}}$) $\pm$ {} (VR syst) & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred17, eM1718[region+" pre"][0]*rescale17, systFS17, systNC17, M1718[region+" obs"],
                           pred16, eM16[region+" pre"][0]*rescale16, systFS16, systNC16, M16[region+" obs"])


def getMergedLineMC_Combined(region,rescale16=1.0,rescale17=1.0): # rescale multiplies prediction to enable partial unblinding
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]    
    pred16 = M16[region+" pre"]*rescale16
    pred17 = M1718[region+" pre"]*rescale17
    if pred16 > 0:
        syst16 = "{:.3f}".format(sqrt(sM16[region+" fs"]**2 + sM16[region+" nc"]**2))
    else: 
        systFS16 = "$<${:.1f}\%$>$".format(100*sqrt(sM16[region+" fsrel"]**2 + sM16[region+" ncrel"]**2))
    if pred17 > 0:
        systFS17 = "{:.3f}".format(sqrt(sM1718[region+" fs"]**2 + sM1718[region+" nc"]**2))
    else: 
        systFS17 = "$<${:.1f}\%$>$".format(100*sqrt(sM1718[region+" fsrel"]**2 + sM1718[region+" ncrel"]**2))

    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (syst) & {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred16, eM16[region+" pre"][0]*rescale16, syst16, M16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (syst) & {:.3f} & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred17, eM1718[region+" pre"][0]*rescale17, syst17, M1718[region+" obs"])

    else:
        return colorline+"{} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (VR syst) & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (VR syst) & {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred17, eM1718[region+" pre"][0]*rescale17, syst17, M1718[region+" obs"],
                           pred16, eM16[region+" pre"][0]*rescale16, syst16, M16[region+" obs"])



def getMergedFSLineData(cat):
    if cat[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif cat[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"                                   
    subcat = cat[0:2]
    if subcat == "P ": # Don't return 2017-18
        return colorline+"{} & - & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {:.3f} (syst)\\\\ \n".format(cat,D16f[cat],eD16f[cat][0],eD16f[cat][1],sD16f[cat])
    elif subcat == "P3" or subcat == "P4": # don't return 2016
        return colorline+"{} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {:.3f} (syst) & -\\\\ \n".format(cat,D1718f[cat],eD1718f[cat][0],eD1718f[cat][1],sD1718f[cat])
    else:
        return colorline+"{} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {:.3f} (syst) & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {:.3f} (syst)\\\\ \n".format(cat,D1718f[cat],eD1718f[cat][0],eD1718f[cat][1],sD1718f[cat],D16f[cat],eD16f[cat][0],eD16f[cat][1],sD16f[cat])

def getMergedFSLineMC(cat):
    if cat[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif cat[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"                                   
    subcat = cat[0:2]
    if subcat == "P ": # Don't return 2017-18
        return colorline+"{} & - & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst)\\\\ \n".format(cat,M16f[cat],eM16f[cat][0],sM16f[cat])
    elif subcat == "P3" or subcat == "P4": # don't return 2016
        return colorline+"{} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst) & -\\\\ \n".format(cat,M1718f[cat],eM1718f[cat][0],sM1718f[cat])
    else:
        return colorline+"{} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst) & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst)\\\\ \n".format(cat,M1718f[cat],eM1718f[cat][0],sM1718f[cat],M16f[cat],eM16f[cat][0],eM16f[cat][1],sM16f[cat])

#############
# Make output
#############

regionsNoL16VR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["P","M"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"] ]
regionsNoL16SR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P","M"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"] ]
regionsNoL1718VR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["P3","P4","M"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"]]
regionsNoL1718SR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P3","P4","M"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"]]
regionsLVR = ["L " + kin + " " + reg for kin in ["LLM","LH","HLM","HH"] for reg in ["VR"]]
regionsLSR = ["L " + kin + " " + reg for kin in ["LLM","LH","HLM","HH"] for reg in ["SR"]]

regionsNicelyOrderedSR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P","P3","P4","M"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"] ] + regionsLSR
regionsNicelyOrderedVR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["P","P3","P4","M"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"] ] + regionsLVR

allVRnoL = regionsNoL16VR + regionsNoL1718VR
allSRnoL = regionsNoL16SR + regionsNoL1718SR

allVR1718 = regionsNoL1718VR+regionsLVR
allVR16 = regionsNoL16VR+regionsLVR

allSR1718 = regionsNoL1718SR+regionsLSR
allSR16 = regionsNoL16SR+regionsLSR

allVR = allVR16 + allVR1718
allSR = allSR16 + allSR1718

allregions = allVR + allSR

rescale16 = 1/4.0 if not full_unblind else 1.0
rescale1718 = 1/5.0 if not full_unblind else 1.0

#for region in allVR:
#    print region, D16[region+" pre"],eD16[region+" pre"],sD16[region+" fs"],sD16[region+" nc"]

#print getAsymmetricErrors(0)
#print getAsymmetricErrors(1)
#print getAsymmetricErrors(2)
#print getAsymmetricErrors(3)
#print getAsymmetricErrors(20)

makePlotRaw(allVR1718,D1718,eD1718,sD1718,"2017-18 DATA VR")
makePlotRaw(allVR16,D16,eD16,sD16,"2016 DATA VR")
makePlotRaw(allSR1718,D1718,eD1718,sD1718,"2017-18 DATA SR",rescale1718)
makePlotRaw(allSR16,D16,eD16,sD16,"2016 DATA SR",rescale16)

makePlotRaw(allVR1718,M1718,eM1718,sM1718,"2017-18 MC VR")
makePlotRaw(allVR16,M16,eM16,sM16,"2016 MC VR")
makePlotRaw(allSR1718,M1718,eM1718,sM1718,"2017-18 MC SR")
makePlotRaw(allSR16,M16,eM16,sM16,"2016 MC SR")

makePlotDiscrepancies([allVR16,allVR1718],[D16,D1718],[eD16,eD1718],[sD16,sD1718],"All DATA VR")
makePlotDiscrepancies([allVR16,allVR1718],[M16,M1718],[eM16,eM1718],[sM16,sM1718],"All MC VR")

makePlotDiscrepancies([allSR16,allSR1718],[D16,D1718],[eD16,eD1718],[sD16,sD1718],"All DATA SR",rescale=[rescale16,rescale1718])
makePlotDiscrepancies([allSR16,allSR1718],[M16,M1718],[eM16,eM1718],[sM16,sM1718],"All MC SR")

for sp in signal_points:
    m1=sp[0]
    m2=sp[1]
    ct=sp[2]
    makeSignalPlot(allSR1718,D1718,eD1718,sD1718,[S1718[sp]],[eS1718[sp]],1+(58.83/41.97),"2017-18 ({}, {}) GeV, {} cm".format(m1,m2,ct),["({}, {}) GeV".format(m1,m2)],[ROOT.kRed],rescale1718)
    makeSignalPlot(allSR16,D16,eD16,sD16,[S1718[sp]],[eS1718[sp]],35.9/41.97,"2016 ({}, {}) GeV, {} cm".format(m1,m2,ct),["({}, {}) GeV".format(m1,m2)],[ROOT.kRed],rescale16)

list_of_vals_10 = [S1718[sp] for sp in signal_points_10]
list_of_errs_10 = [eS1718[sp] for sp in signal_points_10]
list_of_vals_90 = [S1718[sp] for sp in signal_points_90]
list_of_errs_90 = [eS1718[sp] for sp in signal_points_90]
list_of_tags = ["(1800, {}) GeV".format(m2) for m2 in [1400,1600,1700]]
colors = [ROOT.kMagenta,ROOT.kYellow+1,ROOT.kGreen+2]
makeSignalPlot(allSR1718,D1718,eD1718,sD1718,list_of_vals_10,list_of_errs_10,1+(58.83/41.97),"2017-18 10 cm",list_of_tags,colors,rescale1718)
makeSignalPlot(allSR1718,D1718,eD1718,sD1718,list_of_vals_90,list_of_errs_90,1+(58.83/41.97),"2017-18 90 cm",list_of_tags,colors,rescale1718)
makeSignalPlot(allSR16,D16,eD16,sD16,list_of_vals_10,list_of_errs_10,35.9/41.97,"2016 10 cm",list_of_tags,colors,rescale16)
makeSignalPlot(allSR16,D16,eD16,sD16,list_of_vals_90,list_of_errs_90,35.9/41.97,"2016 90 cm",list_of_tags,colors,rescale16)

fshort_regions_16 = [cat + " " + nj + " " + pt for cat in ["P","M"] for nj in ["23","4"] for pt in ["hi","lo"]]
fshort_regions_16 += [ "L 23", "L 4" ]

fshort_regions_1718 = [cat + " " + nj + " " + pt for cat in ["P3","P4","M"] for nj in ["23","4"] for pt in ["hi","lo"]]
fshort_regions_1718 += [ "L 23", "L 4" ]

makePlotFshort(fshort_regions_16,D16f,eD16f,sD16f,M16f,eM16f,sM16f,"2016")
makePlotFshort(fshort_regions_1718,D1718f,eD1718f,sD1718f,M1718f,eM1718f,sM1718f,"2017-18")

if printTables:
    output = open("{0}/regions_stc_data_{1}_VR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableDataSTC(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineDataSTC(region))
    printFooter(output)
    output.close()

    output = open("{0}/regions_stc_data_{1}_SR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableDataSTC(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineDataSTC(region,rescale16,rescale1718))
    printFooter(output)
    output.close()

    output = open("{0}/counts_data_{1}_SR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedCountsTable(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedCountsLine(region))
    printFooter(output)
    output.close()

    output = open("{0}/counts_data_2016_{1}_SR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedCountsTable(output)
    for region in allSR16:
        output.write(getMergedCountsLine(region,2016))
    printFooter(output)
    output.close()

    output = open("{0}/counts_data_2017and2018_{1}_SR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedCountsTable(output)
    for region in allSR1718:
        output.write(getMergedCountsLine(region,2017))
    printFooter(output)
    output.close()

    output = open("{0}/counts_data_{1}_VR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedCountsTable(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedCountsLine(region))
    printFooter(output)
    output.close()

    output = open("{0}/counts_data_2016_{1}_VR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedCountsTable(output)
    for region in allVR16:
        output.write(getMergedCountsLine(region,2016))
    printFooter(output)
    output.close()

    output = open("{0}/counts_data_2017and2018_{1}_VR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedCountsTable(output)
    for region in allVR1718:
        output.write(getMergedCountsLine(region,2017))
    printFooter(output)
    output.close()

    output = open("{0}/regions_data_{1}_VR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableData(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineData(region))
    printFooter(output)
    output.close()


    output = open("{0}/regions_data_{1}_SR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableData(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineData(region))
    printFooter(output)
    output.close()


if True:
    output = open("{0}/regions_mc_{1}_VR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableMC(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineMC(region))
    printFooter(output)
    output.close()

    output = open("{0}/regions_stc_mc_{1}_VR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableMCSTC(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineMCSTC(region))
    printFooter(output)
    output.close()

    output = open("{0}/regions_mc_{1}_SR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableMC(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineMC(region))
    printFooter(output)
    output.close()

    output = open("{0}/regions_stc_mc_{1}_SR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableMCSTC(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineMCSTC(region))
    printFooter(output)
    output.close()

fs_regions = [cat + " " + nj + " " + pt for cat in ["P","P3","P4","M"] for nj in ["23","4"] for pt in ["hi","lo"]]
fs_regions += ["L 23","L 4"]

output = open("{0}/fshorts_data_{1}.tex".format(tabledir,tag),"w")
printHeader(output)
startMergedFshortTableData(output)
for fs_region in fs_regions:
    output.write(getMergedFSLineData(fs_region))
printFooter(output)
output.close()

output = open("{0}/fshorts_mc_{1}.tex".format(tabledir,tag),"w")
printHeader(output)
startMergedFshortTableMC(output)
for fs_region in fs_regions:
    output.write(getMergedFSLineMC(fs_region))
printFooter(output)
output.close()


print "Done test; still need to fix makePlotSigErr and MRvsSR table"
exit(1)

output = open("{0}/MRvsSR_{1}.tex".format(tabledir,tag),"w")
printHeader(output)
startMRvsSRTable(output)
for region in regionsNicelyOrderedSR:
    output.write(getMRvsSRLine(region))
printFooter(output)
output.close()


limits_10 = [siglimits[sp] for sp in signal_points_10]
limits_90 = [siglimits[sp] for sp in signal_points_90]
makePlotSigErr(allVR1718,D1718,eD1718,sD1718,list_of_vals_10,list_of_tags,colors,limits_10,1+(58.83/41.97),"2017-18 VR 10 cm")
makePlotSigErr(allVR16,D16,eD16,sD16,list_of_vals_10,list_of_tags,colors,limits_10,35.9/41.97,"2016 VR 10 cm")
makePlotSigErr(allVR1718,D1718,eD1718,sD1718,list_of_vals_90,list_of_tags,colors,limits_90,1+(58.83/41.97),"2017-18 VR 90 cm")
makePlotSigErr(allVR16,D16,eD16,sD16,list_of_vals_90,list_of_tags,colors,limits_90,35.9/41.97,"2016 VR 90 cm")

makePlotSigErr(allSR1718,D1718,eD1718,sD1718,list_of_vals_10,list_of_tags,colors,limits_10,1+(58.83/41.97),"2017-18 SR 10 cm")
makePlotSigErr(allSR16,D16,eD16,sD16,list_of_vals_10,list_of_tags,colors,limits_10,35.9/41.97,"2016 SR 10 cm")
makePlotSigErr(allSR1718,D1718,eD1718,sD1718,list_of_vals_90,list_of_tags,colors,limits_90,1+(58.83/41.97),"2017-18 SR 90 cm")
makePlotSigErr(allSR16,D16,eD16,sD16,list_of_vals_90,list_of_tags,colors,limits_90,35.9/41.97,"2016 SR 90 cm")

print "Done"

# Deprecated functions

def getCountsOld(f):
    h_LL_MR_23 = f.Get("h_LL_MR_23")
    h_LM_MR_23 = f.Get("h_LM_MR_23")
    h_LLM_MR_23 = f.Get("h_LLM_MR_23")
    h_LH_MR_23 = f.Get("h_LH_MR_23")
    h_HL_MR_4 = f.Get("h_HL_MR_4")
    h_HLM_MR_4 = f.Get("h_HLM_MR_4")
    h_HM_MR_4 = f.Get("h_HM_MR_4")
    h_HH_MR_4 = f.Get("h_HH_MR_4")
    h_LL_MR_23_hi = f.Get("h_LL_MR_23_hi")
    h_LLM_MR_23_hi = f.Get("h_LLM_MR_23_hi")
    h_LM_MR_23_hi = f.Get("h_LM_MR_23_hi")
    h_LH_MR_23_hi = f.Get("h_LH_MR_23_hi")
    h_HL_MR_4_hi = f.Get("h_HL_MR_4_hi")
    h_HLM_MR_4_hi = f.Get("h_HLM_MR_4_hi")
    h_HM_MR_4_hi = f.Get("h_HM_MR_4_hi")
    h_HH_MR_4_hi = f.Get("h_HH_MR_4_hi")
    h_LL_MR_23_lo = f.Get("h_LL_MR_23_lo")
    h_LLM_MR_23_lo = f.Get("h_LLM_MR_23_lo")
    h_LM_MR_23_lo = f.Get("h_LM_MR_23_lo")
    h_LH_MR_23_lo = f.Get("h_LH_MR_23_lo")
    h_HL_MR_4_lo = f.Get("h_HL_MR_4_lo")
    h_HLM_MR_4_lo = f.Get("h_HLM_MR_4_lo")
    h_HM_MR_4_lo = f.Get("h_HM_MR_4_lo")
    h_HH_MR_4_lo = f.Get("h_HH_MR_4_lo")

    h_LL_VR_23 = f.Get("h_LL_VR_23")
    h_LM_VR_23 = f.Get("h_LM_VR_23")
    h_LLM_VR_23 = f.Get("h_LLM_VR_23")
    h_LH_VR_23 = f.Get("h_LH_VR_23")
    h_HL_VR_4 = f.Get("h_HL_VR_4")
    h_HLM_VR_4 = f.Get("h_HLM_VR_4")
    h_HM_VR_4 = f.Get("h_HM_VR_4")
    h_HH_VR_4 = f.Get("h_HH_VR_4")
    h_LL_VR_23_hi = f.Get("h_LL_VR_23_hi")
    h_LLM_VR_23_hi = f.Get("h_LLM_VR_23_hi")
    h_LM_VR_23_hi = f.Get("h_LM_VR_23_hi")
    h_LH_VR_23_hi = f.Get("h_LH_VR_23_hi")
    h_HL_VR_4_hi = f.Get("h_HL_VR_4_hi")
    h_HLM_VR_4_hi = f.Get("h_HLM_VR_4_hi")
    h_HM_VR_4_hi = f.Get("h_HM_VR_4_hi")
    h_HH_VR_4_hi = f.Get("h_HH_VR_4_hi")
    h_LL_VR_23_lo = f.Get("h_LL_VR_23_lo")
    h_LLM_VR_23_lo = f.Get("h_LLM_VR_23_lo")
    h_LM_VR_23_lo = f.Get("h_LM_VR_23_lo")
    h_LH_VR_23_lo = f.Get("h_LH_VR_23_lo")
    h_HL_VR_4_lo = f.Get("h_HL_VR_4_lo")
    h_HLM_VR_4_lo = f.Get("h_HLM_VR_4_lo")
    h_HM_VR_4_lo = f.Get("h_HM_VR_4_lo")
    h_HH_VR_4_lo = f.Get("h_HH_VR_4_lo")

    h_LL_SR_23 = f.Get("h_LL_SR_23")
    h_LLM_SR_23 = f.Get("h_LLM_SR_23")
    h_LM_SR_23 = f.Get("h_LM_SR_23")
    h_LH_SR_23 = f.Get("h_LH_SR_23")
    h_HL_SR_4 = f.Get("h_HL_SR_4")
    h_HLM_SR_4 = f.Get("h_HLM_SR_4")
    h_HM_SR_4 = f.Get("h_HM_SR_4")
    h_HH_SR_4 = f.Get("h_HH_SR_4")
    h_LL_SR_23_hi = f.Get("h_LL_SR_23_hi")
    h_LLM_SR_23_hi = f.Get("h_LLM_SR_23_hi")
    h_LM_SR_23_hi = f.Get("h_LM_SR_23_hi")
    h_LH_SR_23_hi = f.Get("h_LH_SR_23_hi")
    h_HL_SR_4_hi = f.Get("h_HL_SR_4_hi")
    h_HLM_SR_4_hi = f.Get("h_HLM_SR_4_hi")
    h_HM_SR_4_hi = f.Get("h_HM_SR_4_hi")
    h_HH_SR_4_hi = f.Get("h_HH_SR_4_hi")
    h_LL_SR_23_lo = f.Get("h_LL_SR_23_lo")
    h_LLM_SR_23_lo = f.Get("h_LLM_SR_23_lo")
    h_LM_SR_23_lo = f.Get("h_LM_SR_23_lo")
    h_LH_SR_23_lo = f.Get("h_LH_SR_23_lo")
    h_HL_SR_4_lo = f.Get("h_HL_SR_4_lo")
    h_HLM_SR_4_lo = f.Get("h_HLM_SR_4_lo")
    h_HM_SR_4_lo = f.Get("h_HM_SR_4_lo")
    h_HH_SR_4_lo = f.Get("h_HH_SR_4_lo")

    h_2STC_VR = f.Get("h_2STC_VR")
    h_2STC_SR = f.Get("h_2STC_SR")

    # Systs
    h_LL_VR_23_ns = f.Get("h_LL_VR_23_nonclosure_systematic")
    h_LM_VR_23_ns = f.Get("h_LM_VR_23_nonclosure_systematic")
    h_LLM_VR_23_ns = f.Get("h_LLM_VR_23_nonclosure_systematic")
    h_LH_VR_23_ns = f.Get("h_LH_VR_23_nonclosure_systematic")
    h_HL_VR_4_ns = f.Get("h_HL_VR_4_nonclosure_systematic")
    h_HLM_VR_4_ns = f.Get("h_HLM_VR_4_nonclosure_systematic")
    h_HM_VR_4_ns = f.Get("h_HM_VR_4_nonclosure_systematic")
    h_HH_VR_4_ns = f.Get("h_HH_VR_4_nonclosure_systematic")
    h_LL_VR_23_hi_ns = f.Get("h_LL_VR_23_hi_nonclosure_systematic")
    h_LLM_VR_23_hi_ns = f.Get("h_LLM_VR_23_hi_nonclosure_systematic")
    h_LM_VR_23_hi_ns = f.Get("h_LM_VR_23_hi_nonclosure_systematic")
    h_LH_VR_23_hi_ns = f.Get("h_LH_VR_23_hi_nonclosure_systematic")
    h_HL_VR_4_hi_ns = f.Get("h_HL_VR_4_hi_nonclosure_systematic")
    h_HLM_VR_4_hi_ns = f.Get("h_HLM_VR_4_hi_nonclosure_systematic")
    h_HM_VR_4_hi_ns = f.Get("h_HM_VR_4_hi_nonclosure_systematic")
    h_HH_VR_4_hi_ns = f.Get("h_HH_VR_4_hi_nonclosure_systematic")
    h_LL_VR_23_lo_ns = f.Get("h_LL_VR_23_lo_nonclosure_systematic")
    h_LLM_VR_23_lo_ns = f.Get("h_LLM_VR_23_lo_nonclosure_systematic")
    h_LM_VR_23_lo_ns = f.Get("h_LM_VR_23_lo_nonclosure_systematic")
    h_LH_VR_23_lo_ns = f.Get("h_LH_VR_23_lo_nonclosure_systematic")
    h_HL_VR_4_lo_ns = f.Get("h_HL_VR_4_lo_nonclosure_systematic")
    h_HLM_VR_4_lo_ns = f.Get("h_HLM_VR_4_lo_nonclosure_systematic")
    h_HM_VR_4_lo_ns = f.Get("h_HM_VR_4_lo_nonclosure_systematic")
    h_HH_VR_4_lo_ns = f.Get("h_HH_VR_4_lo_nonclosure_systematic")
    # VR hists are used for SR as well

    h_2STC_VR_ns = f.Get("h_2STC_VR_nonclosure_systematic")


    vals = {}
    errs = {}
    systs = {}

    # 2STC
    if h_2STC_VR != None:
        vals["2 VR STC"] = h_2STC_VR.GetBinContent(1,3)
        vals["2 VR pre"] = h_2STC_VR.GetBinContent(1,2)
        vals["2 VR obs"] = h_2STC_VR.GetBinContent(1,1)
        vals["2 SR STC"] = h_2STC_SR.GetBinContent(1,3)
        vals["2 SR pre"] = h_2STC_SR.GetBinContent(1,2)
        vals["2 SR obs"] = h_2STC_SR.GetBinContent(1,1)

        errs["2 VR STC"] = h_2STC_VR.GetBinError(1,3)
        errs["2 VR pre"] = h_2STC_VR.GetBinError(1,2)
        errs["2 VR obs"] = h_2STC_VR.GetBinError(1,1)
        errs["2 SR STC"] = h_2STC_SR.GetBinError(1,3)
        errs["2 SR pre"] = h_2STC_SR.GetBinError(1,2)
        errs["2 SR obs"] = h_2STC_SR.GetBinError(1,1)


    # MR vals
    vals["P LL MR STC"] = h_LL_MR_23.GetBinContent(1,3)
    vals["P LL MR pre"] = h_LL_MR_23.GetBinContent(1,2)
    vals["P LL MR obs"] = h_LL_MR_23.GetBinContent(1,1)
    vals["P3 LL MR STC"] = h_LL_MR_23.GetBinContent(2,3)
    vals["P3 LL MR pre"] = h_LL_MR_23.GetBinContent(2,2)
    vals["P3 LL MR obs"] = h_LL_MR_23.GetBinContent(2,1)
    vals["P4 LL MR STC"] = h_LL_MR_23.GetBinContent(3,3)
    vals["P4 LL MR pre"] = h_LL_MR_23.GetBinContent(3,2)
    vals["P4 LL MR obs"] = h_LL_MR_23.GetBinContent(3,1)
    vals["M LL MR STC"] = h_LL_MR_23.GetBinContent(4,3)
    vals["M LL MR pre"] = h_LL_MR_23.GetBinContent(4,2)
    vals["M LL MR obs"] = h_LL_MR_23.GetBinContent(4,1)
    vals["L LL MR STC"] = h_LL_MR_23.GetBinContent(5,3)
    vals["L LL MR pre"] = h_LL_MR_23.GetBinContent(5,2)
    vals["L LL MR obs"] = h_LL_MR_23.GetBinContent(5,1)

    vals["P LLM MR STC"] = h_LLM_MR_23.GetBinContent(1,3)
    vals["P LLM MR pre"] = h_LLM_MR_23.GetBinContent(1,2)
    vals["P LLM MR obs"] = h_LLM_MR_23.GetBinContent(1,1)
    vals["P3 LLM MR STC"] = h_LLM_MR_23.GetBinContent(2,3)
    vals["P3 LLM MR pre"] = h_LLM_MR_23.GetBinContent(2,2)
    vals["P3 LLM MR obs"] = h_LLM_MR_23.GetBinContent(2,1)
    vals["P4 LLM MR STC"] = h_LLM_MR_23.GetBinContent(3,3)
    vals["P4 LLM MR pre"] = h_LLM_MR_23.GetBinContent(3,2)
    vals["P4 LLM MR obs"] = h_LLM_MR_23.GetBinContent(3,1)
    vals["M LLM MR STC"] = h_LLM_MR_23.GetBinContent(4,3)
    vals["M LLM MR pre"] = h_LLM_MR_23.GetBinContent(4,2)
    vals["M LLM MR obs"] = h_LLM_MR_23.GetBinContent(4,1)
    vals["L LLM MR STC"] = h_LLM_MR_23.GetBinContent(5,3)
    vals["L LLM MR pre"] = h_LLM_MR_23.GetBinContent(5,2)
    vals["L LLM MR obs"] = h_LLM_MR_23.GetBinContent(5,1)

    vals["P LM MR STC"] = h_LM_MR_23.GetBinContent(1,3)
    vals["P LM MR pre"] = h_LM_MR_23.GetBinContent(1,2)
    vals["P LM MR obs"] = h_LM_MR_23.GetBinContent(1,1)
    vals["P3 LM MR STC"] = h_LM_MR_23.GetBinContent(2,3)
    vals["P3 LM MR pre"] = h_LM_MR_23.GetBinContent(2,2)
    vals["P3 LM MR obs"] = h_LM_MR_23.GetBinContent(2,1)
    vals["P4 LM MR STC"] = h_LM_MR_23.GetBinContent(3,3)
    vals["P4 LM MR pre"] = h_LM_MR_23.GetBinContent(3,2)
    vals["P4 LM MR obs"] = h_LM_MR_23.GetBinContent(3,1)
    vals["M LM MR STC"] = h_LM_MR_23.GetBinContent(4,3)
    vals["M LM MR pre"] = h_LM_MR_23.GetBinContent(4,2)
    vals["M LM MR obs"] = h_LM_MR_23.GetBinContent(4,1)
    vals["L LM MR STC"] = h_LM_MR_23.GetBinContent(5,3)
    vals["L LM MR pre"] = h_LM_MR_23.GetBinContent(5,2)
    vals["L LM MR obs"] = h_LM_MR_23.GetBinContent(5,1)

    vals["P LH MR STC"] = h_LH_MR_23.GetBinContent(1,3)
    vals["P LH MR pre"] = h_LH_MR_23.GetBinContent(1,2)
    vals["P LH MR obs"] = h_LH_MR_23.GetBinContent(1,1)
    vals["P3 LH MR STC"] = h_LH_MR_23.GetBinContent(2,3)
    vals["P3 LH MR pre"] = h_LH_MR_23.GetBinContent(2,2)
    vals["P3 LH MR obs"] = h_LH_MR_23.GetBinContent(2,1)
    vals["P4 LH MR STC"] = h_LH_MR_23.GetBinContent(3,3)
    vals["P4 LH MR pre"] = h_LH_MR_23.GetBinContent(3,2)
    vals["P4 LH MR obs"] = h_LH_MR_23.GetBinContent(3,1)
    vals["M LH MR STC"] = h_LH_MR_23.GetBinContent(4,3)
    vals["M LH MR pre"] = h_LH_MR_23.GetBinContent(4,2)
    vals["M LH MR obs"] = h_LH_MR_23.GetBinContent(4,1)
    vals["L LH MR STC"] = h_LH_MR_23.GetBinContent(5,3)
    vals["L LH MR pre"] = h_LH_MR_23.GetBinContent(5,2)
    vals["L LH MR obs"] = h_LH_MR_23.GetBinContent(5,1)

    vals["P HL MR STC"] = h_HL_MR_4.GetBinContent(1,3)
    vals["P HL MR pre"] = h_HL_MR_4.GetBinContent(1,2)
    vals["P HL MR obs"] = h_HL_MR_4.GetBinContent(1,1)
    vals["P3 HL MR STC"] = h_HL_MR_4.GetBinContent(2,3)
    vals["P3 HL MR pre"] = h_HL_MR_4.GetBinContent(2,2)
    vals["P3 HL MR obs"] = h_HL_MR_4.GetBinContent(2,1)
    vals["P4 HL MR STC"] = h_HL_MR_4.GetBinContent(3,3)
    vals["P4 HL MR pre"] = h_HL_MR_4.GetBinContent(3,2)
    vals["P4 HL MR obs"] = h_HL_MR_4.GetBinContent(3,1)
    vals["M HL MR STC"] = h_HL_MR_4.GetBinContent(4,3)
    vals["M HL MR pre"] = h_HL_MR_4.GetBinContent(4,2)
    vals["M HL MR obs"] = h_HL_MR_4.GetBinContent(4,1)
    vals["L HL MR STC"] = h_HL_MR_4.GetBinContent(5,3)
    vals["L HL MR pre"] = h_HL_MR_4.GetBinContent(5,2)
    vals["L HL MR obs"] = h_HL_MR_4.GetBinContent(5,1)

    vals["P HLM MR STC"] = h_HLM_MR_4.GetBinContent(1,3)
    vals["P HLM MR pre"] = h_HLM_MR_4.GetBinContent(1,2)
    vals["P HLM MR obs"] = h_HLM_MR_4.GetBinContent(1,1)
    vals["P3 HLM MR STC"] = h_HLM_MR_4.GetBinContent(2,3)
    vals["P3 HLM MR pre"] = h_HLM_MR_4.GetBinContent(2,2)
    vals["P3 HLM MR obs"] = h_HLM_MR_4.GetBinContent(2,1)
    vals["P4 HLM MR STC"] = h_HLM_MR_4.GetBinContent(3,3)
    vals["P4 HLM MR pre"] = h_HLM_MR_4.GetBinContent(3,2)
    vals["P4 HLM MR obs"] = h_HLM_MR_4.GetBinContent(3,1)
    vals["M HLM MR STC"] = h_HLM_MR_4.GetBinContent(4,3)
    vals["M HLM MR pre"] = h_HLM_MR_4.GetBinContent(4,2)
    vals["M HLM MR obs"] = h_HLM_MR_4.GetBinContent(4,1)
    vals["L HLM MR STC"] = h_HLM_MR_4.GetBinContent(5,3)
    vals["L HLM MR pre"] = h_HLM_MR_4.GetBinContent(5,2)
    vals["L HLM MR obs"] = h_HLM_MR_4.GetBinContent(5,1)

    vals["P HM MR STC"] = h_HM_MR_4.GetBinContent(1,3)
    vals["P HM MR pre"] = h_HM_MR_4.GetBinContent(1,2)
    vals["P HM MR obs"] = h_HM_MR_4.GetBinContent(1,1)
    vals["P3 HM MR STC"] = h_HM_MR_4.GetBinContent(2,3)
    vals["P3 HM MR pre"] = h_HM_MR_4.GetBinContent(2,2)
    vals["P3 HM MR obs"] = h_HM_MR_4.GetBinContent(2,1)
    vals["P4 HM MR STC"] = h_HM_MR_4.GetBinContent(3,3)
    vals["P4 HM MR pre"] = h_HM_MR_4.GetBinContent(3,2)
    vals["P4 HM MR obs"] = h_HM_MR_4.GetBinContent(3,1)
    vals["M HM MR STC"] = h_HM_MR_4.GetBinContent(4,3)
    vals["M HM MR pre"] = h_HM_MR_4.GetBinContent(4,2)
    vals["M HM MR obs"] = h_HM_MR_4.GetBinContent(4,1)
    vals["L HM MR STC"] = h_HM_MR_4.GetBinContent(5,3)
    vals["L HM MR pre"] = h_HM_MR_4.GetBinContent(5,2)
    vals["L HM MR obs"] = h_HM_MR_4.GetBinContent(5,1)

    vals["P HH MR STC"] = h_HH_MR_4.GetBinContent(1,3)
    vals["P HH MR pre"] = h_HH_MR_4.GetBinContent(1,2)
    vals["P HH MR obs"] = h_HH_MR_4.GetBinContent(1,1)
    vals["P3 HH MR STC"] = h_HH_MR_4.GetBinContent(2,3)
    vals["P3 HH MR pre"] = h_HH_MR_4.GetBinContent(2,2)
    vals["P3 HH MR obs"] = h_HH_MR_4.GetBinContent(2,1)
    vals["P4 HH MR STC"] = h_HH_MR_4.GetBinContent(3,3)
    vals["P4 HH MR pre"] = h_HH_MR_4.GetBinContent(3,2)
    vals["P4 HH MR obs"] = h_HH_MR_4.GetBinContent(3,1)
    vals["M HH MR STC"] = h_HH_MR_4.GetBinContent(4,3)
    vals["M HH MR pre"] = h_HH_MR_4.GetBinContent(4,2)
    vals["M HH MR obs"] = h_HH_MR_4.GetBinContent(4,1)
    vals["L HH MR STC"] = h_HH_MR_4.GetBinContent(5,3)
    vals["L HH MR pre"] = h_HH_MR_4.GetBinContent(5,2)
    vals["L HH MR obs"] = h_HH_MR_4.GetBinContent(5,1)

    # hi pt
    vals["P LL MR hi STC"] = h_LL_MR_23_hi.GetBinContent(1,3)
    vals["P LL MR hi pre"] = h_LL_MR_23_hi.GetBinContent(1,2)
    vals["P LL MR hi obs"] = h_LL_MR_23_hi.GetBinContent(1,1)
    vals["P3 LL MR hi STC"] = h_LL_MR_23_hi.GetBinContent(2,3)
    vals["P3 LL MR hi pre"] = h_LL_MR_23_hi.GetBinContent(2,2)
    vals["P3 LL MR hi obs"] = h_LL_MR_23_hi.GetBinContent(2,1)
    vals["P4 LL MR hi STC"] = h_LL_MR_23_hi.GetBinContent(3,3)
    vals["P4 LL MR hi pre"] = h_LL_MR_23_hi.GetBinContent(3,2)
    vals["P4 LL MR hi obs"] = h_LL_MR_23_hi.GetBinContent(3,1)
    vals["M LL MR hi STC"] = h_LL_MR_23_hi.GetBinContent(4,3)
    vals["M LL MR hi pre"] = h_LL_MR_23_hi.GetBinContent(4,2)
    vals["M LL MR hi obs"] = h_LL_MR_23_hi.GetBinContent(4,1)
    vals["L LL MR hi STC"] = h_LL_MR_23_hi.GetBinContent(5,3)
    vals["L LL MR hi pre"] = h_LL_MR_23_hi.GetBinContent(5,2)
    vals["L LL MR hi obs"] = h_LL_MR_23_hi.GetBinContent(5,1)

    vals["P LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinContent(1,3)
    vals["P LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinContent(1,2)
    vals["P LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinContent(1,1)
    vals["P3 LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinContent(2,3)
    vals["P3 LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinContent(2,2)
    vals["P3 LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinContent(2,1)
    vals["P4 LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinContent(3,3)
    vals["P4 LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinContent(3,2)
    vals["P4 LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinContent(3,1)
    vals["M LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinContent(4,3)
    vals["M LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinContent(4,2)
    vals["M LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinContent(4,1)
    vals["L LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinContent(5,3)
    vals["L LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinContent(5,2)
    vals["L LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinContent(5,1)

    vals["P LM MR hi STC"] = h_LM_MR_23_hi.GetBinContent(1,3)
    vals["P LM MR hi pre"] = h_LM_MR_23_hi.GetBinContent(1,2)
    vals["P LM MR hi obs"] = h_LM_MR_23_hi.GetBinContent(1,1)
    vals["P3 LM MR hi STC"] = h_LM_MR_23_hi.GetBinContent(2,3)
    vals["P3 LM MR hi pre"] = h_LM_MR_23_hi.GetBinContent(2,2)
    vals["P3 LM MR hi obs"] = h_LM_MR_23_hi.GetBinContent(2,1)
    vals["P4 LM MR hi STC"] = h_LM_MR_23_hi.GetBinContent(3,3)
    vals["P4 LM MR hi pre"] = h_LM_MR_23_hi.GetBinContent(3,2)
    vals["P4 LM MR hi obs"] = h_LM_MR_23_hi.GetBinContent(3,1)
    vals["M LM MR hi STC"] = h_LM_MR_23_hi.GetBinContent(4,3)
    vals["M LM MR hi pre"] = h_LM_MR_23_hi.GetBinContent(4,2)
    vals["M LM MR hi obs"] = h_LM_MR_23_hi.GetBinContent(4,1)
    vals["L LM MR hi STC"] = h_LM_MR_23_hi.GetBinContent(5,3)
    vals["L LM MR hi pre"] = h_LM_MR_23_hi.GetBinContent(5,2)
    vals["L LM MR hi obs"] = h_LM_MR_23_hi.GetBinContent(5,1)

    vals["P LH MR hi STC"] = h_LH_MR_23_hi.GetBinContent(1,3)
    vals["P LH MR hi pre"] = h_LH_MR_23_hi.GetBinContent(1,2)
    vals["P LH MR hi obs"] = h_LH_MR_23_hi.GetBinContent(1,1)
    vals["P3 LH MR hi STC"] = h_LH_MR_23_hi.GetBinContent(2,3)
    vals["P3 LH MR hi pre"] = h_LH_MR_23_hi.GetBinContent(2,2)
    vals["P3 LH MR hi obs"] = h_LH_MR_23_hi.GetBinContent(2,1)
    vals["P4 LH MR hi STC"] = h_LH_MR_23_hi.GetBinContent(3,3)
    vals["P4 LH MR hi pre"] = h_LH_MR_23_hi.GetBinContent(3,2)
    vals["P4 LH MR hi obs"] = h_LH_MR_23_hi.GetBinContent(3,1)
    vals["M LH MR hi STC"] = h_LH_MR_23_hi.GetBinContent(4,3)
    vals["M LH MR hi pre"] = h_LH_MR_23_hi.GetBinContent(4,2)
    vals["M LH MR hi obs"] = h_LH_MR_23_hi.GetBinContent(4,1)
    vals["L LH MR hi STC"] = h_LH_MR_23_hi.GetBinContent(5,3)
    vals["L LH MR hi pre"] = h_LH_MR_23_hi.GetBinContent(5,2)
    vals["L LH MR hi obs"] = h_LH_MR_23_hi.GetBinContent(5,1)

    vals["P HL MR hi STC"] = h_HL_MR_4_hi.GetBinContent(1,3)
    vals["P HL MR hi pre"] = h_HL_MR_4_hi.GetBinContent(1,2)
    vals["P HL MR hi obs"] = h_HL_MR_4_hi.GetBinContent(1,1)
    vals["P3 HL MR hi STC"] = h_HL_MR_4_hi.GetBinContent(2,3)
    vals["P3 HL MR hi pre"] = h_HL_MR_4_hi.GetBinContent(2,2)
    vals["P3 HL MR hi obs"] = h_HL_MR_4_hi.GetBinContent(2,1)
    vals["P4 HL MR hi STC"] = h_HL_MR_4_hi.GetBinContent(3,3)
    vals["P4 HL MR hi pre"] = h_HL_MR_4_hi.GetBinContent(3,2)
    vals["P4 HL MR hi obs"] = h_HL_MR_4_hi.GetBinContent(3,1)
    vals["M HL MR hi STC"] = h_HL_MR_4_hi.GetBinContent(4,3)
    vals["M HL MR hi pre"] = h_HL_MR_4_hi.GetBinContent(4,2)
    vals["M HL MR hi obs"] = h_HL_MR_4_hi.GetBinContent(4,1)
    vals["L HL MR hi STC"] = h_HL_MR_4_hi.GetBinContent(5,3)
    vals["L HL MR hi pre"] = h_HL_MR_4_hi.GetBinContent(5,2)
    vals["L HL MR hi obs"] = h_HL_MR_4_hi.GetBinContent(5,1)

    vals["P HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinContent(1,3)
    vals["P HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinContent(1,2)
    vals["P HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinContent(1,1)
    vals["P3 HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinContent(2,3)
    vals["P3 HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinContent(2,2)
    vals["P3 HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinContent(2,1)
    vals["P4 HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinContent(3,3)
    vals["P4 HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinContent(3,2)
    vals["P4 HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinContent(3,1)
    vals["M HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinContent(4,3)
    vals["M HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinContent(4,2)
    vals["M HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinContent(4,1)
    vals["L HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinContent(5,3)
    vals["L HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinContent(5,2)
    vals["L HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinContent(5,1)

    vals["P HM MR hi STC"] = h_HM_MR_4_hi.GetBinContent(1,3)
    vals["P HM MR hi pre"] = h_HM_MR_4_hi.GetBinContent(1,2)
    vals["P HM MR hi obs"] = h_HM_MR_4_hi.GetBinContent(1,1)
    vals["P3 HM MR hi STC"] = h_HM_MR_4_hi.GetBinContent(2,3)
    vals["P3 HM MR hi pre"] = h_HM_MR_4_hi.GetBinContent(2,2)
    vals["P3 HM MR hi obs"] = h_HM_MR_4_hi.GetBinContent(2,1)
    vals["P4 HM MR hi STC"] = h_HM_MR_4_hi.GetBinContent(3,3)
    vals["P4 HM MR hi pre"] = h_HM_MR_4_hi.GetBinContent(3,2)
    vals["P4 HM MR hi obs"] = h_HM_MR_4_hi.GetBinContent(3,1)
    vals["M HM MR hi STC"] = h_HM_MR_4_hi.GetBinContent(4,3)
    vals["M HM MR hi pre"] = h_HM_MR_4_hi.GetBinContent(4,2)
    vals["M HM MR hi obs"] = h_HM_MR_4_hi.GetBinContent(4,1)
    vals["L HM MR hi STC"] = h_HM_MR_4_hi.GetBinContent(5,3)
    vals["L HM MR hi pre"] = h_HM_MR_4_hi.GetBinContent(5,2)
    vals["L HM MR hi obs"] = h_HM_MR_4_hi.GetBinContent(5,1)

    vals["P HH MR hi STC"] = h_HH_MR_4_hi.GetBinContent(1,3)
    vals["P HH MR hi pre"] = h_HH_MR_4_hi.GetBinContent(1,2)
    vals["P HH MR hi obs"] = h_HH_MR_4_hi.GetBinContent(1,1)
    vals["P3 HH MR hi STC"] = h_HH_MR_4_hi.GetBinContent(2,3)
    vals["P3 HH MR hi pre"] = h_HH_MR_4_hi.GetBinContent(2,2)
    vals["P3 HH MR hi obs"] = h_HH_MR_4_hi.GetBinContent(2,1)
    vals["P4 HH MR hi STC"] = h_HH_MR_4_hi.GetBinContent(3,3)
    vals["P4 HH MR hi pre"] = h_HH_MR_4_hi.GetBinContent(3,2)
    vals["P4 HH MR hi obs"] = h_HH_MR_4_hi.GetBinContent(3,1)
    vals["M HH MR hi STC"] = h_HH_MR_4_hi.GetBinContent(4,3)
    vals["M HH MR hi pre"] = h_HH_MR_4_hi.GetBinContent(4,2)
    vals["M HH MR hi obs"] = h_HH_MR_4_hi.GetBinContent(4,1)
    vals["L HH MR hi STC"] = h_HH_MR_4_hi.GetBinContent(5,3)
    vals["L HH MR hi pre"] = h_HH_MR_4_hi.GetBinContent(5,2)
    vals["L HH MR hi obs"] = h_HH_MR_4_hi.GetBinContent(5,1)

    # lo pt
    vals["P LL MR lo STC"] = h_LL_MR_23_lo.GetBinContent(1,3)
    vals["P LL MR lo pre"] = h_LL_MR_23_lo.GetBinContent(1,2)
    vals["P LL MR lo obs"] = h_LL_MR_23_lo.GetBinContent(1,1)
    vals["P3 LL MR lo STC"] = h_LL_MR_23_lo.GetBinContent(2,3)
    vals["P3 LL MR lo pre"] = h_LL_MR_23_lo.GetBinContent(2,2)
    vals["P3 LL MR lo obs"] = h_LL_MR_23_lo.GetBinContent(2,1)
    vals["P4 LL MR lo STC"] = h_LL_MR_23_lo.GetBinContent(3,3)
    vals["P4 LL MR lo pre"] = h_LL_MR_23_lo.GetBinContent(3,2)
    vals["P4 LL MR lo obs"] = h_LL_MR_23_lo.GetBinContent(3,1)
    vals["M LL MR lo STC"] = h_LL_MR_23_lo.GetBinContent(4,3)
    vals["M LL MR lo pre"] = h_LL_MR_23_lo.GetBinContent(4,2)
    vals["M LL MR lo obs"] = h_LL_MR_23_lo.GetBinContent(4,1)
    vals["L LL MR lo STC"] = h_LL_MR_23_lo.GetBinContent(5,3)
    vals["L LL MR lo pre"] = h_LL_MR_23_lo.GetBinContent(5,2)
    vals["L LL MR lo obs"] = h_LL_MR_23_lo.GetBinContent(5,1)

    vals["P LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinContent(1,3)
    vals["P LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinContent(1,2)
    vals["P LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinContent(1,1)
    vals["P3 LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinContent(2,3)
    vals["P3 LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinContent(2,2)
    vals["P3 LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinContent(2,1)
    vals["P4 LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinContent(3,3)
    vals["P4 LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinContent(3,2)
    vals["P4 LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinContent(3,1)
    vals["M LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinContent(4,3)
    vals["M LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinContent(4,2)
    vals["M LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinContent(4,1)
    vals["L LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinContent(5,3)
    vals["L LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinContent(5,2)
    vals["L LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinContent(5,1)

    vals["P LM MR lo STC"] = h_LM_MR_23_lo.GetBinContent(1,3)
    vals["P LM MR lo pre"] = h_LM_MR_23_lo.GetBinContent(1,2)
    vals["P LM MR lo obs"] = h_LM_MR_23_lo.GetBinContent(1,1)
    vals["P3 LM MR lo STC"] = h_LM_MR_23_lo.GetBinContent(2,3)
    vals["P3 LM MR lo pre"] = h_LM_MR_23_lo.GetBinContent(2,2)
    vals["P3 LM MR lo obs"] = h_LM_MR_23_lo.GetBinContent(2,1)
    vals["P4 LM MR lo STC"] = h_LM_MR_23_lo.GetBinContent(3,3)
    vals["P4 LM MR lo pre"] = h_LM_MR_23_lo.GetBinContent(3,2)
    vals["P4 LM MR lo obs"] = h_LM_MR_23_lo.GetBinContent(3,1)
    vals["M LM MR lo STC"] = h_LM_MR_23_lo.GetBinContent(4,3)
    vals["M LM MR lo pre"] = h_LM_MR_23_lo.GetBinContent(4,2)
    vals["M LM MR lo obs"] = h_LM_MR_23_lo.GetBinContent(4,1)
    vals["L LM MR lo STC"] = h_LM_MR_23_lo.GetBinContent(5,3)
    vals["L LM MR lo pre"] = h_LM_MR_23_lo.GetBinContent(5,2)
    vals["L LM MR lo obs"] = h_LM_MR_23_lo.GetBinContent(5,1)

    vals["P LH MR lo STC"] = h_LH_MR_23_lo.GetBinContent(1,3)
    vals["P LH MR lo pre"] = h_LH_MR_23_lo.GetBinContent(1,2)
    vals["P LH MR lo obs"] = h_LH_MR_23_lo.GetBinContent(1,1)
    vals["P3 LH MR lo STC"] = h_LH_MR_23_lo.GetBinContent(2,3)
    vals["P3 LH MR lo pre"] = h_LH_MR_23_lo.GetBinContent(2,2)
    vals["P3 LH MR lo obs"] = h_LH_MR_23_lo.GetBinContent(2,1)
    vals["P4 LH MR lo STC"] = h_LH_MR_23_lo.GetBinContent(3,3)
    vals["P4 LH MR lo pre"] = h_LH_MR_23_lo.GetBinContent(3,2)
    vals["P4 LH MR lo obs"] = h_LH_MR_23_lo.GetBinContent(3,1)
    vals["M LH MR lo STC"] = h_LH_MR_23_lo.GetBinContent(4,3)
    vals["M LH MR lo pre"] = h_LH_MR_23_lo.GetBinContent(4,2)
    vals["M LH MR lo obs"] = h_LH_MR_23_lo.GetBinContent(4,1)
    vals["L LH MR lo STC"] = h_LH_MR_23_lo.GetBinContent(5,3)
    vals["L LH MR lo pre"] = h_LH_MR_23_lo.GetBinContent(5,2)
    vals["L LH MR lo obs"] = h_LH_MR_23_lo.GetBinContent(5,1)

    vals["P HL MR lo STC"] = h_HL_MR_4_lo.GetBinContent(1,3)
    vals["P HL MR lo pre"] = h_HL_MR_4_lo.GetBinContent(1,2)
    vals["P HL MR lo obs"] = h_HL_MR_4_lo.GetBinContent(1,1)
    vals["P3 HL MR lo STC"] = h_HL_MR_4_lo.GetBinContent(2,3)
    vals["P3 HL MR lo pre"] = h_HL_MR_4_lo.GetBinContent(2,2)
    vals["P3 HL MR lo obs"] = h_HL_MR_4_lo.GetBinContent(2,1)
    vals["P4 HL MR lo STC"] = h_HL_MR_4_lo.GetBinContent(3,3)
    vals["P4 HL MR lo pre"] = h_HL_MR_4_lo.GetBinContent(3,2)
    vals["P4 HL MR lo obs"] = h_HL_MR_4_lo.GetBinContent(3,1)
    vals["M HL MR lo STC"] = h_HL_MR_4_lo.GetBinContent(4,3)
    vals["M HL MR lo pre"] = h_HL_MR_4_lo.GetBinContent(4,2)
    vals["M HL MR lo obs"] = h_HL_MR_4_lo.GetBinContent(4,1)
    vals["L HL MR lo STC"] = h_HL_MR_4_lo.GetBinContent(5,3)
    vals["L HL MR lo pre"] = h_HL_MR_4_lo.GetBinContent(5,2)
    vals["L HL MR lo obs"] = h_HL_MR_4_lo.GetBinContent(5,1)

    vals["P HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinContent(1,3)
    vals["P HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinContent(1,2)
    vals["P HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinContent(1,1)
    vals["P3 HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinContent(2,3)
    vals["P3 HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinContent(2,2)
    vals["P3 HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinContent(2,1)
    vals["P4 HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinContent(3,3)
    vals["P4 HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinContent(3,2)
    vals["P4 HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinContent(3,1)
    vals["M HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinContent(4,3)
    vals["M HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinContent(4,2)
    vals["M HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinContent(4,1)
    vals["L HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinContent(5,3)
    vals["L HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinContent(5,2)
    vals["L HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinContent(5,1)

    vals["P HM MR lo STC"] = h_HM_MR_4_lo.GetBinContent(1,3)
    vals["P HM MR lo pre"] = h_HM_MR_4_lo.GetBinContent(1,2)
    vals["P HM MR lo obs"] = h_HM_MR_4_lo.GetBinContent(1,1)
    vals["P3 HM MR lo STC"] = h_HM_MR_4_lo.GetBinContent(2,3)
    vals["P3 HM MR lo pre"] = h_HM_MR_4_lo.GetBinContent(2,2)
    vals["P3 HM MR lo obs"] = h_HM_MR_4_lo.GetBinContent(2,1)
    vals["P4 HM MR lo STC"] = h_HM_MR_4_lo.GetBinContent(3,3)
    vals["P4 HM MR lo pre"] = h_HM_MR_4_lo.GetBinContent(3,2)
    vals["P4 HM MR lo obs"] = h_HM_MR_4_lo.GetBinContent(3,1)
    vals["M HM MR lo STC"] = h_HM_MR_4_lo.GetBinContent(4,3)
    vals["M HM MR lo pre"] = h_HM_MR_4_lo.GetBinContent(4,2)
    vals["M HM MR lo obs"] = h_HM_MR_4_lo.GetBinContent(4,1)
    vals["L HM MR lo STC"] = h_HM_MR_4_lo.GetBinContent(5,3)
    vals["L HM MR lo pre"] = h_HM_MR_4_lo.GetBinContent(5,2)
    vals["L HM MR lo obs"] = h_HM_MR_4_lo.GetBinContent(5,1)

    vals["P HH MR lo STC"] = h_HH_MR_4_lo.GetBinContent(1,3)
    vals["P HH MR lo pre"] = h_HH_MR_4_lo.GetBinContent(1,2)
    vals["P HH MR lo obs"] = h_HH_MR_4_lo.GetBinContent(1,1)
    vals["P3 HH MR lo STC"] = h_HH_MR_4_lo.GetBinContent(2,3)
    vals["P3 HH MR lo pre"] = h_HH_MR_4_lo.GetBinContent(2,2)
    vals["P3 HH MR lo obs"] = h_HH_MR_4_lo.GetBinContent(2,1)
    vals["P4 HH MR lo STC"] = h_HH_MR_4_lo.GetBinContent(3,3)
    vals["P4 HH MR lo pre"] = h_HH_MR_4_lo.GetBinContent(3,2)
    vals["P4 HH MR lo obs"] = h_HH_MR_4_lo.GetBinContent(3,1)
    vals["M HH MR lo STC"] = h_HH_MR_4_lo.GetBinContent(4,3)
    vals["M HH MR lo pre"] = h_HH_MR_4_lo.GetBinContent(4,2)
    vals["M HH MR lo obs"] = h_HH_MR_4_lo.GetBinContent(4,1)
    vals["L HH MR lo STC"] = h_HH_MR_4_lo.GetBinContent(5,3)
    vals["L HH MR lo pre"] = h_HH_MR_4_lo.GetBinContent(5,2)
    vals["L HH MR lo obs"] = h_HH_MR_4_lo.GetBinContent(5,1)

    # MR Errors
    errs["P LL MR STC"] = h_LL_MR_23.GetBinError(1,3)
    errs["P LL MR pre"] = h_LL_MR_23.GetBinError(1,2)
    errs["P LL MR obs"] = getAsymmetricErrors(vals["P LL MR obs"])
    errs["P3 LL MR STC"] = h_LL_MR_23.GetBinError(2,3)
    errs["P3 LL MR pre"] = h_LL_MR_23.GetBinError(2,2)
    errs["P3 LL MR obs"] = getAsymmetricErrors(vals["P3 LL MR obs"])
    errs["P4 LL MR STC"] = h_LL_MR_23.GetBinError(3,3)
    errs["P4 LL MR pre"] = h_LL_MR_23.GetBinError(3,2)
    errs["P4 LL MR obs"] = getAsymmetricErrors(vals["P4 LL MR obs"])
    errs["M LL MR STC"] = h_LL_MR_23.GetBinError(4,3)
    errs["M LL MR pre"] = h_LL_MR_23.GetBinError(4,2)
    errs["M LL MR obs"] = getAsymmetricErrors(vals["M LL MR obs"])
    errs["L LL MR STC"] = h_LL_MR_23.GetBinError(5,3)
    errs["L LL MR pre"] = h_LL_MR_23.GetBinError(5,2)
    errs["L LL MR obs"] = getAsymmetricErrors(vals["M LL MR obs"])

    errs["P LLM MR STC"] = h_LLM_MR_23.GetBinError(1,3)
    errs["P LLM MR pre"] = h_LLM_MR_23.GetBinError(1,2)
    errs["P LLM MR obs"] = getAsymmetricErrors(vals["P LLM MR obs"])
    errs["P3 LLM MR STC"] = h_LLM_MR_23.GetBinError(2,3)
    errs["P3 LLM MR pre"] = h_LLM_MR_23.GetBinError(2,2)
    errs["P3 LLM MR obs"] = getAsymmetricErrors(vals["P3 LLM MR obs"])
    errs["P4 LLM MR STC"] = h_LLM_MR_23.GetBinError(3,3)
    errs["P4 LLM MR pre"] = h_LLM_MR_23.GetBinError(3,2)
    errs["P4 LLM MR obs"] = getAsymmetricErrors(vals["P4 LLM MR obs"])
    errs["M LLM MR STC"] = h_LLM_MR_23.GetBinError(4,3)
    errs["M LLM MR pre"] = h_LLM_MR_23.GetBinError(4,2)
    errs["M LLM MR obs"] = h_LLM_MR_23.GetBinError(4,1)
    errs["L LLM MR STC"] = h_LLM_MR_23.GetBinError(5,3)
    errs["L LLM MR pre"] = h_LLM_MR_23.GetBinError(5,2)
    errs["L LLM MR obs"] = h_LLM_MR_23.GetBinError(5,1)

    errs["P LM MR STC"] = h_LM_MR_23.GetBinError(1,3)
    errs["P LM MR pre"] = h_LM_MR_23.GetBinError(1,2)
    errs["P LM MR obs"] = h_LM_MR_23.GetBinError(1,1)
    errs["P3 LM MR STC"] = h_LM_MR_23.GetBinError(2,3)
    errs["P3 LM MR pre"] = h_LM_MR_23.GetBinError(2,2)
    errs["P3 LM MR obs"] = h_LM_MR_23.GetBinError(2,1)
    errs["P4 LM MR STC"] = h_LM_MR_23.GetBinError(3,3)
    errs["P4 LM MR pre"] = h_LM_MR_23.GetBinError(3,2)
    errs["P4 LM MR obs"] = h_LM_MR_23.GetBinError(3,1)
    errs["M LM MR STC"] = h_LM_MR_23.GetBinError(4,3)
    errs["M LM MR pre"] = h_LM_MR_23.GetBinError(4,2)
    errs["M LM MR obs"] = h_LM_MR_23.GetBinError(4,1)
    errs["L LM MR STC"] = h_LM_MR_23.GetBinError(5,3)
    errs["L LM MR pre"] = h_LM_MR_23.GetBinError(5,2)
    errs["L LM MR obs"] = h_LM_MR_23.GetBinError(5,1)

    errs["P LH MR STC"] = h_LH_MR_23.GetBinError(1,3)
    errs["P LH MR pre"] = h_LH_MR_23.GetBinError(1,2)
    errs["P LH MR obs"] = h_LH_MR_23.GetBinError(1,1)
    errs["P3 LH MR STC"] = h_LH_MR_23.GetBinError(2,3)
    errs["P3 LH MR pre"] = h_LH_MR_23.GetBinError(2,2)
    errs["P3 LH MR obs"] = h_LH_MR_23.GetBinError(2,1)
    errs["P4 LH MR STC"] = h_LH_MR_23.GetBinError(3,3)
    errs["P4 LH MR pre"] = h_LH_MR_23.GetBinError(3,2)
    errs["P4 LH MR obs"] = h_LH_MR_23.GetBinError(3,1)
    errs["M LH MR STC"] = h_LH_MR_23.GetBinError(4,3)
    errs["M LH MR pre"] = h_LH_MR_23.GetBinError(4,2)
    errs["M LH MR obs"] = h_LH_MR_23.GetBinError(4,1)
    errs["L LH MR STC"] = h_LH_MR_23.GetBinError(5,3)
    errs["L LH MR pre"] = h_LH_MR_23.GetBinError(5,2)
    errs["L LH MR obs"] = h_LH_MR_23.GetBinError(5,1)

    errs["P HL MR STC"] = h_HL_MR_4.GetBinError(1,3)
    errs["P HL MR pre"] = h_HL_MR_4.GetBinError(1,2)
    errs["P HL MR obs"] = h_HL_MR_4.GetBinError(1,1)
    errs["P3 HL MR STC"] = h_HL_MR_4.GetBinError(2,3)
    errs["P3 HL MR pre"] = h_HL_MR_4.GetBinError(2,2)
    errs["P3 HL MR obs"] = h_HL_MR_4.GetBinError(2,1)
    errs["P4 HL MR STC"] = h_HL_MR_4.GetBinError(3,3)
    errs["P4 HL MR pre"] = h_HL_MR_4.GetBinError(3,2)
    errs["P4 HL MR obs"] = h_HL_MR_4.GetBinError(3,1)
    errs["M HL MR STC"] = h_HL_MR_4.GetBinError(4,3)
    errs["M HL MR pre"] = h_HL_MR_4.GetBinError(4,2)
    errs["M HL MR obs"] = h_HL_MR_4.GetBinError(4,1)
    errs["L HL MR STC"] = h_HL_MR_4.GetBinError(5,3)
    errs["L HL MR pre"] = h_HL_MR_4.GetBinError(5,2)
    errs["L HL MR obs"] = h_HL_MR_4.GetBinError(5,1)

    errs["P HLM MR STC"] = h_HLM_MR_4.GetBinError(1,3)
    errs["P HLM MR pre"] = h_HLM_MR_4.GetBinError(1,2)
    errs["P HLM MR obs"] = h_HLM_MR_4.GetBinError(1,1)
    errs["P3 HLM MR STC"] = h_HLM_MR_4.GetBinError(2,3)
    errs["P3 HLM MR pre"] = h_HLM_MR_4.GetBinError(2,2)
    errs["P3 HLM MR obs"] = h_HLM_MR_4.GetBinError(2,1)
    errs["P4 HLM MR STC"] = h_HLM_MR_4.GetBinError(3,3)
    errs["P4 HLM MR pre"] = h_HLM_MR_4.GetBinError(3,2)
    errs["P4 HLM MR obs"] = h_HLM_MR_4.GetBinError(3,1)
    errs["M HLM MR STC"] = h_HLM_MR_4.GetBinError(4,3)
    errs["M HLM MR pre"] = h_HLM_MR_4.GetBinError(4,2)
    errs["M HLM MR obs"] = h_HLM_MR_4.GetBinError(4,1)
    errs["L HLM MR STC"] = h_HLM_MR_4.GetBinError(5,3)
    errs["L HLM MR pre"] = h_HLM_MR_4.GetBinError(5,2)
    errs["L HLM MR obs"] = h_HLM_MR_4.GetBinError(5,1)

    errs["P HM MR STC"] = h_HM_MR_4.GetBinError(1,3)
    errs["P HM MR pre"] = h_HM_MR_4.GetBinError(1,2)
    errs["P HM MR obs"] = h_HM_MR_4.GetBinError(1,1)
    errs["P3 HM MR STC"] = h_HM_MR_4.GetBinError(2,3)
    errs["P3 HM MR pre"] = h_HM_MR_4.GetBinError(2,2)
    errs["P3 HM MR obs"] = h_HM_MR_4.GetBinError(2,1)
    errs["P4 HM MR STC"] = h_HM_MR_4.GetBinError(3,3)
    errs["P4 HM MR pre"] = h_HM_MR_4.GetBinError(3,2)
    errs["P4 HM MR obs"] = h_HM_MR_4.GetBinError(3,1)
    errs["M HM MR STC"] = h_HM_MR_4.GetBinError(4,3)
    errs["M HM MR pre"] = h_HM_MR_4.GetBinError(4,2)
    errs["M HM MR obs"] = h_HM_MR_4.GetBinError(4,1)
    errs["L HM MR STC"] = h_HM_MR_4.GetBinError(5,3)
    errs["L HM MR pre"] = h_HM_MR_4.GetBinError(5,2)
    errs["L HM MR obs"] = h_HM_MR_4.GetBinError(5,1)

    errs["P HH MR STC"] = h_HH_MR_4.GetBinError(1,3)
    errs["P HH MR pre"] = h_HH_MR_4.GetBinError(1,2)
    errs["P HH MR obs"] = h_HH_MR_4.GetBinError(1,1)
    errs["P3 HH MR STC"] = h_HH_MR_4.GetBinError(2,3)
    errs["P3 HH MR pre"] = h_HH_MR_4.GetBinError(2,2)
    errs["P3 HH MR obs"] = h_HH_MR_4.GetBinError(2,1)
    errs["P4 HH MR STC"] = h_HH_MR_4.GetBinError(3,3)
    errs["P4 HH MR pre"] = h_HH_MR_4.GetBinError(3,2)
    errs["P4 HH MR obs"] = h_HH_MR_4.GetBinError(3,1)
    errs["M HH MR STC"] = h_HH_MR_4.GetBinError(4,3)
    errs["M HH MR pre"] = h_HH_MR_4.GetBinError(4,2)
    errs["M HH MR obs"] = h_HH_MR_4.GetBinError(4,1)
    errs["L HH MR STC"] = h_HH_MR_4.GetBinError(5,3)
    errs["L HH MR pre"] = h_HH_MR_4.GetBinError(5,2)
    errs["L HH MR obs"] = h_HH_MR_4.GetBinError(5,1)

    # hi pt
    errs["P LL MR hi STC"] = h_LL_MR_23_hi.GetBinError(1,3)
    errs["P LL MR hi pre"] = h_LL_MR_23_hi.GetBinError(1,2)
    errs["P LL MR hi obs"] = h_LL_MR_23_hi.GetBinError(1,1)
    errs["P3 LL MR hi STC"] = h_LL_MR_23_hi.GetBinError(2,3)
    errs["P3 LL MR hi pre"] = h_LL_MR_23_hi.GetBinError(2,2)
    errs["P3 LL MR hi obs"] = h_LL_MR_23_hi.GetBinError(2,1)
    errs["P4 LL MR hi STC"] = h_LL_MR_23_hi.GetBinError(3,3)
    errs["P4 LL MR hi pre"] = h_LL_MR_23_hi.GetBinError(3,2)
    errs["P4 LL MR hi obs"] = h_LL_MR_23_hi.GetBinError(3,1)
    errs["M LL MR hi STC"] = h_LL_MR_23_hi.GetBinError(4,3)
    errs["M LL MR hi pre"] = h_LL_MR_23_hi.GetBinError(4,2)
    errs["M LL MR hi obs"] = h_LL_MR_23_hi.GetBinError(4,1)
    errs["L LL MR hi STC"] = h_LL_MR_23_hi.GetBinError(5,3)
    errs["L LL MR hi pre"] = h_LL_MR_23_hi.GetBinError(5,2)
    errs["L LL MR hi obs"] = h_LL_MR_23_hi.GetBinError(5,1)

    errs["P LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinError(1,3)
    errs["P LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinError(1,2)
    errs["P LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinError(1,1)
    errs["P3 LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinError(2,3)
    errs["P3 LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinError(2,2)
    errs["P3 LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinError(2,1)
    errs["P4 LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinError(3,3)
    errs["P4 LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinError(3,2)
    errs["P4 LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinError(3,1)
    errs["M LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinError(4,3)
    errs["M LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinError(4,2)
    errs["M LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinError(4,1)
    errs["L LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinError(5,3)
    errs["L LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinError(5,2)
    errs["L LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinError(5,1)

    errs["P LM MR hi STC"] = h_LM_MR_23_hi.GetBinError(1,3)
    errs["P LM MR hi pre"] = h_LM_MR_23_hi.GetBinError(1,2)
    errs["P LM MR hi obs"] = h_LM_MR_23_hi.GetBinError(1,1)
    errs["P3 LM MR hi STC"] = h_LM_MR_23_hi.GetBinError(2,3)
    errs["P3 LM MR hi pre"] = h_LM_MR_23_hi.GetBinError(2,2)
    errs["P3 LM MR hi obs"] = h_LM_MR_23_hi.GetBinError(2,1)
    errs["P4 LM MR hi STC"] = h_LM_MR_23_hi.GetBinError(3,3)
    errs["P4 LM MR hi pre"] = h_LM_MR_23_hi.GetBinError(3,2)
    errs["P4 LM MR hi obs"] = h_LM_MR_23_hi.GetBinError(3,1)
    errs["M LM MR hi STC"] = h_LM_MR_23_hi.GetBinError(4,3)
    errs["M LM MR hi pre"] = h_LM_MR_23_hi.GetBinError(4,2)
    errs["M LM MR hi obs"] = h_LM_MR_23_hi.GetBinError(4,1)
    errs["L LM MR hi STC"] = h_LM_MR_23_hi.GetBinError(5,3)
    errs["L LM MR hi pre"] = h_LM_MR_23_hi.GetBinError(5,2)
    errs["L LM MR hi obs"] = h_LM_MR_23_hi.GetBinError(5,1)

    errs["P LH MR hi STC"] = h_LH_MR_23_hi.GetBinError(1,3)
    errs["P LH MR hi pre"] = h_LH_MR_23_hi.GetBinError(1,2)
    errs["P LH MR hi obs"] = h_LH_MR_23_hi.GetBinError(1,1)
    errs["P3 LH MR hi STC"] = h_LH_MR_23_hi.GetBinError(2,3)
    errs["P3 LH MR hi pre"] = h_LH_MR_23_hi.GetBinError(2,2)
    errs["P3 LH MR hi obs"] = h_LH_MR_23_hi.GetBinError(2,1)
    errs["P4 LH MR hi STC"] = h_LH_MR_23_hi.GetBinError(3,3)
    errs["P4 LH MR hi pre"] = h_LH_MR_23_hi.GetBinError(3,2)
    errs["P4 LH MR hi obs"] = h_LH_MR_23_hi.GetBinError(3,1)
    errs["M LH MR hi STC"] = h_LH_MR_23_hi.GetBinError(4,3)
    errs["M LH MR hi pre"] = h_LH_MR_23_hi.GetBinError(4,2)
    errs["M LH MR hi obs"] = h_LH_MR_23_hi.GetBinError(4,1)
    errs["L LH MR hi STC"] = h_LH_MR_23_hi.GetBinError(5,3)
    errs["L LH MR hi pre"] = h_LH_MR_23_hi.GetBinError(5,2)
    errs["L LH MR hi obs"] = h_LH_MR_23_hi.GetBinError(5,1)

    errs["P HL MR hi STC"] = h_HL_MR_4_hi.GetBinError(1,3)
    errs["P HL MR hi pre"] = h_HL_MR_4_hi.GetBinError(1,2)
    errs["P HL MR hi obs"] = h_HL_MR_4_hi.GetBinError(1,1)
    errs["P3 HL MR hi STC"] = h_HL_MR_4_hi.GetBinError(2,3)
    errs["P3 HL MR hi pre"] = h_HL_MR_4_hi.GetBinError(2,2)
    errs["P3 HL MR hi obs"] = h_HL_MR_4_hi.GetBinError(2,1)
    errs["P4 HL MR hi STC"] = h_HL_MR_4_hi.GetBinError(3,3)
    errs["P4 HL MR hi pre"] = h_HL_MR_4_hi.GetBinError(3,2)
    errs["P4 HL MR hi obs"] = h_HL_MR_4_hi.GetBinError(3,1)
    errs["M HL MR hi STC"] = h_HL_MR_4_hi.GetBinError(4,3)
    errs["M HL MR hi pre"] = h_HL_MR_4_hi.GetBinError(4,2)
    errs["M HL MR hi obs"] = h_HL_MR_4_hi.GetBinError(4,1)
    errs["L HL MR hi STC"] = h_HL_MR_4_hi.GetBinError(5,3)
    errs["L HL MR hi pre"] = h_HL_MR_4_hi.GetBinError(5,2)
    errs["L HL MR hi obs"] = h_HL_MR_4_hi.GetBinError(5,1)

    errs["P HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinError(1,3)
    errs["P HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinError(1,2)
    errs["P HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinError(1,1)
    errs["P3 HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinError(2,3)
    errs["P3 HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinError(2,2)
    errs["P3 HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinError(2,1)
    errs["P4 HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinError(3,3)
    errs["P4 HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinError(3,2)
    errs["P4 HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinError(3,1)
    errs["M HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinError(4,3)
    errs["M HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinError(4,2)
    errs["M HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinError(4,1)
    errs["L HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinError(5,3)
    errs["L HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinError(5,2)
    errs["L HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinError(5,1)

    errs["P HM MR hi STC"] = h_HM_MR_4_hi.GetBinError(1,3)
    errs["P HM MR hi pre"] = h_HM_MR_4_hi.GetBinError(1,2)
    errs["P HM MR hi obs"] = h_HM_MR_4_hi.GetBinError(1,1)
    errs["P3 HM MR hi STC"] = h_HM_MR_4_hi.GetBinError(2,3)
    errs["P3 HM MR hi pre"] = h_HM_MR_4_hi.GetBinError(2,2)
    errs["P3 HM MR hi obs"] = h_HM_MR_4_hi.GetBinError(2,1)
    errs["P4 HM MR hi STC"] = h_HM_MR_4_hi.GetBinError(3,3)
    errs["P4 HM MR hi pre"] = h_HM_MR_4_hi.GetBinError(3,2)
    errs["P4 HM MR hi obs"] = h_HM_MR_4_hi.GetBinError(3,1)
    errs["M HM MR hi STC"] = h_HM_MR_4_hi.GetBinError(4,3)
    errs["M HM MR hi pre"] = h_HM_MR_4_hi.GetBinError(4,2)
    errs["M HM MR hi obs"] = h_HM_MR_4_hi.GetBinError(4,1)
    errs["L HM MR hi STC"] = h_HM_MR_4_hi.GetBinError(5,3)
    errs["L HM MR hi pre"] = h_HM_MR_4_hi.GetBinError(5,2)
    errs["L HM MR hi obs"] = h_HM_MR_4_hi.GetBinError(5,1)

    errs["P HH MR hi STC"] = h_HH_MR_4_hi.GetBinError(1,3)
    errs["P HH MR hi pre"] = h_HH_MR_4_hi.GetBinError(1,2)
    errs["P HH MR hi obs"] = h_HH_MR_4_hi.GetBinError(1,1)
    errs["P3 HH MR hi STC"] = h_HH_MR_4_hi.GetBinError(2,3)
    errs["P3 HH MR hi pre"] = h_HH_MR_4_hi.GetBinError(2,2)
    errs["P3 HH MR hi obs"] = h_HH_MR_4_hi.GetBinError(2,1)
    errs["P4 HH MR hi STC"] = h_HH_MR_4_hi.GetBinError(3,3)
    errs["P4 HH MR hi pre"] = h_HH_MR_4_hi.GetBinError(3,2)
    errs["P4 HH MR hi obs"] = h_HH_MR_4_hi.GetBinError(3,1)
    errs["M HH MR hi STC"] = h_HH_MR_4_hi.GetBinError(4,3)
    errs["M HH MR hi pre"] = h_HH_MR_4_hi.GetBinError(4,2)
    errs["M HH MR hi obs"] = h_HH_MR_4_hi.GetBinError(4,1)
    errs["L HH MR hi STC"] = h_HH_MR_4_hi.GetBinError(5,3)
    errs["L HH MR hi pre"] = h_HH_MR_4_hi.GetBinError(5,2)
    errs["L HH MR hi obs"] = h_HH_MR_4_hi.GetBinError(5,1)

    # lo pt
    errs["P LL MR lo STC"] = h_LL_MR_23_lo.GetBinError(1,3)
    errs["P LL MR lo pre"] = h_LL_MR_23_lo.GetBinError(1,2)
    errs["P LL MR lo obs"] = h_LL_MR_23_lo.GetBinError(1,1)
    errs["P3 LL MR lo STC"] = h_LL_MR_23_lo.GetBinError(2,3)
    errs["P3 LL MR lo pre"] = h_LL_MR_23_lo.GetBinError(2,2)
    errs["P3 LL MR lo obs"] = h_LL_MR_23_lo.GetBinError(2,1)
    errs["P4 LL MR lo STC"] = h_LL_MR_23_lo.GetBinError(3,3)
    errs["P4 LL MR lo pre"] = h_LL_MR_23_lo.GetBinError(3,2)
    errs["P4 LL MR lo obs"] = h_LL_MR_23_lo.GetBinError(3,1)
    errs["M LL MR lo STC"] = h_LL_MR_23_lo.GetBinError(4,3)
    errs["M LL MR lo pre"] = h_LL_MR_23_lo.GetBinError(4,2)
    errs["M LL MR lo obs"] = h_LL_MR_23_lo.GetBinError(4,1)
    errs["L LL MR lo STC"] = h_LL_MR_23_lo.GetBinError(5,3)
    errs["L LL MR lo pre"] = h_LL_MR_23_lo.GetBinError(5,2)
    errs["L LL MR lo obs"] = h_LL_MR_23_lo.GetBinError(5,1)

    errs["P LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinError(1,3)
    errs["P LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinError(1,2)
    errs["P LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinError(1,1)
    errs["P3 LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinError(2,3)
    errs["P3 LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinError(2,2)
    errs["P3 LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinError(2,1)
    errs["P4 LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinError(3,3)
    errs["P4 LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinError(3,2)
    errs["P4 LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinError(3,1)
    errs["M LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinError(4,3)
    errs["M LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinError(4,2)
    errs["M LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinError(4,1)
    errs["L LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinError(5,3)
    errs["L LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinError(5,2)
    errs["L LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinError(5,1)

    errs["P LM MR lo STC"] = h_LM_MR_23_lo.GetBinError(1,3)
    errs["P LM MR lo pre"] = h_LM_MR_23_lo.GetBinError(1,2)
    errs["P LM MR lo obs"] = h_LM_MR_23_lo.GetBinError(1,1)
    errs["P3 LM MR lo STC"] = h_LM_MR_23_lo.GetBinError(2,3)
    errs["P3 LM MR lo pre"] = h_LM_MR_23_lo.GetBinError(2,2)
    errs["P3 LM MR lo obs"] = h_LM_MR_23_lo.GetBinError(2,1)
    errs["P4 LM MR lo STC"] = h_LM_MR_23_lo.GetBinError(3,3)
    errs["P4 LM MR lo pre"] = h_LM_MR_23_lo.GetBinError(3,2)
    errs["P4 LM MR lo obs"] = h_LM_MR_23_lo.GetBinError(3,1)
    errs["M LM MR lo STC"] = h_LM_MR_23_lo.GetBinError(4,3)
    errs["M LM MR lo pre"] = h_LM_MR_23_lo.GetBinError(4,2)
    errs["M LM MR lo obs"] = h_LM_MR_23_lo.GetBinError(4,1)
    errs["L LM MR lo STC"] = h_LM_MR_23_lo.GetBinError(5,3)
    errs["L LM MR lo pre"] = h_LM_MR_23_lo.GetBinError(5,2)
    errs["L LM MR lo obs"] = h_LM_MR_23_lo.GetBinError(5,1)

    errs["P LH MR lo STC"] = h_LH_MR_23_lo.GetBinError(1,3)
    errs["P LH MR lo pre"] = h_LH_MR_23_lo.GetBinError(1,2)
    errs["P LH MR lo obs"] = h_LH_MR_23_lo.GetBinError(1,1)
    errs["P3 LH MR lo STC"] = h_LH_MR_23_lo.GetBinError(2,3)
    errs["P3 LH MR lo pre"] = h_LH_MR_23_lo.GetBinError(2,2)
    errs["P3 LH MR lo obs"] = h_LH_MR_23_lo.GetBinError(2,1)
    errs["P4 LH MR lo STC"] = h_LH_MR_23_lo.GetBinError(3,3)
    errs["P4 LH MR lo pre"] = h_LH_MR_23_lo.GetBinError(3,2)
    errs["P4 LH MR lo obs"] = h_LH_MR_23_lo.GetBinError(3,1)
    errs["M LH MR lo STC"] = h_LH_MR_23_lo.GetBinError(4,3)
    errs["M LH MR lo pre"] = h_LH_MR_23_lo.GetBinError(4,2)
    errs["M LH MR lo obs"] = h_LH_MR_23_lo.GetBinError(4,1)
    errs["L LH MR lo STC"] = h_LH_MR_23_lo.GetBinError(5,3)
    errs["L LH MR lo pre"] = h_LH_MR_23_lo.GetBinError(5,2)
    errs["L LH MR lo obs"] = h_LH_MR_23_lo.GetBinError(5,1)

    errs["P HL MR lo STC"] = h_HL_MR_4_lo.GetBinError(1,3)
    errs["P HL MR lo pre"] = h_HL_MR_4_lo.GetBinError(1,2)
    errs["P HL MR lo obs"] = h_HL_MR_4_lo.GetBinError(1,1)
    errs["P3 HL MR lo STC"] = h_HL_MR_4_lo.GetBinError(2,3)
    errs["P3 HL MR lo pre"] = h_HL_MR_4_lo.GetBinError(2,2)
    errs["P3 HL MR lo obs"] = h_HL_MR_4_lo.GetBinError(2,1)
    errs["P4 HL MR lo STC"] = h_HL_MR_4_lo.GetBinError(3,3)
    errs["P4 HL MR lo pre"] = h_HL_MR_4_lo.GetBinError(3,2)
    errs["P4 HL MR lo obs"] = h_HL_MR_4_lo.GetBinError(3,1)
    errs["M HL MR lo STC"] = h_HL_MR_4_lo.GetBinError(4,3)
    errs["M HL MR lo pre"] = h_HL_MR_4_lo.GetBinError(4,2)
    errs["M HL MR lo obs"] = h_HL_MR_4_lo.GetBinError(4,1)
    errs["L HL MR lo STC"] = h_HL_MR_4_lo.GetBinError(5,3)
    errs["L HL MR lo pre"] = h_HL_MR_4_lo.GetBinError(5,2)
    errs["L HL MR lo obs"] = h_HL_MR_4_lo.GetBinError(5,1)

    errs["P HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinError(1,3)
    errs["P HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinError(1,2)
    errs["P HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinError(1,1)
    errs["P3 HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinError(2,3)
    errs["P3 HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinError(2,2)
    errs["P3 HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinError(2,1)
    errs["P4 HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinError(3,3)
    errs["P4 HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinError(3,2)
    errs["P4 HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinError(3,1)
    errs["M HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinError(4,3)
    errs["M HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinError(4,2)
    errs["M HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinError(4,1)
    errs["L HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinError(5,3)
    errs["L HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinError(5,2)
    errs["L HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinError(5,1)

    errs["P HM MR lo STC"] = h_HM_MR_4_lo.GetBinError(1,3)
    errs["P HM MR lo pre"] = h_HM_MR_4_lo.GetBinError(1,2)
    errs["P HM MR lo obs"] = h_HM_MR_4_lo.GetBinError(1,1)
    errs["P3 HM MR lo STC"] = h_HM_MR_4_lo.GetBinError(2,3)
    errs["P3 HM MR lo pre"] = h_HM_MR_4_lo.GetBinError(2,2)
    errs["P3 HM MR lo obs"] = h_HM_MR_4_lo.GetBinError(2,1)
    errs["P4 HM MR lo STC"] = h_HM_MR_4_lo.GetBinError(3,3)
    errs["P4 HM MR lo pre"] = h_HM_MR_4_lo.GetBinError(3,2)
    errs["P4 HM MR lo obs"] = h_HM_MR_4_lo.GetBinError(3,1)
    errs["M HM MR lo STC"] = h_HM_MR_4_lo.GetBinError(4,3)
    errs["M HM MR lo pre"] = h_HM_MR_4_lo.GetBinError(4,2)
    errs["M HM MR lo obs"] = h_HM_MR_4_lo.GetBinError(4,1)
    errs["L HM MR lo STC"] = h_HM_MR_4_lo.GetBinError(5,3)
    errs["L HM MR lo pre"] = h_HM_MR_4_lo.GetBinError(5,2)
    errs["L HM MR lo obs"] = h_HM_MR_4_lo.GetBinError(5,1)

    errs["P HH MR lo STC"] = h_HH_MR_4_lo.GetBinError(1,3)
    errs["P HH MR lo pre"] = h_HH_MR_4_lo.GetBinError(1,2)
    errs["P HH MR lo obs"] = h_HH_MR_4_lo.GetBinError(1,1)
    errs["P3 HH MR lo STC"] = h_HH_MR_4_lo.GetBinError(2,3)
    errs["P3 HH MR lo pre"] = h_HH_MR_4_lo.GetBinError(2,2)
    errs["P3 HH MR lo obs"] = h_HH_MR_4_lo.GetBinError(2,1)
    errs["P4 HH MR lo STC"] = h_HH_MR_4_lo.GetBinError(3,3)
    errs["P4 HH MR lo pre"] = h_HH_MR_4_lo.GetBinError(3,2)
    errs["P4 HH MR lo obs"] = h_HH_MR_4_lo.GetBinError(3,1)
    errs["M HH MR lo STC"] = h_HH_MR_4_lo.GetBinError(4,3)
    errs["M HH MR lo pre"] = h_HH_MR_4_lo.GetBinError(4,2)
    errs["M HH MR lo obs"] = h_HH_MR_4_lo.GetBinError(4,1)
    errs["L HH MR lo STC"] = h_HH_MR_4_lo.GetBinError(5,3)
    errs["L HH MR lo pre"] = h_HH_MR_4_lo.GetBinError(5,2)
    errs["L HH MR lo obs"] = h_HH_MR_4_lo.GetBinError(5,1)

    # VR vals
    vals["P LL VR STC"] = h_LL_VR_23.GetBinContent(1,3)
    vals["P LL VR pre"] = h_LL_VR_23.GetBinContent(1,2)
    vals["P LL VR obs"] = h_LL_VR_23.GetBinContent(1,1)
    vals["P3 LL VR STC"] = h_LL_VR_23.GetBinContent(2,3)
    vals["P3 LL VR pre"] = h_LL_VR_23.GetBinContent(2,2)
    vals["P3 LL VR obs"] = h_LL_VR_23.GetBinContent(2,1)
    vals["P4 LL VR STC"] = h_LL_VR_23.GetBinContent(3,3)
    vals["P4 LL VR pre"] = h_LL_VR_23.GetBinContent(3,2)
    vals["P4 LL VR obs"] = h_LL_VR_23.GetBinContent(3,1)
    vals["M LL VR STC"] = h_LL_VR_23.GetBinContent(4,3)
    vals["M LL VR pre"] = h_LL_VR_23.GetBinContent(4,2)
    vals["M LL VR obs"] = h_LL_VR_23.GetBinContent(4,1)
    vals["L LL VR STC"] = h_LL_VR_23.GetBinContent(5,3)
    vals["L LL VR pre"] = h_LL_VR_23.GetBinContent(5,2)
    vals["L LL VR obs"] = h_LL_VR_23.GetBinContent(5,1)

    vals["P LLM VR STC"] = h_LLM_VR_23.GetBinContent(1,3)
    vals["P LLM VR pre"] = h_LLM_VR_23.GetBinContent(1,2)
    vals["P LLM VR obs"] = h_LLM_VR_23.GetBinContent(1,1)
    vals["P3 LLM VR STC"] = h_LLM_VR_23.GetBinContent(2,3)
    vals["P3 LLM VR pre"] = h_LLM_VR_23.GetBinContent(2,2)
    vals["P3 LLM VR obs"] = h_LLM_VR_23.GetBinContent(2,1)
    vals["P4 LLM VR STC"] = h_LLM_VR_23.GetBinContent(3,3)
    vals["P4 LLM VR pre"] = h_LLM_VR_23.GetBinContent(3,2)
    vals["P4 LLM VR obs"] = h_LLM_VR_23.GetBinContent(3,1)
    vals["M LLM VR STC"] = h_LLM_VR_23.GetBinContent(4,3)
    vals["M LLM VR pre"] = h_LLM_VR_23.GetBinContent(4,2)
    vals["M LLM VR obs"] = h_LLM_VR_23.GetBinContent(4,1)
    vals["L LLM VR STC"] = h_LLM_VR_23.GetBinContent(5,3)
    vals["L LLM VR pre"] = h_LLM_VR_23.GetBinContent(5,2)
    vals["L LLM VR obs"] = h_LLM_VR_23.GetBinContent(5,1)

    vals["P LM VR STC"] = h_LM_VR_23.GetBinContent(1,3)
    vals["P LM VR pre"] = h_LM_VR_23.GetBinContent(1,2)
    vals["P LM VR obs"] = h_LM_VR_23.GetBinContent(1,1)
    vals["P3 LM VR STC"] = h_LM_VR_23.GetBinContent(2,3)
    vals["P3 LM VR pre"] = h_LM_VR_23.GetBinContent(2,2)
    vals["P3 LM VR obs"] = h_LM_VR_23.GetBinContent(2,1)
    vals["P4 LM VR STC"] = h_LM_VR_23.GetBinContent(3,3)
    vals["P4 LM VR pre"] = h_LM_VR_23.GetBinContent(3,2)
    vals["P4 LM VR obs"] = h_LM_VR_23.GetBinContent(3,1)
    vals["M LM VR STC"] = h_LM_VR_23.GetBinContent(4,3)
    vals["M LM VR pre"] = h_LM_VR_23.GetBinContent(4,2)
    vals["M LM VR obs"] = h_LM_VR_23.GetBinContent(4,1)
    vals["L LM VR STC"] = h_LM_VR_23.GetBinContent(5,3)
    vals["L LM VR pre"] = h_LM_VR_23.GetBinContent(5,2)
    vals["L LM VR obs"] = h_LM_VR_23.GetBinContent(5,1)

    vals["P LH VR STC"] = h_LH_VR_23.GetBinContent(1,3)
    vals["P LH VR pre"] = h_LH_VR_23.GetBinContent(1,2)
    vals["P LH VR obs"] = h_LH_VR_23.GetBinContent(1,1)
    vals["P3 LH VR STC"] = h_LH_VR_23.GetBinContent(2,3)
    vals["P3 LH VR pre"] = h_LH_VR_23.GetBinContent(2,2)
    vals["P3 LH VR obs"] = h_LH_VR_23.GetBinContent(2,1)
    vals["P4 LH VR STC"] = h_LH_VR_23.GetBinContent(3,3)
    vals["P4 LH VR pre"] = h_LH_VR_23.GetBinContent(3,2)
    vals["P4 LH VR obs"] = h_LH_VR_23.GetBinContent(3,1)
    vals["M LH VR STC"] = h_LH_VR_23.GetBinContent(4,3)
    vals["M LH VR pre"] = h_LH_VR_23.GetBinContent(4,2)
    vals["M LH VR obs"] = h_LH_VR_23.GetBinContent(4,1)
    vals["L LH VR STC"] = h_LH_VR_23.GetBinContent(5,3)
    vals["L LH VR pre"] = h_LH_VR_23.GetBinContent(5,2)
    vals["L LH VR obs"] = h_LH_VR_23.GetBinContent(5,1)

    vals["P HL VR STC"] = h_HL_VR_4.GetBinContent(1,3)
    vals["P HL VR pre"] = h_HL_VR_4.GetBinContent(1,2)
    vals["P HL VR obs"] = h_HL_VR_4.GetBinContent(1,1)
    vals["P3 HL VR STC"] = h_HL_VR_4.GetBinContent(2,3)
    vals["P3 HL VR pre"] = h_HL_VR_4.GetBinContent(2,2)
    vals["P3 HL VR obs"] = h_HL_VR_4.GetBinContent(2,1)
    vals["P4 HL VR STC"] = h_HL_VR_4.GetBinContent(3,3)
    vals["P4 HL VR pre"] = h_HL_VR_4.GetBinContent(3,2)
    vals["P4 HL VR obs"] = h_HL_VR_4.GetBinContent(3,1)
    vals["M HL VR STC"] = h_HL_VR_4.GetBinContent(4,3)
    vals["M HL VR pre"] = h_HL_VR_4.GetBinContent(4,2)
    vals["M HL VR obs"] = h_HL_VR_4.GetBinContent(4,1)
    vals["L HL VR STC"] = h_HL_VR_4.GetBinContent(5,3)
    vals["L HL VR pre"] = h_HL_VR_4.GetBinContent(5,2)
    vals["L HL VR obs"] = h_HL_VR_4.GetBinContent(5,1)

    vals["P HLM VR STC"] = h_HLM_VR_4.GetBinContent(1,3)
    vals["P HLM VR pre"] = h_HLM_VR_4.GetBinContent(1,2)
    vals["P HLM VR obs"] = h_HLM_VR_4.GetBinContent(1,1)
    vals["P3 HLM VR STC"] = h_HLM_VR_4.GetBinContent(2,3)
    vals["P3 HLM VR pre"] = h_HLM_VR_4.GetBinContent(2,2)
    vals["P3 HLM VR obs"] = h_HLM_VR_4.GetBinContent(2,1)
    vals["P4 HLM VR STC"] = h_HLM_VR_4.GetBinContent(3,3)
    vals["P4 HLM VR pre"] = h_HLM_VR_4.GetBinContent(3,2)
    vals["P4 HLM VR obs"] = h_HLM_VR_4.GetBinContent(3,1)
    vals["M HLM VR STC"] = h_HLM_VR_4.GetBinContent(4,3)
    vals["M HLM VR pre"] = h_HLM_VR_4.GetBinContent(4,2)
    vals["M HLM VR obs"] = h_HLM_VR_4.GetBinContent(4,1)
    vals["L HLM VR STC"] = h_HLM_VR_4.GetBinContent(5,3)
    vals["L HLM VR pre"] = h_HLM_VR_4.GetBinContent(5,2)
    vals["L HLM VR obs"] = h_HLM_VR_4.GetBinContent(5,1)

    vals["P HM VR STC"] = h_HM_VR_4.GetBinContent(1,3)
    vals["P HM VR pre"] = h_HM_VR_4.GetBinContent(1,2)
    vals["P HM VR obs"] = h_HM_VR_4.GetBinContent(1,1)
    vals["P3 HM VR STC"] = h_HM_VR_4.GetBinContent(2,3)
    vals["P3 HM VR pre"] = h_HM_VR_4.GetBinContent(2,2)
    vals["P3 HM VR obs"] = h_HM_VR_4.GetBinContent(2,1)
    vals["P4 HM VR STC"] = h_HM_VR_4.GetBinContent(3,3)
    vals["P4 HM VR pre"] = h_HM_VR_4.GetBinContent(3,2)
    vals["P4 HM VR obs"] = h_HM_VR_4.GetBinContent(3,1)
    vals["M HM VR STC"] = h_HM_VR_4.GetBinContent(4,3)
    vals["M HM VR pre"] = h_HM_VR_4.GetBinContent(4,2)
    vals["M HM VR obs"] = h_HM_VR_4.GetBinContent(4,1)
    vals["L HM VR STC"] = h_HM_VR_4.GetBinContent(5,3)
    vals["L HM VR pre"] = h_HM_VR_4.GetBinContent(5,2)
    vals["L HM VR obs"] = h_HM_VR_4.GetBinContent(5,1)

    vals["P HH VR STC"] = h_HH_VR_4.GetBinContent(1,3)
    vals["P HH VR pre"] = h_HH_VR_4.GetBinContent(1,2)
    vals["P HH VR obs"] = h_HH_VR_4.GetBinContent(1,1)
    vals["P3 HH VR STC"] = h_HH_VR_4.GetBinContent(2,3)
    vals["P3 HH VR pre"] = h_HH_VR_4.GetBinContent(2,2)
    vals["P3 HH VR obs"] = h_HH_VR_4.GetBinContent(2,1)
    vals["P4 HH VR STC"] = h_HH_VR_4.GetBinContent(3,3)
    vals["P4 HH VR pre"] = h_HH_VR_4.GetBinContent(3,2)
    vals["P4 HH VR obs"] = h_HH_VR_4.GetBinContent(3,1)
    vals["M HH VR STC"] = h_HH_VR_4.GetBinContent(4,3)
    vals["M HH VR pre"] = h_HH_VR_4.GetBinContent(4,2)
    vals["M HH VR obs"] = h_HH_VR_4.GetBinContent(4,1)
    vals["L HH VR STC"] = h_HH_VR_4.GetBinContent(5,3)
    vals["L HH VR pre"] = h_HH_VR_4.GetBinContent(5,2)
    vals["L HH VR obs"] = h_HH_VR_4.GetBinContent(5,1)

    # hi pt
    vals["P LL VR hi STC"] = h_LL_VR_23_hi.GetBinContent(1,3)
    vals["P LL VR hi pre"] = h_LL_VR_23_hi.GetBinContent(1,2)
    vals["P LL VR hi obs"] = h_LL_VR_23_hi.GetBinContent(1,1)
    vals["P3 LL VR hi STC"] = h_LL_VR_23_hi.GetBinContent(2,3)
    vals["P3 LL VR hi pre"] = h_LL_VR_23_hi.GetBinContent(2,2)
    vals["P3 LL VR hi obs"] = h_LL_VR_23_hi.GetBinContent(2,1)
    vals["P4 LL VR hi STC"] = h_LL_VR_23_hi.GetBinContent(3,3)
    vals["P4 LL VR hi pre"] = h_LL_VR_23_hi.GetBinContent(3,2)
    vals["P4 LL VR hi obs"] = h_LL_VR_23_hi.GetBinContent(3,1)
    vals["M LL VR hi STC"] = h_LL_VR_23_hi.GetBinContent(4,3)
    vals["M LL VR hi pre"] = h_LL_VR_23_hi.GetBinContent(4,2)
    vals["M LL VR hi obs"] = h_LL_VR_23_hi.GetBinContent(4,1)
    vals["L LL VR hi STC"] = h_LL_VR_23_hi.GetBinContent(5,3)
    vals["L LL VR hi pre"] = h_LL_VR_23_hi.GetBinContent(5,2)
    vals["L LL VR hi obs"] = h_LL_VR_23_hi.GetBinContent(5,1)

    vals["P LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinContent(1,3)
    vals["P LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinContent(1,2)
    vals["P LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinContent(1,1)
    vals["P3 LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinContent(2,3)
    vals["P3 LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinContent(2,2)
    vals["P3 LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinContent(2,1)
    vals["P4 LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinContent(3,3)
    vals["P4 LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinContent(3,2)
    vals["P4 LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinContent(3,1)
    vals["M LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinContent(4,3)
    vals["M LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinContent(4,2)
    vals["M LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinContent(4,1)
    vals["L LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinContent(5,3)
    vals["L LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinContent(5,2)
    vals["L LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinContent(5,1)

    vals["P LM VR hi STC"] = h_LM_VR_23_hi.GetBinContent(1,3)
    vals["P LM VR hi pre"] = h_LM_VR_23_hi.GetBinContent(1,2)
    vals["P LM VR hi obs"] = h_LM_VR_23_hi.GetBinContent(1,1)
    vals["P3 LM VR hi STC"] = h_LM_VR_23_hi.GetBinContent(2,3)
    vals["P3 LM VR hi pre"] = h_LM_VR_23_hi.GetBinContent(2,2)
    vals["P3 LM VR hi obs"] = h_LM_VR_23_hi.GetBinContent(2,1)
    vals["P4 LM VR hi STC"] = h_LM_VR_23_hi.GetBinContent(3,3)
    vals["P4 LM VR hi pre"] = h_LM_VR_23_hi.GetBinContent(3,2)
    vals["P4 LM VR hi obs"] = h_LM_VR_23_hi.GetBinContent(3,1)
    vals["M LM VR hi STC"] = h_LM_VR_23_hi.GetBinContent(4,3)
    vals["M LM VR hi pre"] = h_LM_VR_23_hi.GetBinContent(4,2)
    vals["M LM VR hi obs"] = h_LM_VR_23_hi.GetBinContent(4,1)
    vals["L LM VR hi STC"] = h_LM_VR_23_hi.GetBinContent(5,3)
    vals["L LM VR hi pre"] = h_LM_VR_23_hi.GetBinContent(5,2)
    vals["L LM VR hi obs"] = h_LM_VR_23_hi.GetBinContent(5,1)

    vals["P LH VR hi STC"] = h_LH_VR_23_hi.GetBinContent(1,3)
    vals["P LH VR hi pre"] = h_LH_VR_23_hi.GetBinContent(1,2)
    vals["P LH VR hi obs"] = h_LH_VR_23_hi.GetBinContent(1,1)
    vals["P3 LH VR hi STC"] = h_LH_VR_23_hi.GetBinContent(2,3)
    vals["P3 LH VR hi pre"] = h_LH_VR_23_hi.GetBinContent(2,2)
    vals["P3 LH VR hi obs"] = h_LH_VR_23_hi.GetBinContent(2,1)
    vals["P4 LH VR hi STC"] = h_LH_VR_23_hi.GetBinContent(3,3)
    vals["P4 LH VR hi pre"] = h_LH_VR_23_hi.GetBinContent(3,2)
    vals["P4 LH VR hi obs"] = h_LH_VR_23_hi.GetBinContent(3,1)
    vals["M LH VR hi STC"] = h_LH_VR_23_hi.GetBinContent(4,3)
    vals["M LH VR hi pre"] = h_LH_VR_23_hi.GetBinContent(4,2)
    vals["M LH VR hi obs"] = h_LH_VR_23_hi.GetBinContent(4,1)
    vals["L LH VR hi STC"] = h_LH_VR_23_hi.GetBinContent(5,3)
    vals["L LH VR hi pre"] = h_LH_VR_23_hi.GetBinContent(5,2)
    vals["L LH VR hi obs"] = h_LH_VR_23_hi.GetBinContent(5,1)

    vals["P HL VR hi STC"] = h_HL_VR_4_hi.GetBinContent(1,3)
    vals["P HL VR hi pre"] = h_HL_VR_4_hi.GetBinContent(1,2)
    vals["P HL VR hi obs"] = h_HL_VR_4_hi.GetBinContent(1,1)
    vals["P3 HL VR hi STC"] = h_HL_VR_4_hi.GetBinContent(2,3)
    vals["P3 HL VR hi pre"] = h_HL_VR_4_hi.GetBinContent(2,2)
    vals["P3 HL VR hi obs"] = h_HL_VR_4_hi.GetBinContent(2,1)
    vals["P4 HL VR hi STC"] = h_HL_VR_4_hi.GetBinContent(3,3)
    vals["P4 HL VR hi pre"] = h_HL_VR_4_hi.GetBinContent(3,2)
    vals["P4 HL VR hi obs"] = h_HL_VR_4_hi.GetBinContent(3,1)
    vals["M HL VR hi STC"] = h_HL_VR_4_hi.GetBinContent(4,3)
    vals["M HL VR hi pre"] = h_HL_VR_4_hi.GetBinContent(4,2)
    vals["M HL VR hi obs"] = h_HL_VR_4_hi.GetBinContent(4,1)
    vals["L HL VR hi STC"] = h_HL_VR_4_hi.GetBinContent(5,3)
    vals["L HL VR hi pre"] = h_HL_VR_4_hi.GetBinContent(5,2)
    vals["L HL VR hi obs"] = h_HL_VR_4_hi.GetBinContent(5,1)

    vals["P HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinContent(1,3)
    vals["P HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinContent(1,2)
    vals["P HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinContent(1,1)
    vals["P3 HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinContent(2,3)
    vals["P3 HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinContent(2,2)
    vals["P3 HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinContent(2,1)
    vals["P4 HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinContent(3,3)
    vals["P4 HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinContent(3,2)
    vals["P4 HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinContent(3,1)
    vals["M HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinContent(4,3)
    vals["M HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinContent(4,2)
    vals["M HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinContent(4,1)
    vals["L HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinContent(5,3)
    vals["L HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinContent(5,2)
    vals["L HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinContent(5,1)

    vals["P HM VR hi STC"] = h_HM_VR_4_hi.GetBinContent(1,3)
    vals["P HM VR hi pre"] = h_HM_VR_4_hi.GetBinContent(1,2)
    vals["P HM VR hi obs"] = h_HM_VR_4_hi.GetBinContent(1,1)
    vals["P3 HM VR hi STC"] = h_HM_VR_4_hi.GetBinContent(2,3)
    vals["P3 HM VR hi pre"] = h_HM_VR_4_hi.GetBinContent(2,2)
    vals["P3 HM VR hi obs"] = h_HM_VR_4_hi.GetBinContent(2,1)
    vals["P4 HM VR hi STC"] = h_HM_VR_4_hi.GetBinContent(3,3)
    vals["P4 HM VR hi pre"] = h_HM_VR_4_hi.GetBinContent(3,2)
    vals["P4 HM VR hi obs"] = h_HM_VR_4_hi.GetBinContent(3,1)
    vals["M HM VR hi STC"] = h_HM_VR_4_hi.GetBinContent(4,3)
    vals["M HM VR hi pre"] = h_HM_VR_4_hi.GetBinContent(4,2)
    vals["M HM VR hi obs"] = h_HM_VR_4_hi.GetBinContent(4,1)
    vals["L HM VR hi STC"] = h_HM_VR_4_hi.GetBinContent(5,3)
    vals["L HM VR hi pre"] = h_HM_VR_4_hi.GetBinContent(5,2)
    vals["L HM VR hi obs"] = h_HM_VR_4_hi.GetBinContent(5,1)

    vals["P HH VR hi STC"] = h_HH_VR_4_hi.GetBinContent(1,3)
    vals["P HH VR hi pre"] = h_HH_VR_4_hi.GetBinContent(1,2)
    vals["P HH VR hi obs"] = h_HH_VR_4_hi.GetBinContent(1,1)
    vals["P3 HH VR hi STC"] = h_HH_VR_4_hi.GetBinContent(2,3)
    vals["P3 HH VR hi pre"] = h_HH_VR_4_hi.GetBinContent(2,2)
    vals["P3 HH VR hi obs"] = h_HH_VR_4_hi.GetBinContent(2,1)
    vals["P4 HH VR hi STC"] = h_HH_VR_4_hi.GetBinContent(3,3)
    vals["P4 HH VR hi pre"] = h_HH_VR_4_hi.GetBinContent(3,2)
    vals["P4 HH VR hi obs"] = h_HH_VR_4_hi.GetBinContent(3,1)
    vals["M HH VR hi STC"] = h_HH_VR_4_hi.GetBinContent(4,3)
    vals["M HH VR hi pre"] = h_HH_VR_4_hi.GetBinContent(4,2)
    vals["M HH VR hi obs"] = h_HH_VR_4_hi.GetBinContent(4,1)
    vals["L HH VR hi STC"] = h_HH_VR_4_hi.GetBinContent(5,3)
    vals["L HH VR hi pre"] = h_HH_VR_4_hi.GetBinContent(5,2)
    vals["L HH VR hi obs"] = h_HH_VR_4_hi.GetBinContent(5,1)

    # lo pt
    vals["P LL VR lo STC"] = h_LL_VR_23_lo.GetBinContent(1,3)
    vals["P LL VR lo pre"] = h_LL_VR_23_lo.GetBinContent(1,2)
    vals["P LL VR lo obs"] = h_LL_VR_23_lo.GetBinContent(1,1)
    vals["P3 LL VR lo STC"] = h_LL_VR_23_lo.GetBinContent(2,3)
    vals["P3 LL VR lo pre"] = h_LL_VR_23_lo.GetBinContent(2,2)
    vals["P3 LL VR lo obs"] = h_LL_VR_23_lo.GetBinContent(2,1)
    vals["P4 LL VR lo STC"] = h_LL_VR_23_lo.GetBinContent(3,3)
    vals["P4 LL VR lo pre"] = h_LL_VR_23_lo.GetBinContent(3,2)
    vals["P4 LL VR lo obs"] = h_LL_VR_23_lo.GetBinContent(3,1)
    vals["M LL VR lo STC"] = h_LL_VR_23_lo.GetBinContent(4,3)
    vals["M LL VR lo pre"] = h_LL_VR_23_lo.GetBinContent(4,2)
    vals["M LL VR lo obs"] = h_LL_VR_23_lo.GetBinContent(4,1)
    vals["L LL VR lo STC"] = h_LL_VR_23_lo.GetBinContent(5,3)
    vals["L LL VR lo pre"] = h_LL_VR_23_lo.GetBinContent(5,2)
    vals["L LL VR lo obs"] = h_LL_VR_23_lo.GetBinContent(5,1)

    vals["P LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinContent(1,3)
    vals["P LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinContent(1,2)
    vals["P LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinContent(1,1)
    vals["P3 LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinContent(2,3)
    vals["P3 LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinContent(2,2)
    vals["P3 LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinContent(2,1)
    vals["P4 LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinContent(3,3)
    vals["P4 LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinContent(3,2)
    vals["P4 LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinContent(3,1)
    vals["M LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinContent(4,3)
    vals["M LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinContent(4,2)
    vals["M LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinContent(4,1)
    vals["L LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinContent(5,3)
    vals["L LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinContent(5,2)
    vals["L LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinContent(5,1)

    vals["P LM VR lo STC"] = h_LM_VR_23_lo.GetBinContent(1,3)
    vals["P LM VR lo pre"] = h_LM_VR_23_lo.GetBinContent(1,2)
    vals["P LM VR lo obs"] = h_LM_VR_23_lo.GetBinContent(1,1)
    vals["P3 LM VR lo STC"] = h_LM_VR_23_lo.GetBinContent(2,3)
    vals["P3 LM VR lo pre"] = h_LM_VR_23_lo.GetBinContent(2,2)
    vals["P3 LM VR lo obs"] = h_LM_VR_23_lo.GetBinContent(2,1)
    vals["P4 LM VR lo STC"] = h_LM_VR_23_lo.GetBinContent(3,3)
    vals["P4 LM VR lo pre"] = h_LM_VR_23_lo.GetBinContent(3,2)
    vals["P4 LM VR lo obs"] = h_LM_VR_23_lo.GetBinContent(3,1)
    vals["M LM VR lo STC"] = h_LM_VR_23_lo.GetBinContent(4,3)
    vals["M LM VR lo pre"] = h_LM_VR_23_lo.GetBinContent(4,2)
    vals["M LM VR lo obs"] = h_LM_VR_23_lo.GetBinContent(4,1)
    vals["L LM VR lo STC"] = h_LM_VR_23_lo.GetBinContent(5,3)
    vals["L LM VR lo pre"] = h_LM_VR_23_lo.GetBinContent(5,2)
    vals["L LM VR lo obs"] = h_LM_VR_23_lo.GetBinContent(5,1)

    vals["P LH VR lo STC"] = h_LH_VR_23_lo.GetBinContent(1,3)
    vals["P LH VR lo pre"] = h_LH_VR_23_lo.GetBinContent(1,2)
    vals["P LH VR lo obs"] = h_LH_VR_23_lo.GetBinContent(1,1)
    vals["P3 LH VR lo STC"] = h_LH_VR_23_lo.GetBinContent(2,3)
    vals["P3 LH VR lo pre"] = h_LH_VR_23_lo.GetBinContent(2,2)
    vals["P3 LH VR lo obs"] = h_LH_VR_23_lo.GetBinContent(2,1)
    vals["P4 LH VR lo STC"] = h_LH_VR_23_lo.GetBinContent(3,3)
    vals["P4 LH VR lo pre"] = h_LH_VR_23_lo.GetBinContent(3,2)
    vals["P4 LH VR lo obs"] = h_LH_VR_23_lo.GetBinContent(3,1)
    vals["M LH VR lo STC"] = h_LH_VR_23_lo.GetBinContent(4,3)
    vals["M LH VR lo pre"] = h_LH_VR_23_lo.GetBinContent(4,2)
    vals["M LH VR lo obs"] = h_LH_VR_23_lo.GetBinContent(4,1)
    vals["L LH VR lo STC"] = h_LH_VR_23_lo.GetBinContent(5,3)
    vals["L LH VR lo pre"] = h_LH_VR_23_lo.GetBinContent(5,2)
    vals["L LH VR lo obs"] = h_LH_VR_23_lo.GetBinContent(5,1)

    vals["P HL VR lo STC"] = h_HL_VR_4_lo.GetBinContent(1,3)
    vals["P HL VR lo pre"] = h_HL_VR_4_lo.GetBinContent(1,2)
    vals["P HL VR lo obs"] = h_HL_VR_4_lo.GetBinContent(1,1)
    vals["P3 HL VR lo STC"] = h_HL_VR_4_lo.GetBinContent(2,3)
    vals["P3 HL VR lo pre"] = h_HL_VR_4_lo.GetBinContent(2,2)
    vals["P3 HL VR lo obs"] = h_HL_VR_4_lo.GetBinContent(2,1)
    vals["P4 HL VR lo STC"] = h_HL_VR_4_lo.GetBinContent(3,3)
    vals["P4 HL VR lo pre"] = h_HL_VR_4_lo.GetBinContent(3,2)
    vals["P4 HL VR lo obs"] = h_HL_VR_4_lo.GetBinContent(3,1)
    vals["M HL VR lo STC"] = h_HL_VR_4_lo.GetBinContent(4,3)
    vals["M HL VR lo pre"] = h_HL_VR_4_lo.GetBinContent(4,2)
    vals["M HL VR lo obs"] = h_HL_VR_4_lo.GetBinContent(4,1)
    vals["L HL VR lo STC"] = h_HL_VR_4_lo.GetBinContent(5,3)
    vals["L HL VR lo pre"] = h_HL_VR_4_lo.GetBinContent(5,2)
    vals["L HL VR lo obs"] = h_HL_VR_4_lo.GetBinContent(5,1)

    vals["P HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinContent(1,3)
    vals["P HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinContent(1,2)
    vals["P HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinContent(1,1)
    vals["P3 HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinContent(2,3)
    vals["P3 HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinContent(2,2)
    vals["P3 HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinContent(2,1)
    vals["P4 HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinContent(3,3)
    vals["P4 HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinContent(3,2)
    vals["P4 HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinContent(3,1)
    vals["M HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinContent(4,3)
    vals["M HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinContent(4,2)
    vals["M HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinContent(4,1)
    vals["L HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinContent(5,3)
    vals["L HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinContent(5,2)
    vals["L HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinContent(5,1)

    vals["P HM VR lo STC"] = h_HM_VR_4_lo.GetBinContent(1,3)
    vals["P HM VR lo pre"] = h_HM_VR_4_lo.GetBinContent(1,2)
    vals["P HM VR lo obs"] = h_HM_VR_4_lo.GetBinContent(1,1)
    vals["P3 HM VR lo STC"] = h_HM_VR_4_lo.GetBinContent(2,3)
    vals["P3 HM VR lo pre"] = h_HM_VR_4_lo.GetBinContent(2,2)
    vals["P3 HM VR lo obs"] = h_HM_VR_4_lo.GetBinContent(2,1)
    vals["P4 HM VR lo STC"] = h_HM_VR_4_lo.GetBinContent(3,3)
    vals["P4 HM VR lo pre"] = h_HM_VR_4_lo.GetBinContent(3,2)
    vals["P4 HM VR lo obs"] = h_HM_VR_4_lo.GetBinContent(3,1)
    vals["M HM VR lo STC"] = h_HM_VR_4_lo.GetBinContent(4,3)
    vals["M HM VR lo pre"] = h_HM_VR_4_lo.GetBinContent(4,2)
    vals["M HM VR lo obs"] = h_HM_VR_4_lo.GetBinContent(4,1)
    vals["L HM VR lo STC"] = h_HM_VR_4_lo.GetBinContent(5,3)
    vals["L HM VR lo pre"] = h_HM_VR_4_lo.GetBinContent(5,2)
    vals["L HM VR lo obs"] = h_HM_VR_4_lo.GetBinContent(5,1)

    vals["P HH VR lo STC"] = h_HH_VR_4_lo.GetBinContent(1,3)
    vals["P HH VR lo pre"] = h_HH_VR_4_lo.GetBinContent(1,2)
    vals["P HH VR lo obs"] = h_HH_VR_4_lo.GetBinContent(1,1)
    vals["P3 HH VR lo STC"] = h_HH_VR_4_lo.GetBinContent(2,3)
    vals["P3 HH VR lo pre"] = h_HH_VR_4_lo.GetBinContent(2,2)
    vals["P3 HH VR lo obs"] = h_HH_VR_4_lo.GetBinContent(2,1)
    vals["P4 HH VR lo STC"] = h_HH_VR_4_lo.GetBinContent(3,3)
    vals["P4 HH VR lo pre"] = h_HH_VR_4_lo.GetBinContent(3,2)
    vals["P4 HH VR lo obs"] = h_HH_VR_4_lo.GetBinContent(3,1)
    vals["M HH VR lo STC"] = h_HH_VR_4_lo.GetBinContent(4,3)
    vals["M HH VR lo pre"] = h_HH_VR_4_lo.GetBinContent(4,2)
    vals["M HH VR lo obs"] = h_HH_VR_4_lo.GetBinContent(4,1)
    vals["L HH VR lo STC"] = h_HH_VR_4_lo.GetBinContent(5,3)
    vals["L HH VR lo pre"] = h_HH_VR_4_lo.GetBinContent(5,2)
    vals["L HH VR lo obs"] = h_HH_VR_4_lo.GetBinContent(5,1)

    # VR Errors
    errs["P LL VR STC"] = h_LL_VR_23.GetBinError(1,3)
    errs["P LL VR pre"] = h_LL_VR_23.GetBinError(1,2)
    errs["P LL VR obs"] = h_LL_VR_23.GetBinError(1,1)
    errs["P3 LL VR STC"] = h_LL_VR_23.GetBinError(2,3)
    errs["P3 LL VR pre"] = h_LL_VR_23.GetBinError(2,2)
    errs["P3 LL VR obs"] = h_LL_VR_23.GetBinError(2,1)
    errs["P4 LL VR STC"] = h_LL_VR_23.GetBinError(3,3)
    errs["P4 LL VR pre"] = h_LL_VR_23.GetBinError(3,2)
    errs["P4 LL VR obs"] = h_LL_VR_23.GetBinError(3,1)
    errs["M LL VR STC"] = h_LL_VR_23.GetBinError(4,3)
    errs["M LL VR pre"] = h_LL_VR_23.GetBinError(4,2)
    errs["M LL VR obs"] = h_LL_VR_23.GetBinError(4,1)
    errs["L LL VR STC"] = h_LL_VR_23.GetBinError(5,3)
    errs["L LL VR pre"] = h_LL_VR_23.GetBinError(5,2)
    errs["L LL VR obs"] = h_LL_VR_23.GetBinError(5,1)

    errs["P LLM VR STC"] = h_LLM_VR_23.GetBinError(1,3)
    errs["P LLM VR pre"] = h_LLM_VR_23.GetBinError(1,2)
    errs["P LLM VR obs"] = h_LLM_VR_23.GetBinError(1,1)
    errs["P3 LLM VR STC"] = h_LLM_VR_23.GetBinError(2,3)
    errs["P3 LLM VR pre"] = h_LLM_VR_23.GetBinError(2,2)
    errs["P3 LLM VR obs"] = h_LLM_VR_23.GetBinError(2,1)
    errs["P4 LLM VR STC"] = h_LLM_VR_23.GetBinError(3,3)
    errs["P4 LLM VR pre"] = h_LLM_VR_23.GetBinError(3,2)
    errs["P4 LLM VR obs"] = h_LLM_VR_23.GetBinError(3,1)
    errs["M LLM VR STC"] = h_LLM_VR_23.GetBinError(4,3)
    errs["M LLM VR pre"] = h_LLM_VR_23.GetBinError(4,2)
    errs["M LLM VR obs"] = h_LLM_VR_23.GetBinError(4,1)
    errs["L LLM VR STC"] = h_LLM_VR_23.GetBinError(5,3)
    errs["L LLM VR pre"] = h_LLM_VR_23.GetBinError(5,2)
    errs["L LLM VR obs"] = h_LLM_VR_23.GetBinError(5,1)

    errs["P LM VR STC"] = h_LM_VR_23.GetBinError(1,3)
    errs["P LM VR pre"] = h_LM_VR_23.GetBinError(1,2)
    errs["P LM VR obs"] = h_LM_VR_23.GetBinError(1,1)
    errs["P3 LM VR STC"] = h_LM_VR_23.GetBinError(2,3)
    errs["P3 LM VR pre"] = h_LM_VR_23.GetBinError(2,2)
    errs["P3 LM VR obs"] = h_LM_VR_23.GetBinError(2,1)
    errs["P4 LM VR STC"] = h_LM_VR_23.GetBinError(3,3)
    errs["P4 LM VR pre"] = h_LM_VR_23.GetBinError(3,2)
    errs["P4 LM VR obs"] = h_LM_VR_23.GetBinError(3,1)
    errs["M LM VR STC"] = h_LM_VR_23.GetBinError(4,3)
    errs["M LM VR pre"] = h_LM_VR_23.GetBinError(4,2)
    errs["M LM VR obs"] = h_LM_VR_23.GetBinError(4,1)
    errs["L LM VR STC"] = h_LM_VR_23.GetBinError(5,3)
    errs["L LM VR pre"] = h_LM_VR_23.GetBinError(5,2)
    errs["L LM VR obs"] = h_LM_VR_23.GetBinError(5,1)

    errs["P LH VR STC"] = h_LH_VR_23.GetBinError(1,3)
    errs["P LH VR pre"] = h_LH_VR_23.GetBinError(1,2)
    errs["P LH VR obs"] = h_LH_VR_23.GetBinError(1,1)
    errs["P3 LH VR STC"] = h_LH_VR_23.GetBinError(2,3)
    errs["P3 LH VR pre"] = h_LH_VR_23.GetBinError(2,2)
    errs["P3 LH VR obs"] = h_LH_VR_23.GetBinError(2,1)
    errs["P4 LH VR STC"] = h_LH_VR_23.GetBinError(3,3)
    errs["P4 LH VR pre"] = h_LH_VR_23.GetBinError(3,2)
    errs["P4 LH VR obs"] = h_LH_VR_23.GetBinError(3,1)
    errs["M LH VR STC"] = h_LH_VR_23.GetBinError(4,3)
    errs["M LH VR pre"] = h_LH_VR_23.GetBinError(4,2)
    errs["M LH VR obs"] = h_LH_VR_23.GetBinError(4,1)
    errs["L LH VR STC"] = h_LH_VR_23.GetBinError(5,3)
    errs["L LH VR pre"] = h_LH_VR_23.GetBinError(5,2)
    errs["L LH VR obs"] = h_LH_VR_23.GetBinError(5,1)

    errs["P HL VR STC"] = h_HL_VR_4.GetBinError(1,3)
    errs["P HL VR pre"] = h_HL_VR_4.GetBinError(1,2)
    errs["P HL VR obs"] = h_HL_VR_4.GetBinError(1,1)
    errs["P3 HL VR STC"] = h_HL_VR_4.GetBinError(2,3)
    errs["P3 HL VR pre"] = h_HL_VR_4.GetBinError(2,2)
    errs["P3 HL VR obs"] = h_HL_VR_4.GetBinError(2,1)
    errs["P4 HL VR STC"] = h_HL_VR_4.GetBinError(3,3)
    errs["P4 HL VR pre"] = h_HL_VR_4.GetBinError(3,2)
    errs["P4 HL VR obs"] = h_HL_VR_4.GetBinError(3,1)
    errs["M HL VR STC"] = h_HL_VR_4.GetBinError(4,3)
    errs["M HL VR pre"] = h_HL_VR_4.GetBinError(4,2)
    errs["M HL VR obs"] = h_HL_VR_4.GetBinError(4,1)
    errs["L HL VR STC"] = h_HL_VR_4.GetBinError(5,3)
    errs["L HL VR pre"] = h_HL_VR_4.GetBinError(5,2)
    errs["L HL VR obs"] = h_HL_VR_4.GetBinError(5,1)

    errs["P HLM VR STC"] = h_HLM_VR_4.GetBinError(1,3)
    errs["P HLM VR pre"] = h_HLM_VR_4.GetBinError(1,2)
    errs["P HLM VR obs"] = h_HLM_VR_4.GetBinError(1,1)
    errs["P3 HLM VR STC"] = h_HLM_VR_4.GetBinError(2,3)
    errs["P3 HLM VR pre"] = h_HLM_VR_4.GetBinError(2,2)
    errs["P3 HLM VR obs"] = h_HLM_VR_4.GetBinError(2,1)
    errs["P4 HLM VR STC"] = h_HLM_VR_4.GetBinError(3,3)
    errs["P4 HLM VR pre"] = h_HLM_VR_4.GetBinError(3,2)
    errs["P4 HLM VR obs"] = h_HLM_VR_4.GetBinError(3,1)
    errs["M HLM VR STC"] = h_HLM_VR_4.GetBinError(4,3)
    errs["M HLM VR pre"] = h_HLM_VR_4.GetBinError(4,2)
    errs["M HLM VR obs"] = h_HLM_VR_4.GetBinError(4,1)
    errs["L HLM VR STC"] = h_HLM_VR_4.GetBinError(5,3)
    errs["L HLM VR pre"] = h_HLM_VR_4.GetBinError(5,2)
    errs["L HLM VR obs"] = h_HLM_VR_4.GetBinError(5,1)

    errs["P HM VR STC"] = h_HM_VR_4.GetBinError(1,3)
    errs["P HM VR pre"] = h_HM_VR_4.GetBinError(1,2)
    errs["P HM VR obs"] = h_HM_VR_4.GetBinError(1,1)
    errs["P3 HM VR STC"] = h_HM_VR_4.GetBinError(2,3)
    errs["P3 HM VR pre"] = h_HM_VR_4.GetBinError(2,2)
    errs["P3 HM VR obs"] = h_HM_VR_4.GetBinError(2,1)
    errs["P4 HM VR STC"] = h_HM_VR_4.GetBinError(3,3)
    errs["P4 HM VR pre"] = h_HM_VR_4.GetBinError(3,2)
    errs["P4 HM VR obs"] = h_HM_VR_4.GetBinError(3,1)
    errs["M HM VR STC"] = h_HM_VR_4.GetBinError(4,3)
    errs["M HM VR pre"] = h_HM_VR_4.GetBinError(4,2)
    errs["M HM VR obs"] = h_HM_VR_4.GetBinError(4,1)
    errs["L HM VR STC"] = h_HM_VR_4.GetBinError(5,3)
    errs["L HM VR pre"] = h_HM_VR_4.GetBinError(5,2)
    errs["L HM VR obs"] = h_HM_VR_4.GetBinError(5,1)

    errs["P HH VR STC"] = h_HH_VR_4.GetBinError(1,3)
    errs["P HH VR pre"] = h_HH_VR_4.GetBinError(1,2)
    errs["P HH VR obs"] = h_HH_VR_4.GetBinError(1,1)
    errs["P3 HH VR STC"] = h_HH_VR_4.GetBinError(2,3)
    errs["P3 HH VR pre"] = h_HH_VR_4.GetBinError(2,2)
    errs["P3 HH VR obs"] = h_HH_VR_4.GetBinError(2,1)
    errs["P4 HH VR STC"] = h_HH_VR_4.GetBinError(3,3)
    errs["P4 HH VR pre"] = h_HH_VR_4.GetBinError(3,2)
    errs["P4 HH VR obs"] = h_HH_VR_4.GetBinError(3,1)
    errs["M HH VR STC"] = h_HH_VR_4.GetBinError(4,3)
    errs["M HH VR pre"] = h_HH_VR_4.GetBinError(4,2)
    errs["M HH VR obs"] = h_HH_VR_4.GetBinError(4,1)
    errs["L HH VR STC"] = h_HH_VR_4.GetBinError(5,3)
    errs["L HH VR pre"] = h_HH_VR_4.GetBinError(5,2)
    errs["L HH VR obs"] = h_HH_VR_4.GetBinError(5,1)

    # hi pt
    errs["P LL VR hi STC"] = h_LL_VR_23_hi.GetBinError(1,3)
    errs["P LL VR hi pre"] = h_LL_VR_23_hi.GetBinError(1,2)
    errs["P LL VR hi obs"] = h_LL_VR_23_hi.GetBinError(1,1)
    errs["P3 LL VR hi STC"] = h_LL_VR_23_hi.GetBinError(2,3)
    errs["P3 LL VR hi pre"] = h_LL_VR_23_hi.GetBinError(2,2)
    errs["P3 LL VR hi obs"] = h_LL_VR_23_hi.GetBinError(2,1)
    errs["P4 LL VR hi STC"] = h_LL_VR_23_hi.GetBinError(3,3)
    errs["P4 LL VR hi pre"] = h_LL_VR_23_hi.GetBinError(3,2)
    errs["P4 LL VR hi obs"] = h_LL_VR_23_hi.GetBinError(3,1)
    errs["M LL VR hi STC"] = h_LL_VR_23_hi.GetBinError(4,3)
    errs["M LL VR hi pre"] = h_LL_VR_23_hi.GetBinError(4,2)
    errs["M LL VR hi obs"] = h_LL_VR_23_hi.GetBinError(4,1)
    errs["L LL VR hi STC"] = h_LL_VR_23_hi.GetBinError(5,3)
    errs["L LL VR hi pre"] = h_LL_VR_23_hi.GetBinError(5,2)
    errs["L LL VR hi obs"] = h_LL_VR_23_hi.GetBinError(5,1)

    errs["P LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinError(1,3)
    errs["P LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinError(1,2)
    errs["P LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinError(1,1)
    errs["P3 LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinError(2,3)
    errs["P3 LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinError(2,2)
    errs["P3 LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinError(2,1)
    errs["P4 LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinError(3,3)
    errs["P4 LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinError(3,2)
    errs["P4 LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinError(3,1)
    errs["M LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinError(4,3)
    errs["M LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinError(4,2)
    errs["M LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinError(4,1)
    errs["L LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinError(5,3)
    errs["L LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinError(5,2)
    errs["L LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinError(5,1)

    errs["P LM VR hi STC"] = h_LM_VR_23_hi.GetBinError(1,3)
    errs["P LM VR hi pre"] = h_LM_VR_23_hi.GetBinError(1,2)
    errs["P LM VR hi obs"] = h_LM_VR_23_hi.GetBinError(1,1)
    errs["P3 LM VR hi STC"] = h_LM_VR_23_hi.GetBinError(2,3)
    errs["P3 LM VR hi pre"] = h_LM_VR_23_hi.GetBinError(2,2)
    errs["P3 LM VR hi obs"] = h_LM_VR_23_hi.GetBinError(2,1)
    errs["P4 LM VR hi STC"] = h_LM_VR_23_hi.GetBinError(3,3)
    errs["P4 LM VR hi pre"] = h_LM_VR_23_hi.GetBinError(3,2)
    errs["P4 LM VR hi obs"] = h_LM_VR_23_hi.GetBinError(3,1)
    errs["M LM VR hi STC"] = h_LM_VR_23_hi.GetBinError(4,3)
    errs["M LM VR hi pre"] = h_LM_VR_23_hi.GetBinError(4,2)
    errs["M LM VR hi obs"] = h_LM_VR_23_hi.GetBinError(4,1)
    errs["L LM VR hi STC"] = h_LM_VR_23_hi.GetBinError(5,3)
    errs["L LM VR hi pre"] = h_LM_VR_23_hi.GetBinError(5,2)
    errs["L LM VR hi obs"] = h_LM_VR_23_hi.GetBinError(5,1)

    errs["P LH VR hi STC"] = h_LH_VR_23_hi.GetBinError(1,3)
    errs["P LH VR hi pre"] = h_LH_VR_23_hi.GetBinError(1,2)
    errs["P LH VR hi obs"] = h_LH_VR_23_hi.GetBinError(1,1)
    errs["P3 LH VR hi STC"] = h_LH_VR_23_hi.GetBinError(2,3)
    errs["P3 LH VR hi pre"] = h_LH_VR_23_hi.GetBinError(2,2)
    errs["P3 LH VR hi obs"] = h_LH_VR_23_hi.GetBinError(2,1)
    errs["P4 LH VR hi STC"] = h_LH_VR_23_hi.GetBinError(3,3)
    errs["P4 LH VR hi pre"] = h_LH_VR_23_hi.GetBinError(3,2)
    errs["P4 LH VR hi obs"] = h_LH_VR_23_hi.GetBinError(3,1)
    errs["M LH VR hi STC"] = h_LH_VR_23_hi.GetBinError(4,3)
    errs["M LH VR hi pre"] = h_LH_VR_23_hi.GetBinError(4,2)
    errs["M LH VR hi obs"] = h_LH_VR_23_hi.GetBinError(4,1)
    errs["L LH VR hi STC"] = h_LH_VR_23_hi.GetBinError(5,3)
    errs["L LH VR hi pre"] = h_LH_VR_23_hi.GetBinError(5,2)
    errs["L LH VR hi obs"] = h_LH_VR_23_hi.GetBinError(5,1)

    errs["P HL VR hi STC"] = h_HL_VR_4_hi.GetBinError(1,3)
    errs["P HL VR hi pre"] = h_HL_VR_4_hi.GetBinError(1,2)
    errs["P HL VR hi obs"] = h_HL_VR_4_hi.GetBinError(1,1)
    errs["P3 HL VR hi STC"] = h_HL_VR_4_hi.GetBinError(2,3)
    errs["P3 HL VR hi pre"] = h_HL_VR_4_hi.GetBinError(2,2)
    errs["P3 HL VR hi obs"] = h_HL_VR_4_hi.GetBinError(2,1)
    errs["P4 HL VR hi STC"] = h_HL_VR_4_hi.GetBinError(3,3)
    errs["P4 HL VR hi pre"] = h_HL_VR_4_hi.GetBinError(3,2)
    errs["P4 HL VR hi obs"] = h_HL_VR_4_hi.GetBinError(3,1)
    errs["M HL VR hi STC"] = h_HL_VR_4_hi.GetBinError(4,3)
    errs["M HL VR hi pre"] = h_HL_VR_4_hi.GetBinError(4,2)
    errs["M HL VR hi obs"] = h_HL_VR_4_hi.GetBinError(4,1)
    errs["L HL VR hi STC"] = h_HL_VR_4_hi.GetBinError(5,3)
    errs["L HL VR hi pre"] = h_HL_VR_4_hi.GetBinError(5,2)
    errs["L HL VR hi obs"] = h_HL_VR_4_hi.GetBinError(5,1)

    errs["P HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinError(1,3)
    errs["P HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinError(1,2)
    errs["P HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinError(1,1)
    errs["P3 HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinError(2,3)
    errs["P3 HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinError(2,2)
    errs["P3 HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinError(2,1)
    errs["P4 HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinError(3,3)
    errs["P4 HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinError(3,2)
    errs["P4 HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinError(3,1)
    errs["M HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinError(4,3)
    errs["M HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinError(4,2)
    errs["M HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinError(4,1)
    errs["L HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinError(5,3)
    errs["L HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinError(5,2)
    errs["L HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinError(5,1)

    errs["P HM VR hi STC"] = h_HM_VR_4_hi.GetBinError(1,3)
    errs["P HM VR hi pre"] = h_HM_VR_4_hi.GetBinError(1,2)
    errs["P HM VR hi obs"] = h_HM_VR_4_hi.GetBinError(1,1)
    errs["P3 HM VR hi STC"] = h_HM_VR_4_hi.GetBinError(2,3)
    errs["P3 HM VR hi pre"] = h_HM_VR_4_hi.GetBinError(2,2)
    errs["P3 HM VR hi obs"] = h_HM_VR_4_hi.GetBinError(2,1)
    errs["P4 HM VR hi STC"] = h_HM_VR_4_hi.GetBinError(3,3)
    errs["P4 HM VR hi pre"] = h_HM_VR_4_hi.GetBinError(3,2)
    errs["P4 HM VR hi obs"] = h_HM_VR_4_hi.GetBinError(3,1)
    errs["M HM VR hi STC"] = h_HM_VR_4_hi.GetBinError(4,3)
    errs["M HM VR hi pre"] = h_HM_VR_4_hi.GetBinError(4,2)
    errs["M HM VR hi obs"] = h_HM_VR_4_hi.GetBinError(4,1)
    errs["L HM VR hi STC"] = h_HM_VR_4_hi.GetBinError(5,3)
    errs["L HM VR hi pre"] = h_HM_VR_4_hi.GetBinError(5,2)
    errs["L HM VR hi obs"] = h_HM_VR_4_hi.GetBinError(5,1)

    errs["P HH VR hi STC"] = h_HH_VR_4_hi.GetBinError(1,3)
    errs["P HH VR hi pre"] = h_HH_VR_4_hi.GetBinError(1,2)
    errs["P HH VR hi obs"] = h_HH_VR_4_hi.GetBinError(1,1)
    errs["P3 HH VR hi STC"] = h_HH_VR_4_hi.GetBinError(2,3)
    errs["P3 HH VR hi pre"] = h_HH_VR_4_hi.GetBinError(2,2)
    errs["P3 HH VR hi obs"] = h_HH_VR_4_hi.GetBinError(2,1)
    errs["P4 HH VR hi STC"] = h_HH_VR_4_hi.GetBinError(3,3)
    errs["P4 HH VR hi pre"] = h_HH_VR_4_hi.GetBinError(3,2)
    errs["P4 HH VR hi obs"] = h_HH_VR_4_hi.GetBinError(3,1)
    errs["M HH VR hi STC"] = h_HH_VR_4_hi.GetBinError(4,3)
    errs["M HH VR hi pre"] = h_HH_VR_4_hi.GetBinError(4,2)
    errs["M HH VR hi obs"] = h_HH_VR_4_hi.GetBinError(4,1)
    errs["L HH VR hi STC"] = h_HH_VR_4_hi.GetBinError(5,3)
    errs["L HH VR hi pre"] = h_HH_VR_4_hi.GetBinError(5,2)
    errs["L HH VR hi obs"] = h_HH_VR_4_hi.GetBinError(5,1)

    # lo pt
    errs["P LL VR lo STC"] = h_LL_VR_23_lo.GetBinError(1,3)
    errs["P LL VR lo pre"] = h_LL_VR_23_lo.GetBinError(1,2)
    errs["P LL VR lo obs"] = h_LL_VR_23_lo.GetBinError(1,1)
    errs["P3 LL VR lo STC"] = h_LL_VR_23_lo.GetBinError(2,3)
    errs["P3 LL VR lo pre"] = h_LL_VR_23_lo.GetBinError(2,2)
    errs["P3 LL VR lo obs"] = h_LL_VR_23_lo.GetBinError(2,1)
    errs["P4 LL VR lo STC"] = h_LL_VR_23_lo.GetBinError(3,3)
    errs["P4 LL VR lo pre"] = h_LL_VR_23_lo.GetBinError(3,2)
    errs["P4 LL VR lo obs"] = h_LL_VR_23_lo.GetBinError(3,1)
    errs["M LL VR lo STC"] = h_LL_VR_23_lo.GetBinError(4,3)
    errs["M LL VR lo pre"] = h_LL_VR_23_lo.GetBinError(4,2)
    errs["M LL VR lo obs"] = h_LL_VR_23_lo.GetBinError(4,1)
    errs["L LL VR lo STC"] = h_LL_VR_23_lo.GetBinError(5,3)
    errs["L LL VR lo pre"] = h_LL_VR_23_lo.GetBinError(5,2)
    errs["L LL VR lo obs"] = h_LL_VR_23_lo.GetBinError(5,1)

    errs["P LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinError(1,3)
    errs["P LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinError(1,2)
    errs["P LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinError(1,1)
    errs["P3 LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinError(2,3)
    errs["P3 LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinError(2,2)
    errs["P3 LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinError(2,1)
    errs["P4 LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinError(3,3)
    errs["P4 LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinError(3,2)
    errs["P4 LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinError(3,1)
    errs["M LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinError(4,3)
    errs["M LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinError(4,2)
    errs["M LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinError(4,1)
    errs["L LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinError(5,3)
    errs["L LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinError(5,2)
    errs["L LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinError(5,1)

    errs["P LM VR lo STC"] = h_LM_VR_23_lo.GetBinError(1,3)
    errs["P LM VR lo pre"] = h_LM_VR_23_lo.GetBinError(1,2)
    errs["P LM VR lo obs"] = h_LM_VR_23_lo.GetBinError(1,1)
    errs["P3 LM VR lo STC"] = h_LM_VR_23_lo.GetBinError(2,3)
    errs["P3 LM VR lo pre"] = h_LM_VR_23_lo.GetBinError(2,2)
    errs["P3 LM VR lo obs"] = h_LM_VR_23_lo.GetBinError(2,1)
    errs["P4 LM VR lo STC"] = h_LM_VR_23_lo.GetBinError(3,3)
    errs["P4 LM VR lo pre"] = h_LM_VR_23_lo.GetBinError(3,2)
    errs["P4 LM VR lo obs"] = h_LM_VR_23_lo.GetBinError(3,1)
    errs["M LM VR lo STC"] = h_LM_VR_23_lo.GetBinError(4,3)
    errs["M LM VR lo pre"] = h_LM_VR_23_lo.GetBinError(4,2)
    errs["M LM VR lo obs"] = h_LM_VR_23_lo.GetBinError(4,1)
    errs["L LM VR lo STC"] = h_LM_VR_23_lo.GetBinError(5,3)
    errs["L LM VR lo pre"] = h_LM_VR_23_lo.GetBinError(5,2)
    errs["L LM VR lo obs"] = h_LM_VR_23_lo.GetBinError(5,1)

    errs["P LH VR lo STC"] = h_LH_VR_23_lo.GetBinError(1,3)
    errs["P LH VR lo pre"] = h_LH_VR_23_lo.GetBinError(1,2)
    errs["P LH VR lo obs"] = h_LH_VR_23_lo.GetBinError(1,1)
    errs["P3 LH VR lo STC"] = h_LH_VR_23_lo.GetBinError(2,3)
    errs["P3 LH VR lo pre"] = h_LH_VR_23_lo.GetBinError(2,2)
    errs["P3 LH VR lo obs"] = h_LH_VR_23_lo.GetBinError(2,1)
    errs["P4 LH VR lo STC"] = h_LH_VR_23_lo.GetBinError(3,3)
    errs["P4 LH VR lo pre"] = h_LH_VR_23_lo.GetBinError(3,2)
    errs["P4 LH VR lo obs"] = h_LH_VR_23_lo.GetBinError(3,1)
    errs["M LH VR lo STC"] = h_LH_VR_23_lo.GetBinError(4,3)
    errs["M LH VR lo pre"] = h_LH_VR_23_lo.GetBinError(4,2)
    errs["M LH VR lo obs"] = h_LH_VR_23_lo.GetBinError(4,1)
    errs["L LH VR lo STC"] = h_LH_VR_23_lo.GetBinError(5,3)
    errs["L LH VR lo pre"] = h_LH_VR_23_lo.GetBinError(5,2)
    errs["L LH VR lo obs"] = h_LH_VR_23_lo.GetBinError(5,1)

    errs["P HL VR lo STC"] = h_HL_VR_4_lo.GetBinError(1,3)
    errs["P HL VR lo pre"] = h_HL_VR_4_lo.GetBinError(1,2)
    errs["P HL VR lo obs"] = h_HL_VR_4_lo.GetBinError(1,1)
    errs["P3 HL VR lo STC"] = h_HL_VR_4_lo.GetBinError(2,3)
    errs["P3 HL VR lo pre"] = h_HL_VR_4_lo.GetBinError(2,2)
    errs["P3 HL VR lo obs"] = h_HL_VR_4_lo.GetBinError(2,1)
    errs["P4 HL VR lo STC"] = h_HL_VR_4_lo.GetBinError(3,3)
    errs["P4 HL VR lo pre"] = h_HL_VR_4_lo.GetBinError(3,2)
    errs["P4 HL VR lo obs"] = h_HL_VR_4_lo.GetBinError(3,1)
    errs["M HL VR lo STC"] = h_HL_VR_4_lo.GetBinError(4,3)
    errs["M HL VR lo pre"] = h_HL_VR_4_lo.GetBinError(4,2)
    errs["M HL VR lo obs"] = h_HL_VR_4_lo.GetBinError(4,1)
    errs["L HL VR lo STC"] = h_HL_VR_4_lo.GetBinError(5,3)
    errs["L HL VR lo pre"] = h_HL_VR_4_lo.GetBinError(5,2)
    errs["L HL VR lo obs"] = h_HL_VR_4_lo.GetBinError(5,1)

    errs["P HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinError(1,3)
    errs["P HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinError(1,2)
    errs["P HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinError(1,1)
    errs["P3 HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinError(2,3)
    errs["P3 HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinError(2,2)
    errs["P3 HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinError(2,1)
    errs["P4 HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinError(3,3)
    errs["P4 HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinError(3,2)
    errs["P4 HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinError(3,1)
    errs["M HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinError(4,3)
    errs["M HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinError(4,2)
    errs["M HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinError(4,1)
    errs["L HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinError(5,3)
    errs["L HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinError(5,2)
    errs["L HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinError(5,1)

    errs["P HM VR lo STC"] = h_HM_VR_4_lo.GetBinError(1,3)
    errs["P HM VR lo pre"] = h_HM_VR_4_lo.GetBinError(1,2)
    errs["P HM VR lo obs"] = h_HM_VR_4_lo.GetBinError(1,1)
    errs["P3 HM VR lo STC"] = h_HM_VR_4_lo.GetBinError(2,3)
    errs["P3 HM VR lo pre"] = h_HM_VR_4_lo.GetBinError(2,2)
    errs["P3 HM VR lo obs"] = h_HM_VR_4_lo.GetBinError(2,1)
    errs["P4 HM VR lo STC"] = h_HM_VR_4_lo.GetBinError(3,3)
    errs["P4 HM VR lo pre"] = h_HM_VR_4_lo.GetBinError(3,2)
    errs["P4 HM VR lo obs"] = h_HM_VR_4_lo.GetBinError(3,1)
    errs["M HM VR lo STC"] = h_HM_VR_4_lo.GetBinError(4,3)
    errs["M HM VR lo pre"] = h_HM_VR_4_lo.GetBinError(4,2)
    errs["M HM VR lo obs"] = h_HM_VR_4_lo.GetBinError(4,1)
    errs["L HM VR lo STC"] = h_HM_VR_4_lo.GetBinError(5,3)
    errs["L HM VR lo pre"] = h_HM_VR_4_lo.GetBinError(5,2)
    errs["L HM VR lo obs"] = h_HM_VR_4_lo.GetBinError(5,1)

    errs["P HH VR lo STC"] = h_HH_VR_4_lo.GetBinError(1,3)
    errs["P HH VR lo pre"] = h_HH_VR_4_lo.GetBinError(1,2)
    errs["P HH VR lo obs"] = h_HH_VR_4_lo.GetBinError(1,1)
    errs["P3 HH VR lo STC"] = h_HH_VR_4_lo.GetBinError(2,3)
    errs["P3 HH VR lo pre"] = h_HH_VR_4_lo.GetBinError(2,2)
    errs["P3 HH VR lo obs"] = h_HH_VR_4_lo.GetBinError(2,1)
    errs["P4 HH VR lo STC"] = h_HH_VR_4_lo.GetBinError(3,3)
    errs["P4 HH VR lo pre"] = h_HH_VR_4_lo.GetBinError(3,2)
    errs["P4 HH VR lo obs"] = h_HH_VR_4_lo.GetBinError(3,1)
    errs["M HH VR lo STC"] = h_HH_VR_4_lo.GetBinError(4,3)
    errs["M HH VR lo pre"] = h_HH_VR_4_lo.GetBinError(4,2)
    errs["M HH VR lo obs"] = h_HH_VR_4_lo.GetBinError(4,1)
    errs["L HH VR lo STC"] = h_HH_VR_4_lo.GetBinError(5,3)
    errs["L HH VR lo pre"] = h_HH_VR_4_lo.GetBinError(5,2)
    errs["L HH VR lo obs"] = h_HH_VR_4_lo.GetBinError(5,1)

    # VR systs
    if h_LL_VR_23_ns != None:
        systs["P LL VR"] = h_LL_VR_23_ns.GetBinContent(1)
        systs["P3 LL VR"] = h_LL_VR_23_ns.GetBinContent(2)
        systs["P4 LL VR"] = h_LL_VR_23_ns.GetBinContent(3)
        systs["M LL VR"] = h_LL_VR_23_ns.GetBinContent(4)
        systs["L LL VR"] = h_LL_VR_23_ns.GetBinContent(5)
        systs["P LLM VR"] = h_LLM_VR_23_ns.GetBinContent(1)
        systs["P3 LLM VR"] = h_LLM_VR_23_ns.GetBinContent(2)
        systs["P4 LLM VR"] = h_LLM_VR_23_ns.GetBinContent(3)
        systs["M LLM VR"] = h_LLM_VR_23_ns.GetBinContent(4)
        systs["L LLM VR"] = h_LLM_VR_23_ns.GetBinContent(5)
        systs["P LM VR"] = h_LM_VR_23_ns.GetBinContent(1)
        systs["P3 LM VR"] = h_LM_VR_23_ns.GetBinContent(2)
        systs["P4 LM VR"] = h_LM_VR_23_ns.GetBinContent(3)
        systs["M LM VR"] = h_LM_VR_23_ns.GetBinContent(4)
        systs["L LM VR"] = h_LM_VR_23_ns.GetBinContent(5)
        systs["P LH VR"] = h_LH_VR_23_ns.GetBinContent(1)
        systs["P3 LH VR"] = h_LH_VR_23_ns.GetBinContent(2)
        systs["P4 LH VR"] = h_LH_VR_23_ns.GetBinContent(3)
        systs["M LH VR"] = h_LH_VR_23_ns.GetBinContent(4)
        systs["L LH VR"] = h_LH_VR_23_ns.GetBinContent(5)
        systs["P HL VR"] = h_HL_VR_4_ns.GetBinContent(1)
        systs["P3 HL VR"] = h_HL_VR_4_ns.GetBinContent(2)
        systs["P4 HL VR"] = h_HL_VR_4_ns.GetBinContent(3)
        systs["M HL VR"] = h_HL_VR_4_ns.GetBinContent(4)
        systs["L HL VR"] = h_HL_VR_4_ns.GetBinContent(5)
        systs["P HLM VR"] = h_HLM_VR_4_ns.GetBinContent(1)
        systs["P3 HLM VR"] = h_HLM_VR_4_ns.GetBinContent(2)
        systs["P4 HLM VR"] = h_HLM_VR_4_ns.GetBinContent(3)
        systs["M HLM VR"] = h_HLM_VR_4_ns.GetBinContent(4)
        systs["L HLM VR"] = h_HLM_VR_4_ns.GetBinContent(5)
        systs["P HM VR"] = h_HM_VR_4_ns.GetBinContent(1)
        systs["P3 HM VR"] = h_HM_VR_4_ns.GetBinContent(2)
        systs["P4 HM VR"] = h_HM_VR_4_ns.GetBinContent(3)
        systs["M HM VR"] = h_HM_VR_4_ns.GetBinContent(4)
        systs["L HM VR"] = h_HM_VR_4_ns.GetBinContent(5)
        systs["P HH VR"] = h_HH_VR_4_ns.GetBinContent(1)
        systs["P3 HH VR"] = h_HH_VR_4_ns.GetBinContent(2)
        systs["P4 HH VR"] = h_HH_VR_4_ns.GetBinContent(3)
        systs["M HH VR"] = h_HH_VR_4_ns.GetBinContent(4)
        systs["L HH VR"] = h_HH_VR_4_ns.GetBinContent(5)
        systs["P LL VR hi"] = h_LL_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LL VR hi"] = h_LL_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LL VR hi"] = h_LL_VR_23_hi_ns.GetBinContent(3)
        systs["M LL VR hi"] = h_LL_VR_23_hi_ns.GetBinContent(4)
        systs["L LL VR hi"] = h_LL_VR_23_hi_ns.GetBinContent(5)
        systs["P LLM VR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LLM VR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LLM VR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(3)
        systs["M LLM VR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(4)
        systs["L LLM VR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(5)
        systs["P LM VR hi"] = h_LM_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LM VR hi"] = h_LM_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LM VR hi"] = h_LM_VR_23_hi_ns.GetBinContent(3)
        systs["M LM VR hi"] = h_LM_VR_23_hi_ns.GetBinContent(4)
        systs["L LM VR hi"] = h_LM_VR_23_hi_ns.GetBinContent(5)
        systs["P LH VR hi"] = h_LH_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LH VR hi"] = h_LH_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LH VR hi"] = h_LH_VR_23_hi_ns.GetBinContent(3)
        systs["M LH VR hi"] = h_LH_VR_23_hi_ns.GetBinContent(4)
        systs["L LH VR hi"] = h_LH_VR_23_hi_ns.GetBinContent(5)
        systs["P HL VR hi"] = h_HL_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HL VR hi"] = h_HL_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HL VR hi"] = h_HL_VR_4_hi_ns.GetBinContent(3)
        systs["M HL VR hi"] = h_HL_VR_4_hi_ns.GetBinContent(4)
        systs["L HL VR hi"] = h_HL_VR_4_hi_ns.GetBinContent(5)
        systs["P HLM VR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HLM VR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HLM VR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(3)
        systs["M HLM VR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(4)
        systs["L HLM VR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(5)
        systs["P HM VR hi"] = h_HM_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HM VR hi"] = h_HM_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HM VR hi"] = h_HM_VR_4_hi_ns.GetBinContent(3)
        systs["M HM VR hi"] = h_HM_VR_4_hi_ns.GetBinContent(4)
        systs["L HM VR hi"] = h_HM_VR_4_hi_ns.GetBinContent(5)
        systs["P HH VR hi"] = h_HH_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HH VR hi"] = h_HH_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HH VR hi"] = h_HH_VR_4_hi_ns.GetBinContent(3)
        systs["M HH VR hi"] = h_HH_VR_4_hi_ns.GetBinContent(4)
        systs["L HH VR hi"] = h_HH_VR_4_hi_ns.GetBinContent(5)
        systs["P LL VR lo"] = h_LL_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LL VR lo"] = h_LL_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LL VR lo"] = h_LL_VR_23_lo_ns.GetBinContent(3)
        systs["M LL VR lo"] = h_LL_VR_23_lo_ns.GetBinContent(4)
        systs["L LL VR lo"] = h_LL_VR_23_lo_ns.GetBinContent(5)
        systs["P LLM VR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LLM VR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LLM VR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(3)
        systs["M LLM VR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(4)
        systs["L LLM VR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(5)
        systs["P LM VR lo"] = h_LM_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LM VR lo"] = h_LM_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LM VR lo"] = h_LM_VR_23_lo_ns.GetBinContent(3)
        systs["M LM VR lo"] = h_LM_VR_23_lo_ns.GetBinContent(4)
        systs["L LM VR lo"] = h_LM_VR_23_lo_ns.GetBinContent(5)
        systs["P LH VR lo"] = h_LH_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LH VR lo"] = h_LH_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LH VR lo"] = h_LH_VR_23_lo_ns.GetBinContent(3)
        systs["M LH VR lo"] = h_LH_VR_23_lo_ns.GetBinContent(4)
        systs["L LH VR lo"] = h_LH_VR_23_lo_ns.GetBinContent(5)
        systs["P HL VR lo"] = h_HL_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HL VR lo"] = h_HL_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HL VR lo"] = h_HL_VR_4_lo_ns.GetBinContent(3)
        systs["M HL VR lo"] = h_HL_VR_4_lo_ns.GetBinContent(4)
        systs["L HL VR lo"] = h_HL_VR_4_lo_ns.GetBinContent(5)
        systs["P HLM VR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HLM VR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HLM VR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(3)
        systs["M HLM VR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(4)
        systs["L HLM VR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(5)
        systs["P HM VR lo"] = h_HM_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HM VR lo"] = h_HM_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HM VR lo"] = h_HM_VR_4_lo_ns.GetBinContent(3)
        systs["M HM VR lo"] = h_HM_VR_4_lo_ns.GetBinContent(4)
        systs["L HM VR lo"] = h_HM_VR_4_lo_ns.GetBinContent(5)
        systs["P HH VR lo"] = h_HH_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HH VR lo"] = h_HH_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HH VR lo"] = h_HH_VR_4_lo_ns.GetBinContent(3)
        systs["M HH VR lo"] = h_HH_VR_4_lo_ns.GetBinContent(4)
        systs["L HH VR lo"] = h_HH_VR_4_lo_ns.GetBinContent(5)

    # SR vals
    vals["P LL SR STC"] = h_LL_SR_23.GetBinContent(1,3)
    vals["P LL SR pre"] = h_LL_SR_23.GetBinContent(1,2)
    vals["P LL SR obs"] = h_LL_SR_23.GetBinContent(1,1)
    vals["P3 LL SR STC"] = h_LL_SR_23.GetBinContent(2,3)
    vals["P3 LL SR pre"] = h_LL_SR_23.GetBinContent(2,2)
    vals["P3 LL SR obs"] = h_LL_SR_23.GetBinContent(2,1)
    vals["P4 LL SR STC"] = h_LL_SR_23.GetBinContent(3,3)
    vals["P4 LL SR pre"] = h_LL_SR_23.GetBinContent(3,2)
    vals["P4 LL SR obs"] = h_LL_SR_23.GetBinContent(3,1)
    vals["M LL SR STC"] = h_LL_SR_23.GetBinContent(4,3)
    vals["M LL SR pre"] = h_LL_SR_23.GetBinContent(4,2)
    vals["M LL SR obs"] = h_LL_SR_23.GetBinContent(4,1)
    vals["L LL SR STC"] = h_LL_SR_23.GetBinContent(5,3)
    vals["L LL SR pre"] = h_LL_SR_23.GetBinContent(5,2)
    vals["L LL SR obs"] = h_LL_SR_23.GetBinContent(5,1)

    vals["P LLM SR STC"] = h_LLM_SR_23.GetBinContent(1,3)
    vals["P LLM SR pre"] = h_LLM_SR_23.GetBinContent(1,2)
    vals["P LLM SR obs"] = h_LLM_SR_23.GetBinContent(1,1)
    vals["P3 LLM SR STC"] = h_LLM_SR_23.GetBinContent(2,3)
    vals["P3 LLM SR pre"] = h_LLM_SR_23.GetBinContent(2,2)
    vals["P3 LLM SR obs"] = h_LLM_SR_23.GetBinContent(2,1)
    vals["P4 LLM SR STC"] = h_LLM_SR_23.GetBinContent(3,3)
    vals["P4 LLM SR pre"] = h_LLM_SR_23.GetBinContent(3,2)
    vals["P4 LLM SR obs"] = h_LLM_SR_23.GetBinContent(3,1)
    vals["M LLM SR STC"] = h_LLM_SR_23.GetBinContent(4,3)
    vals["M LLM SR pre"] = h_LLM_SR_23.GetBinContent(4,2)
    vals["M LLM SR obs"] = h_LLM_SR_23.GetBinContent(4,1)
    vals["L LLM SR STC"] = h_LLM_SR_23.GetBinContent(5,3)
    vals["L LLM SR pre"] = h_LLM_SR_23.GetBinContent(5,2)
    vals["L LLM SR obs"] = h_LLM_SR_23.GetBinContent(5,1)

    vals["P LM SR STC"] = h_LM_SR_23.GetBinContent(1,3)
    vals["P LM SR pre"] = h_LM_SR_23.GetBinContent(1,2)
    vals["P LM SR obs"] = h_LM_SR_23.GetBinContent(1,1)
    vals["P3 LM SR STC"] = h_LM_SR_23.GetBinContent(2,3)
    vals["P3 LM SR pre"] = h_LM_SR_23.GetBinContent(2,2)
    vals["P3 LM SR obs"] = h_LM_SR_23.GetBinContent(2,1)
    vals["P4 LM SR STC"] = h_LM_SR_23.GetBinContent(3,3)
    vals["P4 LM SR pre"] = h_LM_SR_23.GetBinContent(3,2)
    vals["P4 LM SR obs"] = h_LM_SR_23.GetBinContent(3,1)
    vals["M LM SR STC"] = h_LM_SR_23.GetBinContent(4,3)
    vals["M LM SR pre"] = h_LM_SR_23.GetBinContent(4,2)
    vals["M LM SR obs"] = h_LM_SR_23.GetBinContent(4,1)
    vals["L LM SR STC"] = h_LM_SR_23.GetBinContent(5,3)
    vals["L LM SR pre"] = h_LM_SR_23.GetBinContent(5,2)
    vals["L LM SR obs"] = h_LM_SR_23.GetBinContent(5,1)

    vals["P LH SR STC"] = h_LH_SR_23.GetBinContent(1,3)
    vals["P LH SR pre"] = h_LH_SR_23.GetBinContent(1,2)
    vals["P LH SR obs"] = h_LH_SR_23.GetBinContent(1,1)
    vals["P3 LH SR STC"] = h_LH_SR_23.GetBinContent(2,3)
    vals["P3 LH SR pre"] = h_LH_SR_23.GetBinContent(2,2)
    vals["P3 LH SR obs"] = h_LH_SR_23.GetBinContent(2,1)
    vals["P4 LH SR STC"] = h_LH_SR_23.GetBinContent(3,3)
    vals["P4 LH SR pre"] = h_LH_SR_23.GetBinContent(3,2)
    vals["P4 LH SR obs"] = h_LH_SR_23.GetBinContent(3,1)
    vals["M LH SR STC"] = h_LH_SR_23.GetBinContent(4,3)
    vals["M LH SR pre"] = h_LH_SR_23.GetBinContent(4,2)
    vals["M LH SR obs"] = h_LH_SR_23.GetBinContent(4,1)
    vals["L LH SR STC"] = h_LH_SR_23.GetBinContent(5,3)
    vals["L LH SR pre"] = h_LH_SR_23.GetBinContent(5,2)
    vals["L LH SR obs"] = h_LH_SR_23.GetBinContent(5,1)

    vals["P HL SR STC"] = h_HL_SR_4.GetBinContent(1,3)
    vals["P HL SR pre"] = h_HL_SR_4.GetBinContent(1,2)
    vals["P HL SR obs"] = h_HL_SR_4.GetBinContent(1,1)
    vals["P3 HL SR STC"] = h_HL_SR_4.GetBinContent(2,3)
    vals["P3 HL SR pre"] = h_HL_SR_4.GetBinContent(2,2)
    vals["P3 HL SR obs"] = h_HL_SR_4.GetBinContent(2,1)
    vals["P4 HL SR STC"] = h_HL_SR_4.GetBinContent(3,3)
    vals["P4 HL SR pre"] = h_HL_SR_4.GetBinContent(3,2)
    vals["P4 HL SR obs"] = h_HL_SR_4.GetBinContent(3,1)
    vals["M HL SR STC"] = h_HL_SR_4.GetBinContent(4,3)
    vals["M HL SR pre"] = h_HL_SR_4.GetBinContent(4,2)
    vals["M HL SR obs"] = h_HL_SR_4.GetBinContent(4,1)
    vals["L HL SR STC"] = h_HL_SR_4.GetBinContent(5,3)
    vals["L HL SR pre"] = h_HL_SR_4.GetBinContent(5,2)
    vals["L HL SR obs"] = h_HL_SR_4.GetBinContent(5,1)

    vals["P HLM SR STC"] = h_HLM_SR_4.GetBinContent(1,3)
    vals["P HLM SR pre"] = h_HLM_SR_4.GetBinContent(1,2)
    vals["P HLM SR obs"] = h_HLM_SR_4.GetBinContent(1,1)
    vals["P3 HLM SR STC"] = h_HLM_SR_4.GetBinContent(2,3)
    vals["P3 HLM SR pre"] = h_HLM_SR_4.GetBinContent(2,2)
    vals["P3 HLM SR obs"] = h_HLM_SR_4.GetBinContent(2,1)
    vals["P4 HLM SR STC"] = h_HLM_SR_4.GetBinContent(3,3)
    vals["P4 HLM SR pre"] = h_HLM_SR_4.GetBinContent(3,2)
    vals["P4 HLM SR obs"] = h_HLM_SR_4.GetBinContent(3,1)
    vals["M HLM SR STC"] = h_HLM_SR_4.GetBinContent(4,3)
    vals["M HLM SR pre"] = h_HLM_SR_4.GetBinContent(4,2)
    vals["M HLM SR obs"] = h_HLM_SR_4.GetBinContent(4,1)
    vals["L HLM SR STC"] = h_HLM_SR_4.GetBinContent(5,3)
    vals["L HLM SR pre"] = h_HLM_SR_4.GetBinContent(5,2)
    vals["L HLM SR obs"] = h_HLM_SR_4.GetBinContent(5,1)

    vals["P HM SR STC"] = h_HM_SR_4.GetBinContent(1,3)
    vals["P HM SR pre"] = h_HM_SR_4.GetBinContent(1,2)
    vals["P HM SR obs"] = h_HM_SR_4.GetBinContent(1,1)
    vals["P3 HM SR STC"] = h_HM_SR_4.GetBinContent(2,3)
    vals["P3 HM SR pre"] = h_HM_SR_4.GetBinContent(2,2)
    vals["P3 HM SR obs"] = h_HM_SR_4.GetBinContent(2,1)
    vals["P4 HM SR STC"] = h_HM_SR_4.GetBinContent(3,3)
    vals["P4 HM SR pre"] = h_HM_SR_4.GetBinContent(3,2)
    vals["P4 HM SR obs"] = h_HM_SR_4.GetBinContent(3,1)
    vals["M HM SR STC"] = h_HM_SR_4.GetBinContent(4,3)
    vals["M HM SR pre"] = h_HM_SR_4.GetBinContent(4,2)
    vals["M HM SR obs"] = h_HM_SR_4.GetBinContent(4,1)
    vals["L HM SR STC"] = h_HM_SR_4.GetBinContent(5,3)
    vals["L HM SR pre"] = h_HM_SR_4.GetBinContent(5,2)
    vals["L HM SR obs"] = h_HM_SR_4.GetBinContent(5,1)

    vals["P HH SR STC"] = h_HH_SR_4.GetBinContent(1,3)
    vals["P HH SR pre"] = h_HH_SR_4.GetBinContent(1,2)
    vals["P HH SR obs"] = h_HH_SR_4.GetBinContent(1,1)
    vals["P3 HH SR STC"] = h_HH_SR_4.GetBinContent(2,3)
    vals["P3 HH SR pre"] = h_HH_SR_4.GetBinContent(2,2)
    vals["P3 HH SR obs"] = h_HH_SR_4.GetBinContent(2,1)
    vals["P4 HH SR STC"] = h_HH_SR_4.GetBinContent(3,3)
    vals["P4 HH SR pre"] = h_HH_SR_4.GetBinContent(3,2)
    vals["P4 HH SR obs"] = h_HH_SR_4.GetBinContent(3,1)
    vals["M HH SR STC"] = h_HH_SR_4.GetBinContent(4,3)
    vals["M HH SR pre"] = h_HH_SR_4.GetBinContent(4,2)
    vals["M HH SR obs"] = h_HH_SR_4.GetBinContent(4,1)
    vals["L HH SR STC"] = h_HH_SR_4.GetBinContent(5,3)
    vals["L HH SR pre"] = h_HH_SR_4.GetBinContent(5,2)
    vals["L HH SR obs"] = h_HH_SR_4.GetBinContent(5,1)

    # hi pt
    vals["P LL SR hi STC"] = h_LL_SR_23_hi.GetBinContent(1,3)
    vals["P LL SR hi pre"] = h_LL_SR_23_hi.GetBinContent(1,2)
    vals["P LL SR hi obs"] = h_LL_SR_23_hi.GetBinContent(1,1)
    vals["P3 LL SR hi STC"] = h_LL_SR_23_hi.GetBinContent(2,3)
    vals["P3 LL SR hi pre"] = h_LL_SR_23_hi.GetBinContent(2,2)
    vals["P3 LL SR hi obs"] = h_LL_SR_23_hi.GetBinContent(2,1)
    vals["P4 LL SR hi STC"] = h_LL_SR_23_hi.GetBinContent(3,3)
    vals["P4 LL SR hi pre"] = h_LL_SR_23_hi.GetBinContent(3,2)
    vals["P4 LL SR hi obs"] = h_LL_SR_23_hi.GetBinContent(3,1)
    vals["M LL SR hi STC"] = h_LL_SR_23_hi.GetBinContent(4,3)
    vals["M LL SR hi pre"] = h_LL_SR_23_hi.GetBinContent(4,2)
    vals["M LL SR hi obs"] = h_LL_SR_23_hi.GetBinContent(4,1)
    vals["L LL SR hi STC"] = h_LL_SR_23_hi.GetBinContent(5,3)
    vals["L LL SR hi pre"] = h_LL_SR_23_hi.GetBinContent(5,2)
    vals["L LL SR hi obs"] = h_LL_SR_23_hi.GetBinContent(5,1)

    vals["P LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinContent(1,3)
    vals["P LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinContent(1,2)
    vals["P LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinContent(1,1)
    vals["P3 LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinContent(2,3)
    vals["P3 LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinContent(2,2)
    vals["P3 LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinContent(2,1)
    vals["P4 LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinContent(3,3)
    vals["P4 LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinContent(3,2)
    vals["P4 LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinContent(3,1)
    vals["M LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinContent(4,3)
    vals["M LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinContent(4,2)
    vals["M LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinContent(4,1)
    vals["L LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinContent(5,3)
    vals["L LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinContent(5,2)
    vals["L LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinContent(5,1)

    vals["P LM SR hi STC"] = h_LM_SR_23_hi.GetBinContent(1,3)
    vals["P LM SR hi pre"] = h_LM_SR_23_hi.GetBinContent(1,2)
    vals["P LM SR hi obs"] = h_LM_SR_23_hi.GetBinContent(1,1)
    vals["P3 LM SR hi STC"] = h_LM_SR_23_hi.GetBinContent(2,3)
    vals["P3 LM SR hi pre"] = h_LM_SR_23_hi.GetBinContent(2,2)
    vals["P3 LM SR hi obs"] = h_LM_SR_23_hi.GetBinContent(2,1)
    vals["P4 LM SR hi STC"] = h_LM_SR_23_hi.GetBinContent(3,3)
    vals["P4 LM SR hi pre"] = h_LM_SR_23_hi.GetBinContent(3,2)
    vals["P4 LM SR hi obs"] = h_LM_SR_23_hi.GetBinContent(3,1)
    vals["M LM SR hi STC"] = h_LM_SR_23_hi.GetBinContent(4,3)
    vals["M LM SR hi pre"] = h_LM_SR_23_hi.GetBinContent(4,2)
    vals["M LM SR hi obs"] = h_LM_SR_23_hi.GetBinContent(4,1)
    vals["L LM SR hi STC"] = h_LM_SR_23_hi.GetBinContent(5,3)
    vals["L LM SR hi pre"] = h_LM_SR_23_hi.GetBinContent(5,2)
    vals["L LM SR hi obs"] = h_LM_SR_23_hi.GetBinContent(5,1)

    vals["P LH SR hi STC"] = h_LH_SR_23_hi.GetBinContent(1,3)
    vals["P LH SR hi pre"] = h_LH_SR_23_hi.GetBinContent(1,2)
    vals["P LH SR hi obs"] = h_LH_SR_23_hi.GetBinContent(1,1)
    vals["P3 LH SR hi STC"] = h_LH_SR_23_hi.GetBinContent(2,3)
    vals["P3 LH SR hi pre"] = h_LH_SR_23_hi.GetBinContent(2,2)
    vals["P3 LH SR hi obs"] = h_LH_SR_23_hi.GetBinContent(2,1)
    vals["P4 LH SR hi STC"] = h_LH_SR_23_hi.GetBinContent(3,3)
    vals["P4 LH SR hi pre"] = h_LH_SR_23_hi.GetBinContent(3,2)
    vals["P4 LH SR hi obs"] = h_LH_SR_23_hi.GetBinContent(3,1)
    vals["M LH SR hi STC"] = h_LH_SR_23_hi.GetBinContent(4,3)
    vals["M LH SR hi pre"] = h_LH_SR_23_hi.GetBinContent(4,2)
    vals["M LH SR hi obs"] = h_LH_SR_23_hi.GetBinContent(4,1)
    vals["L LH SR hi STC"] = h_LH_SR_23_hi.GetBinContent(5,3)
    vals["L LH SR hi pre"] = h_LH_SR_23_hi.GetBinContent(5,2)
    vals["L LH SR hi obs"] = h_LH_SR_23_hi.GetBinContent(5,1)

    vals["P HL SR hi STC"] = h_HL_SR_4_hi.GetBinContent(1,3)
    vals["P HL SR hi pre"] = h_HL_SR_4_hi.GetBinContent(1,2)
    vals["P HL SR hi obs"] = h_HL_SR_4_hi.GetBinContent(1,1)
    vals["P3 HL SR hi STC"] = h_HL_SR_4_hi.GetBinContent(2,3)
    vals["P3 HL SR hi pre"] = h_HL_SR_4_hi.GetBinContent(2,2)
    vals["P3 HL SR hi obs"] = h_HL_SR_4_hi.GetBinContent(2,1)
    vals["P4 HL SR hi STC"] = h_HL_SR_4_hi.GetBinContent(3,3)
    vals["P4 HL SR hi pre"] = h_HL_SR_4_hi.GetBinContent(3,2)
    vals["P4 HL SR hi obs"] = h_HL_SR_4_hi.GetBinContent(3,1)
    vals["M HL SR hi STC"] = h_HL_SR_4_hi.GetBinContent(4,3)
    vals["M HL SR hi pre"] = h_HL_SR_4_hi.GetBinContent(4,2)
    vals["M HL SR hi obs"] = h_HL_SR_4_hi.GetBinContent(4,1)
    vals["L HL SR hi STC"] = h_HL_SR_4_hi.GetBinContent(5,3)
    vals["L HL SR hi pre"] = h_HL_SR_4_hi.GetBinContent(5,2)
    vals["L HL SR hi obs"] = h_HL_SR_4_hi.GetBinContent(5,1)

    vals["P HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinContent(1,3)
    vals["P HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinContent(1,2)
    vals["P HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinContent(1,1)
    vals["P3 HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinContent(2,3)
    vals["P3 HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinContent(2,2)
    vals["P3 HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinContent(2,1)
    vals["P4 HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinContent(3,3)
    vals["P4 HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinContent(3,2)
    vals["P4 HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinContent(3,1)
    vals["M HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinContent(4,3)
    vals["M HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinContent(4,2)
    vals["M HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinContent(4,1)
    vals["L HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinContent(5,3)
    vals["L HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinContent(5,2)
    vals["L HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinContent(5,1)

    vals["P HM SR hi STC"] = h_HM_SR_4_hi.GetBinContent(1,3)
    vals["P HM SR hi pre"] = h_HM_SR_4_hi.GetBinContent(1,2)
    vals["P HM SR hi obs"] = h_HM_SR_4_hi.GetBinContent(1,1)
    vals["P3 HM SR hi STC"] = h_HM_SR_4_hi.GetBinContent(2,3)
    vals["P3 HM SR hi pre"] = h_HM_SR_4_hi.GetBinContent(2,2)
    vals["P3 HM SR hi obs"] = h_HM_SR_4_hi.GetBinContent(2,1)
    vals["P4 HM SR hi STC"] = h_HM_SR_4_hi.GetBinContent(3,3)
    vals["P4 HM SR hi pre"] = h_HM_SR_4_hi.GetBinContent(3,2)
    vals["P4 HM SR hi obs"] = h_HM_SR_4_hi.GetBinContent(3,1)
    vals["M HM SR hi STC"] = h_HM_SR_4_hi.GetBinContent(4,3)
    vals["M HM SR hi pre"] = h_HM_SR_4_hi.GetBinContent(4,2)
    vals["M HM SR hi obs"] = h_HM_SR_4_hi.GetBinContent(4,1)
    vals["L HM SR hi STC"] = h_HM_SR_4_hi.GetBinContent(5,3)
    vals["L HM SR hi pre"] = h_HM_SR_4_hi.GetBinContent(5,2)
    vals["L HM SR hi obs"] = h_HM_SR_4_hi.GetBinContent(5,1)

    vals["P HH SR hi STC"] = h_HH_SR_4_hi.GetBinContent(1,3)
    vals["P HH SR hi pre"] = h_HH_SR_4_hi.GetBinContent(1,2)
    vals["P HH SR hi obs"] = h_HH_SR_4_hi.GetBinContent(1,1)
    vals["P3 HH SR hi STC"] = h_HH_SR_4_hi.GetBinContent(2,3)
    vals["P3 HH SR hi pre"] = h_HH_SR_4_hi.GetBinContent(2,2)
    vals["P3 HH SR hi obs"] = h_HH_SR_4_hi.GetBinContent(2,1)
    vals["P4 HH SR hi STC"] = h_HH_SR_4_hi.GetBinContent(3,3)
    vals["P4 HH SR hi pre"] = h_HH_SR_4_hi.GetBinContent(3,2)
    vals["P4 HH SR hi obs"] = h_HH_SR_4_hi.GetBinContent(3,1)
    vals["M HH SR hi STC"] = h_HH_SR_4_hi.GetBinContent(4,3)
    vals["M HH SR hi pre"] = h_HH_SR_4_hi.GetBinContent(4,2)
    vals["M HH SR hi obs"] = h_HH_SR_4_hi.GetBinContent(4,1)
    vals["L HH SR hi STC"] = h_HH_SR_4_hi.GetBinContent(5,3)
    vals["L HH SR hi pre"] = h_HH_SR_4_hi.GetBinContent(5,2)
    vals["L HH SR hi obs"] = h_HH_SR_4_hi.GetBinContent(5,1)

    # lo pt
    vals["P LL SR lo STC"] = h_LL_SR_23_lo.GetBinContent(1,3)
    vals["P LL SR lo pre"] = h_LL_SR_23_lo.GetBinContent(1,2)
    vals["P LL SR lo obs"] = h_LL_SR_23_lo.GetBinContent(1,1)
    vals["P3 LL SR lo STC"] = h_LL_SR_23_lo.GetBinContent(2,3)
    vals["P3 LL SR lo pre"] = h_LL_SR_23_lo.GetBinContent(2,2)
    vals["P3 LL SR lo obs"] = h_LL_SR_23_lo.GetBinContent(2,1)
    vals["P4 LL SR lo STC"] = h_LL_SR_23_lo.GetBinContent(3,3)
    vals["P4 LL SR lo pre"] = h_LL_SR_23_lo.GetBinContent(3,2)
    vals["P4 LL SR lo obs"] = h_LL_SR_23_lo.GetBinContent(3,1)
    vals["M LL SR lo STC"] = h_LL_SR_23_lo.GetBinContent(4,3)
    vals["M LL SR lo pre"] = h_LL_SR_23_lo.GetBinContent(4,2)
    vals["M LL SR lo obs"] = h_LL_SR_23_lo.GetBinContent(4,1)
    vals["L LL SR lo STC"] = h_LL_SR_23_lo.GetBinContent(5,3)
    vals["L LL SR lo pre"] = h_LL_SR_23_lo.GetBinContent(5,2)
    vals["L LL SR lo obs"] = h_LL_SR_23_lo.GetBinContent(5,1)

    vals["P LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinContent(1,3)
    vals["P LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinContent(1,2)
    vals["P LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinContent(1,1)
    vals["P3 LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinContent(2,3)
    vals["P3 LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinContent(2,2)
    vals["P3 LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinContent(2,1)
    vals["P4 LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinContent(3,3)
    vals["P4 LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinContent(3,2)
    vals["P4 LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinContent(3,1)
    vals["M LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinContent(4,3)
    vals["M LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinContent(4,2)
    vals["M LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinContent(4,1)
    vals["L LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinContent(5,3)
    vals["L LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinContent(5,2)
    vals["L LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinContent(5,1)

    vals["P LM SR lo STC"] = h_LM_SR_23_lo.GetBinContent(1,3)
    vals["P LM SR lo pre"] = h_LM_SR_23_lo.GetBinContent(1,2)
    vals["P LM SR lo obs"] = h_LM_SR_23_lo.GetBinContent(1,1)
    vals["P3 LM SR lo STC"] = h_LM_SR_23_lo.GetBinContent(2,3)
    vals["P3 LM SR lo pre"] = h_LM_SR_23_lo.GetBinContent(2,2)
    vals["P3 LM SR lo obs"] = h_LM_SR_23_lo.GetBinContent(2,1)
    vals["P4 LM SR lo STC"] = h_LM_SR_23_lo.GetBinContent(3,3)
    vals["P4 LM SR lo pre"] = h_LM_SR_23_lo.GetBinContent(3,2)
    vals["P4 LM SR lo obs"] = h_LM_SR_23_lo.GetBinContent(3,1)
    vals["M LM SR lo STC"] = h_LM_SR_23_lo.GetBinContent(4,3)
    vals["M LM SR lo pre"] = h_LM_SR_23_lo.GetBinContent(4,2)
    vals["M LM SR lo obs"] = h_LM_SR_23_lo.GetBinContent(4,1)
    vals["L LM SR lo STC"] = h_LM_SR_23_lo.GetBinContent(5,3)
    vals["L LM SR lo pre"] = h_LM_SR_23_lo.GetBinContent(5,2)
    vals["L LM SR lo obs"] = h_LM_SR_23_lo.GetBinContent(5,1)

    vals["P LH SR lo STC"] = h_LH_SR_23_lo.GetBinContent(1,3)
    vals["P LH SR lo pre"] = h_LH_SR_23_lo.GetBinContent(1,2)
    vals["P LH SR lo obs"] = h_LH_SR_23_lo.GetBinContent(1,1)
    vals["P3 LH SR lo STC"] = h_LH_SR_23_lo.GetBinContent(2,3)
    vals["P3 LH SR lo pre"] = h_LH_SR_23_lo.GetBinContent(2,2)
    vals["P3 LH SR lo obs"] = h_LH_SR_23_lo.GetBinContent(2,1)
    vals["P4 LH SR lo STC"] = h_LH_SR_23_lo.GetBinContent(3,3)
    vals["P4 LH SR lo pre"] = h_LH_SR_23_lo.GetBinContent(3,2)
    vals["P4 LH SR lo obs"] = h_LH_SR_23_lo.GetBinContent(3,1)
    vals["M LH SR lo STC"] = h_LH_SR_23_lo.GetBinContent(4,3)
    vals["M LH SR lo pre"] = h_LH_SR_23_lo.GetBinContent(4,2)
    vals["M LH SR lo obs"] = h_LH_SR_23_lo.GetBinContent(4,1)
    vals["L LH SR lo STC"] = h_LH_SR_23_lo.GetBinContent(5,3)
    vals["L LH SR lo pre"] = h_LH_SR_23_lo.GetBinContent(5,2)
    vals["L LH SR lo obs"] = h_LH_SR_23_lo.GetBinContent(5,1)

    vals["P HL SR lo STC"] = h_HL_SR_4_lo.GetBinContent(1,3)
    vals["P HL SR lo pre"] = h_HL_SR_4_lo.GetBinContent(1,2)
    vals["P HL SR lo obs"] = h_HL_SR_4_lo.GetBinContent(1,1)
    vals["P3 HL SR lo STC"] = h_HL_SR_4_lo.GetBinContent(2,3)
    vals["P3 HL SR lo pre"] = h_HL_SR_4_lo.GetBinContent(2,2)
    vals["P3 HL SR lo obs"] = h_HL_SR_4_lo.GetBinContent(2,1)
    vals["P4 HL SR lo STC"] = h_HL_SR_4_lo.GetBinContent(3,3)
    vals["P4 HL SR lo pre"] = h_HL_SR_4_lo.GetBinContent(3,2)
    vals["P4 HL SR lo obs"] = h_HL_SR_4_lo.GetBinContent(3,1)
    vals["M HL SR lo STC"] = h_HL_SR_4_lo.GetBinContent(4,3)
    vals["M HL SR lo pre"] = h_HL_SR_4_lo.GetBinContent(4,2)
    vals["M HL SR lo obs"] = h_HL_SR_4_lo.GetBinContent(4,1)
    vals["L HL SR lo STC"] = h_HL_SR_4_lo.GetBinContent(5,3)
    vals["L HL SR lo pre"] = h_HL_SR_4_lo.GetBinContent(5,2)
    vals["L HL SR lo obs"] = h_HL_SR_4_lo.GetBinContent(5,1)

    vals["P HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinContent(1,3)
    vals["P HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinContent(1,2)
    vals["P HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinContent(1,1)
    vals["P3 HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinContent(2,3)
    vals["P3 HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinContent(2,2)
    vals["P3 HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinContent(2,1)
    vals["P4 HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinContent(3,3)
    vals["P4 HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinContent(3,2)
    vals["P4 HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinContent(3,1)
    vals["M HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinContent(4,3)
    vals["M HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinContent(4,2)
    vals["M HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinContent(4,1)
    vals["L HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinContent(5,3)
    vals["L HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinContent(5,2)
    vals["L HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinContent(5,1)

    vals["P HM SR lo STC"] = h_HM_SR_4_lo.GetBinContent(1,3)
    vals["P HM SR lo pre"] = h_HM_SR_4_lo.GetBinContent(1,2)
    vals["P HM SR lo obs"] = h_HM_SR_4_lo.GetBinContent(1,1)
    vals["P3 HM SR lo STC"] = h_HM_SR_4_lo.GetBinContent(2,3)
    vals["P3 HM SR lo pre"] = h_HM_SR_4_lo.GetBinContent(2,2)
    vals["P3 HM SR lo obs"] = h_HM_SR_4_lo.GetBinContent(2,1)
    vals["P4 HM SR lo STC"] = h_HM_SR_4_lo.GetBinContent(3,3)
    vals["P4 HM SR lo pre"] = h_HM_SR_4_lo.GetBinContent(3,2)
    vals["P4 HM SR lo obs"] = h_HM_SR_4_lo.GetBinContent(3,1)
    vals["M HM SR lo STC"] = h_HM_SR_4_lo.GetBinContent(4,3)
    vals["M HM SR lo pre"] = h_HM_SR_4_lo.GetBinContent(4,2)
    vals["M HM SR lo obs"] = h_HM_SR_4_lo.GetBinContent(4,1)
    vals["L HM SR lo STC"] = h_HM_SR_4_lo.GetBinContent(5,3)
    vals["L HM SR lo pre"] = h_HM_SR_4_lo.GetBinContent(5,2)
    vals["L HM SR lo obs"] = h_HM_SR_4_lo.GetBinContent(5,1)

    vals["P HH SR lo STC"] = h_HH_SR_4_lo.GetBinContent(1,3)
    vals["P HH SR lo pre"] = h_HH_SR_4_lo.GetBinContent(1,2)
    vals["P HH SR lo obs"] = h_HH_SR_4_lo.GetBinContent(1,1)
    vals["P3 HH SR lo STC"] = h_HH_SR_4_lo.GetBinContent(2,3)
    vals["P3 HH SR lo pre"] = h_HH_SR_4_lo.GetBinContent(2,2)
    vals["P3 HH SR lo obs"] = h_HH_SR_4_lo.GetBinContent(2,1)
    vals["P4 HH SR lo STC"] = h_HH_SR_4_lo.GetBinContent(3,3)
    vals["P4 HH SR lo pre"] = h_HH_SR_4_lo.GetBinContent(3,2)
    vals["P4 HH SR lo obs"] = h_HH_SR_4_lo.GetBinContent(3,1)
    vals["M HH SR lo STC"] = h_HH_SR_4_lo.GetBinContent(4,3)
    vals["M HH SR lo pre"] = h_HH_SR_4_lo.GetBinContent(4,2)
    vals["M HH SR lo obs"] = h_HH_SR_4_lo.GetBinContent(4,1)
    vals["L HH SR lo STC"] = h_HH_SR_4_lo.GetBinContent(5,3)
    vals["L HH SR lo pre"] = h_HH_SR_4_lo.GetBinContent(5,2)
    vals["L HH SR lo obs"] = h_HH_SR_4_lo.GetBinContent(5,1)

    # SR Errors
    errs["P LL SR STC"] = h_LL_SR_23.GetBinError(1,3)
    errs["P LL SR pre"] = h_LL_SR_23.GetBinError(1,2)
    errs["P LL SR obs"] = h_LL_SR_23.GetBinError(1,1)
    errs["P3 LL SR STC"] = h_LL_SR_23.GetBinError(2,3)
    errs["P3 LL SR pre"] = h_LL_SR_23.GetBinError(2,2)
    errs["P3 LL SR obs"] = h_LL_SR_23.GetBinError(2,1)
    errs["P4 LL SR STC"] = h_LL_SR_23.GetBinError(3,3)
    errs["P4 LL SR pre"] = h_LL_SR_23.GetBinError(3,2)
    errs["P4 LL SR obs"] = h_LL_SR_23.GetBinError(3,1)
    errs["M LL SR STC"] = h_LL_SR_23.GetBinError(4,3)
    errs["M LL SR pre"] = h_LL_SR_23.GetBinError(4,2)
    errs["M LL SR obs"] = h_LL_SR_23.GetBinError(4,1)
    errs["L LL SR STC"] = h_LL_SR_23.GetBinError(5,3)
    errs["L LL SR pre"] = h_LL_SR_23.GetBinError(5,2)
    errs["L LL SR obs"] = h_LL_SR_23.GetBinError(5,1)

    errs["P LLM SR STC"] = h_LLM_SR_23.GetBinError(1,3)
    errs["P LLM SR pre"] = h_LLM_SR_23.GetBinError(1,2)
    errs["P LLM SR obs"] = h_LLM_SR_23.GetBinError(1,1)
    errs["P3 LLM SR STC"] = h_LLM_SR_23.GetBinError(2,3)
    errs["P3 LLM SR pre"] = h_LLM_SR_23.GetBinError(2,2)
    errs["P3 LLM SR obs"] = h_LLM_SR_23.GetBinError(2,1)
    errs["P4 LLM SR STC"] = h_LLM_SR_23.GetBinError(3,3)
    errs["P4 LLM SR pre"] = h_LLM_SR_23.GetBinError(3,2)
    errs["P4 LLM SR obs"] = h_LLM_SR_23.GetBinError(3,1)
    errs["M LLM SR STC"] = h_LLM_SR_23.GetBinError(4,3)
    errs["M LLM SR pre"] = h_LLM_SR_23.GetBinError(4,2)
    errs["M LLM SR obs"] = h_LLM_SR_23.GetBinError(4,1)
    errs["L LLM SR STC"] = h_LLM_SR_23.GetBinError(5,3)
    errs["L LLM SR pre"] = h_LLM_SR_23.GetBinError(5,2)
    errs["L LLM SR obs"] = h_LLM_SR_23.GetBinError(5,1)

    errs["P LM SR STC"] = h_LM_SR_23.GetBinError(1,3)
    errs["P LM SR pre"] = h_LM_SR_23.GetBinError(1,2)
    errs["P LM SR obs"] = h_LM_SR_23.GetBinError(1,1)
    errs["P3 LM SR STC"] = h_LM_SR_23.GetBinError(2,3)
    errs["P3 LM SR pre"] = h_LM_SR_23.GetBinError(2,2)
    errs["P3 LM SR obs"] = h_LM_SR_23.GetBinError(2,1)
    errs["P4 LM SR STC"] = h_LM_SR_23.GetBinError(3,3)
    errs["P4 LM SR pre"] = h_LM_SR_23.GetBinError(3,2)
    errs["P4 LM SR obs"] = h_LM_SR_23.GetBinError(3,1)
    errs["M LM SR STC"] = h_LM_SR_23.GetBinError(4,3)
    errs["M LM SR pre"] = h_LM_SR_23.GetBinError(4,2)
    errs["M LM SR obs"] = h_LM_SR_23.GetBinError(4,1)
    errs["L LM SR STC"] = h_LM_SR_23.GetBinError(5,3)
    errs["L LM SR pre"] = h_LM_SR_23.GetBinError(5,2)
    errs["L LM SR obs"] = h_LM_SR_23.GetBinError(5,1)

    errs["P LH SR STC"] = h_LH_SR_23.GetBinError(1,3)
    errs["P LH SR pre"] = h_LH_SR_23.GetBinError(1,2)
    errs["P LH SR obs"] = h_LH_SR_23.GetBinError(1,1)
    errs["P3 LH SR STC"] = h_LH_SR_23.GetBinError(2,3)
    errs["P3 LH SR pre"] = h_LH_SR_23.GetBinError(2,2)
    errs["P3 LH SR obs"] = h_LH_SR_23.GetBinError(2,1)
    errs["P4 LH SR STC"] = h_LH_SR_23.GetBinError(3,3)
    errs["P4 LH SR pre"] = h_LH_SR_23.GetBinError(3,2)
    errs["P4 LH SR obs"] = h_LH_SR_23.GetBinError(3,1)
    errs["M LH SR STC"] = h_LH_SR_23.GetBinError(4,3)
    errs["M LH SR pre"] = h_LH_SR_23.GetBinError(4,2)
    errs["M LH SR obs"] = h_LH_SR_23.GetBinError(4,1)
    errs["L LH SR STC"] = h_LH_SR_23.GetBinError(5,3)
    errs["L LH SR pre"] = h_LH_SR_23.GetBinError(5,2)
    errs["L LH SR obs"] = h_LH_SR_23.GetBinError(5,1)

    errs["P HL SR STC"] = h_HL_SR_4.GetBinError(1,3)
    errs["P HL SR pre"] = h_HL_SR_4.GetBinError(1,2)
    errs["P HL SR obs"] = h_HL_SR_4.GetBinError(1,1)
    errs["P3 HL SR STC"] = h_HL_SR_4.GetBinError(2,3)
    errs["P3 HL SR pre"] = h_HL_SR_4.GetBinError(2,2)
    errs["P3 HL SR obs"] = h_HL_SR_4.GetBinError(2,1)
    errs["P4 HL SR STC"] = h_HL_SR_4.GetBinError(3,3)
    errs["P4 HL SR pre"] = h_HL_SR_4.GetBinError(3,2)
    errs["P4 HL SR obs"] = h_HL_SR_4.GetBinError(3,1)
    errs["M HL SR STC"] = h_HL_SR_4.GetBinError(4,3)
    errs["M HL SR pre"] = h_HL_SR_4.GetBinError(4,2)
    errs["M HL SR obs"] = h_HL_SR_4.GetBinError(4,1)
    errs["L HL SR STC"] = h_HL_SR_4.GetBinError(5,3)
    errs["L HL SR pre"] = h_HL_SR_4.GetBinError(5,2)
    errs["L HL SR obs"] = h_HL_SR_4.GetBinError(5,1)

    errs["P HLM SR STC"] = h_HLM_SR_4.GetBinError(1,3)
    errs["P HLM SR pre"] = h_HLM_SR_4.GetBinError(1,2)
    errs["P HLM SR obs"] = h_HLM_SR_4.GetBinError(1,1)
    errs["P3 HLM SR STC"] = h_HLM_SR_4.GetBinError(2,3)
    errs["P3 HLM SR pre"] = h_HLM_SR_4.GetBinError(2,2)
    errs["P3 HLM SR obs"] = h_HLM_SR_4.GetBinError(2,1)
    errs["P4 HLM SR STC"] = h_HLM_SR_4.GetBinError(3,3)
    errs["P4 HLM SR pre"] = h_HLM_SR_4.GetBinError(3,2)
    errs["P4 HLM SR obs"] = h_HLM_SR_4.GetBinError(3,1)
    errs["M HLM SR STC"] = h_HLM_SR_4.GetBinError(4,3)
    errs["M HLM SR pre"] = h_HLM_SR_4.GetBinError(4,2)
    errs["M HLM SR obs"] = h_HLM_SR_4.GetBinError(4,1)
    errs["L HLM SR STC"] = h_HLM_SR_4.GetBinError(5,3)
    errs["L HLM SR pre"] = h_HLM_SR_4.GetBinError(5,2)
    errs["L HLM SR obs"] = h_HLM_SR_4.GetBinError(5,1)

    errs["P HM SR STC"] = h_HM_SR_4.GetBinError(1,3)
    errs["P HM SR pre"] = h_HM_SR_4.GetBinError(1,2)
    errs["P HM SR obs"] = h_HM_SR_4.GetBinError(1,1)
    errs["P3 HM SR STC"] = h_HM_SR_4.GetBinError(2,3)
    errs["P3 HM SR pre"] = h_HM_SR_4.GetBinError(2,2)
    errs["P3 HM SR obs"] = h_HM_SR_4.GetBinError(2,1)
    errs["P4 HM SR STC"] = h_HM_SR_4.GetBinError(3,3)
    errs["P4 HM SR pre"] = h_HM_SR_4.GetBinError(3,2)
    errs["P4 HM SR obs"] = h_HM_SR_4.GetBinError(3,1)
    errs["M HM SR STC"] = h_HM_SR_4.GetBinError(4,3)
    errs["M HM SR pre"] = h_HM_SR_4.GetBinError(4,2)
    errs["M HM SR obs"] = h_HM_SR_4.GetBinError(4,1)
    errs["L HM SR STC"] = h_HM_SR_4.GetBinError(5,3)
    errs["L HM SR pre"] = h_HM_SR_4.GetBinError(5,2)
    errs["L HM SR obs"] = h_HM_SR_4.GetBinError(5,1)

    errs["P HH SR STC"] = h_HH_SR_4.GetBinError(1,3)
    errs["P HH SR pre"] = h_HH_SR_4.GetBinError(1,2)
    errs["P HH SR obs"] = h_HH_SR_4.GetBinError(1,1)
    errs["P3 HH SR STC"] = h_HH_SR_4.GetBinError(2,3)
    errs["P3 HH SR pre"] = h_HH_SR_4.GetBinError(2,2)
    errs["P3 HH SR obs"] = h_HH_SR_4.GetBinError(2,1)
    errs["P4 HH SR STC"] = h_HH_SR_4.GetBinError(3,3)
    errs["P4 HH SR pre"] = h_HH_SR_4.GetBinError(3,2)
    errs["P4 HH SR obs"] = h_HH_SR_4.GetBinError(3,1)
    errs["M HH SR STC"] = h_HH_SR_4.GetBinError(4,3)
    errs["M HH SR pre"] = h_HH_SR_4.GetBinError(4,2)
    errs["M HH SR obs"] = h_HH_SR_4.GetBinError(4,1)
    errs["L HH SR STC"] = h_HH_SR_4.GetBinError(5,3)
    errs["L HH SR pre"] = h_HH_SR_4.GetBinError(5,2)
    errs["L HH SR obs"] = h_HH_SR_4.GetBinError(5,1)

    # hi pt
    errs["P LL SR hi STC"] = h_LL_SR_23_hi.GetBinError(1,3)
    errs["P LL SR hi pre"] = h_LL_SR_23_hi.GetBinError(1,2)
    errs["P LL SR hi obs"] = h_LL_SR_23_hi.GetBinError(1,1)
    errs["P3 LL SR hi STC"] = h_LL_SR_23_hi.GetBinError(2,3)
    errs["P3 LL SR hi pre"] = h_LL_SR_23_hi.GetBinError(2,2)
    errs["P3 LL SR hi obs"] = h_LL_SR_23_hi.GetBinError(2,1)
    errs["P4 LL SR hi STC"] = h_LL_SR_23_hi.GetBinError(3,3)
    errs["P4 LL SR hi pre"] = h_LL_SR_23_hi.GetBinError(3,2)
    errs["P4 LL SR hi obs"] = h_LL_SR_23_hi.GetBinError(3,1)
    errs["M LL SR hi STC"] = h_LL_SR_23_hi.GetBinError(4,3)
    errs["M LL SR hi pre"] = h_LL_SR_23_hi.GetBinError(4,2)
    errs["M LL SR hi obs"] = h_LL_SR_23_hi.GetBinError(4,1)
    errs["L LL SR hi STC"] = h_LL_SR_23_hi.GetBinError(5,3)
    errs["L LL SR hi pre"] = h_LL_SR_23_hi.GetBinError(5,2)
    errs["L LL SR hi obs"] = h_LL_SR_23_hi.GetBinError(5,1)

    errs["P LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinError(1,3)
    errs["P LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinError(1,2)
    errs["P LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinError(1,1)
    errs["P3 LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinError(2,3)
    errs["P3 LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinError(2,2)
    errs["P3 LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinError(2,1)
    errs["P4 LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinError(3,3)
    errs["P4 LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinError(3,2)
    errs["P4 LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinError(3,1)
    errs["M LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinError(4,3)
    errs["M LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinError(4,2)
    errs["M LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinError(4,1)
    errs["L LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinError(5,3)
    errs["L LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinError(5,2)
    errs["L LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinError(5,1)

    errs["P LM SR hi STC"] = h_LM_SR_23_hi.GetBinError(1,3)
    errs["P LM SR hi pre"] = h_LM_SR_23_hi.GetBinError(1,2)
    errs["P LM SR hi obs"] = h_LM_SR_23_hi.GetBinError(1,1)
    errs["P3 LM SR hi STC"] = h_LM_SR_23_hi.GetBinError(2,3)
    errs["P3 LM SR hi pre"] = h_LM_SR_23_hi.GetBinError(2,2)
    errs["P3 LM SR hi obs"] = h_LM_SR_23_hi.GetBinError(2,1)
    errs["P4 LM SR hi STC"] = h_LM_SR_23_hi.GetBinError(3,3)
    errs["P4 LM SR hi pre"] = h_LM_SR_23_hi.GetBinError(3,2)
    errs["P4 LM SR hi obs"] = h_LM_SR_23_hi.GetBinError(3,1)
    errs["M LM SR hi STC"] = h_LM_SR_23_hi.GetBinError(4,3)
    errs["M LM SR hi pre"] = h_LM_SR_23_hi.GetBinError(4,2)
    errs["M LM SR hi obs"] = h_LM_SR_23_hi.GetBinError(4,1)
    errs["L LM SR hi STC"] = h_LM_SR_23_hi.GetBinError(5,3)
    errs["L LM SR hi pre"] = h_LM_SR_23_hi.GetBinError(5,2)
    errs["L LM SR hi obs"] = h_LM_SR_23_hi.GetBinError(5,1)

    errs["P LH SR hi STC"] = h_LH_SR_23_hi.GetBinError(1,3)
    errs["P LH SR hi pre"] = h_LH_SR_23_hi.GetBinError(1,2)
    errs["P LH SR hi obs"] = h_LH_SR_23_hi.GetBinError(1,1)
    errs["P3 LH SR hi STC"] = h_LH_SR_23_hi.GetBinError(2,3)
    errs["P3 LH SR hi pre"] = h_LH_SR_23_hi.GetBinError(2,2)
    errs["P3 LH SR hi obs"] = h_LH_SR_23_hi.GetBinError(2,1)
    errs["P4 LH SR hi STC"] = h_LH_SR_23_hi.GetBinError(3,3)
    errs["P4 LH SR hi pre"] = h_LH_SR_23_hi.GetBinError(3,2)
    errs["P4 LH SR hi obs"] = h_LH_SR_23_hi.GetBinError(3,1)
    errs["M LH SR hi STC"] = h_LH_SR_23_hi.GetBinError(4,3)
    errs["M LH SR hi pre"] = h_LH_SR_23_hi.GetBinError(4,2)
    errs["M LH SR hi obs"] = h_LH_SR_23_hi.GetBinError(4,1)
    errs["L LH SR hi STC"] = h_LH_SR_23_hi.GetBinError(5,3)
    errs["L LH SR hi pre"] = h_LH_SR_23_hi.GetBinError(5,2)
    errs["L LH SR hi obs"] = h_LH_SR_23_hi.GetBinError(5,1)

    errs["P HL SR hi STC"] = h_HL_SR_4_hi.GetBinError(1,3)
    errs["P HL SR hi pre"] = h_HL_SR_4_hi.GetBinError(1,2)
    errs["P HL SR hi obs"] = h_HL_SR_4_hi.GetBinError(1,1)
    errs["P3 HL SR hi STC"] = h_HL_SR_4_hi.GetBinError(2,3)
    errs["P3 HL SR hi pre"] = h_HL_SR_4_hi.GetBinError(2,2)
    errs["P3 HL SR hi obs"] = h_HL_SR_4_hi.GetBinError(2,1)
    errs["P4 HL SR hi STC"] = h_HL_SR_4_hi.GetBinError(3,3)
    errs["P4 HL SR hi pre"] = h_HL_SR_4_hi.GetBinError(3,2)
    errs["P4 HL SR hi obs"] = h_HL_SR_4_hi.GetBinError(3,1)
    errs["M HL SR hi STC"] = h_HL_SR_4_hi.GetBinError(4,3)
    errs["M HL SR hi pre"] = h_HL_SR_4_hi.GetBinError(4,2)
    errs["M HL SR hi obs"] = h_HL_SR_4_hi.GetBinError(4,1)
    errs["L HL SR hi STC"] = h_HL_SR_4_hi.GetBinError(5,3)
    errs["L HL SR hi pre"] = h_HL_SR_4_hi.GetBinError(5,2)
    errs["L HL SR hi obs"] = h_HL_SR_4_hi.GetBinError(5,1)

    errs["P HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinError(1,3)
    errs["P HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinError(1,2)
    errs["P HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinError(1,1)
    errs["P3 HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinError(2,3)
    errs["P3 HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinError(2,2)
    errs["P3 HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinError(2,1)
    errs["P4 HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinError(3,3)
    errs["P4 HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinError(3,2)
    errs["P4 HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinError(3,1)
    errs["M HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinError(4,3)
    errs["M HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinError(4,2)
    errs["M HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinError(4,1)
    errs["L HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinError(5,3)
    errs["L HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinError(5,2)
    errs["L HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinError(5,1)

    errs["P HM SR hi STC"] = h_HM_SR_4_hi.GetBinError(1,3)
    errs["P HM SR hi pre"] = h_HM_SR_4_hi.GetBinError(1,2)
    errs["P HM SR hi obs"] = h_HM_SR_4_hi.GetBinError(1,1)
    errs["P3 HM SR hi STC"] = h_HM_SR_4_hi.GetBinError(2,3)
    errs["P3 HM SR hi pre"] = h_HM_SR_4_hi.GetBinError(2,2)
    errs["P3 HM SR hi obs"] = h_HM_SR_4_hi.GetBinError(2,1)
    errs["P4 HM SR hi STC"] = h_HM_SR_4_hi.GetBinError(3,3)
    errs["P4 HM SR hi pre"] = h_HM_SR_4_hi.GetBinError(3,2)
    errs["P4 HM SR hi obs"] = h_HM_SR_4_hi.GetBinError(3,1)
    errs["M HM SR hi STC"] = h_HM_SR_4_hi.GetBinError(4,3)
    errs["M HM SR hi pre"] = h_HM_SR_4_hi.GetBinError(4,2)
    errs["M HM SR hi obs"] = h_HM_SR_4_hi.GetBinError(4,1)
    errs["L HM SR hi STC"] = h_HM_SR_4_hi.GetBinError(5,3)
    errs["L HM SR hi pre"] = h_HM_SR_4_hi.GetBinError(5,2)
    errs["L HM SR hi obs"] = h_HM_SR_4_hi.GetBinError(5,1)

    errs["P HH SR hi STC"] = h_HH_SR_4_hi.GetBinError(1,3)
    errs["P HH SR hi pre"] = h_HH_SR_4_hi.GetBinError(1,2)
    errs["P HH SR hi obs"] = h_HH_SR_4_hi.GetBinError(1,1)
    errs["P3 HH SR hi STC"] = h_HH_SR_4_hi.GetBinError(2,3)
    errs["P3 HH SR hi pre"] = h_HH_SR_4_hi.GetBinError(2,2)
    errs["P3 HH SR hi obs"] = h_HH_SR_4_hi.GetBinError(2,1)
    errs["P4 HH SR hi STC"] = h_HH_SR_4_hi.GetBinError(3,3)
    errs["P4 HH SR hi pre"] = h_HH_SR_4_hi.GetBinError(3,2)
    errs["P4 HH SR hi obs"] = h_HH_SR_4_hi.GetBinError(3,1)
    errs["M HH SR hi STC"] = h_HH_SR_4_hi.GetBinError(4,3)
    errs["M HH SR hi pre"] = h_HH_SR_4_hi.GetBinError(4,2)
    errs["M HH SR hi obs"] = h_HH_SR_4_hi.GetBinError(4,1)
    errs["L HH SR hi STC"] = h_HH_SR_4_hi.GetBinError(5,3)
    errs["L HH SR hi pre"] = h_HH_SR_4_hi.GetBinError(5,2)
    errs["L HH SR hi obs"] = h_HH_SR_4_hi.GetBinError(5,1)

    # lo pt
    errs["P LL SR lo STC"] = h_LL_SR_23_lo.GetBinError(1,3)
    errs["P LL SR lo pre"] = h_LL_SR_23_lo.GetBinError(1,2)
    errs["P LL SR lo obs"] = h_LL_SR_23_lo.GetBinError(1,1)
    errs["P3 LL SR lo STC"] = h_LL_SR_23_lo.GetBinError(2,3)
    errs["P3 LL SR lo pre"] = h_LL_SR_23_lo.GetBinError(2,2)
    errs["P3 LL SR lo obs"] = h_LL_SR_23_lo.GetBinError(2,1)
    errs["P4 LL SR lo STC"] = h_LL_SR_23_lo.GetBinError(3,3)
    errs["P4 LL SR lo pre"] = h_LL_SR_23_lo.GetBinError(3,2)
    errs["P4 LL SR lo obs"] = h_LL_SR_23_lo.GetBinError(3,1)
    errs["M LL SR lo STC"] = h_LL_SR_23_lo.GetBinError(4,3)
    errs["M LL SR lo pre"] = h_LL_SR_23_lo.GetBinError(4,2)
    errs["M LL SR lo obs"] = h_LL_SR_23_lo.GetBinError(4,1)
    errs["L LL SR lo STC"] = h_LL_SR_23_lo.GetBinError(5,3)
    errs["L LL SR lo pre"] = h_LL_SR_23_lo.GetBinError(5,2)
    errs["L LL SR lo obs"] = h_LL_SR_23_lo.GetBinError(5,1)

    errs["P LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinError(1,3)
    errs["P LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinError(1,2)
    errs["P LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinError(1,1)
    errs["P3 LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinError(2,3)
    errs["P3 LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinError(2,2)
    errs["P3 LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinError(2,1)
    errs["P4 LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinError(3,3)
    errs["P4 LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinError(3,2)
    errs["P4 LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinError(3,1)
    errs["M LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinError(4,3)
    errs["M LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinError(4,2)
    errs["M LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinError(4,1)
    errs["L LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinError(5,3)
    errs["L LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinError(5,2)
    errs["L LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinError(5,1)

    errs["P LM SR lo STC"] = h_LM_SR_23_lo.GetBinError(1,3)
    errs["P LM SR lo pre"] = h_LM_SR_23_lo.GetBinError(1,2)
    errs["P LM SR lo obs"] = h_LM_SR_23_lo.GetBinError(1,1)
    errs["P3 LM SR lo STC"] = h_LM_SR_23_lo.GetBinError(2,3)
    errs["P3 LM SR lo pre"] = h_LM_SR_23_lo.GetBinError(2,2)
    errs["P3 LM SR lo obs"] = h_LM_SR_23_lo.GetBinError(2,1)
    errs["P4 LM SR lo STC"] = h_LM_SR_23_lo.GetBinError(3,3)
    errs["P4 LM SR lo pre"] = h_LM_SR_23_lo.GetBinError(3,2)
    errs["P4 LM SR lo obs"] = h_LM_SR_23_lo.GetBinError(3,1)
    errs["M LM SR lo STC"] = h_LM_SR_23_lo.GetBinError(4,3)
    errs["M LM SR lo pre"] = h_LM_SR_23_lo.GetBinError(4,2)
    errs["M LM SR lo obs"] = h_LM_SR_23_lo.GetBinError(4,1)
    errs["L LM SR lo STC"] = h_LM_SR_23_lo.GetBinError(5,3)
    errs["L LM SR lo pre"] = h_LM_SR_23_lo.GetBinError(5,2)
    errs["L LM SR lo obs"] = h_LM_SR_23_lo.GetBinError(5,1)

    errs["P LH SR lo STC"] = h_LH_SR_23_lo.GetBinError(1,3)
    errs["P LH SR lo pre"] = h_LH_SR_23_lo.GetBinError(1,2)
    errs["P LH SR lo obs"] = h_LH_SR_23_lo.GetBinError(1,1)
    errs["P3 LH SR lo STC"] = h_LH_SR_23_lo.GetBinError(2,3)
    errs["P3 LH SR lo pre"] = h_LH_SR_23_lo.GetBinError(2,2)
    errs["P3 LH SR lo obs"] = h_LH_SR_23_lo.GetBinError(2,1)
    errs["P4 LH SR lo STC"] = h_LH_SR_23_lo.GetBinError(3,3)
    errs["P4 LH SR lo pre"] = h_LH_SR_23_lo.GetBinError(3,2)
    errs["P4 LH SR lo obs"] = h_LH_SR_23_lo.GetBinError(3,1)
    errs["M LH SR lo STC"] = h_LH_SR_23_lo.GetBinError(4,3)
    errs["M LH SR lo pre"] = h_LH_SR_23_lo.GetBinError(4,2)
    errs["M LH SR lo obs"] = h_LH_SR_23_lo.GetBinError(4,1)
    errs["L LH SR lo STC"] = h_LH_SR_23_lo.GetBinError(5,3)
    errs["L LH SR lo pre"] = h_LH_SR_23_lo.GetBinError(5,2)
    errs["L LH SR lo obs"] = h_LH_SR_23_lo.GetBinError(5,1)

    errs["P HL SR lo STC"] = h_HL_SR_4_lo.GetBinError(1,3)
    errs["P HL SR lo pre"] = h_HL_SR_4_lo.GetBinError(1,2)
    errs["P HL SR lo obs"] = h_HL_SR_4_lo.GetBinError(1,1)
    errs["P3 HL SR lo STC"] = h_HL_SR_4_lo.GetBinError(2,3)
    errs["P3 HL SR lo pre"] = h_HL_SR_4_lo.GetBinError(2,2)
    errs["P3 HL SR lo obs"] = h_HL_SR_4_lo.GetBinError(2,1)
    errs["P4 HL SR lo STC"] = h_HL_SR_4_lo.GetBinError(3,3)
    errs["P4 HL SR lo pre"] = h_HL_SR_4_lo.GetBinError(3,2)
    errs["P4 HL SR lo obs"] = h_HL_SR_4_lo.GetBinError(3,1)
    errs["M HL SR lo STC"] = h_HL_SR_4_lo.GetBinError(4,3)
    errs["M HL SR lo pre"] = h_HL_SR_4_lo.GetBinError(4,2)
    errs["M HL SR lo obs"] = h_HL_SR_4_lo.GetBinError(4,1)
    errs["L HL SR lo STC"] = h_HL_SR_4_lo.GetBinError(5,3)
    errs["L HL SR lo pre"] = h_HL_SR_4_lo.GetBinError(5,2)
    errs["L HL SR lo obs"] = h_HL_SR_4_lo.GetBinError(5,1)

    errs["P HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinError(1,3)
    errs["P HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinError(1,2)
    errs["P HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinError(1,1)
    errs["P3 HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinError(2,3)
    errs["P3 HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinError(2,2)
    errs["P3 HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinError(2,1)
    errs["P4 HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinError(3,3)
    errs["P4 HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinError(3,2)
    errs["P4 HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinError(3,1)
    errs["M HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinError(4,3)
    errs["M HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinError(4,2)
    errs["M HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinError(4,1)
    errs["L HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinError(5,3)
    errs["L HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinError(5,2)
    errs["L HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinError(5,1)

    errs["P HM SR lo STC"] = h_HM_SR_4_lo.GetBinError(1,3)
    errs["P HM SR lo pre"] = h_HM_SR_4_lo.GetBinError(1,2)
    errs["P HM SR lo obs"] = h_HM_SR_4_lo.GetBinError(1,1)
    errs["P3 HM SR lo STC"] = h_HM_SR_4_lo.GetBinError(2,3)
    errs["P3 HM SR lo pre"] = h_HM_SR_4_lo.GetBinError(2,2)
    errs["P3 HM SR lo obs"] = h_HM_SR_4_lo.GetBinError(2,1)
    errs["P4 HM SR lo STC"] = h_HM_SR_4_lo.GetBinError(3,3)
    errs["P4 HM SR lo pre"] = h_HM_SR_4_lo.GetBinError(3,2)
    errs["P4 HM SR lo obs"] = h_HM_SR_4_lo.GetBinError(3,1)
    errs["M HM SR lo STC"] = h_HM_SR_4_lo.GetBinError(4,3)
    errs["M HM SR lo pre"] = h_HM_SR_4_lo.GetBinError(4,2)
    errs["M HM SR lo obs"] = h_HM_SR_4_lo.GetBinError(4,1)
    errs["L HM SR lo STC"] = h_HM_SR_4_lo.GetBinError(5,3)
    errs["L HM SR lo pre"] = h_HM_SR_4_lo.GetBinError(5,2)
    errs["L HM SR lo obs"] = h_HM_SR_4_lo.GetBinError(5,1)

    errs["P HH SR lo STC"] = h_HH_SR_4_lo.GetBinError(1,3)
    errs["P HH SR lo pre"] = h_HH_SR_4_lo.GetBinError(1,2)
    errs["P HH SR lo obs"] = h_HH_SR_4_lo.GetBinError(1,1)
    errs["P3 HH SR lo STC"] = h_HH_SR_4_lo.GetBinError(2,3)
    errs["P3 HH SR lo pre"] = h_HH_SR_4_lo.GetBinError(2,2)
    errs["P3 HH SR lo obs"] = h_HH_SR_4_lo.GetBinError(2,1)
    errs["P4 HH SR lo STC"] = h_HH_SR_4_lo.GetBinError(3,3)
    errs["P4 HH SR lo pre"] = h_HH_SR_4_lo.GetBinError(3,2)
    errs["P4 HH SR lo obs"] = h_HH_SR_4_lo.GetBinError(3,1)
    errs["M HH SR lo STC"] = h_HH_SR_4_lo.GetBinError(4,3)
    errs["M HH SR lo pre"] = h_HH_SR_4_lo.GetBinError(4,2)
    errs["M HH SR lo obs"] = h_HH_SR_4_lo.GetBinError(4,1)
    errs["L HH SR lo STC"] = h_HH_SR_4_lo.GetBinError(5,3)
    errs["L HH SR lo pre"] = h_HH_SR_4_lo.GetBinError(5,2)
    errs["L HH SR lo obs"] = h_HH_SR_4_lo.GetBinError(5,1)

    # SR systs; The systs here are based on VR, so the VR in the hist names is intentional, not a bug. 
    # (These percentage values are all the same as for the VR equivalents, above.)
    if h_LL_VR_23_ns != None:
        systs["P LL SR"] = h_LL_VR_23_ns.GetBinContent(1)
        systs["P3 LL SR"] = h_LL_VR_23_ns.GetBinContent(2)
        systs["P4 LL SR"] = h_LL_VR_23_ns.GetBinContent(3)
        systs["M LL SR"] = h_LL_VR_23_ns.GetBinContent(4)
        systs["L LL SR"] = h_LL_VR_23_ns.GetBinContent(5)
        systs["P LLM SR"] = h_LLM_VR_23_ns.GetBinContent(1)
        systs["P3 LLM SR"] = h_LLM_VR_23_ns.GetBinContent(2)
        systs["P4 LLM SR"] = h_LLM_VR_23_ns.GetBinContent(3)
        systs["M LLM SR"] = h_LLM_VR_23_ns.GetBinContent(4)
        systs["L LLM SR"] = h_LLM_VR_23_ns.GetBinContent(5)
        systs["P LM SR"] = h_LM_VR_23_ns.GetBinContent(1)
        systs["P3 LM SR"] = h_LM_VR_23_ns.GetBinContent(2)
        systs["P4 LM SR"] = h_LM_VR_23_ns.GetBinContent(3)
        systs["M LM SR"] = h_LM_VR_23_ns.GetBinContent(4)
        systs["L LM SR"] = h_LM_VR_23_ns.GetBinContent(5)
        systs["P LH SR"] = h_LH_VR_23_ns.GetBinContent(1)
        systs["P3 LH SR"] = h_LH_VR_23_ns.GetBinContent(2)
        systs["P4 LH SR"] = h_LH_VR_23_ns.GetBinContent(3)
        systs["M LH SR"] = h_LH_VR_23_ns.GetBinContent(4)
        systs["L LH SR"] = h_LH_VR_23_ns.GetBinContent(5)
        systs["P HL SR"] = h_HL_VR_4_ns.GetBinContent(1)
        systs["P3 HL SR"] = h_HL_VR_4_ns.GetBinContent(2)
        systs["P4 HL SR"] = h_HL_VR_4_ns.GetBinContent(3)
        systs["M HL SR"] = h_HL_VR_4_ns.GetBinContent(4)
        systs["L HL SR"] = h_HL_VR_4_ns.GetBinContent(5)
        systs["P HLM SR"] = h_HLM_VR_4_ns.GetBinContent(1)
        systs["P3 HLM SR"] = h_HLM_VR_4_ns.GetBinContent(2)
        systs["P4 HLM SR"] = h_HLM_VR_4_ns.GetBinContent(3)
        systs["M HLM SR"] = h_HLM_VR_4_ns.GetBinContent(4)
        systs["L HLM SR"] = h_HLM_VR_4_ns.GetBinContent(5)
        systs["P HM SR"] = h_HM_VR_4_ns.GetBinContent(1)
        systs["P3 HM SR"] = h_HM_VR_4_ns.GetBinContent(2)
        systs["P4 HM SR"] = h_HM_VR_4_ns.GetBinContent(3)
        systs["M HM SR"] = h_HM_VR_4_ns.GetBinContent(4)
        systs["L HM SR"] = h_HM_VR_4_ns.GetBinContent(5)
        systs["P HH SR"] = h_HH_VR_4_ns.GetBinContent(1)
        systs["P3 HH SR"] = h_HH_VR_4_ns.GetBinContent(2)
        systs["P4 HH SR"] = h_HH_VR_4_ns.GetBinContent(3)
        systs["M HH SR"] = h_HH_VR_4_ns.GetBinContent(4)
        systs["L HH SR"] = h_HH_VR_4_ns.GetBinContent(5)
        systs["P LL SR hi"] = h_LL_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LL SR hi"] = h_LL_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LL SR hi"] = h_LL_VR_23_hi_ns.GetBinContent(3)
        systs["M LL SR hi"] = h_LL_VR_23_hi_ns.GetBinContent(4)
        systs["L LL SR hi"] = h_LL_VR_23_hi_ns.GetBinContent(5)
        systs["P LLM SR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LLM SR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LLM SR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(3)
        systs["M LLM SR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(4)
        systs["L LLM SR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(5)
        systs["P LM SR hi"] = h_LM_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LM SR hi"] = h_LM_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LM SR hi"] = h_LM_VR_23_hi_ns.GetBinContent(3)
        systs["M LM SR hi"] = h_LM_VR_23_hi_ns.GetBinContent(4)
        systs["L LM SR hi"] = h_LM_VR_23_hi_ns.GetBinContent(5)
        systs["P LH SR hi"] = h_LH_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LH SR hi"] = h_LH_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LH SR hi"] = h_LH_VR_23_hi_ns.GetBinContent(3)
        systs["M LH SR hi"] = h_LH_VR_23_hi_ns.GetBinContent(4)
        systs["L LH SR hi"] = h_LH_VR_23_hi_ns.GetBinContent(5)
        systs["P HL SR hi"] = h_HL_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HL SR hi"] = h_HL_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HL SR hi"] = h_HL_VR_4_hi_ns.GetBinContent(3)
        systs["M HL SR hi"] = h_HL_VR_4_hi_ns.GetBinContent(4)
        systs["L HL SR hi"] = h_HL_VR_4_hi_ns.GetBinContent(5)
        systs["P HLM SR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HLM SR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HLM SR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(3)
        systs["M HLM SR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(4)
        systs["L HLM SR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(5)
        systs["P HM SR hi"] = h_HM_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HM SR hi"] = h_HM_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HM SR hi"] = h_HM_VR_4_hi_ns.GetBinContent(3)
        systs["M HM SR hi"] = h_HM_VR_4_hi_ns.GetBinContent(4)
        systs["L HM SR hi"] = h_HM_VR_4_hi_ns.GetBinContent(5)
        systs["P HH SR hi"] = h_HH_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HH SR hi"] = h_HH_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HH SR hi"] = h_HH_VR_4_hi_ns.GetBinContent(3)
        systs["M HH SR hi"] = h_HH_VR_4_hi_ns.GetBinContent(4)
        systs["L HH SR hi"] = h_HH_VR_4_hi_ns.GetBinContent(5)
        systs["P LL SR lo"] = h_LL_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LL SR lo"] = h_LL_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LL SR lo"] = h_LL_VR_23_lo_ns.GetBinContent(3)
        systs["M LL SR lo"] = h_LL_VR_23_lo_ns.GetBinContent(4)
        systs["L LL SR lo"] = h_LL_VR_23_lo_ns.GetBinContent(5)
        systs["P LLM SR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LLM SR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LLM SR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(3)
        systs["M LLM SR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(4)
        systs["L LLM SR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(5)
        systs["P LM SR lo"] = h_LM_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LM SR lo"] = h_LM_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LM SR lo"] = h_LM_VR_23_lo_ns.GetBinContent(3)
        systs["M LM SR lo"] = h_LM_VR_23_lo_ns.GetBinContent(4)
        systs["L LM SR lo"] = h_LM_VR_23_lo_ns.GetBinContent(5)
        systs["P LH SR lo"] = h_LH_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LH SR lo"] = h_LH_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LH SR lo"] = h_LH_VR_23_lo_ns.GetBinContent(3)
        systs["M LH SR lo"] = h_LH_VR_23_lo_ns.GetBinContent(4)
        systs["L LH SR lo"] = h_LH_VR_23_lo_ns.GetBinContent(5)
        systs["P HL SR lo"] = h_HL_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HL SR lo"] = h_HL_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HL SR lo"] = h_HL_VR_4_lo_ns.GetBinContent(3)
        systs["M HL SR lo"] = h_HL_VR_4_lo_ns.GetBinContent(4)
        systs["L HL SR lo"] = h_HL_VR_4_lo_ns.GetBinContent(5)
        systs["P HLM SR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HLM SR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HLM SR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(3)
        systs["M HLM SR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(4)
        systs["L HLM SR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(5)
        systs["P HM SR lo"] = h_HM_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HM SR lo"] = h_HM_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HM SR lo"] = h_HM_VR_4_lo_ns.GetBinContent(3)
        systs["M HM SR lo"] = h_HM_VR_4_lo_ns.GetBinContent(4)
        systs["L HM SR lo"] = h_HM_VR_4_lo_ns.GetBinContent(5)
        systs["P HH SR lo"] = h_HH_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HH SR lo"] = h_HH_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HH SR lo"] = h_HH_VR_4_lo_ns.GetBinContent(3)
        systs["M HH SR lo"] = h_HH_VR_4_lo_ns.GetBinContent(4)
        systs["L HH SR lo"] = h_HH_VR_4_lo_ns.GetBinContent(5)

    return vals,errs,systs

def makePlot(regions,vals,errs,systs,desc): 
    simplecanvas.cd()
    tl.Clear()
    nregions=len(regions)
    hobs=ROOT.TH1D(desc,desc+";;Normalized Short Track Counts",nregions,0,nregions)
    hobs.SetLineWidth(3)
    hpred=hobs.Clone(hobs.GetName()+"_prediction")
    ptindices = ["dummy0"]
    tag_index = 1
    for region in regions:
        ptindices.append(0 if region.find("lo") >= 0 else 1)
        hpred.GetXaxis().SetBinLabel(tag_index,region)
        pred = vals[region+" pre"]
        perr = errs[region+" pre"]
        obs = vals[region+" obs"]
        oerr = 1.83 if obs == 0 else errs[region+" obs"]
        if obs == 0 and verbose:
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
    hpred_syst = hpred.Clone(hpred.GetName()+"_syst")
    for bin in range(1,hpred_syst.GetNbinsX()+1):
        hpred_syst.SetBinError(bin,sqrt(hpred.GetBinError(bin)**2+nonclosure_error[ptindices[bin]]**2)) # nonclosure error is syst/sqrt(prediction)...kinda weird to normalize here
    hpred.SetFillColor(ROOT.kGray)
    hpred_syst.SetFillColor(ROOT.kGray+2)
    tl.AddEntry(hobs,"Observation")
    tl.AddEntry(hpred,"Prediction, with f_{short} Stat & Syst")
    tl.AddEntry(hpred_syst,"Prediction, with Non-closure Syst")
    hpred_syst.Draw("E2")
    hpred.Draw("E2 same")
    hpred_nofill.Draw("hist same")
    hobs.Draw("same E0")
    tl.Draw()
    simplecanvas.SaveAs("{0}/{1}.png".format(plotdir,desc.replace(" ","_")))

def makePlotRawOld(regions,vals,errs,systs,desc,rescale=1.0): # Raw means non-normalized. "rescale" multiplies prediction, to enable partial unblinding.
    ratiocanvas.cd()
    tl.Clear()
    nregions=len(regions)
    hobs=ROOT.TH1D(desc,desc+";;Short Track Counts",nregions,0,nregions)
    hobs.SetLineWidth(3)
    hpred=hobs.Clone(hobs.GetName()+"_prediction")
    perrs = []
    oerrs = []
    for index,region in enumerate(regions):
        bin_index = index+1
        hpred.GetXaxis().SetBinLabel(bin_index,region)
        pred = vals[region+" pre"]*rescale
        perr = errs[region+" pre"]*rescale
        obs = vals[region+" obs"]
        oerr = errs[region+" obs"]
        hpred.SetBinContent(bin_index,pred)
        perrs.append(perr)
        hobs.SetBinContent(bin_index,obs)
        oerrs.append(oerr)
    gpred = getPoissonGraph( hpred, perrs )
    gobs = getPoissonGraph( hobs, oerrs )
    hpred.GetXaxis().LabelsOption("v")
    hpred.GetXaxis().SetTitleOffset(4.8)
    hpred.SetMinimum(-0.001)
    hpred.SetMaximum(2.0*max(hpred.GetMaximum(),hobs.GetMaximum()))
    hobs.SetLineColor(ROOT.kRed)
    hpred.SetLineColor(ROOT.kBlack)
    hpred_nofill = hpred.Clone(hpred.GetName()+"nofill")
    hpred_syst = hpred.Clone(hpred.GetName()+"_syst")
    for index,region in enumerate(regions):
        bin_index = index+1
        hpred_syst.SetBinError(bin_index,sqrt(hpred_syst.GetBinError(bin_index)**2+(systs[region]*hpred_syst.GetBinContent(bin_index))**2))
    hpred.SetFillColor(ROOT.kGray)
    hpred_syst.SetFillColor(ROOT.kGray+2)
    tl.AddEntry(hobs,"Observation")
    tl.AddEntry(hpred,"Prediction, with f_{short} Stat & Syst")
    tl.AddEntry(hpred_syst,"Prediction, with Non-Closure Systematic")
    pads[0].cd()
    hpred_syst.Draw("E2")
    hpred.Draw("E2 same")
    hpred_nofill.Draw("hist same")
    hobs.Draw("same E0")
    tl.Draw()
    hratio = hobs.Clone("ratio"+desc)
    hratio.SetTitle("")
    hratio.SetLineColor(ROOT.kRed)
    hratio.SetFillColor(0)
    hratio.GetYaxis().SetLabelSize(hpred_syst.GetYaxis().GetLabelSize()*.83/.16/2)
    hratio.GetYaxis().SetTitleSize(hpred_syst.GetYaxis().GetTitleSize()*.83/.16/2)
    hratio.GetYaxis().SetTitleOffset(0.35)
    hratio.GetYaxis().SetTitle("Obs/Pred")
    hratio.Divide(hpred)
    for bin in range(1,hratio.GetNbinsX()+1):
        hratio.SetBinError(bin,hobs.GetBinError(bin)/hpred.GetBinContent(bin) if hpred.GetBinContent(bin) > 0 else 0)
    pads[1].cd()
    h1 = hratio.Clone(region+"1")
    hprojection = ROOT.TH1D("h_projection_"+desc,"Distribution of Obs/Pred Ratios;Obs/Pred;Region Count",12,0,3)
    hprojection.SetLineWidth(3)
    hprojection.SetLineColor(ROOT.kBlack)
    maxval = 0
    for bin in range(1,hratio.GetNbinsX()+1):
        h1.SetBinContent(bin,1)
        h1.GetXaxis().SetBinLabel(bin,"")
        h1.SetBinError(bin,0.0)
        ratio_content = hratio.GetBinContent(bin)
        maxval = max(ratio_content,maxval)
        if ratio_content >= 3: ratio_content = 2.99
        if ratio_content == 0: continue
        hprojection.Fill(ratio_content)
    h1_err = h1.Clone(region+"1err")
    h1_systerr = h1.Clone(region+"1systerr")
    for bin in range(1,hratio.GetNbinsX()+1):
        h1_err.SetBinError(bin,hpred.GetBinError(bin)/hpred.GetBinContent(bin) if hpred.GetBinContent(bin) > 0 else 0)
        h1_systerr.SetBinError(bin,hpred_syst.GetBinError(bin)/hpred.GetBinContent(bin) if hpred.GetBinContent(bin) > 0 else 0)
    h1_err.SetFillColor(ROOT.kGray)
    h1_systerr.SetFillColor(ROOT.kGray+2)
    h1_systerr.SetMinimum(-0.01)
    h1_systerr.SetMaximum(min(round(maxval+1),5))
    h1_systerr.SetLineColor(ROOT.kBlack)
    h1_systerr.Draw("E2")
    h1_err.Draw("E2 same")
    h1.SetLineColor(ROOT.kBlack)
    h1.Draw("hist same")
    hratio.Draw("same")
    ratiocanvas.SaveAs("{0}/{1}_raw.png".format(plotdir,desc.replace(" ","_")))
    simplecanvas.cd()
    hprojection.Draw()
    simplecanvas.SaveAs("{0}/{1}_ratios_zerosuppressed.png".format(plotdir,desc.replace(" ","_")))

def makePlotDiscrepanciesOld(regions_sets,vals_sets,errs_sets,systs_sets,desc,onlyNonMin=False,rescale=[1.0,1.0]): # Raw means non-normalized. "rescale" multiplies prediction, to enable partial unblinding.
    simplecanvas.cd()
    tl.Clear()
    # X.001 so slightly over X.0 is not in overlow, specifically for floating point weirdness in VR, which is +/- 1.0 by construction
    maxsigma = 1.001 if desc.find("VR") >= 0 else 2.001
    nbins = 10 if desc.find("VR") >= 0 else 20
    hsigma=ROOT.TH1D(desc,"N_{Pred} - N_{Obs}, "+desc+" Regions, N_{obs} > 0;(N_{Pred}-N_{Obs})/Error;Region Count",nbins+1,-maxsigma,maxsigma)
    hsigma.SetLineWidth(3)
    print "begin mpd"
    for set_index in range(len(regions_sets)):
        regions = regions_sets[set_index]
        vals = vals_sets[set_index]
        errs = errs_sets[set_index]
        systs = systs_sets[set_index]
        for index,region in enumerate(regions):
            bin_index = index+1
            pred = vals[region+" pre"]*rescale[set_index]
            perr = errs[region+" pre"]*rescale[set_index]
#            if pred < 0.5: continue
            obs = vals[region+" obs"]
            oerr = errs[region+" obs"]
            if oerr == 0: oerr = 1.83
            perr_syst = sqrt(perr**2 + (systs[region]*pred)**2)
            delta = pred - obs
            total_err = sqrt( oerr**2 + perr_syst**2 )
            sigma = delta/total_err
            if hsigma.FindBin(sigma) == 0 or hsigma.FindBin(sigma) == hsigma.GetNbinsX()+1:
                print "Sigma =", sigma, "is overflow", desc, "Setting to fall in max bin"
                sigma = maxsigma - 0.001 if sigma > 0 else maxsigma + 0.001 # Automatic overflow bins
            hsigma.Fill(sigma)
    print "end mpd"
    hsigma.SetMinimum(0)
    hsigma.SetMaximum(1.5*hsigma.GetMaximum())
    hsigma.SetLineColor(ROOT.kBlack)
    hsigma.Draw("hist")
    tl.AddEntry(hsigma,"(N_{{Pred}}-N_{{Obs}})/#sigma, Mean = {:.2f}, Deviation = {:.2f}".format(hsigma.GetMean(),hsigma.GetStdDev()))
    tl.Draw()
    simplecanvas.SaveAs("{0}/{1}_sigma_zerosuppressed.png".format(plotdir,desc.replace(" ","_")))


def makeSignalPlotOld(regions,vals_bg,errs_bg,list_of_vals_sig,list_of_errs_sig, systs, rescale_lumi, desc, sig_tags, sig_colors): # Don't rescale counts
    simplecanvas.cd()
    tl.Clear()
    nregions=len(regions)
    hbg=ROOT.TH1D(desc+"_counts",desc+";;Short Track Counts",nregions,0,nregions)
    hbg.SetLineWidth(3)
    hsigs = [hbg.Clone(hbg.GetName()+"_withSig_"+sig_tag) for sig_tag in sig_tags]
    for index,region in enumerate(regions):
        bin_index = index+1
        hbg.GetXaxis().SetBinLabel(bin_index,region)
        val_bg = vals_bg[region+" pre"]
        err_bg = errs_bg[region+" pre"]
        hbg.SetBinContent(bin_index,val_bg)
        hbg.SetBinError(bin_index,err_bg)
        for sig_index,sig_tag in enumerate(sig_tags):
            vals_sig = list_of_vals_sig[sig_index]
            errs_sig = list_of_errs_sig[sig_index]
            hsig = hsigs[sig_index]
            val_sig = vals_sig[region+" obs"] * rescale_lumi
            err_sig = errs_sig[region+" obs"]
            hsig.SetBinContent(bin_index,val_sig+val_bg) # Add val_bg so sig is sitting atop background, for easy error comparison
            hsig.SetBinError(bin_index,err_sig)
    hbg.GetXaxis().LabelsOption("v")
    hbg.GetXaxis().SetTitleOffset(4.8)
    hbg.SetMinimum(-0.001)
    hbg.SetMaximum(2.5*hsig.GetMaximum())
    for sig_index,hsig in enumerate(hsigs):
        hsig.SetLineColor(sig_colors[sig_index])
    hbg.SetLineColor(ROOT.kBlack)
    hbg_nofill = hbg.Clone(hbg.GetName()+"nofill")
    hbg_syst = hbg.Clone(hbg.GetName()+"_syst")
    for index,region in enumerate(regions):
        bin_index = index+1
        hbg_syst.SetBinError(bin_index,sqrt(hbg.GetBinError(bin_index)**2+(systs[region]*hbg.GetBinContent(bin_index))**2))
    hbg.SetFillColor(ROOT.kGray)
    hbg_syst.SetFillColor(ROOT.kGray+2)
    tl.AddEntry(hbg,"Predicted BG")
    tl.AddEntry(hbg_syst,"Predicted BG, with Non-closure Syst")
    hbg_syst.Draw("E2")
    hbg.Draw("E2 same")
    hbg_nofill.Draw("hist same")    
    for sig_index,hsig in enumerate(hsigs):
        hsig.Draw("same E0")
        tl.AddEntry(hsig,sig_tags[sig_index]+" + Pred BG (Signal MC Errors)")
    tl.Draw()
    simplecanvas.SaveAs("{0}/{1}_counts.png".format(plotdir,desc.replace(" ","_").replace("(","").replace(")","").replace(",","")))

def getMergedLineDataSTCOld(region,rescale16=1.0,rescale1718=1.0): # rescale multiplies prediction to enable partial unblinding
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]
    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & - & {:.0f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst) & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D16[region+" STC"], D16[region+" pre"]*rescale16, eD16[region+" pre"]*rescale16, sD16[region]*D16[region+" pre"]*rescale16, D16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & {:.0f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst) & {:.0f} & - & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D1718[region+" STC"], D1718[region+" pre"]*rescale1718, eD1718[region+" pre"]*rescale1718, sD1718[region]*D1718[region+" pre"]*rescale1718, D1718[region+" obs"])

    else:
        return colorline+"{} & {:.0f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst) & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst) & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D1718[region+" STC"], D1718[region+" pre"]*rescale1718, eD1718[region+" pre"]*rescale1718, sD1718[region]*D1718[region+" pre"]*rescale1718, D1718[region+" obs"],
                                                                                                                                                                                                                                    D16[region+" STC"], D16[region+" pre"]*rescale16, eD16[region+" pre"]*rescale16, sD16[region]*D16[region+" pre"]*rescale16, D16[region+" obs"])

def getMergedLineMCSTCOld(region):
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]
    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & - & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst)& {:.3f} $\pm$ {:.3f} \\\\ \n".format(region.replace(" VR","").replace(" SR",""),

                                  M16[region+" STC"], eM16[region+" STC"], M16[region+" pre"], eM16[region+" pre"], sM16[region]*M16[region+" pre"], M16[region+" obs"], eM16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst) & {:.3f} $\pm$ {:.3f} & - & - & - \\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M17[region+" STC"], eM17[region+" STC"], M17[region+" pre"], eM17[region+" pre"], sM17[region]*M17[region+" pre"], M17[region+" obs"], eM17[region+" obs"])

    else:
        return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst)& {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst) & {:.3f} $\pm$ {:.3f} \\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M17[region+" STC"], eM17[region+" STC"], M17[region+" pre"], eM17[region+" pre"], sM17[region]*M17[region+" pre"], M17[region+" obs"], eM17[region+" obs"],
                                  M16[region+" STC"], eM16[region+" STC"], M16[region+" pre"], eM16[region+" pre"], sM16[region]*M16[region+" pre"], M16[region+" obs"], eM16[region+" obs"])


def getLineData(region):
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D18[region+" pre"], eD18[region+" pre"], D18[region+" obs"],
                                                                                                                                                                                                                                    D17[region+" pre"], eD17[region+" pre"], D17[region+" obs"],
                                                                                                                                                                                                                                    D16[region+" pre"], eD16[region+" pre"], D16[region+" obs"])

def getLineMC(region):
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} \\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M17[region+" pre"], eM17[region+" pre"], M17[region+" obs"], eM17[region+" obs"],
                                  M16[region+" pre"], eM16[region+" pre"], M16[region+" obs"], eM16[region+" obs"])

def getLineDataSTC(region):
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    return colorline+"{} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D18[region+" STC"], D18[region+" pre"], eD18[region+" pre"], D18[region+" obs"],
                                                                                                                                                                                                                                    D17[region+" STC"], D17[region+" pre"], eD17[region+" pre"], D17[region+" obs"],
                                                                                                                                                                                                                                    D16[region+" STC"], D16[region+" pre"], eD16[region+" pre"], D16[region+" obs"])

def getLineMCSTC(region):
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} \\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M17[region+" STC"], eM17[region+" STC"], M17[region+" pre"], eM17[region+" pre"], M17[region+" obs"], eM17[region+" obs"],
                                  M16[region+" STC"], eM16[region+" STC"], M16[region+" pre"], eM16[region+" pre"], M16[region+" obs"], eM16[region+" obs"])


def getMergedLineDataOld(region,rescale16=1.0,rescale1718=1.0): # rescale multiplies prediction, to enable partial unblinding
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]
    if cat == "P ": # don't return 2017-2018
        return colorline+"{} & - & - & {:.3f} $\pm$ {:.3f} & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D16[region+" pre"]*rescale16, eD16[region+" pre"]*rescale16, D16[region+" obs"])
    elif cat == "P3" or cat == "P4": # don't return 2016
        return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.0f} & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D1718[region+" pre"]*rescale1718, eD1718[region+" pre"]*rescale1718, D1718[region+" obs"])
    else:
        return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D1718[region+" pre"]*rescale1718, eD1718[region+" pre"]*rescale1718, D1718[region+" obs"],
                                                                                                                                                                                                                                    D16[region+" pre"]*rescale16, eD16[region+" pre"]*rescale16, D16[region+" obs"])

def getMergedLineMC(region):
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]
    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} \\\\ \n".format(region.replace(" VR","").replace(" SR",""),

                                  M16[region+" pre"], eM16[region+" pre"], M16[region+" obs"], eM16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & - & - \\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M17[region+" pre"], eM17[region+" pre"], M17[region+" obs"], eM17[region+" obs"])
    else:
        return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} \\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M17[region+" pre"], eM17[region+" pre"], M17[region+" obs"], eM17[region+" obs"],
                                  M16[region+" pre"], eM16[region+" pre"], M16[region+" obs"], eM16[region+" obs"])

def getFSLineDataOld(cat):
    return "{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(cat,D18f[cat],eD18f[cat],D17f[cat],eD17f[cat],D16f[cat],eD16f[cat])

def getFSLineMCOld(cat):
    return "{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(cat,M17f[cat],eM17f[cat],M16f[cat],eM16f[cat])

def getMergedFSLineDataOld(cat):
    subcat = cat[0:2]
    if subcat == "P ": # Don't return 2017-18
        return "{} & - & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst)\\\\ \n".format(cat,D16f[cat],eD16f[cat],sD16f[cat])
    elif subcat == "P3" or subcat == "P4": # don't return 2016
        return "{} & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst) & -\\\\ \n".format(cat,D1718f[cat],eD1718f[cat],sD1718f[cat])
    else:
        return "{} & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst) & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst)\\\\ \n".format(cat,D1718f[cat],eD1718f[cat],sD1718f[cat],D16f[cat],eD16f[cat],sD16f[cat])

def getMergedFSLineMCOld(cat):
    subcat = cat[0:2]
    if subcat == "P ": # Don't return 2017-18
        return "{} & - & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst)\\\\ \n".format(cat,M16f[cat],eM16f[cat],sM16f[cat])
    elif subcat == "P3" or subcat == "P4": # don't return 2016
        return "{} & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst) & -\\\\ \n".format(cat,M17f[cat],eM17f[cat],sM17f[cat])
    else:
        return "{} & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst) & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst) \\\\ \n".format(cat,M17f[cat],eM17f[cat],sM17f[cat],M16f[cat],eM16f[cat],sM16f[cat])
