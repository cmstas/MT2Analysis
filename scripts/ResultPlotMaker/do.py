#! /usr/bin/env python
import ROOT
from ResultPlotMaker import *
import ResultPlotUtils as utils

datacard_dir = "../CardMaker/cards_FullRunII_17MCfor18_ttbbWeights_v2"

utils.LoadPickledDatacards(os.path.join(datacard_dir,"templates","template_datacards.pkl"))

outdir = "/home/users/bemarsh/public_html/mt2/result_plots/2019_FSR/V00-10-10_FullRunII_17MCfor18_ttbbWeights_v2"

# utils.lumi = 58.83
# utils.lumi = 35.9
utils.lumi = 137.4

# scalePred = 15.0 / 136.3
# scalePred = (35.9+10+15) / 136.3
scalePred = 1.0

MakePlot("HT250to450", outdir, userMax=5e7, drawObs=True, scalePred=scalePred)
MakePlot("HT450to575", outdir, userMax=7e6, drawObs=True, scalePred=scalePred)
MakePlot("HT575to1200", outdir, userMax=7e6, drawObs=True, scalePred=scalePred, ratioRange=(0,3))
MakePlot("HT1200to1500", outdir, userMax=7e6, drawObs=True, scalePred=scalePred, ratioRange=(0,4))
MakePlot("HT1500toInf", outdir, userMax=7e5, drawObs=True, scalePred=scalePred, ratioRange=(0,4))
# monojet is handled specially, just use "monojet" as the ht region name
MakePlot("monojet", outdir, userMax=7e7, drawObs=True, scalePred=scalePred)

MakePlot("HT250to450", outdir, userMax=5e7, drawObs=True, scalePred=scalePred, doPull=True)
MakePlot("HT450to575", outdir, userMax=7e6, drawObs=True, scalePred=scalePred, doPull=True)
MakePlot("HT575to1200", outdir, userMax=7e6, drawObs=True, scalePred=scalePred, doPull=True, ratioRange=(-4,4))
MakePlot("HT1200to1500", outdir, userMax=7e6, drawObs=True, scalePred=scalePred, doPull=True, ratioRange=(-4,4))
MakePlot("HT1500toInf", outdir, userMax=7e5, drawObs=True, scalePred=scalePred, doPull=True, ratioRange=(-5,5))
MakePlot("monojet", outdir, userMax=7e7, drawObs=True, scalePred=scalePred, doPull=True)

# MakeHTBinPlot(datacard_dir, datacard_name, outdir, userMax=1e8, scalePred=scalePred, ratioRange=(0.5,1.5))

# MakeInclusivePlot(datacard_dir, datacard_name, outdir, userMax=1e8, ratioRange=(0,2))

# datacard_dir_eth = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/scripts/cards_test_eth"
# datacard_name_eth = "datacard_{0}_{1}_{2}.txt"
# MakeComparison("HT200to450", datacard_dir, datacard_dir_eth, datacard_name, datacard_name_eth, outdir, iBkgd=1, userMax=7e5)
# MakeComparison("HT200to450", datacard_dir, datacard_dir_eth, datacard_name, datacard_name_eth, outdir, iBkgd=2, userMax=7e5)
# MakeComparison("HT575to1000", datacard_dir, datacard_dir_eth, datacard_name, datacard_name_eth, outdir, iBkgd=1, userMax=7e5)
# MakeComparison("HT575to1000", datacard_dir, datacard_dir_eth, datacard_name, datacard_name_eth, outdir, iBkgd=2, userMax=7e5)

#datacard_name_topo = "datacard_{0}_{1}_ttw_mg_lo.txt"
#MakeMacroRegionPlot(1,datacard_dir, datacard_name_topo, outdir, userMax=5e6)
#MakeMacroRegionPlot(2,datacard_dir, datacard_name_topo, outdir, userMax=5e6)
#MakeMacroRegionPlot(3,datacard_dir, datacard_name_topo, outdir, userMax=5e6)
#MakeMacroRegionPlot(4,datacard_dir, datacard_name_topo, outdir, userMax=5e6)
#MakeMacroRegionPlot(5,datacard_dir, datacard_name_topo, outdir, userMax=5e6)
#MakeMacroRegionPlot(6,datacard_dir, datacard_name_topo, outdir, userMax=5e6)
#MakeMacroRegionPlot(7,datacard_dir, datacard_name_topo, outdir, userMax=5e6)
#MakeMacroRegionPlot(8,datacard_dir, datacard_name_topo, outdir, userMax=5e6)
