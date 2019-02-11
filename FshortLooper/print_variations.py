# Make plots of fshort variations.

import ROOT
from math import sqrt
import re
import sys
import os
import subprocess

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

verbose = False # Print more status messages
printTables = False
format = "pdf"

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
simplecanvas.SetBottomMargin(0.45)

tl=ROOT.TLegend(0.2,0.75,0.4,0.85)

if len(sys.argv) < 2:
    print "Which tag?"
    exit(1)

tag=sys.argv[1]

os.system("mkdir -p fshort_variations")

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


def startFshortTableData(outfile, merge2017and2018=False):
    outfile.write("\chead{Fshort Table}\n")
    outfile.write("\\begin{document}\n\n")

    outfile.write("\\begin{tabular}{l | *2c g | *2c g | *2c g}\n") if not merge2017and2018 else outfile.write("\\begin{tabular}{l | *2c g | *2c g}\n")
    outfile.write("\\toprule\n")
    if not merge2017and2018:
        outfile.write("\multirow{2}{*}{Variation} & \multicolumn{3}{c}{2018 Data} & \multicolumn{3}{c}{2017 Data} & \multicolumn{3}{c}{2016 Data}\\\\ \n")
        outfile.write(" & STC & ST & $f_{short}$ & STC & ST & $f_{short}$ & STC & ST & $f_{short}$\\\\ \n ")
    else:
        outfile.write("\multirow{2}{*}{Variation} & \multicolumn{3}{c}{2017 \& 2018 Data} & \multicolumn{3}{c}{2016 Data}\\\\ \n")
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
    vals18 = {}
    errs18 = {}
    if region[0] == "P":
        if region[1] == "3":
            cat = 2
            fileregion = region[3:]
        elif region[1] == "4":
            cat = 3
            fileregion = region[3:]
        elif region[1] == "_":
            cat = 1
            fileregion = region[2:]
        else:
            print "Did not recognized a P-like region",region
            exit(1)
    elif region[0] == "M":
        cat = 4
        fileregion = region[2:]
    elif region[0] == "L":
        cat = 5
        fileregion = region[2:]
    else: 
        print "Did not recognize region",region
    d18=ROOT.TFile.Open("output/Fshort_data_2018_{}.root".format(tag)) if data else ROOT.TFile.Open("output/Fshort_mc_2017_{}.root".format(tag))
    d17=ROOT.TFile.Open("output/Fshort_data_2017_{}.root".format(tag)) if data else ROOT.TFile.Open("output/Fshort_mc_2017_{}.root".format(tag))
    d16=ROOT.TFile.Open("output/Fshort_data_2016_{}.root".format(tag)) if data else ROOT.TFile.Open("output/Fshort_mc_2016_{}.root".format(tag))
    for ptstring in ["","_lowpt","_hipt"]:
        for variation in ["Baseline","HT250","HT450","HT450MET100","MET30","MET100","MET250"]:
            hname = "h_fs"+fileregion+"_"+variation+ptstring

            h = d18.Get(hname)
            vals18[variation+ptstring + " STC"] = h.GetBinContent(cat,3)
            vals18[variation+ptstring + " ST"] = h.GetBinContent(cat,2)
            vals18[variation+ptstring + " FS"] = h.GetBinContent(cat,1)
            errs18[variation+ptstring + " STC"] = h.GetBinError(cat,3)
            errs18[variation+ptstring + " ST"] = h.GetBinError(cat,2)
            errs18[variation+ptstring + " FS"] = h.GetBinError(cat,1)

            h = d17.Get(hname)
            vals17[variation+ptstring + " STC"] = h.GetBinContent(cat,3)
            vals17[variation+ptstring + " ST"] = h.GetBinContent(cat,2)
            vals17[variation+ptstring + " FS"] = h.GetBinContent(cat,1)
            errs17[variation+ptstring + " STC"] = h.GetBinError(cat,3)
            errs17[variation+ptstring + " ST"] = h.GetBinError(cat,2)
            errs17[variation+ptstring + " FS"] = h.GetBinError(cat,1)

            h = d16.Get(hname)
            vals16[variation+ptstring + " STC"] = h.GetBinContent(cat,3)
            vals16[variation+ptstring + " ST"] = h.GetBinContent(cat,2)
            vals16[variation+ptstring + " FS"] = h.GetBinContent(cat,1)
            errs16[variation+ptstring + " STC"] = h.GetBinError(cat,3)
            errs16[variation+ptstring + " ST"] = h.GetBinError(cat,2)
            errs16[variation+ptstring + " FS"] = h.GetBinError(cat,1)

    d18.Close()
    d17.Close()
    d16.Close()

    return vals18,errs18,vals17,errs17,vals16,errs16

