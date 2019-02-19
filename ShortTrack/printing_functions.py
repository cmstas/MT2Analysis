import ROOT
from math import sqrt
import numpy as np
import math
import ppmUtils as utils

verbose = False

tag = ""
plotdir = ""
tabledir = ""
colorTables = False
format = "pdf"

def SetTag(toSet):
    global tag
    tag = toSet

def SetPlotDir(toSet):
    global plotdir
    plotdir = toSet

def SetTableDir(toSet):
    global tabledir
    tabledir = ""

def SetColorTables(toSet):
    global colorTables
    colorTables = toSet

def SetFormat(toSet):
    global format
    format = toSet

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
tr=ROOT.TLegend(0.55,0.70,0.75,0.90)

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

def start17vs18FshortTableData(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | c | c}\n")
    outfile.write("\\toprule\n")
    outfile.write("Category & 2017 Data & 2018 Data\\\\ \n")
    outfile.write("\hline\n")

def start17vs18FshortTableMC(outfile):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | c | c}\n")
    outfile.write("\\toprule\n")
    outfile.write("Category & 2017 MC & 2018 MC\\\\ \n")
    outfile.write("\hline\n")

def startMergedCountsTable(outfile,year):
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | c c | *3c| *3c}\n")
    outfile.write("\\toprule\n")
    outfile.write("\multirow{2}{*}{Region} & \multicolumn{2}{c}{"+year+" Data} & \multicolumn{3}{c}{10 cm Signal} & \multicolumn{3}{c}{90 cm Signal} \\\\ \n")
    outfile.write(" & Pred & Obs & 1800, 1400 GeV & 1800, 1600 GeV & 1800, 1700 GeV & 1800, 1400 GeV & 1800, 1600 GeV & 1800, 1700 GeV\\\\ \n ")
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

def GetPullGamma(pred, syst, obs, nstc, fshort, N=1000000):
    s, t = 0, 0
    for i in range( N ):
        #gamma_toy = fshort * np.random.gamma(nstc + 1)
        gamma_toy = np.random.gamma(nstc + 1,fshort)
        if gamma_toy <= 0: continue
        #    relsyst = 1.0 + syst/gamma_toy
        relsyst = 1.0 + syst
        seed = gamma_toy*(relsyst**np.random.randn())
        seed = int(seed) & 0xFFFFFFFF
        try: toyobs = np.random.poisson(seed)
        except:
            print "failed with seed: {}".format(seed)
            continue
        t += 1
        if toyobs < obs:
            s += 1
        if obs < pred and toyobs == obs:
            s += 1
    return ROOT.Math.normal_quantile(float(s)/t, 1)

def getPullPlotGamma(g_data, g_pred, g_ratio, nstcs, fshorts):
    for ibin in range(g_data.GetN()):
        nstc = nstcs[ibin-1]
        fshort = fshorts[ibin-1]
        x, ydata, ypred = ROOT.Double(), ROOT.Double(), ROOT.Double()
        g_data.GetPoint(ibin, x, ydata)
        g_pred.GetPoint(ibin, x, ypred)
        if ydata==0.0 and ypred==0.0:
            continue
        elif ypred == 0:
            print x,ydata,ypred
            continue
        if ypred < 200 and ydata > 0.0:
            if ydata > ypred:
                pull = GetPull(ypred, g_pred.GetErrorYhigh(ibin), ydata, nstc, fshort)
            else:
                pull = GetPull(ypred, g_pred.GetErrorYlow(ibin), ydata, nstc, fshort)
        else:
            if ydata > ypred:
                err_data = sqrt(ypred)
                err_pred = g_pred.GetErrorYhigh(ibin)
            else:
                err_data = sqrt(ypred)
                err_pred = g_pred.GetErrorYlow(ibin)

            pull = (ydata - ypred) / math.sqrt(err_data**2 + err_pred**2)


        if abs(pull) > 2.5:
            print "BIN WITH BIG PULL: ", pull
        thisPoint = g_ratio.GetN()
        g_ratio.SetPoint(thisPoint, x, pull)
        g_ratio.SetPointError(thisPoint, 0.0, 0.0, 0.0, 0.0)

def GetPull(pred, syst, obs, N=500000):
    relsyst = 1.0 + syst/pred
    s, t = 0, 0
    for i in range( N ):
        t += 1
        toyobs = np.random.poisson(pred*(relsyst**np.random.randn()))
        if toyobs < obs:
            s += 1
        if obs < pred and toyobs == obs:
            s += 1
    return ROOT.Math.normal_quantile(float(s)/t, 1)

def getPullPlot(g_data, g_pred, g_ratio):
    for ibin in range(g_data.GetN()):
        x, ydata, ypred = ROOT.Double(), ROOT.Double(), ROOT.Double()
        g_data.GetPoint(ibin, x, ydata)
        g_pred.GetPoint(ibin, x, ypred)
        if ydata==0.0 and ypred==0.0:
            continue
        elif ypred == 0:
            print "prediction 0 but obs nonzero",x
            continue
        if ypred < 200 and ydata > 0.0:
            if ydata > ypred:
                pull = GetPull(ypred, g_pred.GetErrorYhigh(ibin), ydata)
            else:
                pull = GetPull(ypred, g_pred.GetErrorYlow(ibin), ydata)
        else:
            if ydata > ypred:
                err_data = sqrt(ypred)
                err_pred = g_pred.GetErrorYhigh(ibin)
            else:
                err_data = sqrt(ypred)
                err_pred = g_pred.GetErrorYlow(ibin)

            pull = (ydata - ypred) / math.sqrt(err_data**2 + err_pred**2)


        if abs(pull) > 3.0:
            print "BIN WITH BIG PULL: ", pull
        thisPoint = g_ratio.GetN()
        g_ratio.SetPoint(thisPoint, x, pull)
        g_ratio.SetPointError(thisPoint, 0.0, 0.0, 0.0, 0.0)


def getRatioGraph( histo_data, histo_mc, data_errs = None, mc_errs = None ):

    if not histo_data or not histo_mc: 
        return 0
    
    graph  = ROOT.TGraphAsymmErrors()
    graph_data = getPoissonGraph(histo_data, data_errs)
    graph_mc = getPoissoGraph(histo_mc, mc_errs)
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
    if region.find(" H") >= 0:
        nj = " 4"
    elif region.find(" L") >= 0:
        nj = " 23"
    elif region.find(" 1") >= 0:
        nj = " 1"
    else:
        print "Could not identify corresponding fshort for region {}, aborting".format(region)
        exit(1)
    if region.find("lo") >= 0:
        pt = " lo"
    elif region.find("hi") >= 0:
        pt = " hi"
    else:
        pt = ""
    return cat+nj+pt

def getFshorts(f,inclMonojet=False):
    filename = f.GetName()[:f.GetName().find(".root")]
    h_FS = f.Get("h_FS").Clone(filename+"fs1")
    h_FS_23 = f.Get("h_FS_23").Clone(filename+"fs2")
    h_FS_4 = f.Get("h_FS_4").Clone(filename+"fs3")
    if inclMonojet: h_FS_1 = f.Get("h_FS_1").Clone(filename+"fs2_1")
    h_FS_hi = f.Get("h_FS_hi").Clone(filename+"fs4")
    h_FS_23_hi = f.Get("h_FS_23_hi").Clone(filename+"fs5")
    h_FS_4_hi = f.Get("h_FS_4_hi").Clone(filename+"fs6")
    if inclMonojet: h_FS_1 = f.Get("h_FS_1_hi").Clone(filename+"fs2_1")
    h_FS_lo = f.Get("h_FS_lo").Clone(filename+"fs7")
    h_FS_23_lo = f.Get("h_FS_23_lo").Clone(filename+"fs8")
    h_FS_4_lo = f.Get("h_FS_4_lo").Clone(filename+"fs9")
    if inclMonojet: h_FS_1_lo = f.Get("h_FS_1_lo").Clone(filename+"fs7_1")
    h_FS_up = f.Get("h_FS_up").Clone(filename+"fs10")
    h_FS_23_up = f.Get("h_FS_23_up").Clone(filename+"fs11")
    h_FS_4_up = f.Get("h_FS_4_up").Clone(filename+"fs12")
    if inclMonojet: h_FS_1_up = f.Get("h_FS_1_up").Clone(filename+"fs12")
    h_FS_hi_up = f.Get("h_FS_hi_up").Clone(filename+"fs13")
    h_FS_23_hi_up = f.Get("h_FS_23_hi_up").Clone(filename+"fs14")
    h_FS_4_hi_up = f.Get("h_FS_4_hi_up").Clone(filename+"fs15")
    h_FS_lo_up = f.Get("h_FS_lo_up").Clone(filename+"fs16")
    h_FS_23_lo_up = f.Get("h_FS_23_lo_up").Clone(filename+"fs17")
    h_FS_4_lo_up = f.Get("h_FS_4_lo_up").Clone(filename+"fs18")
    if inclMonojet: h_FS_1_lo_up = f.Get("h_FS_1_lo_up").Clone(filename+"fs18_1")
    h_FS_dn = f.Get("h_FS_dn").Clone(filename+"fs19")
    h_FS_23_dn = f.Get("h_FS_23_dn").Clone(filename+"fs20")
    h_FS_4_dn = f.Get("h_FS_4_dn").Clone(filename+"fs21")
    if inclMonojet: h_FS_1_dn = f.Get("h_FS_1_dn").Clone(filename+"fs21_1")
    h_FS_hi_dn = f.Get("h_FS_hi_dn").Clone(filename+"fs22")
    h_FS_23_hi_dn = f.Get("h_FS_23_hi_dn").Clone(filename+"fs23")
    h_FS_4_hi_dn = f.Get("h_FS_4_hi_dn").Clone(filename+"fs24")
    if inclMonojet: h_FS_1_hi_dn = f.Get("h_FS_1_hi_dn").Clone(filename+"fs24_1")
    h_FS_lo_dn = f.Get("h_FS_lo_dn").Clone(filename+"fs25")
    h_FS_23_lo_dn = f.Get("h_FS_23_lo_dn").Clone(filename+"fs26")
    h_FS_4_lo_dn = f.Get("h_FS_4_lo_dn").Clone(filename+"fs27")
    if inclMonojet: h_FS_1_lo_dn = f.Get("h_FS_1_lo_dn").Clone(filename+"fs27_1")
    h_FS_syst = f.Get("h_FS_syst").Clone(filename+"fs28")
    h_FS_23_syst = f.Get("h_FS_23_syst").Clone(filename+"fs29")
    h_FS_4_syst = f.Get("h_FS_4_syst").Clone(filename+"fs30")
    if inclMonojet: h_FS_1_syst = f.Get("h_FS_1_syst").Clone(filename+"fs30_1")
    h_FS_hi_syst = f.Get("h_FS_hi_syst").Clone(filename+"fs31")
    h_FS_23_hi_syst = f.Get("h_FS_23_hi_syst").Clone(filename+"fs32")
    h_FS_4_hi_syst = f.Get("h_FS_4_hi_syst").Clone(filename+"fs33")
    if inclMonojet: h_FS_1_hi_syst = f.Get("h_FS_1_hi_syst").Clone(filename+"fs33_1")
    h_FS_lo_syst = f.Get("h_FS_lo_syst").Clone(filename+"fs34")
    h_FS_23_lo_syst = f.Get("h_FS_23_lo_syst").Clone(filename+"fs35")
    h_FS_4_lo_syst = f.Get("h_FS_4_lo_syst").Clone(filename+"fs36")
    if inclMonojet: h_FS_1_lo_syst = f.Get("h_FS_1_lo_syst").Clone(filename+"fs36_1")
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

    if inclMonojet: 
        vals["P 1"] = h_FS_1.GetBinContent(1)
        vals["P3 1"] = h_FS_1.GetBinContent(2)
        vals["P4 1"] = h_FS_1.GetBinContent(3)
        vals["M 1"] = h_FS_1.GetBinContent(4)
        vals["L 1"] = h_FS_1.GetBinContent(5)

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

    if inclMonojet: 
        errs["P 1"] = [h_FS_1_up.GetBinError(1), h_FS_1_dn.GetBinError(1)]
        errs["P3 1"] = [h_FS_1_up.GetBinError(2), h_FS_1_dn.GetBinError(2)]
        errs["P4 1"] = [h_FS_1_up.GetBinError(3), h_FS_1_dn.GetBinError(3)]
        errs["M 1"] = [h_FS_1_up.GetBinError(4), h_FS_1_dn.GetBinError(4)]
        errs["L 1"] = [h_FS_1_up.GetBinError(5), h_FS_1_dn.GetBinError(5)]

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

    if inclMonojet: 
        systs["P 1"] = h_FS_1_syst.GetBinError(1)
        systs["P3 1"] = h_FS_1_syst.GetBinError(2)
        systs["P4 1"] = h_FS_1_syst.GetBinError(3)
        systs["M 1"] = h_FS_1_syst.GetBinError(4)
        systs["L 1"] = h_FS_1_syst.GetBinError(5)

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

    if inclMonojet: 
        vals["P 1 hi"] = h_FS_1_hi.GetBinContent(1)
        vals["P3 1 hi"] = h_FS_1_hi.GetBinContent(2)
        vals["P4 1 hi"] = h_FS_1_hi.GetBinContent(3)
        vals["M 1 hi"] = h_FS_1_hi.GetBinContent(4)
        vals["L 1 hi"] = h_FS_1_hi.GetBinContent(5)

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

    if inclMonojet: 
        errs["P 1 hi"] = [h_FS_1_hi_up.GetBinError(1), h_FS_1_hi_dn.GetBinError(1)]
        errs["P3 1 hi"] = [h_FS_1_hi_up.GetBinError(2), h_FS_1_hi_dn.GetBinError(2)]
        errs["P4 1 hi"] = [h_FS_1_hi_up.GetBinError(3), h_FS_1_hi_dn.GetBinError(3)]
        errs["M 1 hi"] = [h_FS_1_hi_up.GetBinError(4), h_FS_1_hi_dn.GetBinError(4)]
        errs["L 1 hi"] = [h_FS_1_hi_up.GetBinError(5), h_FS_1_hi_dn.GetBinError(5)]

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

    if inclMonojet: 
        systs["P 1 hi"] = h_FS_1_hi_syst.GetBinError(1)
        systs["P3 1 hi"] = h_FS_1_hi_syst.GetBinError(2)
        systs["P4 1 hi"] = h_FS_1_hi_syst.GetBinError(3)
        systs["M 1 hi"] = h_FS_1_hi_syst.GetBinError(4)
        systs["L 1 hi"] = h_FS_1_hi_syst.GetBinError(5)


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

    if inclMonojet: 
        vals["P 1 lo"] = h_FS_1_lo.GetBinContent(1)
        vals["P3 1 lo"] = h_FS_1_lo.GetBinContent(2)
        vals["P4 1 lo"] = h_FS_1_lo.GetBinContent(3)
        vals["M 1 lo"] = h_FS_1_lo.GetBinContent(4)
        vals["L 1 lo"] = h_FS_1_lo.GetBinContent(5)

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

    if inclMonojet: 
        errs["P 1 lo"] = [h_FS_1_lo_up.GetBinError(1), h_FS_1_lo_dn.GetBinError(1)]
        errs["P3 1 lo"] = [h_FS_1_lo_up.GetBinError(2), h_FS_1_lo_dn.GetBinError(2)]
        errs["P4 1 lo"] = [h_FS_1_lo_up.GetBinError(3), h_FS_1_lo_dn.GetBinError(3)]
        errs["M 1 lo"] = [h_FS_1_lo_up.GetBinError(4), h_FS_1_lo_dn.GetBinError(4)]
        errs["L 1 lo"] = [h_FS_1_lo_up.GetBinError(5), h_FS_1_lo_dn.GetBinError(5)]

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

    if inclMonojet: 
        systs["P 1 lo"] = h_FS_1_lo_syst.GetBinError(1)
        systs["P3 1 lo"] = h_FS_1_lo_syst.GetBinError(2)
        systs["P4 1 lo"] = h_FS_1_lo_syst.GetBinError(3)
        systs["M 1 lo"] = h_FS_1_lo_syst.GetBinError(4)
        systs["L 1 lo"] = h_FS_1_lo_syst.GetBinError(5)

    return vals,errs,systs

