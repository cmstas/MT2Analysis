#! /usr/bin/env python
import sys
import ROOT
# run in batch mode to suppress plot windows
ROOT.gROOT.SetBatch(1)

from MT2PlotMaker import *
import MT2PlotDefs as pd

pd.lumi = 19.3 
pd.lumiUnit = "fb"
input_dir = "/home/users/jguiang/projects/mt2/MT2Analysis/MT2looper/output/V00-10-01_2017_31Mar2018_correctJECs/"
data1_name = "data_Run2018"
data2_name = "data_Run2017_scaled"
data3_name = "data_Run2016_scaled"
data1_leg = "Data 2018"
data2_leg = "Data 2017 (scaled)"
data3_leg = "Data 2016 (scaled)"
output_dir = "crplots/V00-10-01_2018_16p59fb_2017C_JECs"

# pd.lumi = 42.0
# pd.lumiUnit = "fb"
# input_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2looper/output/V00-09-04_41p96fb"
# data1_name = "data_Run2017"
# data2_name = "data_Run2016_scaled"
# data1_leg = "Data 2017 (prompt)"
# data2_leg = "Data 2016 (scaled)"
# output_dir = "crplots/V00-09-04_41p96fb_v3"

exts = ["pdf","png"]

# removed lepton
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], [data1_name, data2_name, data3_name], "crrlbase", pd.rl_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], [data1_name, data2_name, data3_name], "crrlbaseJ", pd.ht_njet_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], [data1_name, data2_name, data3_name], "crrlbaseIncl", pd.ht_njet_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])

# single lepton
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], [data1_name, data2_name, data3_name], "crslbase", pd.sl_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], [data1_name, data2_name, data3_name], "crslwjets", pd.sl_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["top", "wjets_ht", "qcd_ht"], [data1_name, data2_name, data3_name], "crslttbar", pd.sl_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], [data1_name, data2_name, data3_name], "crslelbase", pd.sl_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], [data1_name, data2_name, data3_name], "crslmubase", pd.sl_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])

MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], [data1_name, data2_name, data3_name], "crslbaseJ", pd.basic_kinematic, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], [data1_name, data2_name, data3_name], "crslbaseVL", pd.basic_kinematic, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], [data1_name, data2_name, data3_name], "crslbaseL", pd.basic_kinematic, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], [data1_name, data2_name, data3_name], "crslbaseM", pd.basic_kinematic, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], [data1_name, data2_name, data3_name], "crslbaseH", pd.basic_kinematic, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], [data1_name, data2_name, data3_name], "crslbaseUH", pd.basic_kinematic, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])

# photon+jets
MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], [data1_name, data2_name, data3_name], "crgjbase", pd.gj_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])

MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], [data1_name, data2_name, data3_name], "crgjbaseJ", pd.basic_kinematic, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], [data1_name, data2_name, data3_name], "crgjbaseIncl", pd.gamma_vars_only, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], [data1_name, data2_name, data3_name], "crgjbaseIncl", pd.ht_njet_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])

MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], [data1_name, data2_name, data3_name], "crgjbaseVL", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], [data1_name, data2_name, data3_name], "crgjbaseL", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], [data1_name, data2_name, data3_name], "crgjbaseM", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], [data1_name, data2_name, data3_name], "crgjbaseH", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["gjets_dr0p05_ht", "fragphoton", "fakephoton"], [data1_name, data2_name, data3_name], "crgjbaseUH", pd.basic_kinematic+pd.gamma_vars_only, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])

# z->ll 
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], [data1_name, data2_name, data3_name], "crdybase", pd.dy_plots+pd.dy_OF_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])

MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], [data1_name, data2_name, data3_name], "crdybaseJ", pd.ht_njet_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], [data1_name, data2_name, data3_name], "crdybaseIncl", pd.ht_njet_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])

MT2PlotMaker(input_dir, ["dyjetsll_ht", "ww", "top"], [data1_name, data2_name, data3_name], "crdybaseInclLowPtOF", pd.dy_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["dyjetsll_ht", "ww", "top"], [data1_name, data2_name, data3_name], "crdybaseInclLowPtSF", pd.dy_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])

MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], [data1_name, data2_name, data3_name], "crdybaseJ", pd.basic_kinematic, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], [data1_name, data2_name, data3_name], "crdybaseVL", pd.basic_kinematic, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], [data1_name, data2_name, data3_name], "crdybaseL", pd.basic_kinematic, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], [data1_name, data2_name, data3_name], "crdybaseM", pd.basic_kinematic, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], [data1_name, data2_name, data3_name], "crdybaseH", pd.basic_kinematic, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])
MT2PlotMaker(input_dir, ["dyjetsll_ht", "top"], [data1_name, data2_name, data3_name], "crdybaseUH", pd.basic_kinematic, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])

# QCD monojet
MT2PlotMaker(input_dir, ["qcd_ht", "wjets_ht", "zinv_ht"], [data1_name, data2_name, data3_name], "crqcdbaseJ", pd.qcdJ_plots, output_dir, exts, dataNames=[data1_leg,data2_leg, data3_leg])