def getFshortMerged(region,data):
    vals1718 = {}
    vals18 = {}
    vals17 = {}
    vals16 = {}
    errs1718 = {}
    errs18 = {}
    errs17 = {}    
    errs16 = {}
    systs1718 = {}
    systs18 = {}
    systs17 = {}
    systs16 = {}
    if region[0] == "P":
        if region[1] == "3":
            cat = 2
            fileregion = region[3:]
        elif region[1] == "4":
            cat = 3
            fileregion = region[3:]
        elif region[1] == "_":
            cat = 1
            fileregion = region[2:]
        else:
            print "Did not recognized a P-like region",region
            exit(1)
    elif region[0] == "M":
        cat = 4
        fileregion = region[2:]
    elif region[0] == "L":
        cat = 5
        fileregion = region[2:]
    else: 
        print "Did not recognize region",region
    data_string = "data" if data else "mc"
    d1718=ROOT.TFile.Open("output/Fshort_data_2017and2018_{}.root".format(tag)) if data else ROOT.TFile.Open("output/Fshort_mc_2017and2018_{}.root".format(tag))
    d18=ROOT.TFile.Open("output/Fshort_data_2018_{}.root".format(tag)) if data else ROOT.TFile.Open("output/Fshort_mc_2018_{}.root".format(tag))
    d17=ROOT.TFile.Open("output/Fshort_data_2017_{}.root".format(tag)) if data else ROOT.TFile.Open("output/Fshort_mc_2017_{}.root".format(tag))
    d16=ROOT.TFile.Open("output/Fshort_data_2016_{}.root".format(tag)) if data else ROOT.TFile.Open("output/Fshort_mc_2016_{}.root".format(tag))
    for ptstring in ["","_lowpt","_hipt"]:
        for variation in ["Baseline","HT250","HT450","HT450MET100","MET30","MET100","MET250"]:
            hname = "h_fs"+fileregion+"_"+variation+ptstring

            h = d1718.Get(hname).Clone(data_string+hname+"1718")
            h_up = d1718.Get(hname+"_up").Clone(data_string+hname+"_up1718")
            h_dn = d1718.Get(hname+"_dn").Clone(data_string+hname+"_dn1718")
            vals1718[variation+ptstring + " STC"] = h.GetBinContent(cat,3)
            vals1718[variation+ptstring + " ST"] = h.GetBinContent(cat,2)
            vals1718[variation+ptstring + " FS"] = h.GetBinContent(cat,1)
            errs1718[variation+ptstring + " STC"] = h.GetBinError(cat,3)
            errs1718[variation+ptstring + " ST"] = h.GetBinError(cat,2)
            errs1718[variation+ptstring + " FS"] = (h_up.GetBinError(cat,1),h_dn.GetBinError(cat,1))

            h = d18.Get(hname).Clone(data_string+hname+"18")
            h_up = d18.Get(hname+"_up").Clone(data_string+hname+"_up18")
            h_dn = d18.Get(hname+"_dn").Clone(data_string+hname+"_dn18")
            vals18[variation+ptstring + " STC"] = h.GetBinContent(cat,3)
            vals18[variation+ptstring + " ST"] = h.GetBinContent(cat,2)
            vals18[variation+ptstring + " FS"] = h.GetBinContent(cat,1)
            errs18[variation+ptstring + " STC"] = h.GetBinError(cat,3)
            errs18[variation+ptstring + " ST"] = h.GetBinError(cat,2)
            errs18[variation+ptstring + " FS"] = (h_up.GetBinError(cat,1), h_dn.GetBinError(cat,1))
            
            h = d17.Get(hname).Clone(data_string+hname+"17")
            h_up = d17.Get(hname+"_up").Clone(data_string+hname+"_up17")
            h_dn = d17.Get(hname+"_dn").Clone(data_string+hname+"_dn17")
            vals17[variation+ptstring + " STC"] = h.GetBinContent(cat,3)
            vals17[variation+ptstring + " ST"] = h.GetBinContent(cat,2)
            vals17[variation+ptstring + " FS"] = h.GetBinContent(cat,1)
            errs17[variation+ptstring + " STC"] = h.GetBinError(cat,3)
            errs17[variation+ptstring + " ST"] = h.GetBinError(cat,2)
            errs17[variation+ptstring + " FS"] = (h_up.GetBinError(cat,1), h_dn.GetBinError(cat,1))
            
            h = d16.Get(hname).Clone(data_string+hname+"16")
            h_up = d16.Get(hname+"_up").Clone(data_string+hname+"_up16")
            h_dn = d16.Get(hname+"_dn").Clone(data_string+hname+"_dn16")
            vals16[variation+ptstring + " STC"] = h.GetBinContent(cat,3)
            vals16[variation+ptstring + " ST"] = h.GetBinContent(cat,2)
            vals16[variation+ptstring + " FS"] = h.GetBinContent(cat,1)
            errs16[variation+ptstring + " STC"] = h.GetBinError(cat,3)
            errs16[variation+ptstring + " ST"] = h.GetBinError(cat,2)
            errs16[variation+ptstring + " FS"] = (h_up.GetBinError(cat,1), h_dn.GetBinError(cat,1))
            

        hnamesyst = "h_fs"+fileregion+"_Baseline"+ptstring+"_syst"
        h = d1718.Get(hnamesyst).Clone(data_string+hnamesyst+"_1718")
        systs1718["Baseline"+ptstring] = h.GetBinContent(cat)
        h = d18.Get(hnamesyst).Clone(data_string+hnamesyst+"_18")
        systs18["Baseline"+ptstring] = h.GetBinContent(cat)
        h = d17.Get(hnamesyst).Clone(data_string+hnamesyst+"_17")
        systs17["Baseline"+ptstring] = h.GetBinContent(cat)
        h = d16.Get(hnamesyst).Clone(data_string+hnamesyst+"_16")
        systs16["Baseline"+ptstring] = h.GetBinContent(cat)

    d1718.Close()
    d18.Close()
    d17.Close()
    d16.Close()

    return vals1718,errs1718,systs1718,vals18,errs18,systs18,vals17,errs17,systs17,vals16,errs16,systs16

