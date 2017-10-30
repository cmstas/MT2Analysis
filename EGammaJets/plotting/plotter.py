import ppmUtils as utils
import pyRootPlotMaker as ppm
import ROOT
import sys

ROOT.gROOT.SetBatch(True)

# if len(sys.argv) > 1 and sys.argv[1] == "mode":

name16="~/MT2AnalysisNebraskaCMS3/EGammaJets/output/output_v2_B.root"
name17="~/2017/MT2AnalysisUpdate/EGammaJets/output/data_2017.root"

f16 = ROOT.TFile(name16)
f17 = ROOT.TFile(name17)

hists=["h_ph","h_el","h_el_norm","h_ph_norm","h_el_norm_no0","h_ph_norm_no0","h_ph_par","h_ph_per","h_el_par","h_el_per"]

for hist in hists:

    h16=f16.Get(hist)
    h17=f17.Get(hist)

    # area scaling due to incomplete 2016 dataset
    h16.Scale(h17.Integral(0,31)/h16.Integral(0,31))

    if hist.find("ph") != -1:
        desc = "Overlapping Photon-Jet Pairs"
        pt = "p_{T}^{#gamma} > 100 GeV, p_{T}^{Jet} > 300 GeV"
        if hist.find("norm") == -1 and hist.find("par") == -1 and hist.find("per") == -1:
            xtitle = "p_{T}^{#gamma}/p_{T}^{Jet}"
        elif hist.find("norm") != -1:
            xtitle = "(p_{T}^{#gamma}/p_{T}^{Jet}) / Jet NEMF"
        elif hist.find("par") != -1:
            xtitle = "MET (anti)parallel to photon [GeV]"
        elif hist.find("per") != -1:
            xtitle = "MET perpendicular to photon [GeV]"
    else: 
        pt = "p_{T}^{e} > 100 GeV, p_{T}^{Jet} > 300 GeV"
        if hist.find("norm") == -1 and hist.find("par") == -1 and hist.find("per") == -1:
            xtitle = "p_{T}^{e}/p_{T}^{Jet}"
        elif hist.find("norm") != -1:
            xtitle = "(p_{T}^{e}/p_{T}^{Jet}) / Jet CEMF"
        elif hist.find("par") != -1:
            xtitle = "MET (anti)parallel to electron [GeV]"
        elif hist.find("per") != -1:
            xtitle = "MET perpendicular to electron [GeV]"
        if hist.find("id") != -1:
            desc = "Overlapping Electron-Jet Pairs (Tighter ID)"
        else:
            desc = "Overlapping Electron-Jet Pairs"

    h_16_vec = [h16]
    h_16_names = ["Data (2016, Scaled)"]

    desc2 = "(2016 and 2017 JetHT Datasets)"
    
    ht = "H_{T} > 1200 GeV"

    jets = "2+ Jets with p_{T} > 300 GeV"

    subtitles = [desc,desc2,ht,jets,pt]

    xunit = None

    outputFiles = ["pngs/"+hist+".png","pdfs/"+hist+".pdf"]

    for outputFile in outputFiles:
        ppm.plotDataMC(h_16_vec,h_16_names,h_data=h17,dataTitle="Data (2017)",isLog = False,ratioTitle="2017 / 2016", subtitles=subtitles, subtitleSize=0.025,
                           xAxisTitle=xtitle, xAxisUnit=xunit, yRangeUserRatio=[0,3], markerSize=1.1,lumi=11.379,lumiUnit="fb",energy=13,saveAs=outputFile)

def SavePlot (tcanvas, name="plot", extensions=["png","pdf"]):
    utils.DrawCmsText(canvas,text="CMS Preliminary",textFont=62,textSize=0.035)
    for extension in extensions:
        tcanvas.SaveAs(extension+"s/"+name+"."+extension)

profiles = ["p_ph","p_el"]

for profile in profiles:
    p16 = f16.Get(profile)
    p17 = f17.Get(profile)
    
    p17.SetLineColor(ROOT.kRed)
    p17.SetLineWidth(3)
    p16.SetLineWidth(3)
    p16.SetMinimum(0.0)
    p16.SetMaximum(1.6)

    canvas = ROOT.TCanvas()
    canvas.SetCanvasSize(700,700)
    canvas.SetTicks(1,2)
    pads = [canvas]
    pads[0].SetLeftMargin(0.12)
    pads[0].SetTopMargin(0.12)
    pads[0].SetRightMargin(0.12)
    pads[0].cd()

    ROOT.gStyle.SetLegendBorderSize(0) # borderless legend
    tl = ROOT.TLegend(0.55,0.70,0.80,0.85)
    tl.SetMargin(0.1)

    p16.Draw()
    p17.Draw("same")
    tl.AddEntry(p16,"Data (2016)")
    tl.AddEntry(p17,"Data (2017)")
    tl.Draw()
    SavePlot(canvas,profile)
    