def getCounts(f,fshorts,fshort_systs,isMC = False,isSig = False,inclMonojet=False):
    filename = f.GetName()[:f.GetName().find(".root")]
    vals = {}
    stats = {}
    systs = {}
    njhts = ["LL","LLM","LM","LH","HL","HLM","HM","HH"]
    regions = ["MR","VR","SR"]
    pts = ["","_hi","_lo"]
    histnames = ["h_{}_{}_{}{}".format(njht,region,"23" if njht[0] == "L" else "4",pt) for njht in njhts for region in regions for pt in pts] # Multijet
    if inclMonojet: histnames += ["h_{}_1{}".format(region,pt) for region in regions for pt in pts] # Monojet
    for histname in histnames:
        isMRorSig = histname.find("MR") >= 0 or isSig
        tokens = histname.split("_")
        njht = tokens[1]
        region = tokens[2]
        pt = " "+tokens[4] if len(tokens) > 4 else ""
        core = "{} {}{}".format(njht,region,pt)
        try:
            hist_stcstats = f.Get(histname+"_stcstats_up" if not isSig else histname).Clone(filename+"1") # we only care about the central value, so just use the one that exists also for MR
        except:
            print "Couldn't get stcstats_up for {}, skipping".format(histname)
            continue
        hist_allstats_up = f.Get(histname+"_allstats_up").Clone(filename+"2") if not isMRorSig else None
        hist_allstats_dn = f.Get(histname+"_allstats_dn").Clone(filename+"3") if not isMRorSig else None
        hist_fs = f.Get(histname+"_fshortsyst").Clone(filename+"4") if not isMRorSig else None
        hist_nc = f.Get(histname.replace("SR","VR")+"_nonclosure_systematic").Clone(filename+"5") if not isMRorSig else None # use VR-derived syst in SR
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
            if isMRorSig: continue
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

def getCountsPostfit(f,year):
    filename = f.GetName()[:f.GetName().find(".root")]
    vals = {}
    errs = {}
    njhts = ["LL","LLM","LM","LH","HL","HLM","HM","HH"]
    pts = ["","_hi","_lo"]
    lengths = ["P","P3","P4","M","L"]
    histnames = ["shapes_fit_b/{}_SR_{}{}_{}_{}/total_background".format(njht,"23" if njht[0] == "L" else "4",pt,length,year) for njht in njhts for pt in pts for length in lengths] # Multijet
    for histname in histnames:
        h = f.Get(histname)
        if h == None: continue
        else:
            datacard_region = histname.split("/")[1]
            tokens = datacard_region.split("_")
            njht = tokens[0]
            pt = "" if len(tokens) == 5 else " "+tokens[3]
            length = tokens[len(tokens)-2]
            region_key=length+" "+njht+" SR"+pt
            vals[region_key] = h.GetBinContent(1)
            errs[region_key] = h.GetBinError(1)
    return vals,errs

def getSignalCountsPostfit(f,year):
    filename = f.GetName()[:f.GetName().find(".root")]
    vals = {}
    errs = {}
    njhts = ["LL","LLM","LM","LH","HL","HLM","HM","HH"]
    pts = ["","_hi","_lo"]
    lengths = ["P","P3","P4","M","L"]
    histnames = ["shapes_fit_s/{}_SR_{}{}_{}_{}/total_background".format(njht,"23" if njht[0] == "L" else "4",pt,length,year) for njht in njhts for pt in pts for length in lengths] # Multijet
    for histname in histnames:
        h = f.Get(histname)
        if h == None: continue
        else:
            datacard_region = histname.split("/")[1]
            tokens = datacard_region.split("_")
            njht = tokens[0]
            pt = "" if len(tokens) == 5 else " "+tokens[3]
            length = tokens[len(tokens)-2]
            region_key=length+" "+njht+" SR"+pt
            vals[region_key+" B"] = h.GetBinContent(1)
            errs[region_key+" B"] = h.GetBinError(1)
    histnames = ["shapes_fit_s/{}_SR_{}{}_{}_{}/total_signal".format(njht,"23" if njht[0] == "L" else "4",pt,length,year) for njht in njhts for pt in pts for length in lengths] # Multijet
    for histname in histnames:
        h = f.Get(histname)
        datacard_region = histname.split("/")[1]
        tokens = datacard_region.split("_")
        njht = tokens[0]
        pt = "" if len(tokens) == 5 else " "+tokens[3]
        length = tokens[len(tokens)-2]
        region_key=length+" "+njht+" SR"+pt
        if h == None: 
            vals[region_key+" S"] = 0
            errs[region_key+" S"] = 0
            continue
        else:
            vals[region_key+" S"] = h.GetBinContent(1)
            errs[region_key+" S"] = h.GetBinError(1)
    return vals,errs

def makePlotFshort(regions,dvals,derrs,dsysts,mvals,merrs,msysts,desc):
    if desc.find("16") >= 0:
        lumi = 35.92
    elif desc.find("17"):
        lumi = 41.53
        if desc.find("18"):
            lumi += 59.97
    elif desc.find("18"):
        lumi = 59.97
    else: lumi = 1.0
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
    utils.DrawCmsText(simplecanvas)
    utils.DrawLumiText(simplecanvas,lumi)
    simplecanvas.SaveAs("{}/{}_fshort.{}".format(plotdir,desc.replace(" ","_"),format))

def getBinLabelColor(region):
    tokens = region.split(" ")
    track_category = tokens[0]
    njet = tokens[1][0]
    pt = "" if len(tokens) < 4 else tokens[3]
    if track_category == "P":
        if njet == "L":
            if pt == "hi":
                color = ROOT.kRed    
            else:
                color = ROOT.kYellow+1
        else:
            if pt == "hi":
                color = ROOT.kBlack
            else:
                color = ROOT.kGray+2
    elif track_category == "P3":
        if njet == "L":
            if pt == "hi":
                color = ROOT.kBlue
            else:
                color = ROOT.kBlue+2
        else:
            if pt == "hi":
                color = ROOT.kGreen+1
            else:
                color = ROOT.kGreen+3
    elif track_category == "P4":
        if njet == "L":
            if pt == "hi":
                color = ROOT.kCyan
            else:
                color = ROOT.kCyan+3
        else:
            if pt == "hi":
                color = ROOT.kMagenta
            else:
                color = ROOT.kMagenta+3
    elif track_category == "M":
        if njet == "L":
            if pt == "hi":
                color = ROOT.kRed+2
            else:
                color = ROOT.kYellow+3
        else:
            if pt == "hi":
                color = ROOT.kRed-9
            else:
                color = ROOT.kPink-9
    else:
       if njet == "L":
           color = ROOT.kOrange-3
       else:
           color = ROOT.kOrange+4
    return color