writeabletag = { "Baseline" : "Baseline",
                 "HT250"   : "$250 < H_{T} < 450$ GeV",
                 "HT450"   : "$H_{T} > 450$ GeV",
                 "HT450MET100" : "$H_{T} > 450$ and $\\text{MET} > 100$ GeV",
                 "MET30"   : "$30 < \\text{MET} < 100$ GeV",
                 "MET100" : "$ 100 < \\text{MET} < 250$ GeV",
                 "MET250"  : "$\\text{MET} > 250$ GeV",
                 "Baseline_lowpt" : "Baseline ($p_{T} < 50$ GeV)",
                 "HT250_lowpt"   : "$250 < H_{T} < 450$ GeV ($p_{T} < 50$ GeV)",
                 "HT450_lowpt"   : "$H_{T} > 450$ GeV ($p_{T} < 50$ GeV)",
                 "HT450MET100_lowpt" : "$H_{T} > 450$ and $\\text{MET} > 100$ GeV ($p_{T} < 50$ GeV)",
                 "MET30_lowpt"   : "$30 < \\text{MET} < 100$ GeV ($p_{T} < 50$ GeV)",
                 "MET100_lowpt" : "$ 100 < \\text{MET} < 250$ GeV ($p_{T} < 50$ GeV)",
                 "MET250_lowpt"  : "$\\text{MET} > 250$ GeV ($p_{T} < 50$ GeV)",
                 "Baseline_hipt" : "Baseline ($p_{T} > 50$ GeV)",
                 "HT250_hipt"   : "$250 < H_{T} < 450$ GeV ($p_{T} > 50$ GeV)",
                 "HT450_hipt"   : "$H_{T} > 450$ GeV ($p_{T} > 50$ GeV)",
                 "HT450MET100_hipt" : "$H_{T} > 450$ and $\\text{MET} > 100$ GeV ($p_{T} > 50$ GeV)",
                 "MET30_hipt"   : "$30 < \\text{MET} < 100$ GeV ($p_{T} > 50$ GeV)",
                 "MET100_hipt" : "$ 100 < \\text{MET} < 250$ GeV ($p_{T} > 50$ GeV)",
                 "MET250_hipt"  : "$\\text{MET} > 250$ GeV ($p_{T} > 50$ GeV)"
                 }

