#! /usr/bin/env python
import sys
import ROOT
# run in batch mode to suppress plot windows
ROOT.gROOT.SetBatch(1)

from MT2PlotMaker import *
import MT2PlotDefs as pd


plot_defs = [
    ("nJet30_ge7j", True, None, None),
    ("nJet30_ge7j_0b", True, None, None),
    ("nBJet20_ge7j", True, None, None),
    ("nJet30_ge7j_ge1b", True, None, None),
    ("nBJet20_ge10j", True, None, None),
]

plot_defs_VLL = plot_defs[:4]

pd.lumiUnit = "fb"
mcScale = 1.0

# pd.lumi = 35.9
# input_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2Looper/output/V00-10-10_2016fullYear_v2"
# output_dir = "crplots/V00-10-10_NJNB_extrap_plots_2016"
# exts = ["pdf","png"]
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016", "crslbase", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016", "crslbaseVL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016", "crslbaseL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016", "crslbaseM", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016", "crslbaseH", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016", "crslbaseUH", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2016", "crdybase", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2016", "crdybaseVL", plot_defs_VLL[:3], output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2016", "crdybaseL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2016", "crdybaseM", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2016", "crdybaseH", plot_defs[:4], output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2016", "crdybaseUH", plot_defs, output_dir, exts, mcScale=mcScale)

# pd.lumi = 41.5
# input_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2Looper/output/V00-10-10_2017fullYear_withZNJetWeights"
# output_dir = "crplots/V00-10-10_NJNB_extrap_plots_2017_withZNJetWeights"
# exts = ["pdf","png"]
# # MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslbase", plot_defs, output_dir, exts, mcScale=mcScale)
# # MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslbaseVL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# # MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslbaseL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# # MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslbaseM", plot_defs, output_dir, exts, mcScale=mcScale)
# # MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslbaseH", plot_defs, output_dir, exts, mcScale=mcScale)
# # MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslbaseUH", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2017", "crdybase", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2017", "crdybaseVL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2017", "crdybaseL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2017", "crdybaseM", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2017", "crdybaseH", plot_defs[:4], output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2017", "crdybaseUH", plot_defs, output_dir, exts, mcScale=mcScale)

# pd.lumi = 58.8
# input_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2Looper/output/V00-10-10_2018fullYear_with2018MC"
# output_dir = "crplots/V00-10-10_NJNB_extrap_plots_2018_with2018MC"
# exts = ["pdf","png"]
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2018", "crslbase", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2018", "crslbaseVL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2018", "crslbaseL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2018", "crslbaseM", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2018", "crslbaseH", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2018", "crslbaseUH", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2018", "crdybase", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2018", "crdybaseVL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2018", "crdybaseL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2018", "crdybaseM", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2018", "crdybaseH", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_Run2018", "crdybaseUH", plot_defs, output_dir, exts, mcScale=mcScale)

pd.lumi = 137
input_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2Looper/output/V00-10-16_combined_newEGamma"
output_dir = "crplots/V00-10-16_NJNB_extrap_plots_combined"
exts = ["pdf","png"]
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_RunAll", "crslbase", plot_defs, output_dir, exts, mcScale=mcScale)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_RunAll", "crslbaseVL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_RunAll", "crslbaseL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_RunAll", "crslbaseM", plot_defs, output_dir, exts, mcScale=mcScale)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_RunAll", "crslbaseH", plot_defs, output_dir, exts, mcScale=mcScale)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_RunAll", "crslbaseUH", plot_defs, output_dir, exts, mcScale=mcScale)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_RunAll", "crdybase", plot_defs, output_dir, exts, mcScale=mcScale)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_RunAll", "crdybaseVL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_RunAll", "crdybaseL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_RunAll", "crdybaseM", plot_defs, output_dir, exts, mcScale=mcScale)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_RunAll", "crdybaseH", plot_defs, output_dir, exts, mcScale=mcScale)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], "data_RunAll", "crdybaseUH", plot_defs, output_dir, exts, mcScale=mcScale)