def makePlotRaw(regions,vals,stats,systs,fshorts,desc,rescale=1.0, combineSysts = True, doPullPlot = False): # Raw means non-normalized. "rescale" multiplies prediction, to enable partial unblinding.
    if desc.find("16") >= 0:
        lumi = 35.92
    elif desc.find("17"):
        lumi = 41.53
        if desc.find("18"):
            lumi += 59.97
    elif desc.find("18"):
        lumi = 59.97
    else: lumi = 1.0
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
    perrs_justsyst = []
    most_discrepant_sigma = 0
    most_discrepant_obs = 0
    most_discrepant_pred = 0
    most_discrepant_perr = 0
    most_discrepant_region = "None"
    for index,region in enumerate(regions):
        bin_index = index+1
        hpred.GetXaxis().SetBinLabel(bin_index,"#color[{}]{{{}}}".format(getBinLabelColor(region),region.replace(" SR","").replace(" VR","")))
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
        # find maximally discrepant bin
        relevant_perr = perrs_all[index][0] if obs > pred else perrs_all[index][1]
        relevant_oerr = oerr[0] if obs < pred else oerr[1]
        total_err = sqrt(relevant_perr**2 + relevant_oerr**2)
        delta = abs(pred - obs)
        sigma = delta / total_err if total_err > 0 else -1
        if sigma > most_discrepant_sigma:
            most_discrepant_sigma = sigma
            most_discrepant_obs = obs
            most_discrepant_pred = pred
            most_discrepant_perr = relevant_perr
            most_discrepant_region = region
    print desc,"Most discrepant:",most_discrepant_region,"sigma",most_discrepant_sigma,"obs",most_discrepant_obs,"pred",most_discrepant_pred,"perr",most_discrepant_perr
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
    utils.DrawCmsText(pads[0])
    utils.DrawLumiText(pads[0],lumi)
    tl.AddEntry(gobs,"Observation","pLE")
    tl.AddEntry(gpred_stat,"Prediction, Statistical Errors")
    if not combineSysts:
        tl.AddEntry(gpred_withfs,"Prediction, with f_{short} Syst")
    tl.AddEntry(gpred_all,"Prediction, with Total Error" if combineSysts else "Prediction, with also VR Syst")
    tl.Draw()
    pads[1].cd()
    if doPullPlot:
        print desc,"pull plot"
        h1 = ROOT.TH1D("fill_for_"+desc,";;Pull",len(regions),0,len(regions))
        h2 = ROOT.TH1D("fill2_for_"+desc,";;Pull",len(regions),0,len(regions))
        h2.GetYaxis().SetTitleSize(h2.GetYaxis().GetTitleSize()*2)
        h2.GetYaxis().SetTitleOffset(0.6)
        h2.GetYaxis().SetLabelSize(h2.GetYaxis().GetLabelSize()*2)
        h1.SetFillColor(ROOT.kWhite)
        h1.SetLineColor(ROOT.kBlack)
        h2.SetFillColor(ROOT.kGray)
        h2.SetLineColor(ROOT.kGray)
        h2.SetMaximum(2.5)
        h2.SetMinimum(-2.5)
#        fshort_list = []
#        nstcs = []
        perrs_justsyst = []
        for index,region in enumerate(regions):
            bin_index = index+1
#            perr_fsrel = systs[region+" fsrel"] * rescale
#            perr_ncrel = systs[region+" ncrel"] * rescale
#            syst_err = sqrt(perr_fsrel**2 + perr_ncrel**2)
#            perrs_justsyst.append([syst_err, syst_err])
#            fshort_name = correspondingFshort(region)
#            fshort_list.append(fshorts[fshort_name])
#            nstcs.append(vals[region+" STC"])
            perr_fs = systs[region+" fs"] * rescale
            perr_nc = systs[region+" nc"] * rescale
            syst_err = sqrt(perr_fs**2 + perr_nc**2)
            perrs_justsyst.append( [syst_err, syst_err] )
            h2.SetBinContent(bin_index,0)
            h2.SetBinError(bin_index,2)
            h2.GetXaxis().SetBinLabel(bin_index,"")
            h1.SetBinContent(bin_index,0)
            h1.SetBinError(bin_index,1)
        hpred.GetXaxis().LabelsOption("v")
        hpred.GetXaxis().SetTitleOffset(4.8)
        simplecanvas.cd()
#        g_pred_all = getPoissonGraph( hpred, perrs_justsyst )
        g_pred_all = getPoissonGraph( hpred, perrs_all )
        g_obs = gobs.Clone()
        g_pull = ROOT.TGraphAsymmErrors()
#        getPullPlot(g_obs, g_pred_all, g_pull, nstcs, fshort_list)
        getPullPlot(g_obs, g_pred_all, g_pull)
        g_pull.SetMarkerStyle(20)
        g_pull.SetMarkerSize(1)
        g_pull.SetLineWidth(1)
        pads[1].cd()
        h2.Draw("E2")
        h1.Draw("same E2")
        h1.Draw("same hist")
        g_pull.Draw("SAME P0")
        h2.Draw("same axis")
    else:        
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
    ratiocanvas.SaveAs("{}/{}_raw_{}.{}".format(plotdir,desc.replace(" ","_"),"pull" if doPullPlot else "ratio",format))
    pads[0].cd()
    hpred.SetMinimum(0.1)
    hpred.SetMaximum(200)
    hpred.Draw("") # only want the axis from the histogram, for bin titles
    gpred_all.Draw("2 same") # 2 means draw filled rectangles for errors
    if not combineSysts:
        gpred_withfs.Draw("2 same")
    gpred_stat.Draw("2 same")
    hpred.Draw("same") # drawn without errors, and without connecting the lines, see above
    gobs.Draw("p same") # p draws in a typical histogram style, with markers
    hpred.Draw("AXIS same") # make tick marks show above fill areas
    utils.DrawCmsText(pads[0])
    utils.DrawLumiText(pads[0],lumi)
    tr.Clear()
    tr.AddEntry(gobs,"Observation","pLE")
    tr.AddEntry(gpred_stat,"Prediction, Statistical Errors")
    if not combineSysts:
        tr.AddEntry(gpred_withfs,"Prediction, with f_{short} Syst")
    tr.AddEntry(gpred_all,"Prediction, with Total Error" if combineSysts else "Prediction, with also VR Syst")
    tr.Draw()
    pads[0].SetLogy(True)
    ratiocanvas.SaveAs("{}/{}_raw_{}_logscale.{}".format(plotdir,desc.replace(" ","_"),"pull" if doPullPlot else "ratio",format))
    pads[0].SetLogy(False)

def makePlotPostfit(regions,vals,stats,systs,fshorts,vals_postfit,errs_postfit,desc,rescale=1.0, combineSysts = True, doPullPlot = False): # Raw means non-normalized. "rescale" multiplies prediction, to enable partial unblinding.
    if desc.find("16") >= 0:
        lumi = 35.92
    elif desc.find("17"):
        lumi = 41.53
        if desc.find("18"):
            lumi += 59.97
    elif desc.find("18"):
        lumi = 59.97
    else: lumi = 1.0
    ratiocanvas.cd()
    tl.Clear()
    nregions=len(regions)
    hobs=ROOT.TH1D(desc,desc+";;Short Track Counts",nregions,0,nregions)
    hobs.SetLineWidth(3)
    hpred=hobs.Clone(hobs.GetName()+"_prediction")
    hpost=hobs.Clone(hobs.GetName()+"_postfit")
    perrs = []
    oerrs = []
    perrs_withfs = []
    perrs_all = []
    perrs_justsyst = []
    for index,region in enumerate(regions):
        bin_index = index+1
        hpred.GetXaxis().SetBinLabel(bin_index,"#color[{}]{{{}}}".format(getBinLabelColor(region),region.replace(" SR","").replace(" VR","")))
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
        hpost.SetBinContent(bin_index,vals_postfit[region]*rescale)
        hpost.SetBinError(bin_index,errs_postfit[region]*rescale)
    hpred.GetXaxis().LabelsOption("v")
    hpred.GetXaxis().SetTitleOffset(4.8)
    hpred.SetMinimum(-0.001)
    hpred.SetMaximum(2.0*max(hpred.GetMaximum(),hobs.GetMaximum()))
    hobs.SetLineColor(ROOT.kBlack)
    hobs.SetMinimum(hpred.GetMinimum())
    hobs.SetMaximum(hpred.GetMaximum())
    hpred.SetLineColor(ROOT.kRed)
    hpost.SetLineColor(ROOT.kBlue)
    hpost_noerr = hpost.Clone(hpost.GetName()+"_noerr")
    for bin in range(1,hpost.GetNbinsX()+1): hpost_noerr.SetBinError(bin,1e-9)
    hpost.SetFillColor(ROOT.kBlue)
    hpost.SetFillStyle(3244) # hashed fill
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
    hpost.Draw("E2 same")
    hpost_noerr.Draw("same")
    hpred.Draw("AXIS same") # make tick marks show above fill areas
    utils.DrawCmsText(pads[0])
    utils.DrawLumiText(pads[0],lumi)
    tl.AddEntry(gobs,"Observation","pLE")
    tl.AddEntry(gpred_stat,"Prediction, Statistical Errors")
    if not combineSysts:
        tl.AddEntry(gpred_withfs,"Prediction, with f_{short} Syst")
    tl.AddEntry(gpred_all,"Prediction, with Total Error" if combineSysts else "Prediction, with also VR Syst")
    tl.AddEntry(hpost,"Prediction, Post-Fit")
    tl.Draw()
    pads[1].cd()
    if doPullPlot:
        print desc,"pull plot not yet implemented for pre-vs-post fit, use ratio instead if that is desired"
        h1 = ROOT.TH1D("fill_for_"+desc,";;Pull",len(regions),0,len(regions))
        h2 = ROOT.TH1D("fill2_for_"+desc,";;Pull",len(regions),0,len(regions))
        h2.GetYaxis().SetTitleSize(h2.GetYaxis().GetTitleSize()*2)
        h2.GetYaxis().SetTitleOffset(0.6)
        h2.GetYaxis().SetLabelSize(h2.GetYaxis().GetLabelSize()*2)
        h1.SetFillColor(ROOT.kWhite)
        h1.SetLineColor(ROOT.kBlack)
        h2.SetFillColor(ROOT.kGray)
        h2.SetLineColor(ROOT.kGray)
        h2.SetMaximum(2.5)
        h2.SetMinimum(-2.5)
#        fshort_list = []
#        nstcs = []
        perrs_justsyst = []
        for index,region in enumerate(regions):
            bin_index = index+1
#            perr_fsrel = systs[region+" fsrel"] * rescale
#            perr_ncrel = systs[region+" ncrel"] * rescale
#            syst_err = sqrt(perr_fsrel**2 + perr_ncrel**2)
#            perrs_justsyst.append([syst_err, syst_err])
#            fshort_name = correspondingFshort(region)
#            fshort_list.append(fshorts[fshort_name])
#            nstcs.append(vals[region+" STC"])
            perr_fs = systs[region+" fs"] * rescale
            perr_nc = systs[region+" nc"] * rescale
            syst_err = sqrt(perr_fs**2 + perr_nc**2)
            perrs_justsyst.append( [syst_err, syst_err] )
            h2.SetBinContent(bin_index,0)
            h2.SetBinError(bin_index,2)
            h2.GetXaxis().SetBinLabel(bin_index,"")
            h1.SetBinContent(bin_index,0)
            h1.SetBinError(bin_index,1)
        hpred.GetXaxis().LabelsOption("v")
        hpred.GetXaxis().SetTitleOffset(4.8)
        simplecanvas.cd()
#        g_pred_all = getPoissonGraph( hpred, perrs_justsyst )
        g_pred_all = getPoissonGraph( hpred, perrs_all )
        g_obs = gobs.Clone()
        g_pull = ROOT.TGraphAsymmErrors()
