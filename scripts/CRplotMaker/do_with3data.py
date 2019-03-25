#! /usr/bin/env python
import sys
import ROOT
# run in batch mode to suppress plot windows
ROOT.gROOT.SetBatch(1)

from MT2PlotMaker import *
import MT2PlotDefs as pd

# pd.lumi = 41.4
# pd.lumiUnit = "fb"
# input_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2Looper/output/V00-10-02_testHcalOfflineFix"
# data1_name = "data_Run2017"
# data2_name = "data_Run2018"
# data3_name = "data2018_afterHcalFix"
# data1_leg = "Data 2017"
# data2_leg = "Data Run2018A/B"
# data3_leg = "Data Run2018C-v3"
# data1_scale = 1.0
# data2_scale = 41.4/19.3
# data3_scale = 41.4/2.84
# mcScale = 41.4/41.4
# output_dir = "crplots/2018_testAfterHcalFix"

pd.lumi = 19.3
pd.lumiUnit = "fb"
input_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2Looper/output/V00-10-04_94x_2016_2017"
# input_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2Looper/output/V00-10-04_80x_MC"
data1_name = "data_Run2018"
data2_name = "data_Run2017"
data3_name = "data_Run2016"
data1_leg = "Data 2018"
data2_leg = "Data 2017 (scaled)"
data3_leg = "Data 2016 (scaled)"
data1_scale = 1.0
data2_scale = 19.3/41.52
data3_scale = 19.3/35.92
mcScale = 19.3/35.9
output_dir = "crplots/V00-10-04_2016_2017_2018_94x_Fall17_MC"
# output_dir = "crplots/V00-10-04_2016_2017_2018_80x_MC"


exts = ["pdf","png"]

dnames = [data1_name, data2_name, data3_name]
dlegs = [data1_leg, data2_leg, data3_leg]
dscales = [data1_scale, data2_scale, data3_scale]


# removed lepton
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dnames, "crrlbase", pd.rl_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dnames, "crrlbaseJ", pd.ht_njet_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dnames, "crrlbaseIncl", pd.ht_njet_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)

# single lepton
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dnames, "crslbase", pd.sl_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dnames, "crslwjets", pd.sl_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["top", "wjets_ht", "qcd_ht"], dnames, "crslttbar", pd.sl_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dnames, "crslelbase", pd.sl_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dnames, "crslmubase", pd.sl_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)

MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dnames, "crslbaseJ", pd.basic_kinematic, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dnames, "crslbaseVL", pd.basic_kinematic, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dnames, "crslbaseL", pd.basic_kinematic, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dnames, "crslbaseM", pd.basic_kinematic, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dnames, "crslbaseH", pd.basic_kinematic, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dnames, "crslbaseUH", pd.basic_kinematic, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)

# photon+jets
MT2PlotMaker(input_dir, ["gjets_dr0p4_ht", "fragphoton", "fakephoton"], dnames, "crgjbase", pd.gj_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)

MT2PlotMaker(input_dir, ["gjets_dr0p4_ht", "fragphoton", "fakephoton"], dnames, "crgjbaseJ", pd.basic_kinematic, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["gjets_dr0p4_ht", "fragphoton", "fakephoton"], dnames, "crgjbaseIncl", pd.gamma_vars_only, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["gjets_dr0p4_ht", "fragphoton", "fakephoton"], dnames, "crgjbaseIncl", pd.ht_njet_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)

MT2PlotMaker(input_dir, ["gjets_dr0p4_ht", "fragphoton", "fakephoton"], dnames, "crgjbaseVL", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["gjets_dr0p4_ht", "fragphoton", "fakephoton"], dnames, "crgjbaseL", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["gjets_dr0p4_ht", "fragphoton", "fakephoton"], dnames, "crgjbaseM", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["gjets_dr0p4_ht", "fragphoton", "fakephoton"], dnames, "crgjbaseH", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["gjets_dr0p4_ht", "fragphoton", "fakephoton"], dnames, "crgjbaseUH", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)

# z->ll 
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], dnames, "crdybase", pd.dy_plots+pd.dy_OF_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)

MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], dnames, "crdybaseJ", pd.ht_njet_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], dnames, "crdybaseIncl", pd.ht_njet_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)

MT2PlotMaker(input_dir, ["dyjetsll_ht", "ww", "top"], dnames, "crdybaseInclLowPtOF", pd.dy_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "ww", "top"], dnames, "crdybaseInclLowPtSF", pd.dy_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)

MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], dnames, "crdybaseJ", pd.basic_kinematic, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], dnames, "crdybaseVL", pd.basic_kinematic, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], dnames, "crdybaseL", pd.basic_kinematic, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], dnames, "crdybaseM", pd.basic_kinematic, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], dnames, "crdybaseH", pd.basic_kinematic, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], dnames, "crdybaseUH", pd.basic_kinematic, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)

# QCD monojet
MT2PlotMaker(input_dir, ["qcd_ht", "wjets_ht", "zinv_ht"], dnames, "crqcdbaseJ", pd.qcdJ_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)

# # QCD multijet
# MT2PlotMaker(input_dir, ["qcd_ht", "wjets_ht", "top", "zinv_ht"], dnames, "crqcdbase", pd.qcd_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)
# # MT2PlotMaker(input_dir, ["qcd_ht", "wjets_ht", "top", "zinv_ht"], dnames, "crqcdMT2Sideband", pd.qcd_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)