# pd.lumi = 136.3
# input_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2Looper/output/V00-10-10_withExtraB_combined"
# output_dir = "crplots/V00-10-10_NJNB_extrap_plots_combined_withExtraB_nonweighted"
# exts = ["pdf","png"]
# MT2PlotMaker(input_dir, ["wjets_ht", "top_nonweighted_lt2b", "top_nonweighted_ge2b", "qcd_ht"], "data_RunAll", "crslbase", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top_nonweighted_lt2b", "top_nonweighted_ge2b", "qcd_ht"], "data_RunAll", "crslbaseVL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top_nonweighted_lt2b", "top_nonweighted_ge2b", "qcd_ht"], "data_RunAll", "crslbaseL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top_nonweighted_lt2b", "top_nonweighted_ge2b", "qcd_ht"], "data_RunAll", "crslbaseM", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top_nonweighted_lt2b", "top_nonweighted_ge2b", "qcd_ht"], "data_RunAll", "crslbaseH", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top_nonweighted_lt2b", "top_nonweighted_ge2b", "qcd_ht"], "data_RunAll", "crslbaseUH", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top_nonweighted_lt2b", "top_nonweighted_ge2b", "wjets_ht"], "data_RunAll", "crdybase", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top_nonweighted_lt2b", "top_nonweighted_ge2b", "wjets_ht"], "data_RunAll", "crdybaseVL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top_nonweighted_lt2b", "top_nonweighted_ge2b", "wjets_ht"], "data_RunAll", "crdybaseL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top_nonweighted_lt2b", "top_nonweighted_ge2b", "wjets_ht"], "data_RunAll", "crdybaseM", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top_nonweighted_lt2b", "top_nonweighted_ge2b", "wjets_ht"], "data_RunAll", "crdybaseH", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top_nonweighted_lt2b", "top_nonweighted_ge2b", "wjets_ht"], "data_RunAll", "crdybaseUH", plot_defs, output_dir, exts, mcScale=mcScale)


# pd.lumi = 136.3
# input_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2Looper/output/V00-10-10_withExtraB_combined"
# output_dir = "crplots/V00-10-10_NJNB_extrap_plots_combined_withExtraB_weighted"
# exts = ["pdf","png"]
# MT2PlotMaker(input_dir, ["wjets_ht", "top_nonweighted_lt2b", "top_weighted_ge2b", "qcd_ht"], "data_RunAll", "crslbase", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top_nonweighted_lt2b", "top_weighted_ge2b", "qcd_ht"], "data_RunAll", "crslbaseVL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top_nonweighted_lt2b", "top_weighted_ge2b", "qcd_ht"], "data_RunAll", "crslbaseL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top_nonweighted_lt2b", "top_weighted_ge2b", "qcd_ht"], "data_RunAll", "crslbaseM", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top_nonweighted_lt2b", "top_weighted_ge2b", "qcd_ht"], "data_RunAll", "crslbaseH", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top_nonweighted_lt2b", "top_weighted_ge2b", "qcd_ht"], "data_RunAll", "crslbaseUH", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top_nonweighted_lt2b", "top_weighted_ge2b", "wjets_ht"], "data_RunAll", "crdybase", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top_nonweighted_lt2b", "top_weighted_ge2b", "wjets_ht"], "data_RunAll", "crdybaseVL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top_nonweighted_lt2b", "top_weighted_ge2b", "wjets_ht"], "data_RunAll", "crdybaseL", plot_defs_VLL, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top_nonweighted_lt2b", "top_weighted_ge2b", "wjets_ht"], "data_RunAll", "crdybaseM", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top_nonweighted_lt2b", "top_weighted_ge2b", "wjets_ht"], "data_RunAll", "crdybaseH", plot_defs, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top_nonweighted_lt2b", "top_weighted_ge2b", "wjets_ht"], "data_RunAll", "crdybaseUH", plot_defs, output_dir, exts, mcScale=mcScale)