#        getPullPlot(g_obs, g_pred_all, g_pull, nstcs, fshort_list)
        getPullPlot(g_obs, g_pred_all, g_pull)
        g_pull.SetMarkerStyle(20)
        g_pull.SetMarkerSize(1)
        g_pull.SetLineWidth(1)
        pads[1].cd()
        h2.Draw("E2")
        h1.Draw("same E2")
        h1.Draw("same hist")
        g_pull.Draw("SAME P0")
        h2.Draw("same axis")
    else:        
        h1=ROOT.TH1D("hratiopost"+desc,";;Post-fit / Pre-fit",len(perrs),0,len(perrs))
        h1.SetLineWidth(3)
        h1.SetLineColor(ROOT.kRed)
        hpost_norm = hpost.Clone(hpost.GetName()+"_norm")
        perrs_all_norm = []
        perrs_withfs_norm = []
        perrs_stat_norm = []
        posterrs_norm = []
        maxval = 0.0
        for bin in range(1,len(perrs)+1):
            h1.GetXaxis().SetBinLabel(bin,"")
            this_pred = hpred.GetBinContent(bin)
            h1.SetBinContent(bin,1)
            if this_pred == 0:
                hpost_norm.SetBinContent(bin,-1)
                perrs_all_norm.append( [0,0] )
                perrs_withfs_norm.append( [0,0] )
                perrs_stat_norm.append( [0,0] )
                posterrs_norm.append( [0,0] )
            else: 
                hpost_norm.SetBinContent(bin,hpost.GetBinContent(bin) / this_pred)
                this_perr_all = perrs_all[bin-1]
                this_perr_withfs = perrs_withfs[bin-1]
                this_perr_stat = perrs[bin-1]
                this_posterr = [hpost.GetBinError(bin), hpost.GetBinError(bin)]
                perrs_all_norm.append( [this_perr_all[i]/this_pred for i in [0,1]] )
                perrs_withfs_norm.append( [this_perr_withfs[i]/this_pred for i in [0,1]] )
                perrs_stat_norm.append( [this_perr_stat[i]/this_pred for i in [0,1]] )
                posterrs_norm.append( [this_posterr[i]/this_pred for i in [0,1]] )
        h1.SetMaximum(min(round(hpost_norm.GetMaximum()+1),5))
        h1.SetMinimum(-0.001)
        gall_norm = getPoissonGraph( h1, perrs_all_norm )
        gall_norm.SetFillColor(ROOT.kGray+2)
        gwithfs_norm = getPoissonGraph( h1, perrs_withfs_norm )
        gwithfs_norm.SetFillColor(ROOT.kGray)
        gstat_norm = getPoissonGraph( h1, perrs_stat_norm)
        gstat_norm.SetFillColor(ROOT.kCyan-8)
        gpost_norm = getPoissonGraph(hpost_norm, posterrs_norm, False)
        gpost_norm.SetMarkerStyle(20)
        gpost_norm.SetMarkerSize(2)
        gpost_norm.SetMarkerColor(ROOT.kBlue)
        h1.GetYaxis().SetLabelSize(hpred.GetYaxis().GetLabelSize()*.83/.16/2)
        h1.GetYaxis().SetTitleSize(hpred.GetYaxis().GetTitleSize()*.83/.16/2)
        h1.GetYaxis().SetTitleOffset(0.35)
        h1.Draw("AXIS")
        # "0" option forces the drawing of error bars even if the central value is off-scale
        gall_norm.Draw("0 2 same")
        if not combineSysts:
            gwithfs_norm.Draw("0 2 same")
        gstat_norm.Draw("0 2 same")
        gpost_norm.Draw("0 p same")
        h1.Draw("same")
    ratiocanvas.SaveAs("{}/{}_post_{}.{}".format(plotdir,desc.replace(" ","_"),"pull" if doPullPlot else "ratio",format))
    pads[0].cd()
    hpred.SetMinimum(0.1)
    hpred.SetMaximum(200)
    hpred.Draw("") # only want the axis from the histogram, for bin titles
    gpred_all.Draw("2 same") # 2 means draw filled rectangles for errors
    if not combineSysts:
        gpred_withfs.Draw("2 same")
    gpred_stat.Draw("2 same")
    hpred.Draw("same") # drawn without errors, and without connecting the lines, see above
    gobs.Draw("p same") # p draws in a typical histogram style, with markers
    hpost.Draw("E2 same")
    hpost_noerr.Draw("same")
    hpred.Draw("AXIS same") # make tick marks show above fill areas
    utils.DrawCmsText(pads[0])
    utils.DrawLumiText(pads[0],lumi)
    tr.Clear()
    tr.AddEntry(gobs,"Observation","pLE")
    tr.AddEntry(gpred_stat,"Prediction, Statistical Errors")
    if not combineSysts:
        tr.AddEntry(gpred_withfs,"Prediction, with f_{short} Syst")
    tr.AddEntry(gpred_all,"Prediction, with Total Error" if combineSysts else "Prediction, with also VR Syst")
    tr.AddEntry(hpost,"Prediction, Post-Fit")
    tr.Draw()
    pads[0].SetLogy(True)
    ratiocanvas.SaveAs("{}/{}_post_{}_logscale.{}".format(plotdir,desc.replace(" ","_"),"pull" if doPullPlot else "ratio",format))
    pads[0].SetLogy(False)

def makePlotPostfitOnly(regions,vals,vals_postfit,errs_postfit,desc,rescale=1.0, combineSysts = True, doPullPlot = False): # Raw means non-normalized. "rescale" multiplies prediction, to enable partial unblinding.
    if desc.find("16") >= 0:
        lumi = 35.92
    elif desc.find("17"):
        lumi = 41.53
        if desc.find("18"):
            lumi += 59.97
    elif desc.find("18"):
        lumi = 59.97
    else: lumi = 1.0
    ratiocanvas.cd()
    tl.Clear()
    nregions=len(regions)
    hobs=ROOT.TH1D(desc,desc+";;Short Track Counts",nregions,0,nregions)
    hobs.SetLineWidth(3)
    hpost=hobs.Clone(hobs.GetName()+"_postfit")
    oerrs = []
    perrs = []
    for index,region in enumerate(regions):
        bin_index = index+1
        hpost.GetXaxis().SetBinLabel(bin_index,"#color[{}]{{{}}}".format(getBinLabelColor(region),region.replace(" SR","").replace(" VR","")))
        obs = vals[region+" obs"]
        oerr = getAsymmetricErrors(obs)
        hobs.SetBinContent(bin_index,obs)
        oerrs.append(oerr)
        perrs.append( [errs_postfit[region]*rescale, errs_postfit[region]*rescale] )
        hpost.SetBinContent(bin_index,vals_postfit[region]*rescale)
        hpost.SetBinError(bin_index,errs_postfit[region]*rescale)
    hpost.GetXaxis().LabelsOption("v")
    hpost.GetXaxis().SetTitleOffset(4.8)
    hpost.SetMinimum(-0.001)
    hpost.SetMaximum(2.0*max(hpost.GetMaximum(),hobs.GetMaximum()))
    hobs.SetLineColor(ROOT.kBlack)
    hobs.SetMinimum(hpost.GetMinimum())
    hobs.SetMaximum(hpost.GetMaximum())
    hpost.SetLineColor(ROOT.kBlue)
    hpost_noerr = hpost.Clone(hpost.GetName()+"_noerr")
    for bin in range(1,hpost.GetNbinsX()+1): hpost_noerr.SetBinError(bin,1e-9)
    hpost.SetFillColor(ROOT.kBlue)
    hpost.SetFillStyle(3244) # hashed fill
    pads[0].cd()    
    gobs = getPoissonGraph( hobs, oerrs, False )
    gobs.SetName("obs"+desc.replace("-","_").replace(" ","_"))
    hpost.Draw("E2")
    hpost_noerr.Draw("same")
    gobs.SetMarkerStyle(20)
    gobs.SetMarkerSize(2)
    gobs.SetMarkerColor(ROOT.kBlack)
    gobs.Draw("p same") # p draws in a typical histogram style, with markers
    hpost.Draw("AXIS same")
    utils.DrawCmsText(pads[0])
    utils.DrawLumiText(pads[0],lumi)
    tl.AddEntry(gobs,"Observation","pLE")
    tl.AddEntry(hpost,"Prediction, Post-Fit")
    tl.Draw()
    pads[1].cd()
    if doPullPlot:
        h1 = ROOT.TH1D("fill_for_"+desc,";;Pull",len(regions),0,len(regions))
        h2 = ROOT.TH1D("fill2_for_"+desc,";;Pull",len(regions),0,len(regions))
        h2.GetYaxis().SetTitleSize(h2.GetYaxis().GetTitleSize()*2)
        h2.GetYaxis().SetTitleOffset(0.6)
        h2.GetYaxis().SetLabelSize(h2.GetYaxis().GetLabelSize()*2)
        h1.SetFillColor(ROOT.kWhite)
        h1.SetLineColor(ROOT.kBlack)
        h2.SetFillColor(ROOT.kGray)
        h2.SetLineColor(ROOT.kGray)
        h2.SetMaximum(2.5)
        h2.SetMinimum(-2.5)
        for index,region in enumerate(regions):
            bin_index = index+1
            h2.SetBinContent(bin_index,0)
            h2.SetBinError(bin_index,2)
            h2.GetXaxis().SetBinLabel(bin_index,"")
            h1.SetBinContent(bin_index,0)
            h1.SetBinError(bin_index,1)
        hpost.GetXaxis().LabelsOption("v")
        hpost.GetXaxis().SetTitleOffset(4.8)
        simplecanvas.cd()
        g_post = getPoissonGraph( hpost, perrs )
        g_obs = gobs.Clone()
        g_pull = ROOT.TGraphAsymmErrors()
        getPullPlot(g_obs, g_post, g_pull)
        g_pull.SetMarkerStyle(20)
        g_pull.SetMarkerSize(1)
        g_pull.SetLineWidth(1)
        pads[1].cd()
        h2.Draw("E2")
        h1.Draw("same E2")
        h1.Draw("same hist")
        g_pull.Draw("SAME P0")
        h2.Draw("same axis")
    else:        
        h1=ROOT.TH1D("hratiopost"+desc,";;Obs / Post-fit",len(perrs),0,len(perrs))
        h1.SetLineWidth(3)
        h1.SetLineColor(ROOT.kBlue)
        h1.SetFillColor(ROOT.kBlue)
        h1.SetFillStyle(3244)
        hobs_norm = hobs.Clone(hobs.GetName()+"_norm")
        oerrs_norm = []
        maxval = 0.0
        for bin in range(1,len(perrs)+1):
            h1.GetXaxis().SetBinLabel(bin,"")
            this_obs = hobs.GetBinContent(bin)
            this_post = hpost.GetBinContent(bin)
            h1.SetBinContent(bin,1)
            if this_post == 0:
                hobs_norm.SetBinContent(bin,-1)
                oerrs_norm.append( [0,0] )
            else: 
                hobs_norm.SetBinContent(bin,this_obs / this_post)
                this_oerr = oerrs[bin-1]
                oerrs_norm.append( [this_oerr[0] / this_post, this_oerr[1] / this_post ] )
                h1.SetBinError(bin,hpost.GetBinError(bin) / this_post)
        h1.SetMaximum(min(round(hobs_norm.GetMaximum()+1),5))
        h1.SetMinimum(-0.001)
        gobs_norm = getPoissonGraph(hobs_norm, oerrs_norm, False)
        gobs_norm.SetMarkerStyle(20)
        gobs_norm.SetMarkerSize(2)
        gobs_norm.SetMarkerColor(ROOT.kBlack)
        h1.GetYaxis().SetLabelSize(hpost.GetYaxis().GetLabelSize()*.83/.16/2)
        h1.GetYaxis().SetTitleSize(hpost.GetYaxis().GetTitleSize()*.83/.16/2)
        h1.GetYaxis().SetTitleOffset(0.35)
        # "0" option forces the drawing of error bars even if the central value is off-scale
        h1_noerr=h1.Clone(h1.GetName()+"_noerr")
        for bin in range(1,h1.GetNbinsX()+1): h1_noerr.SetBinError(bin,1e-9)
        h1.Draw("E2")
        h1_noerr.Draw("same")
        gobs_norm.Draw("p same")
    ratiocanvas.SaveAs("{}/{}_postonly_{}.{}".format(plotdir,desc.replace(" ","_"),"pull" if doPullPlot else "ratio",format))
    pads[0].cd()
    hpost.SetMinimum(0.1)
    hpost_noerr.SetMinimum(0.1)
    hpost.SetMaximum(200)
    hpost.Draw("E2")
    hpost_noerr.Draw("same")
    gobs.Draw("p same") # p draws in a typical histogram style, with markers
    utils.DrawCmsText(pads[0])
    utils.DrawLumiText(pads[0],lumi)
    tr.Clear()
    tr.AddEntry(gobs,"Observation","pLE")
    tr.AddEntry(hpost,"Prediction, Post-Fit")
    tr.Draw()
    pads[0].SetLogy(True)
    ratiocanvas.SaveAs("{}/{}_postonly_{}_logscale.{}".format(plotdir,desc.replace(" ","_"),"pull" if doPullPlot else "ratio",format))
    pads[0].SetLogy(False)