roottag = { "Baseline" : "Baseline",
                 "HT250"   : "250 < H_{T} < 450 GeV",
                 "HT450"   : "H_{T} > 450 GeV",
                 "HT450MET100" : "H_{T} > 450 and MET > 100 GeV",
                 "MET30"   : "30 < MET < 100 GeV",
                 "MET100" : " 100 < MET < 250 GeV",
                 "MET250"  : "MET > 250 GeV",
            "Baseline_lowpt" : "Baseline (p_{T} < 50 GeV)",
                 "HT250_lowpt"   : "250 < H_{T} < 450 GeV (p_{T} < 50 GeV)",
                 "HT450_lowpt"   : "H_{T} > 450 GeV (p_{T} < 50 GeV)",
                 "HT450MET100_lowpt" : "H_{T} > 450 and MET > 100 GeV (p_{T} < 50 GeV)",
                 "MET30_lowpt"   : "30 < MET < 100 GeV (p_{T} < 50 GeV)",
                 "MET100_lowpt" : " 100 < MET < 250 GeV (p_{T} < 50 GeV)",
                 "MET250_lowpt"  : "MET > 250 GeV (p_{T} < 50 GeV)",
            "Baseline_hipt" : "Baseline (p_{T} > 50 GeV)",
                 "HT250_hipt"   : "250 < H_{T} < 450 GeV (p_{T} > 50 GeV)",
                 "HT450_hipt"   : "H_{T} > 450 GeV (p_{T} > 50 GeV)",
                 "HT450MET100_hipt" : "H_{T} > 450 and MET > 100 GeV (p_{T} > 50 GeV)",
                 "MET30_hipt"   : "30 < MET < 100 GeV (p_{T} > 50 GeV)",
                 "MET100_hipt" : " 100 < MET < 250 GeV (p_{T} > 50 GeV)",
                 "MET250_hipt"  : "MET > 250 GeV (p_{T} > 50 GeV)"
                 }

def getLineData(variation,D18,eD18,D17,eD17,D16,eD16):
    
    return "{} & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(writeabletag[variation],
                                                                                                                                                                                                                                    D18[variation+" STC"], D18[variation+" ST"], D18[variation+" FS"], eD18[variation+" FS"],                               
                                                                                                                                                                                                                                    D17[variation+" STC"], D17[variation+" ST"], D17[variation+" FS"], eD17[variation+" FS"],                               
                                                                                                                                                                                                                                    D16[variation+" STC"], D16[variation+" ST"], D16[variation+" FS"], eD16[variation+" FS"])

def getLineDataMerged(variation,D1718,eD1718,D16,eD16):

    return "{} & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(writeabletag[variation],
                                                                                                                                                                                                                                    D1718[variation+" STC"], D1718[variation+" ST"], D1718[variation+" FS"], eD1718[variation+" FS"],                               
                                                                                                                                                                                                                                    D16[variation+" STC"], D16[variation+" ST"], D16[variation+" FS"], eD16[variation+" FS"])

def getLineMC(variation,M1718,eM1718,M16,eM16):
    
    return "{} & {:.2f} $\pm$ {:.2f} & {:.2f} $\pm$ {:.2f} & {:.3f} $\pm$ {:.3f} & {:.2f} $\pm$ {:.2f} & {:.2f} $\pm$ {:.2f} & {:.3f} $\pm$ {:.3f}  \\\\ \n".format(writeabletag[variation],
                                                                                                                                                                                                                                    M1718[variation+" STC"], eM1718[variation+" STC"], M1718[variation+" ST"], eM1718[variation+" ST"], M1718[variation+" FS"], eM1718[variation+" FS"],
                                                 M16[variation+" STC"], eM16[variation+" STC"], M16[variation+" ST"], eM16[variation+" ST"], M16[variation+" FS"], eM16[variation+" FS"])

