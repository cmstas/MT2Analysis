#! /usr/bin/env python
import sys
# run in batch mode to suppress plot windows
if '-b' not in sys.argv:
    sys.argv.append('-b')

from MT2PlotMaker import MT2PlotMaker
import MT2PlotDefs as pd

input_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2looper/output/V00-08-01_json_Cert_271036-274240_skim_base_mt2gt200_ZinvV5"
#input_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2looper/output/V00-08-00_json_Cert_271036-273730_skim_base_mt2gt200_ZinvV4"
output_dir = "crplots/crplots804"

exts = ["pdf","png"]

# removed lepton
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016B", "crrlbase", pd.rl_plots, output_dir, exts)

# single lepton
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016B", "crslbase", pd.sl_plots, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016B", "crslwjets", pd.sl_plots, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016B", "crslttbar", pd.sl_plots, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016B", "crslelbase", pd.sl_plots, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016B", "crslmubase", pd.sl_plots, output_dir, exts)

MT2PlotMaker(input_dir, ["wjets_ht", "top"], "data_Run2016B", "crslbaseJ", pd.mt2bins_only, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016B", "crslbaseVL", pd.mt2bins_only, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016B", "crslbaseL", pd.mt2bins_only, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016B", "crslbaseM", pd.mt2bins_only, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016B", "crslbaseH", pd.mt2bins_only, output_dir, exts)
MT2PlotMaker(input_dir, ["wjets_ht", "top", "qcd_ht"], "data_Run2016B", "crslbaseUH", pd.mt2bins_only, output_dir, exts)

# photon+jets
MT2PlotMaker(input_dir, ["gjets_ht", "fragphoton", "fakephoton"], "data_Run2016B", "crgjbase", pd.gj_plots, output_dir, exts)

# z->ll
MT2PlotMaker(input_dir, ["dyjetsll_incl", "top"], "data_Run2016B", "crdybase", pd.dy_plots,output_dir, exts)