def makePlotPostfitSvsBG(regions,vals,vals_postfit,errs_postfit,desc,rescale=1.0, combineSysts = True, doPullPlot = False): # Raw means non-normalized. "rescale" multiplies prediction, to enable partial unblinding.
    if desc.find("16") >= 0:
        lumi = 35.92
    elif desc.find("17"):
        lumi = 41.53
        if desc.find("18"):
            lumi += 59.97
    elif desc.find("18"):
        lumi = 59.97
    else: lumi = 1.0
    ratiocanvas.cd()
    tl.Clear()
    nregions=len(regions)
    hobs=ROOT.TH1D(desc,desc+";;Short Track Counts",nregions,0,nregions)
    hobs.SetLineWidth(3)
    hpost=hobs.Clone(hobs.GetName()+"_postfit")
    hpostS=hpost.Clone(hpost.GetName()+"_sig")
    oerrs = []
    perrs = []
    for index,region in enumerate(regions):
        bin_index = index+1
        hpost.GetXaxis().SetBinLabel(bin_index,"#color[{}]{{{}}}".format(getBinLabelColor(region),region.replace(" SR","").replace(" VR","")))
        obs = vals[region+" obs"]
        oerr = getAsymmetricErrors(obs)
        hobs.SetBinContent(bin_index,obs)
        oerrs.append(oerr)
        perrs.append( [errs_postfit[region+" B"]*rescale, errs_postfit[region+" B"]*rescale] )
        hpost.SetBinContent(bin_index,vals_postfit[region+" B"]*rescale)
        hpost.SetBinError(bin_index,errs_postfit[region+" B"]*rescale)
        hpostS.SetBinContent(bin_index,vals_postfit[region+" S"]*rescale)
        hpostS.SetBinError(bin_index,errs_postfit[region+" S"]*rescale)
    hpost.GetXaxis().LabelsOption("v")
    hpost.GetXaxis().SetTitleOffset(4.8)
    hpost.SetMinimum(-0.001)
    hpost.SetMaximum(2.0*max(hpost.GetMaximum(),hobs.GetMaximum()))
    hobs.SetLineColor(ROOT.kBlack)
    hobs.SetMinimum(hpost.GetMinimum())
    hobs.SetMaximum(hpost.GetMaximum())
    hpost.SetLineColor(ROOT.kBlue)
    hpostS.SetLineColor(ROOT.kGreen)
    hpost_noerr = hpost.Clone(hpost.GetName()+"_noerr")
    hpostS_noerr = hpostS.Clone(hpost.GetName()+"_noerr")
    for bin in range(1,hpost.GetNbinsX()+1): 
        hpost_noerr.SetBinError(bin,1e-9)
        hpostS_noerr.SetBinError(bin,1e-9)
    hpost.SetFillColor(ROOT.kBlue)
    hpost.SetFillStyle(3244) # hashed fill
    hpostS.SetFillColor(ROOT.kGreen)
    hpostS.SetFillStyle(3244) # hashed fill
    pads[0].cd()    
    gobs = getPoissonGraph( hobs, oerrs, False )
    gobs.SetName("obs"+desc.replace("-","_").replace(" ","_"))
    hpost.Draw("E2")
    hpost_noerr.Draw("same")
    hpostS.Draw("E2")
    hpostS_noerr.Draw("same")
    gobs.SetMarkerStyle(20)
    gobs.SetMarkerSize(2)
    gobs.SetMarkerColor(ROOT.kBlack)
    gobs.Draw("p same") # p draws in a typical histogram style, with markers
    hpost.Draw("AXIS same")
    utils.DrawCmsText(pads[0])
    utils.DrawLumiText(pads[0],lumi)
    tl.AddEntry(gobs,"Observation","pLE")
    tl.AddEntry(hpost,"Prediction, Post-Fit")
    tl.Draw()
    pads[1].cd()
    if doPullPlot:
        h1 = ROOT.TH1D("fill_for_"+desc,";;Pull",len(regions),0,len(regions))
        h2 = ROOT.TH1D("fill2_for_"+desc,";;Pull",len(regions),0,len(regions))
        h2.GetYaxis().SetTitleSize(h2.GetYaxis().GetTitleSize()*2)
        h2.GetYaxis().SetTitleOffset(0.6)
        h2.GetYaxis().SetLabelSize(h2.GetYaxis().GetLabelSize()*2)
        h1.SetFillColor(ROOT.kWhite)
        h1.SetLineColor(ROOT.kBlack)
        h2.SetFillColor(ROOT.kGray)
        h2.SetLineColor(ROOT.kGray)
        h2.SetMaximum(2.5)
        h2.SetMinimum(-2.5)
        for index,region in enumerate(regions):
            bin_index = index+1
            h2.SetBinContent(bin_index,0)
            h2.SetBinError(bin_index,2)
            h2.GetXaxis().SetBinLabel(bin_index,"")
            h1.SetBinContent(bin_index,0)
            h1.SetBinError(bin_index,1)
        hpost.GetXaxis().LabelsOption("v")
        hpost.GetXaxis().SetTitleOffset(4.8)
        simplecanvas.cd()
        g_post = getPoissonGraph( hpost, perrs )
        g_obs = gobs.Clone()
        g_pull = ROOT.TGraphAsymmErrors()
        getPullPlot(g_obs, g_post, g_pull)
        g_pull.SetMarkerStyle(20)
        g_pull.SetMarkerSize(1)
        g_pull.SetLineWidth(1)
        pads[1].cd()
        h2.Draw("E2")
        h1.Draw("same E2")
        h1.Draw("same hist")
        g_pull.Draw("SAME P0")
        h2.Draw("same axis")
    else:        
        h1=ROOT.TH1D("hratiopost"+desc,";;Obs / Post-fit",len(perrs),0,len(perrs))
        h1.SetLineWidth(3)
        h1.SetLineColor(ROOT.kBlue)
        h1.SetFillColor(ROOT.kBlue)
        h1.SetFillStyle(3244)
        hobs_norm = hobs.Clone(hobs.GetName()+"_norm")
        oerrs_norm = []
        maxval = 0.0
        for bin in range(1,len(perrs)+1):
            h1.GetXaxis().SetBinLabel(bin,"")
            this_obs = hobs.GetBinContent(bin)
            this_post = hpost.GetBinContent(bin)
            h1.SetBinContent(bin,1)
            if this_post == 0:
                hobs_norm.SetBinContent(bin,-1)
                oerrs_norm.append( [0,0] )
            else: 
                hobs_norm.SetBinContent(bin,this_obs / this_post)
                this_oerr = oerrs[bin-1]
                oerrs_norm.append( [this_oerr[0] / this_post, this_oerr[1] / this_post ] )
                h1.SetBinError(bin,hpost.GetBinError(bin) / this_post)
        h1.SetMaximum(min(round(hobs_norm.GetMaximum()+1),5))
        h1.SetMinimum(-0.001)
        gobs_norm = getPoissonGraph(hobs_norm, oerrs_norm, False)
        gobs_norm.SetMarkerStyle(20)
        gobs_norm.SetMarkerSize(2)
        gobs_norm.SetMarkerColor(ROOT.kBlack)
        h1.GetYaxis().SetLabelSize(hpost.GetYaxis().GetLabelSize()*.83/.16/2)
        h1.GetYaxis().SetTitleSize(hpost.GetYaxis().GetTitleSize()*.83/.16/2)
        h1.GetYaxis().SetTitleOffset(0.35)
        # "0" option forces the drawing of error bars even if the central value is off-scale
        h1_noerr=h1.Clone(h1.GetName()+"_noerr")
        for bin in range(1,h1.GetNbinsX()+1): h1_noerr.SetBinError(bin,1e-9)
        h1.Draw("E2")
        h1_noerr.Draw("same")
        gobs_norm.Draw("p same")
    ratiocanvas.SaveAs("{}/{}_postonlySvsBG_{}.{}".format(plotdir,desc.replace(" ","_"),"pull" if doPullPlot else "ratio",format))
    pads[0].cd()
    hpost.SetMinimum(0.1)
    hpost_noerr.SetMinimum(0.1)
    hpost.SetMaximum(200)
    hpost.Draw("E2")
    hpost_noerr.Draw("same")
    hpostS.Draw("E2 same")
    hpostS_noerr.Draw("same")
    gobs.Draw("p same") # p draws in a typical histogram style, with markers
    utils.DrawCmsText(pads[0])
    utils.DrawLumiText(pads[0],lumi)
    tr.Clear()
    tr.AddEntry(gobs,"Observation","pLE")
    tr.AddEntry(hpost,"Background, Post-Fit")
    tr.AddEntry(hpostS,"Signal, Post-Fit")
    tr.Draw()
    pads[0].SetLogy(True)
    ratiocanvas.SaveAs("{}/{}_postonlySvsBG_{}_logscale.{}".format(plotdir,desc.replace(" ","_"),"pull" if doPullPlot else "ratio",format))
    pads[0].SetLogy(False)

def convertCovarLabel(label):
    tokens = label.split("_")
    njht = tokens[0]
    if tokens[3] == "hi" or tokens[3] == "lo":
        pt = " "+tokens[3]
        length = tokens[4]
    else:
        pt = ""
        length = tokens[3]
    year = tokens[len(tokens)-2]
    return length + " " + njht + pt + " " + year 

def getCorr(f):
    dict_corr = {}
    h_covar = f.Get("shapes_prefit/overall_total_covar").Clone("h_corr")
    for i in range(1,h_covar.GetNbinsX()+1):
        for j in range(1,h_covar.GetNbinsY()+1):
            xlabel = convertCovarLabel(h_covar.GetXaxis().GetBinLabel(i))
            ylabel = convertCovarLabel(h_covar.GetYaxis().GetBinLabel(j))
            val = h_covar.GetBinContent(i,j)
            correlation = val/sqrt(h_covar.GetBinContent(i,i)*h_covar.GetBinContent(j,j))
            dict_corr[xlabel+" "+ylabel] = correlation
    return dict_corr

def getCovar(f):
    dict_covar = {}
    h_covar = f.Get("shapes_prefit/overall_total_covar").Clone("h_covar")
    for i in range(1,h_covar.GetNbinsX()+1):
        for j in range(1,h_covar.GetNbinsY()+1):
            xlabel = convertCovarLabel(h_covar.GetXaxis().GetBinLabel(i))
            ylabel = convertCovarLabel(h_covar.GetYaxis().GetBinLabel(j))
            val = h_covar.GetBinContent(i,j)
            err = h_covar.GetBinError(i,j)
            dict_covar[xlabel+" "+ylabel+" val"] = val
            dict_covar[xlabel+" "+ylabel+" err"] = err
    return dict_covar