def makePlot(region,variations,vals,errs,systs,desc,ptstring=""):
    simplecanvas.cd()
    track_category = region[0:2]
    if desc.find("2016") >= 0 and (track_category == "P3" or track_category == "P4"):
        return
    if (desc.find("2017") >= 0 or desc.find("2018") >= 0) and (track_category == "P_"):
        return
    tl.Clear()
    nvariations=len(variations)             
    title = desc + " " + region
    title = title.replace("_"," ")
    title = title.replace("23","N_{Jet}=2-3")
    title = title.replace("4","N_{Jet}>3")
    title = title.replace("PN_{Jet}>3","P4")
    hist=ROOT.TH1D(region+ptstring,title + " Kinematical Variations;;f_{short}",nvariations,0,nvariations)            
    hist.SetLineWidth(3)
    hdef=hist.Clone(desc+region+ptstring+"_Baseline")
    variation_index = 1
    var_errs = []
    def_errs = []
    for variation in variations:
        hdef.GetXaxis().SetBinLabel(variation_index,roottag[variation])
        hist.SetBinContent(variation_index,vals[variation+" FS"] if vals[variation+" FS"] > 0 else -1)
        hist.SetBinError(variation_index,1e-9)
        var_errs.append(errs[variation+" FS"])
        hdef.SetBinContent(variation_index,vals["Baseline"+ptstring+" FS"])
        hdef.SetBinError(variation_index,1e-9)
        def_errs.append(errs["Baseline"+ptstring+" FS"])
        variation_index += 1    
    hdef.GetXaxis().LabelsOption("v")
    hdef.SetMaximum(hist.GetMaximum() * 2)
    hdef.GetXaxis().SetTitleOffset(5.5)
    hdef.SetMinimum(0)
    hist.SetLineColor(ROOT.kRed)
    hdef.SetLineColor(ROOT.kBlack)
    hdef_nofill = hdef.Clone(hdef.GetName()+"nofill")
    hsyst= hdef.Clone(hdef.GetName()+"_syst")
    hsyst.SetFillColor(ROOT.kGray+2)
    hdef.SetFillColor(ROOT.kGray)
    gdef = getPoissonGraph(hdef, def_errs)
    gvar = getPoissonGraph(hist, var_errs)
    syst_errs = [sqrt(systs["Baseline"+ptstring]**2+errs["Baseline"+ptstring+" FS"][i]**2) for i in [0,1]]
#    if verbose: print syst_errs
    if region+ptstring == "P4_VR_4_lowpt": 
        print hsyst.GetName()
        print desc
        print syst_errs
        print errs["Baseline"+ptstring+" FS"]
    gsyst = getPoissonGraph( hsyst, [syst_errs for i in range(len(def_errs))] )
    tl.AddEntry(hist,"Variations")
    tl.AddEntry(hdef,"Baseline, Stat Error Only")
    tl.AddEntry(hsyst,"Baseline, Quadrature(Stat, Syst)")    
    hsyst.Draw("AXIS")
    gsyst.Draw("2 same")
    gdef.Draw("2 same")
    hdef_nofill.Draw("same")
    gvar.Draw("p same")
    hist.Draw("same")
    hsyst.Draw("AXIS same")
    tl.Draw()
    os.system("mkdir -p fshort_variations/{}".format(desc))
    simplecanvas.SaveAs("fshort_variations/{}/{}.{}".format(desc,hist.GetName(),format))

