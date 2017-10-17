#! /usr/bin/env python
import ROOT
import os
from ResultPlotMaker import MakePlot, MakeComparison, MakeMacroRegionPlot, MakeInclusivePlot
import ResultPlotUtils as utils

utils.lumi = 36.5

point = "1_1200_400"

datacard_dir = "/home/users/bemarsh/analysis/mt2/2016_LLgluino/MT2Analysis/scripts/cards_LLgluino/ctau-{0}".format(point)
#datacard_dir = "../cards_test"

# {0} is the HT region, {1} is the jet/bjet region, {2} is the mt2 bin
datacard_name = "datacard_{{0}}_{{1}}_{{2}}_T1qqqq_ctau-{0}.txt".format(point)
#datacard_name = "datacard_{0}_{1}_{2}_ttw_mg_lo.txt"

outdir = "~/public_html/mt2/plots/2016_results_test/T1qqqq_ctau-{0}".format(point)
try:
    os.system("mkdir -p "+outdir)
    os.system("cp ~/scripts/index.php "+outdir)
except:
    pass

drawSignal = True
sigName = "T1qqqq_{0}".format(point)

MakeInclusivePlot(datacard_dir, datacard_name, outdir, userMax=7e6, drawSignal=drawSignal, sigName=sigName)

MakePlot("HT250to450",datacard_dir, datacard_name, outdir, userMax=5e6, drawSignal=drawSignal, sigName=sigName)
MakePlot("HT450to575",datacard_dir, datacard_name, outdir, userMax=7e5, drawSignal=drawSignal, sigName=sigName)
MakePlot("HT575to1000",datacard_dir, datacard_name, outdir, userMax=7e5, drawSignal=drawSignal, sigName=sigName)
MakePlot("HT1000to1500",datacard_dir, datacard_name, outdir, userMax=7e5, drawSignal=drawSignal, sigName=sigName)
MakePlot("HT1500toInf",datacard_dir, datacard_name, outdir, userMax=7e5, ratioRange=(0,3), drawSignal=drawSignal, sigName=sigName)
# monojet is handled specially, just use "monojet" as the ht region name
MakePlot("monojet",datacard_dir, datacard_name, outdir, userMax=7e7, drawSignal=drawSignal, sigName=sigName)

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