def makePlotSSRsCovar(region_sets,vals,stats,systs, covars, desc, ssr_names, rescale=1.0, doFullCovariance = True): # make plots of merged regions
    if desc.find("16") >= 0:
        lumi = 35.92
        year = "2016"
    elif desc.find("17"):
        lumi = 41.53
        year = "2017"
        if desc.find("18"):
            lumi += 59.97
            year = "2017and2018"
    elif desc.find("18"):
        lumi = 59.97
        year = "2018"
    else: 
        lumi = 1.0
        year = ""
    ratiocanvas.cd()
    tr.Clear()
    ssr_info = []    
    for region_set in region_sets:
        total_obs = 0
        total_pred = 0
        pred_err = []
        for region in region_set:
            pred = vals[region+" pre"]*rescale
            total_pred += pred
            perr = [stats[region+" pre"][i] * rescale for i in [0,1]]            
            obs = vals[region+" obs"]*rescale
            total_obs += obs
            perr_fs = systs[region+" fs"] * rescale
            perr_nc = systs[region+" nc"] * rescale
            pred_err.append( tuple( [sqrt(perr[0]**2 + perr_fs**2 + perr_nc**2), sqrt(perr[1]**2 + perr_fs**2 + perr_nc**2)] ) )
        total_pred_err_sq = [0,0]
        if doFullCovariance:
            for i,region_i in enumerate(region_set):
                covar_region_i = region_i.replace(" SR","").replace(" VR","") + " " + year
                for j,region_j in enumerate(region_set):                
                    covar_region_j = region_j.replace(" SR","").replace(" VR","") + " " + year
                    total_pred_err_sq[0] += covars[covar_region_i + " " + covar_region_j+ " val"] * covars[covar_region_i + " " + covar_region_j+ " err"]
                    total_pred_err_sq[1] += covars[covar_region_i + " " + covar_region_j+ " val"] * covars[covar_region_i + " " + covar_region_j+ " err"]
        else:
            for i in range(len(pred_err)):
                total_pred_err_sq[0] += pred_err[i][0]**2
                total_pred_err_sq[1] += pred_err[i][1]**2
        total_pred_err = tuple( [sqrt(total_pred_err_sq[0]),sqrt(total_pred_err_sq[1])] )
        ssr_info.append( (total_pred,total_obs,getAsymmetricErrors(total_obs),total_pred_err) )
    nregions=len(ssr_info)
    hobs=ROOT.TH1D(desc,desc+";;Short Track Counts",nregions,0,nregions)
    hobs.SetLineWidth(3)
    hpred=hobs.Clone(hobs.GetName()+"_prediction")
    oerrs = []
    perrs_all = []
    most_discrepant_sigma = 0
    most_discrepant_obs = 0
    most_discrepant_pred = 0
    most_discrepant_perr = 0
    most_discrepant_region = "None"            
    for index,region in enumerate(ssr_names):
        bin_index = index+1
        hpred.GetXaxis().SetBinLabel(bin_index,region)
        pred,obs,oerr,perr_total = ssr_info[index]
        hpred.SetBinContent(bin_index,pred)
        hpred.SetBinError(bin_index,1e-9) # Need to explictly set bin error to epsilon so lines aren't connected when drawing later (as for "hist" style)
        perrs_all.append( perr_total )
        hobs.SetBinContent(bin_index,obs)
        oerrs.append(oerr)
        # find maximally discrepant bin
        relevant_perr = perrs_all[index][0] if obs > pred else perrs_all[index][1]
        relevant_oerr = oerr[0] if obs < pred else oerr[1]
        total_err = sqrt(relevant_perr**2 + relevant_oerr**2)
        delta = abs(pred - obs)
        sigma = delta / total_err if total_err > 0 else -1
        if sigma > most_discrepant_sigma:
            most_discrepant_sigma = sigma
            most_discrepant_obs = obs
            most_discrepant_pred = pred
            most_discrepant_perr = relevant_perr
            most_discrepant_region = region
    print desc,"Most discrepant{}:".format("" if doFullCovariance else " (uncorrelated)"),most_discrepant_region,"sigma",most_discrepant_sigma,"obs",most_discrepant_obs,"pred",most_discrepant_pred,"perr",most_discrepant_perr
    hpred.GetXaxis().LabelsOption("v")
    hpred.GetXaxis().SetTitleOffset(4.8)
    hpred.SetMinimum(-0.001)
    hpred.SetMaximum(2.0*max(hpred.GetMaximum(),hobs.GetMaximum()))
    hobs.SetLineColor(ROOT.kBlack)
    hobs.SetMinimum(hpred.GetMinimum())
    hobs.SetMaximum(hpred.GetMaximum())
    hpred.SetLineColor(ROOT.kRed)
    pads[0].cd()    
    gpred_all = getPoissonGraph( hpred, perrs_all )
    gpred_all.SetName("all"+desc.replace("-","_").replace(" ","_"))
    gobs = getPoissonGraph( hobs, oerrs, False )
    gobs.SetName("obs"+desc.replace("-","_").replace(" ","_"))
    gpred_all.SetFillColor(ROOT.kGray+2)
    hpred.Draw("") # only want the axis from the histogram, for bin titles
    gpred_all.Draw("2 same") # 2 means draw filled rectangles for errors
    hpred.Draw("same") # drawn without errors, and without connecting the lines, see above
    gobs.SetMarkerStyle(20)
    gobs.SetMarkerSize(2)
    gobs.SetMarkerColor(ROOT.kBlack)
    gobs.Draw("p same") # p draws in a typical histogram style, with markers
    hpred.Draw("AXIS same") # make tick marks show above fill areas
    utils.DrawCmsText(pads[0])
    utils.DrawLumiText(pads[0],lumi)
    tr.AddEntry(gobs,"Observation","pLE")
    tr.AddEntry(gpred_all,"Prediction, with Total{} Error".format("" if doFullCovariance else " (Uncorrelated)"))
    tr.Draw()
    pads[1].cd()
    h1=ROOT.TH1D("hratio"+desc,";;Obs / Pred",len(perrs_all),0,len(perrs_all))
    h1.SetLineWidth(3)
    h1.SetLineColor(ROOT.kRed)
    hobs_norm = hobs.Clone(hobs.GetName()+"_norm")
    perrs_all_norm = []
    oerrs_norm = []
    maxval = 0.0
    for bin in range(1,len(perrs_all)+1):
        h1.GetXaxis().SetBinLabel(bin,"")
        this_pred = hpred.GetBinContent(bin)
        h1.SetBinContent(bin,1)
        if this_pred == 0:
            hobs_norm.SetBinContent(bin,-1)
            perrs_all_norm.append( [0,0] )
            oerrs_norm.append( [0,0] )
        else: 
            hobs_norm.SetBinContent(bin,hobs.GetBinContent(bin) / this_pred)
            this_perr_all = perrs_all[bin-1]
            this_oerr = oerrs[bin-1]
            perrs_all_norm.append( [this_perr_all[i]/this_pred for i in [0,1]] )
            oerrs_norm.append( [this_oerr[i]/this_pred for i in [0,1]] )
    h1.SetMaximum(min(round(hobs_norm.GetMaximum()+1),5))
    h1.SetMinimum(-0.001)
    gall_norm = getPoissonGraph( h1, perrs_all_norm )
    gall_norm.SetFillColor(ROOT.kGray+2)
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
    gobs_norm.Draw("0 p same")
    h1.Draw("same")
    ratiocanvas.SaveAs("{}/{}_raw{}.{}".format(plotdir,desc.replace(" ","_"),"" if doFullCovariance else "_uncorr",format))
    pads[0].cd()
    hpred.SetMinimum(0.1)
    hpred.SetMaximum(200)
    hpred.Draw("") # only want the axis from the histogram, for bin titles
    gpred_all.Draw("2 same") # 2 means draw filled rectangles for errors
    hpred.Draw("same") # drawn without errors, and without connecting the lines, see above
    gobs.Draw("p same") # p draws in a typical histogram style, with markers
    hpred.Draw("AXIS same") # make tick marks show above fill areas
    utils.DrawCmsText(pads[0])
    utils.DrawLumiText(pads[0],lumi)
    tr.Clear()
    tr.AddEntry(gobs,"Observation","pLE")
    tr.AddEntry(gpred_all,"Prediction, with Total{} Error".format("" if doFullCovariance else " (Uncorrelated)"))
    tr.Draw()
    pads[0].SetLogy(True)
    ratiocanvas.SaveAs("{}/{}_raw_logscale{}.{}".format(plotdir,desc.replace(" ","_"),"" if doFullCovariance else "_uncorr",format))
    pads[0].SetLogy(False)

