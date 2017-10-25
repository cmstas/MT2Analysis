#! /usr/bin/env python
import sys
import ROOT
# run in batch mode to suppress plot windows
ROOT.gROOT.SetBatch(1)

from MT2PlotMaker import *
import MT2PlotDefs as pd

pd.lumi = 21.15
pd.lumiUnit = "fb"

input_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2looper/output/V00-09-02_21p15fb"

output_dir = "crplots/V00-09-02_21p15fb"

exts = ["pdf","png"]

# removed lepton
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crrlbase", pd.rl_plots, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crrlbaseJ", pd.ht_njet_plots, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crrlbaseIncl", pd.ht_njet_plots, output_dir, exts)

# single lepton
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslbase", pd.sl_plots, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslwjets", pd.sl_plots, output_dir, exts)
MT2PlotMaker(input_dir, ["top", "wjets_ht", "qcd_ht"], "data_Run2017", "crslttbar", pd.sl_plots, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslelbase", pd.sl_plots, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslmubase", pd.sl_plots, output_dir, exts)

MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslbaseJ", pd.basic_kinematic, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslbaseVL", pd.basic_kinematic, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslbaseL", pd.basic_kinematic, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslbaseM", pd.basic_kinematic, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslbaseH", pd.basic_kinematic, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2017", "crslbaseUH", pd.basic_kinematic, output_dir, exts)

# photon+jets
MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], "data_Run2017", "crgjbase", pd.gj_plots, output_dir, exts)

MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], "data_Run2017", "crgjbaseJ", pd.basic_kinematic, output_dir, exts)
MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], "data_Run2017", "crgjbaseIncl", pd.gamma_vars_only, output_dir, exts)
MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], "data_Run2017", "crgjbaseIncl", pd.ht_njet_plots, output_dir, exts)

MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], "data_Run2017", "crgjbaseVL", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts)
MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], "data_Run2017", "crgjbaseL", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts)
MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], "data_Run2017", "crgjbaseM", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts)
MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], "data_Run2017", "crgjbaseH", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts)
MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], "data_Run2017", "crgjbaseUH", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts)

# z->ll
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], "data_Run2017", "crdybase", pd.dy_plots, output_dir, exts)

MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], "data_Run2017", "crdybaseJ", pd.ht_njet_plots, output_dir, exts)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], "data_Run2017", "crdybaseIncl", pd.ht_njet_plots, output_dir, exts)

MT2PlotMaker(input_dir, ["dyjetsll_ht", "ww", "top"], "data_Run2017", "crdybaseInclLowPtOF", pd.ht_njet_plots, output_dir, exts)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "ww", "top"], "data_Run2017", "crdybaseInclLowPtSF", pd.ht_njet_plots, output_dir, exts)

MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], "data_Run2017", "crdybaseJ", pd.basic_kinematic, output_dir, exts)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], "data_Run2017", "crdybaseVL", pd.basic_kinematic, output_dir, exts)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], "data_Run2017", "crdybaseL", pd.basic_kinematic, output_dir, exts)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], "data_Run2017", "crdybaseM", pd.basic_kinematic, output_dir, exts)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], "data_Run2017", "crdybaseH", pd.basic_kinematic, output_dir, exts)
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], "data_Run2017", "crdybaseUH", pd.basic_kinematic, output_dir, exts)

# # Lost Lepton MT2 plots, normalized in each HT, nj, nb bin
# makeNormalizedLostLep(input_dir, ["wjets_ht", "top"], "data_Run2017", output_dir, exts, ht_regs=["VL","L","M","H","UH"])


# # Lost Lepton Hybrid MT2 plots
makeLostLepHybrid(input_dir, ["lostlepFromCRs"], "data_Run2017", output_dir, exts, ht_regs=["VL","L","M","H","UH"])
# # makeLostLepHybrid(input_dir, ["lostlepFromCRs"], "data_Run2017", output_dir, exts, ht_regs=["L","M","H","UH"])
# # makeLostLepHybrid(input_dir, ["lostlepFromCRs"], "data_Run2017", output_dir, exts, ht_regs=["M","H","UH"])
# # makeLostLepHybrid(input_dir, ["lostlepFromCRs"], "data_Run2017", output_dir, exts, ht_regs=["H","UH"])
makeLostLepHybrid(input_dir, ["lostlepFromCRs"], "data_Run2017", output_dir, exts, ht_regs=["VL"])
makeLostLepHybrid(input_dir, ["lostlepFromCRs"], "data_Run2017", output_dir, exts, ht_regs=["L"])
makeLostLepHybrid(input_dir, ["lostlepFromCRs"], "data_Run2017", output_dir, exts, ht_regs=["M"])
makeLostLepHybrid(input_dir, ["lostlepFromCRs"], "data_Run2017", output_dir, exts, ht_regs=["H"])
makeLostLepHybrid(input_dir, ["lostlepFromCRs"], "data_Run2017", output_dir, exts, ht_regs=["UH"])

# QCD monojet
MT2PlotMaker(input_dir, ["qcd_ht", "wjets_ht", "zinv_ht"], "data_Run2017", "crqcdbaseJ", pd.qcdJ_plots, output_dir, exts)



# # example of using no data
# MT2PlotMaker(input_dir, ["top","wjets_ht","qcd_ht"], None, "srbaseM", pd.mt2_only, output_dir, exts)


