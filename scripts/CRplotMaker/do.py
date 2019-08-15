#! /usr/bin/env python
import sys
import ROOT
# run in batch mode to suppress plot windows
ROOT.gROOT.SetBatch(1)

from MT2PlotMaker import *
import MT2PlotDefs as pd

year = "All"

pd.lumiUnit = "fb"
if year==2016:
    pd.lumi = 35.9
if year==2017:
    pd.lumi = 41.5
if year==2018:
    pd.lumi = 59.8
if year=="All":
    pd.lumi = 137

# pd.cmsPreliminaryText = "CMS Preliminary"
pd.cmsPreliminaryText = "CMS"

input_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2Looper/output/V00-10-16_combined_newEGamma".format(year)
output_dir = "crplots/V00-10-16_combined_newEGamma".format(year)
mcScale = 1.0
dataname = "data_Run{0}".format(year)

exts = ["pdf","png"]

# # removed lepton
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dataname, "crrlbase", pd.rl_plots, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dataname, "crrlbaseJ", pd.ht_njet_plots, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dataname, "crrlbaseIncl", pd.ht_njet_plots, output_dir, exts, mcScale=mcScale)

# # single lepton
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dataname, "crslbase", pd.sl_plots, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dataname, "crslwjets", pd.sl_plots, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["top", "wjets_ht", "qcd_ht"], dataname, "crslttbar", pd.sl_plots, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dataname, "crslelbase", pd.sl_plots, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dataname, "crslmubase", pd.sl_plots, output_dir, exts, mcScale=mcScale)

# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dataname, "crslbaseJ", pd.basic_kinematic, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dataname, "crslbaseVL", pd.basic_kinematic, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dataname, "crslbaseL", pd.basic_kinematic, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dataname, "crslbaseM", pd.basic_kinematic, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dataname, "crslbaseH", pd.basic_kinematic, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], dataname, "crslbaseUH", pd.basic_kinematic, output_dir, exts, mcScale=mcScale)

# # photon+jets
# MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], dataname, "crgjbase", pd.gj_plots, output_dir, exts, mcScale=mcScale)

# MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], dataname, "crgjbaseJ", pd.basic_kinematic, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], dataname, "crgjbaseIncl", pd.gamma_vars_only, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], dataname, "crgjbaseIncl", pd.ht_njet_plots, output_dir, exts, mcScale=mcScale)

# MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], dataname, "crgjbaseVL", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], dataname, "crgjbaseL", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], dataname, "crgjbaseM", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], dataname, "crgjbaseH", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], dataname, "crgjbaseUH", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts, mcScale=mcScale)

# # z->ll
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], dataname, "crdybase", pd.dy_plots, output_dir, exts, mcScale=mcScale)

# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], dataname, "crdybaseJ", pd.ht_njet_plots, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], dataname, "crdybaseIncl", pd.ht_njet_plots, output_dir, exts, mcScale=mcScale)

# MT2PlotMaker(input_dir, ["dyjetsll_ht", "ww", "top"], dataname, "crdybaseInclLowPtOF", pd.ht_njet_plots, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "ww", "top"], dataname, "crdybaseInclLowPtSF", pd.ht_njet_plots, output_dir, exts, mcScale=mcScale)

# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], dataname, "crdybaseJ", pd.dy_plots, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], dataname, "crdybaseVL", pd.basic_kinematic, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], dataname, "crdybaseL", pd.basic_kinematic, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], dataname, "crdybaseM", pd.basic_kinematic, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], dataname, "crdybaseH", pd.basic_kinematic, output_dir, exts, mcScale=mcScale)
# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top", "wjets_ht"], dataname, "crdybaseUH", pd.basic_kinematic, output_dir, exts, mcScale=mcScale)

# MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], dataname, "crdybaseIncl+crdybaseInclLowPtSF", [pd.dy_plots[7]], output_dir, exts, mcScale=mcScale)

# # QCD monojet
# MT2PlotMaker(input_dir, ["qcd_ht", "wjets_ht", "zinv_ht"], dataname, "crqcdbaseJ", pd.qcdJ_plots, output_dir, exts)

# # QCD multijet
# MT2PlotMaker(input_dir, ["qcd_ht", "wjets_ht", "top", "zinv_ht"], dataname, "crqcdbase", pd.qcd_plots, output_dir, exts, mcScale=mcScale)
# # MT2PlotMaker(input_dir, ["qcd_ht", "wjets_ht", "top", "zinv_ht"], dnames, "crqcdMT2Sideband", pd.qcd_plots, output_dir, exts, dataNames=dlegs, dataScales=dscales, mcScale=mcScale)


 
# # Lost Lepton MT2 plots, normalized in each HT, nj, nb bin
# makeNormalizedLostLep(input_dir, ["wjets_ht", "top"], dataname, output_dir, exts, ht_regs=["VL","L","M","H","UH"])

# # Lost Lepton Hybrid MT2 plots
makeLostLepHybrid(input_dir, ["lostlepFromCRs"], dataname, output_dir, exts, ht_regs=["VL","L","M","H","UH"])
# # makeLostLepHybrid(input_dir, ["lostlepFromCRs"], dataname, output_dir, exts, ht_regs=["L","M","H","UH"])
# # makeLostLepHybrid(input_dir, ["lostlepFromCRs"], dataname, output_dir, exts, ht_regs=["M","H","UH"])
# # makeLostLepHybrid(input_dir, ["lostlepFromCRs"], dataname, output_dir, exts, ht_regs=["H","UH"])
makeLostLepHybrid(input_dir, ["lostlepFromCRs"], dataname, output_dir, exts, ht_regs=["VL"])
makeLostLepHybrid(input_dir, ["lostlepFromCRs"], dataname, output_dir, exts, ht_regs=["L"])
makeLostLepHybrid(input_dir, ["lostlepFromCRs"], dataname, output_dir, exts, ht_regs=["M"])
makeLostLepHybrid(input_dir, ["lostlepFromCRs"], dataname, output_dir, exts, ht_regs=["H"])
makeLostLepHybrid(input_dir, ["lostlepFromCRs"], dataname, output_dir, exts, ht_regs=["UH"])



# # example of using no data
# MT2PlotMaker(input_dir, ["top","wjets_ht","qcd_ht"], None, "srbaseM", pd.mt2_only, output_dir, exts)