def makePlotSSRsCorr(region_sets,vals,stats,systs, covars, desc, ssr_names, rescale=1.0, doFullCovariance = True): # make plots of merged regions
    if desc.find("16") >= 0:
        lumi = 35.92
        year = "2016"
    elif desc.find("17"):
        lumi = 41.53
        year = "2017"
        if desc.find("18"):
            lumi += 59.97
            year = "2017and2018"
    elif desc.find("18"):
        lumi = 59.97
        year = "2018"
    else: 
        lumi = 1.0
        year = ""
    ratiocanvas.cd()
    tr.Clear()
    ssr_info = []    
    for region_set in region_sets:
        total_obs = 0
        total_pred = 0
        pred_err = []
        for region in region_set:
            pred = vals[region+" pre"]*rescale
            total_pred += pred
            perr = [stats[region+" pre"][i] * rescale for i in [0,1]]            
            obs = vals[region+" obs"]*rescale
            total_obs += obs
            perr_fs = systs[region+" fs"] * rescale
            perr_nc = systs[region+" nc"] * rescale
            pred_err.append( tuple( [sqrt(perr[0]**2 + perr_fs**2 + perr_nc**2), sqrt(perr[1]**2 + perr_fs**2 + perr_nc**2)] ) )
        total_pred_err_sq = [0,0]
        if doFullCovariance:
            for i,region_i in enumerate(region_set):
                covar_region_i = region_i.replace(" SR","").replace(" VR","") + " " + year
                for j,region_j in enumerate(region_set):                
                    covar_region_j = region_j.replace(" SR","").replace(" VR","") + " " + year
                    total_pred_err_sq[0] += covars[covar_region_i + " " + covar_region_j] * pred_err[i][0] * pred_err[j][0]
                    total_pred_err_sq[1] += covars[covar_region_i + " " + covar_region_j] * pred_err[i][1] * pred_err[j][1]
        else:
            for i in range(len(pred_err)):
                total_pred_err_sq[0] += pred_err[i][0]**2
                total_pred_err_sq[1] += pred_err[i][1]**2
        total_pred_err = tuple( [sqrt(total_pred_err_sq[0]),sqrt(total_pred_err_sq[1])] )
        ssr_info.append( (total_pred,total_obs,getAsymmetricErrors(total_obs),total_pred_err) )
    nregions=len(ssr_info)
    hobs=ROOT.TH1D(desc,desc+";;Short Track Counts",nregions,0,nregions)
    hobs.SetLineWidth(3)
    hpred=hobs.Clone(hobs.GetName()+"_prediction")
    oerrs = []
    perrs_all = []
    most_discrepant_sigma = 0
    most_discrepant_obs = 0
    most_discrepant_pred = 0
    most_discrepant_perr = 0
    most_discrepant_region = "None"            
    for index,region in enumerate(ssr_names):
        bin_index = index+1
        hpred.GetXaxis().SetBinLabel(bin_index,region)
        pred,obs,oerr,perr_total = ssr_info[index]
        hpred.SetBinContent(bin_index,pred)
        hpred.SetBinError(bin_index,1e-9) # Need to explictly set bin error to epsilon so lines aren't connected when drawing later (as for "hist" style)
        perrs_all.append( perr_total )
        hobs.SetBinContent(bin_index,obs)
        oerrs.append(oerr)
        # find maximally discrepant bin
        relevant_perr = perrs_all[index][0] if obs > pred else perrs_all[index][1]
        relevant_oerr = oerr[0] if obs < pred else oerr[1]
        total_err = sqrt(relevant_perr**2 + relevant_oerr**2)
        delta = abs(pred - obs)
        sigma = delta / total_err if total_err > 0 else -1
        if sigma > most_discrepant_sigma:
            most_discrepant_sigma = sigma
            most_discrepant_obs = obs
            most_discrepant_pred = pred
            most_discrepant_perr = relevant_perr
            most_discrepant_region = region
    print desc,"Most discrepant{}:".format("" if doFullCovariance else " (uncorrelated)"),most_discrepant_region,"sigma",most_discrepant_sigma,"obs",most_discrepant_obs,"pred",most_discrepant_pred,"perr",most_discrepant_perr
    hpred.GetXaxis().LabelsOption("v")
    hpred.GetXaxis().SetTitleOffset(4.8)
    hpred.SetMinimum(-0.001)
    hpred.SetMaximum(2.0*max(hpred.GetMaximum(),hobs.GetMaximum()))
    hobs.SetLineColor(ROOT.kBlack)
    hobs.SetMinimum(hpred.GetMinimum())
    hobs.SetMaximum(hpred.GetMaximum())
    hpred.SetLineColor(ROOT.kRed)
    pads[0].cd()    
    gpred_all = getPoissonGraph( hpred, perrs_all )
    gpred_all.SetName("all"+desc.replace("-","_").replace(" ","_"))
    gobs = getPoissonGraph( hobs, oerrs, False )
    gobs.SetName("obs"+desc.replace("-","_").replace(" ","_"))
    gpred_all.SetFillColor(ROOT.kGray+2)
    hpred.Draw("") # only want the axis from the histogram, for bin titles
    gpred_all.Draw("2 same") # 2 means draw filled rectangles for errors
    hpred.Draw("same") # drawn without errors, and without connecting the lines, see above
    gobs.SetMarkerStyle(20)
    gobs.SetMarkerSize(2)
    gobs.SetMarkerColor(ROOT.kBlack)
    gobs.Draw("p same") # p draws in a typical histogram style, with markers
    hpred.Draw("AXIS same") # make tick marks show above fill areas
    utils.DrawCmsText(pads[0])
    utils.DrawLumiText(pads[0],lumi)
    tr.AddEntry(gobs,"Observation","pLE")
    tr.AddEntry(gpred_all,"Prediction, with Total{} Error".format("" if doFullCovariance else " (Uncorrelated)"))
    tr.Draw()
    pads[1].cd()
    h1=ROOT.TH1D("hratio"+desc,";;Obs / Pred",len(perrs_all),0,len(perrs_all))
    h1.SetLineWidth(3)
    h1.SetLineColor(ROOT.kRed)
    hobs_norm = hobs.Clone(hobs.GetName()+"_norm")
    perrs_all_norm = []
    oerrs_norm = []
    maxval = 0.0
    for bin in range(1,len(perrs_all)+1):
        h1.GetXaxis().SetBinLabel(bin,"")
        this_pred = hpred.GetBinContent(bin)
        h1.SetBinContent(bin,1)
        if this_pred == 0:
            hobs_norm.SetBinContent(bin,-1)
            perrs_all_norm.append( [0,0] )
            oerrs_norm.append( [0,0] )
        else: 
            hobs_norm.SetBinContent(bin,hobs.GetBinContent(bin) / this_pred)
            this_perr_all = perrs_all[bin-1]
            this_oerr = oerrs[bin-1]
            perrs_all_norm.append( [this_perr_all[i]/this_pred for i in [0,1]] )
            oerrs_norm.append( [this_oerr[i]/this_pred for i in [0,1]] )
    h1.SetMaximum(min(round(hobs_norm.GetMaximum()+1),5))
    h1.SetMinimum(-0.001)
    gall_norm = getPoissonGraph( h1, perrs_all_norm )
    gall_norm.SetFillColor(ROOT.kGray+2)
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
    gobs_norm.Draw("0 p same")
    h1.Draw("same")
    ratiocanvas.SaveAs("{}/{}_raw{}.{}".format(plotdir,desc.replace(" ","_"),"" if doFullCovariance else "_uncorr",format))
    pads[0].cd()
    hpred.SetMinimum(0.1)
    hpred.SetMaximum(200)
    hpred.Draw("") # only want the axis from the histogram, for bin titles
    gpred_all.Draw("2 same") # 2 means draw filled rectangles for errors
    hpred.Draw("same") # drawn without errors, and without connecting the lines, see above
    gobs.Draw("p same") # p draws in a typical histogram style, with markers
    hpred.Draw("AXIS same") # make tick marks show above fill areas
    utils.DrawCmsText(pads[0])
    utils.DrawLumiText(pads[0],lumi)
    tr.Clear()
    tr.AddEntry(gobs,"Observation","pLE")
    tr.AddEntry(gpred_all,"Prediction, with Total{} Error".format("" if doFullCovariance else " (Uncorrelated)"))
    tr.Draw()
    pads[0].SetLogy(True)
    ratiocanvas.SaveAs("{}/{}_raw_logscale{}.{}".format(plotdir,desc.replace(" ","_"),"" if doFullCovariance else "_uncorr",format))
    pads[0].SetLogy(False)

def makeSignalPlot(regions,vals_bg,stats_bg,systs_bg,list_of_vals_sig,list_of_errs_sig, rescale_lumi, desc, sig_tags, sig_colors, rescale_unblind, combineErrors = True): 
    year_token = desc.split("_")[0]
    if year_token.find("16") >= 0:
        lumi = 35.92
    elif year_token.find("17"):
        lumi = 41.53
        if year_token.find("18"):
            lumi += 59.97
    elif year_token.find("18"):
        lumi = 59.97
    else: lumi = 1.0
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
    utils.DrawCmsText(simplecanvas)
    utils.DrawLumiText(simplecanvas,lumi)
    unblind = "partialunblind" if rescale_unblind < 1.0 else "fullunblind"
    simplecanvas.SaveAs("{}/{}_counts_{}.{}".format(plotdir,desc.replace(" ","_").replace("(","").replace(")","").replace(",",""),unblind,format))


def makePullPlot(regions,vals,stats,systs,desc,rescale=1.0):
    simplecanvas.cd()
    tl.Clear()
    if desc.find("16") >= 0:
        lumi = 35.92
    elif desc.find("17"):
        lumi = 41.53
        if desc.find("18"):
            lumi += 59.97
    elif desc.find("18"):
        lumi = 59.97
    else: lumi = 1.0
    ratiocanvas.cd()
    tl.Clear()
    nregions=len(regions)
    hobs=ROOT.TH1D(desc,desc+";;Pull",nregions,0,nregions)
    hobs.SetLineWidth(3)
    hpred=hobs.Clone(hobs.GetName()+"_prediction")
    hpred.SetMinimum(-2)
    hpred.SetMaximum(2)
    perrs = []
    oerrs = []
    perrs_all = []
    h1 = ROOT.TH1D("fill_for_"+desc,"1-sigma",len(regions),0,len(regions))
    for index,region in enumerate(regions):
        bin_index = index+1
        hpred.GetXaxis().SetBinLabel(bin_index,"#color[{}]{{{}}}".format(getBinLabelColor(region),region))
        pred = vals[region+" pre"]*rescale
        perr = [stats[region+" pre"][i] * rescale for i in [0,1]]
        obs = vals[region+" obs"]
        oerr = stats[region+" obs"]
        perr_fs = systs[region+" fs"] * rescale
        perr_nc = systs[region+" nc"] * rescale
        hpred.SetBinContent(bin_index,pred)
        hpred.SetBinError(bin_index,1e-9) # Need to explictly set bin error to epsilon so lines aren't connected when drawing later (as for "hist" style)
        perrs.append(perr)
        perrs_all.append( [sqrt(perr[i]**2 + perr_fs**2 + perr_nc**2) for i in [0,1]] )
        hobs.SetBinContent(bin_index,obs)
        oerrs.append(oerr)
        h1.SetBinContent(bin_index,0)
        h1.SetBinError(bin_index,1)
    h1.SetFillColor(ROOT.kGray)
    h1.SetLineColor(ROOT.kGray)
    hpred.GetXaxis().LabelsOption("v")
    hpred.GetXaxis().SetTitleOffset(4.8)
    simplecanvas.cd()
    gpred_all = getPoissonGraph( hpred, perrs_all )
    gpred_all.SetName("all"+desc.replace("-","_").replace(" ","_"))
    gobs = getPoissonGraph( hobs, oerrs, False )
    gobs.SetName("obs"+desc.replace("-","_").replace(" ","_"))
    gpred_all.SetFillColor(ROOT.kGray+2)
    print desc,"Pull"
    g_pull = ROOT.TGraphAsymmErrors()
    getPullPlot(gobs, gpred_all, g_pull)
    g_pull.SetMarkerStyle(20)
    g_pull.SetMarkerSize(1)
    g_pull.SetLineWidth(1)
    hpred.Draw("AXIS") # only want the axis from the histogram, for bin titles
    h1.Draw("same E2")
    g_pull.Draw("SAME P0")
    h1.SetLineColor(ROOT.kBlack)
    h1.Draw("SAME HIST")
    hpred.Draw("AXIS same")
    utils.DrawCmsText(simplecanvas)
    utils.DrawLumiText(simplecanvas,lumi)
    simplecanvas.SaveAs("{}/{}_pull.{}".format(plotdir,desc.replace(" ","_"),format))

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
    simplecanvas.SaveAs("{}/{}_systcontam.{}".format(plotdir,desc.replace(" ","_"),format))
    simplecanvas.SetLogy(False)

def getMergedCountsLine(region,year,D16,eD16,sD16,D1718,eD1718,sD1718,S1718,eS1718):
    if not colorTables:
        colorline = ""
    elif region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]    
    if year == "2016": # don't return 2017-2018
        lumi = 35.9/41.97
        return colorline+"{} & {:.3f} +{:.3f}-{:.3f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR","") + " (2016)",
                                                                                                                                                                                                                                    D16[region+" pre"], sqrt(eD16[region+" pre"][0]**2 + sD16[region+" nc"]**2 + sD16[region+" fs"]**2), sqrt(eD16[region+" pre"][1]**2 + sD16[region+" nc"]**2 + sD16[region+" fs"]**2), D16[region+" obs"], S1718[(1800,1400,10)][region+" obs"]*lumi, eS1718[(1800,1400,10)][region+" obs"][0]*lumi, S1718[(1800,1600,10)][region+" obs"]*lumi, eS1718[(1800,1600,10)][region+" obs"][0]*lumi, S1718[(1800,1700,10)][region+" obs"]*lumi, eS1718[(1800,1700,10)][region+" obs"][0]*lumi, S1718[(1800,1400,90)][region+" obs"]*lumi, eS1718[(1800,1400,90)][region+" obs"][0]*lumi, S1718[(1800,1600,90)][region+" obs"]*lumi, eS1718[(1800,1600,90)][region+" obs"][0]*lumi, S1718[(1800,1700,90)][region+" obs"]*lumi, eS1718[(1800,1700,90)][region+" obs"][0]*lumi)
    elif year == "2017and2018": # don't return 2016
        lumi = 1+(59.97/41.97)
        return colorline+"{} & {:.3f} +{:.3f}-{:.3f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR","")+" (2017-18)",
                                                                                                                                                                                                                                    D1718[region+" pre"], sqrt(eD1718[region+" pre"][0]**2+sD1718[region+" nc"]**2+sD1718[region+" fs"]**2), sqrt(eD1718[region+" pre"][1]**2+sD1718[region+" nc"]**2+sD1718[region+" fs"]**2), D1718[region+" obs"], S1718[(1800,1400,10)][region+" obs"]*lumi, eS1718[(1800,1400,10)][region+" obs"][0]*lumi, S1718[(1800,1600,10)][region+" obs"]*lumi, eS1718[(1800,1600,10)][region+" obs"][0]*lumi, S1718[(1800,1700,10)][region+" obs"]*lumi, eS1718[(1800,1700,10)][region+" obs"][0]*lumi, S1718[(1800,1400,90)][region+" obs"]*lumi, eS1718[(1800,1400,90)][region+" obs"][0]*lumi, S1718[(1800,1600,90)][region+" obs"]*lumi, eS1718[(1800,1600,90)][region+" obs"][0]*lumi, S1718[(1800,1700,90)][region+" obs"]*lumi, eS1718[(1800,1700,90)][region+" obs"][0]*lumi)
    else:
        print "getMergedCountsLine: Unrecognized year",year
        return ""