tablevariations = [variation + ptstring for variation in ["Baseline","HT250","HT450","HT450MET100","MET30","MET100","MET250"] for ptstring in ["","_lowpt","_hipt"]]
plotvariations = list(tablevariations)
plotvariations.remove("Baseline")
plotvariations_nopt = ["HT250","HT450","HT450MET100","MET30","MET100","MET250"]
plotvariations_lowpt = ["HT250_lowpt","HT450_lowpt","HT450MET100_lowpt","MET30_lowpt","MET100_lowpt","MET250_lowpt"]
plotvariations_hipt = ["HT250_hipt","HT450_hipt","HT450MET100_hipt","MET30_hipt","MET100_hipt","MET250_hipt"]

#for region in ["P_MR_23","P3_MR_23","P4_MR_23","M_MR_23","L_MR_23","P_MR_4","P3_MR_4","P4_MR_4","M_MR_4","L_MR_4","P_VR_23","P3_VR_23","P4_VR_23","M_VR_23","L_VR_23","P_VR_4","P3_VR_4","P4_VR_4","M_VR_4","L_VR_4"]:
# L regions lack stats
for region in ["P_MR_23","P3_MR_23","P4_MR_23","M_MR_23","P_MR_4","P3_MR_4","P4_MR_4","M_MR_4","P_VR_23","P3_VR_23","P4_VR_23","M_VR_23","P_VR_4","P3_VR_4","P4_VR_4","M_VR_4"]:
    print region

    D1718,eD1718,sD1718,D18,eD18,sD18,D17,eD17,sD17,D16,eD16,sD16=getFshortMerged(region,True)

    M1718,eM1718,sM1718,M18,eM18,sM18,M17,eM17,sM17,M16,eM16,sM16=getFshortMerged(region,False)

    makePlot(region,plotvariations_lowpt,D1718,eD1718,sD1718,"2017-2018_DATA_lowpt","_lowpt")
    makePlot(region,plotvariations_lowpt,D18,eD18,sD18,"2018_DATA_lowpt","_lowpt")
    makePlot(region,plotvariations_lowpt,D17,eD17,sD17,"2017_DATA_lowpt","_lowpt")
    makePlot(region,plotvariations_lowpt,D16,eD16,sD16,"2016_DATA_lowpt","_lowpt")

    makePlot(region,plotvariations_hipt,D1718,eD1718,sD1718,"2017-2018_DATA_hipt","_hipt")
    makePlot(region,plotvariations_hipt,D18,eD18,sD18,"2018_DATA_hipt","_hipt")
    makePlot(region,plotvariations_hipt,D17,eD17,sD17,"2017_DATA_hipt","_hipt")
    makePlot(region,plotvariations_hipt,D16,eD16,sD16,"2016_DATA_hipt","_hipt")



    makePlot(region,plotvariations_lowpt,M1718,eM1718,sM1718,"2017_MC_lowpt","_lowpt")
    makePlot(region,plotvariations_lowpt,M18,eM18,sM18,"2018_MC_lowpt","_lowpt")
    makePlot(region,plotvariations_lowpt,M17,eM17,sM17,"2017_MC_lowpt","_lowpt")
    makePlot(region,plotvariations_lowpt,M16,eM16,sM16,"2016_MC_lowpt","_lowpt")

    makePlot(region,plotvariations_hipt,M1718,eM1718,sM1718,"2017_MC_hipt","_hipt")
    makePlot(region,plotvariations_hipt,M18,eM18,sM18,"2018_MC_hipt","_hipt")
    makePlot(region,plotvariations_hipt,M17,eM17,sM17,"2017_MC_hipt","_hipt")
    makePlot(region,plotvariations_hipt,M16,eM16,sM16,"2016_MC_hipt","_hipt")

    if not printTables: continue

    output = open("variation_tables/fshort_data_{}.tex".format(region),"w")
    printHeader(output)
    startFshortTableData(output)    
    for variation in tablevariations:
        output.write(getLineData(variation,D18,eD18,D17,eD17,D16,eD16)) if not merge2017and2018 else output.write(getLineDataMerged(variation,D1718,eD1718,D16,eD16))
    printFooter(output)
    output.close()

    output = open("variation_tables/fshort_mc_{}.tex".format(region),"w")
    printHeader(output)
    startFshortTableMC(output)
    for variation in tablevariations:
        output.write(getLineMC(variation,M1718,eM1718,M16,eM16))
    printFooter(output)
    output.close()


print "Done"
