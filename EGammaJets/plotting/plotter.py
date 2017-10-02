import pyRootPlotMaker as ppm
import ROOT
import sys

ROOT.gROOT.SetBatch(True)

# if len(sys.argv) > 1 and sys.argv[1] == "mode":

name16="~/MT2AnalysisEdit/MyCode/EGammaJets/output/data_2016.root"
name17="~/2017/MT2AnalysisTrigFix/EGammaJets/output/data_2017.root"

f16 = ROOT.TFile(name16)
f17 = ROOT.TFile(name17)

hists=["h_ph"]

for hist in hists:

    h16=f16.Get(hist)
    h17=f17.Get(hist)

    h16.Scale(11.379/35.9)

    print(h16.Integral())
    print(h17.Integral())

    h_16_vec = [h16]
    h_16_names = ["Data (2016, Scaled)"]

    desc = "Overlapping Photon-Jet Pairs"

    desc2 = "(2016 and 2017 JetHT Datasets)"
    
    ht = "H_{T} > 1200 GeV"

    jets = "2+ Jets with p_{T} > 300 GeV"

    pt = "p_{T}^{#gamma} > 100 GeV, p_{T}^{Jet} > 300 GeV"

    subtitles = [desc,desc2,ht,jets,pt]

    xtitle = "p_{T}^{#gamma}/p_{T}^{Jet}"

    xunit = None

    outputFiles = ["pngs/"+hist+".png","pdfs/"+hist+".pdf"]

    for outputFile in outputFiles:
        ppm.plotDataMC(h_16_vec,h_16_names,h_data=h17,dataTitle="Data (2017)",isLog = False,ratioTitle="2017 / 2016", subtitles=subtitles, subtitleSize=0.025,
                           xAxisTitle=xtitle, xAxisUnit=xunit, yRangeUserRatio=[0,3], markerSize=1.1,lumi=11.379,lumiUnit="fb",energy=13,saveAs=outputFile)