#        lumi = (35.9/41.97)
#        line_for_2016 = colorline+"{} & - & {:.3f} +{:.3f}-{:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR","") + " (2016)",
                                                                                                                                                                                    #                                                D16[region+" pre"], sqrt(eD16[region+" pre"][0]**2+sD16[region+" fs"]**2+sD16[region+" nc"]**2), sqrt(eD16[region+" pre"][1]**2+sD16[region+" fs"]**2+sD16[region+" nc"]**2), S1718[(1800,1400,10)][region+" obs"]*lumi, eS1718[(1800,1400,10)][region+" obs"][0]*lumi, S1718[(1800,1600,10)][region+" obs"]*lumi, eS1718[(1800,1600,10)][region+" obs"][0]*lumi, S1718[(1800,1700,10)][region+" obs"]*lumi, eS1718[(1800,1700,10)][region+" obs"][0]*lumi, S1718[(1800,1400,90)][region+" obs"]*lumi, eS1718[(1800,1400,90)][region+" obs"][0]*lumi, S1718[(1800,1600,90)][region+" obs"]*lumi, eS1718[(1800,1600,90)][region+" obs"][0]*lumi, S1718[(1800,1700,90)][region+" obs"]*lumi, eS1718[(1800,1700,90)][region+" obs"][0]*lumi)
#        lumi = 1+(59.97/41.97)
#        line_for_2017and2018 = colorline+"{} & {:.3f} +{:.3f}-{:.3f} & - & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR","")+" (2017-18)",
                                                                                                                                                                                    #                                                D1718[region+" pre"], sqrt(eD1718[region+" pre"][0]**2+sD1718[region+" fs"]**2+sD1718[region+" nc"]**2), sqrt(eD1718[region+" pre"][1]**2+sD1718[region+" fs"]**2+sD1718[region+" nc"]**2), S1718[(1800,1400,10)][region+" obs"]*lumi, eS1718[(1800,1400,10)][region+" obs"][0]*lumi, S1718[(1800,1600,10)][region+" obs"]*lumi, eS1718[(1800,1600,10)][region+" obs"][0]*lumi, S1718[(1800,1700,10)][region+" obs"]*lumi, eS1718[(1800,1700,10)][region+" obs"][0]*lumi, S1718[(1800,1400,90)][region+" obs"]*lumi, eS1718[(1800,1400,90)][region+" obs"][0]*lumi, S1718[(1800,1600,90)][region+" obs"]*lumi, eS1718[(1800,1600,90)][region+" obs"][0]*lumi, S1718[(1800,1700,90)][region+" obs"]*lumi, eS1718[(1800,1700,90)][region+" obs"][0]*lumi)
#        return line_for_2016 + line_for_2017and2018


def getMRvsSRLine(region_SR,year=None):
    if not colorTables:
        colorline = ""
    elif region[0] == "P":
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


def getMergedLineDataSTC(region,D16,eD16,sD16,D1718,eD1718,sD1718,rescale16=1.0,rescale1718=1.0): # rescale multiplies prediction to enable partial unblinding
    if not colorTables:
        colorline = ""
    elif region[0] == "P":
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

def getMergedLineDataSTC_Combined(region,D16,eD16,sD16,D1718,eD1718,sD1718,rescale16=1.0,rescale1718=1.0): # rescale multiplies prediction to enable partial unblinding
    if not colorTables:
        colorline = ""
    elif region[0] == "P":
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
        syst16 = "$<${:.1f}\%$>$".format(100*sqrt(sD16[region+" fsrel"]**2 + sD16[region+" ncrel"]**2))
    if pred1718 > 0:
        syst1718 = "{:.3f}".format(sqrt(sD1718[region+" fs"]**2 + sD1718[region+" nc"]**2))
    else: 
        syst1718 = "$<${:.1f}\%$>$".format(100*sqrt(sD1718[region+" fsrel"]**2 + sD1718[region+" ncrel"]**2))

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

def getMergedLineData(region,D16,eD16,sD16,D1718,eD1718,sD1718,rescale16=1.0,rescale1718=1.0): # rescale multiplies prediction to enable partial unblinding
    if not colorTables:
        colorline = ""
    elif region[0] == "P":
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
        return colorline+"{} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.0f} & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred1718, eD1718[region+" pre"][0]*rescale1718, eD1718[region+" pre"][1]*rescale1718, systFS1718, systNC1718, D1718[region+" obs"])

    else:
        return colorline+"{} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.0f} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred1718, eD1718[region+" pre"][0]*rescale1718, eD1718[region+" pre"][1]*rescale1718, systFS1718, systNC1718, D1718[region+" obs"],
                           pred16, eD16[region+" pre"][0]*rescale16, eD16[region+" pre"][1]*rescale16, systFS16, systNC16, D16[region+" obs"])

def getMergedLineData_Combined(region,D16,eD16,sD16,D1718,eD1718,sD1718,rescale16=1.0,rescale1718=1.0): # rescale multiplies prediction to enable partial unblinding
    if not colorTables:
        colorline = ""
    elif region[0] == "P":
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
        syst16 = "$<${:.1f}\%$>$".format(100*sqrt(sD16[region+" fsrel"]**2 + sD16[region+" ncrel"]**2))
    if pred1718 > 0:
        syst1718 = "{:.3f}".format(sqrt(sD1718[region+" fs"]**2 + sD1718[region+" nc"]**2))
    else: 
        syst1718 = "$<${:.1f}\%$>$".format(100*sqrt(sD1718[region+" fsrel"]**2 + sD1718[region+" ncrel"]**2))

    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (syst) & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred16, eD16[region+" pre"][0]*rescale16, eD16[region+" pre"][1]*rescale16, syst16, D16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (syst) & {:.0f} & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred1718, eD1718[region+" pre"][0]*rescale1718, eD1718[region+" pre"][1]*rescale1718, syst1718, D1718[region+" obs"])

    else:
        return colorline+"{} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (syst) & {:.0f} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {} (syst) & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred1718, eD1718[region+" pre"][0]*rescale1718, eD1718[region+" pre"][1]*rescale1718, syst1718, D1718[region+" obs"],
                           pred16, eD16[region+" pre"][0]*rescale16, eD16[region+" pre"][1]*rescale16, syst16, D16[region+" obs"])

def getMergedLineMCSTC(region,M16,eM16,sM16,M1718,eM1718,sM1718,rescale16=1.0,rescale17=1.0): # rescale multiplies prediction to enable partial unblinding
    if not colorTables:
        colorline = ""
    elif region[0] == "P":
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

def getMergedLineMCSTC_Combined(region,M16,eM16,sM16,M1718,eM1718,sM1718,rescale16=1.0,rescale17=1.0): # rescale multiplies prediction to enable partial unblinding
    if not colorTables:
        colorline = ""
    elif region[0] == "P":
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
        syst16 = "$<${:.1f}\%$>$".format(100*sqrt(sM16[region+" fsrel"]**2 + sM16[region+" ncrel"]**2))
    if pred17 > 0:
        syst17 = "{:.3f}".format(sqrt(sM1718[region+" fs"]**2 + sM1718[region+" nc"]**2))
    else: 
        syst17 = "$<${:.1f}\%$>$".format(100*sqrt(sM1718[region+" fsrel"]**2 + sM1718[region+" ncrel"]**2))

    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & - & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (syst) & {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M16[region+" STC"], pred16, eM16[region+" pre"][0]*rescale16, syst16, M16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (syst) & {:.3f} & - & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M1718[region+" STC"], pred17, eM1718[region+" pre"][0]*rescale17, syst17, M1718[region+" obs"])

    else:
        return colorline+"{} & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (syst) & {:.3f} & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (syst) & {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M1718[region+" STC"], pred17, eM1718[region+" pre"][0]*rescale17, syst17, M1718[region+" obs"],
                           M16[region+" STC"], pred16, eM16[region+" pre"][0]*rescale16, syst16, M16[region+" obs"])


def getMergedLineMC(region,M16,eM16,sM16,M1718,eM1718,sM1718,rescale16=1.0,rescale17=1.0): # rescale multiplies prediction to enable partial unblinding
    if not colorTables:
        colorline = ""
    elif region[0] == "P":
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
        return colorline+"{} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.3f} & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred17, eM1718[region+" pre"][0]*rescale17, systFS17, systNC17, M1718[region+" obs"])

    else:
        return colorline+"{} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (f$_{{short}}$ syst) $\pm$ {} (VR syst) & {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred17, eM1718[region+" pre"][0]*rescale17, systFS17, systNC17, M1718[region+" obs"],
                           pred16, eM16[region+" pre"][0]*rescale16, systFS16, systNC16, M16[region+" obs"])


def getMergedLineMC_Combined(region,M16,eM16,sM16,M1718,eM1718,sM1718,rescale16=1.0,rescale17=1.0): # rescale multiplies prediction to enable partial unblinding
    if not colorTables:
        colorline = ""
    elif region[0] == "P":
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
        syst16 = "$<${:.1f}\%$>$".format(100*sqrt(sM16[region+" fsrel"]**2 + sM16[region+" ncrel"]**2))
    if pred17 > 0:
        syst17 = "{:.3f}".format(sqrt(sM1718[region+" fs"]**2 + sM1718[region+" nc"]**2))
    else: 
        syst17 = "$<${:.1f}\%$>$".format(100*sqrt(sM1718[region+" fsrel"]**2 + sM1718[region+" ncrel"]**2))

    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (syst) & {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred16, eM16[region+" pre"][0]*rescale16, syst16, M16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (syst) & {:.3f} & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred17, eM1718[region+" pre"][0]*rescale17, syst17, M1718[region+" obs"])

    else:
        return colorline+"{} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (syst) & {:.3f} & \\textbf{{{:.3f}}} $\pm$ {:.3f} (stat) $\pm$ {} (syst) & {:.3f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    pred17, eM1718[region+" pre"][0]*rescale17, syst17, M1718[region+" obs"],
                           pred16, eM16[region+" pre"][0]*rescale16, syst16, M16[region+" obs"])



def getMergedFSLineData(cat,D16f,eD16f,sD16f,D1718f,eD1718f,sD1718f):
    if not colorTables:
        colorline = ""
    elif cat[0] == "P":
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

def getMergedFSLineMC(cat,M16f,eM16f,sM16f,M1718f,eM1718f,sM1718f):
    if not colorTables:
        colorline = ""
    elif cat[0] == "P":
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

def get17vs18FSLineData(cat):
    if not colorTables:
        colorline = ""
    elif cat[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif cat[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"                                   
    return colorline+"{} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {:.3f} (syst) & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {:.3f} (syst)\\\\ \n".format(cat,D17f[cat],eD17f[cat][0],eD17f[cat][1],sD17f[cat],D18f[cat],eD18f[cat][0],eD18f[cat][1],sD18f[cat])

def get17vs18FSLineMC(cat):
    if not colorTables:
        colorline = ""
    elif cat[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif cat[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"                                   
    return colorline+"{} & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {:.3f} (syst) & \\textbf{{{:.3f}}} +{:.3f}-{:.3f} (stat) $\pm$ {:.3f} (syst)\\\\ \n".format(cat,M17f[cat],eM17f[cat][0],eM17f[cat][1],sM17f[cat],M18f[cat],eM18f[cat][0],eM18f[cat][1],sM18f[cat])
