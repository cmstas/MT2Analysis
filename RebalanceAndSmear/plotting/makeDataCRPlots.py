import ROOT
import os
import pyRootPlotMaker as ppm
import numpy as np
import random
from common import *

tag = "V00-10-09_ptBinned_XXX_JetID_PUID_BTagSFs_core2sigma"
# tag = "V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_BTagSFs_noJERsmear"
# tag = "V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_ecalDeadCell_noJERsmear"
# tag = "V00-10-01_31Mar2018_usedByJason"
RSfromMC = False
doByTopoReg = False

data_year = "All"
data_year_txt = "2016-18"
lumi = 136.3

# data_year = 2018
# data_year_txt = "2018"
# lumi = 58.9

# data_year = 2017
# data_year_txt = "2017"
# lumi = 41.5

# data_year = 2016
# data_year_txt = "2016"
# lumi = 35.9


def GetTotalHistogram(h, fid, h_name, cr, ht_regs, isDataRS=False):
  # h is an already made TH1D with the correct binning
  h.Reset()
  for ht_reg in ht_regs:
    if isDataRS:
      if ht_reg=="VL":  scale = 1
      if ht_reg=="L":  scale = 1#1.0/4  * 1.02
      if ht_reg=="M":  scale = 1#1.0/4  * 1.17
      if ht_reg=="H":  scale = 1#1.0/2  * 1.11
      if ht_reg=="UH": scale = 1#1.0/2  * 1.07
    else:
      scale = 1.0
    top_regs = topo_reg_defs[ht_reg]
    for top_reg in top_regs:
      # print cr, top_reg, ht_reg, h_name
      try:
        h.Add(fid.Get("{0}{1}{2}/h_{3}".format(cr, top_reg, ht_reg, h_name)), scale)
      except:
        pass

ROOT.gROOT.SetBatch(1)

indir_MC = "../SmearLooper/output/V00-10-09_94x_2017_noRS"
indir_RS = "looper_output/{0}/{1}/".format(tag, "qcd" if RSfromMC else "data{0}".format(data_year))
indir_data = "../SmearLooper/output/V00-10-09_94x_2017_noRS/"

username = os.environ["USER"]
# tag = "nocut_aht500_10pctCorr"
outdir = "/home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/cr_kinematics_fromData{2}".format(username,tag,data_year)
if RSfromMC:
  outdir = "/home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/cr_kinematics_fromMC".format(username,tag)

data_name = "merged_hists"
data_noRS_name = "data_Run{0}".format(data_year)

samples = ["top","wjets_ht","zinv_ht"]

rs_file = ROOT.TFile(os.path.join(indir_RS,data_name+".root"))
data_file = ROOT.TFile(os.path.join(indir_data,data_noRS_name+".root"))
#data_file = ROOT.TFile(os.path.join(indir_data,"merged_hists_skims_noPFoverCalo"+".root"))
#data_file = ROOT.TFile(os.path.join(indir_data,"merged_hists_JetHT"+".root"))
#data_file = ROOT.TFile(os.path.join(indir_data,"merged_hists_HTMHT"+".root"))
#data_file = ROOT.TFile(os.path.join(indir_data,"merged_hists_MET"+".root"))
mc_files = [ROOT.TFile(os.path.join(indir_MC,s+".root")) for s in samples]
    
vars = ["ht", "met", "mt2", "nJet30", "nBJet20","J0pt","J1pt"]
rebin = [2, 2, 2, 1, 1,2,2]

xRangeUser = [(1000,3000), (0,1000), (200,500), None, None, None, None]
units = ["GeV","GeV","GeV",None,None,"GeV","GeV"]
titles = ["H_{T}","MET","M_{T2}","N-jet","N-bjet","jet1 pt", "jet2 pt"]

# dirs = [("crRSInvertDPhi", ["VL","L","M"]), ("crRSInvertDPhi", ["H","UH"]), ("crRSMT2SideBand", ["VL","L","M"]), ("crRSMT2SideBand", ["H","UH"])]
# if doByTopoReg:
#   dirs = [("crRSInvertDPhi", ["VL"]), ("crRSInvertDPhi", ["L"]), ("crRSInvertDPhi", ["M"]), ("crRSInvertDPhi", ["H"]), ("crRSInvertDPhi", ["UH"]),
#           ("crRSMT2SideBand", ["VL"]), ("crRSMT2SideBand", ["L"]), ("crRSMT2SideBand", ["M"]), ("crRSMT2SideBand", ["H"]), ("crRSMT2SideBand", ["UH"])]

dirs = [("crRSInvertDPhi", ["L","M"]), ("crRSInvertDPhi", ["H","UH"]), ("crRSMT2SideBand", ["L","M"]), ("crRSMT2SideBand", ["H","UH"]), ("crRSDPhiMT2", ["L","M"]), ("crRSDPhiMT2", ["H","UH"])]
if doByTopoReg:
  dirs = [("crRSInvertDPhi", ["VL"]), ("crRSInvertDPhi", ["L"]), ("crRSInvertDPhi", ["M"]), ("crRSInvertDPhi", ["H"]), ("crRSInvertDPhi", ["UH"]),
          ("crRSMT2SideBand", ["VL"]), ("crRSMT2SideBand", ["L"]), ("crRSMT2SideBand", ["M"]), ("crRSMT2SideBand", ["H"]), ("crRSMT2SideBand", ["UH"])]

# dirs = [("crRSInvertDPhi", ["VL","L","M"]), ("crRSInvertDPhi", ["H","UH"])]
# if doByTopoReg:
#   dirs = [("crRSInvertDPhi", ["VL"]), ("crRSInvertDPhi", ["L"]), ("crRSInvertDPhi", ["M"]), ("crRSInvertDPhi", ["H"]), ("crRSInvertDPhi", ["UH"])]

# dirs = [("crRSMT2SideBand", ["H","UH"])]
# if doByTopoReg:
#   dirs = [("crRSMT2SideBand", ["H"]), ("crRSMT2SideBand", ["UH"])]

# dirs = [("crRSMT2SideBand", ["L","M"]), ("crRSMT2SideBand", ["H","UH"])]
# if doByTopoReg:
#   dirs = [("crRSMT2SideBand", ["L"]), ("crRSMT2SideBand", ["M"])]

# if doByTopoReg:
#   dirs = [("crRSMT2SideBand", ["VL"])]
  

for cr, ht_regs in dirs:
  ht_name = ""
  if ht_regs==["L","M"]:
    ht_name="InclusiveHT450to1200"
  if ht_regs==["VL","L","M"]:
    ht_name="InclusiveHT250to1200"
  if ht_regs==["H","UH"]:
    ht_name="InclusiveHT1200toInf"
  if ht_regs==["VL"]:
    ht_name="InclusiveHT250to450"    
  if ht_regs==["L"]:
    ht_name="InclusiveHT450to575"
  if ht_regs==["M"]:
    ht_name="InclusiveHT575to1200"
  if ht_regs==["H"]:
    ht_name="InclusiveHT1200to1500"
  if ht_regs==["UH"]:
    ht_name="InclusiveHT1500toInf"

  try:
    os.makedirs(os.path.join(outdir,cr+ht_name))
    os.system("cp ~/scripts/index.php "+os.path.join(outdir,cr+ht_name))
  except:
    pass
  if "InvertDPhi" in cr:
    xRangeUser[2] = (200,500)
  if "MT2SideBand" in cr:
    xRangeUser[2] = (100,200)
  if "HT250to1200" in ht_name:
    xRangeUser[0] = (250,1200)
  if "HT450to1200" in ht_name:
    xRangeUser[0] = (450,1200)
  if "HT1200toInf" in ht_name:
    xRangeUser[0] = (1200,3000)
  if "HT250to450" in ht_name:
    xRangeUser[0] = (200,500)
  if "HT450to575" in ht_name:
    xRangeUser[0] = (400,600)
  if "HT575to1200" in ht_name:
    xRangeUser[0] = (550,1100)
  if "HT1200to1500" in ht_name:
    xRangeUser[0] = (900,1600)
  if "HT1500toInf" in ht_name:
    xRangeUser[0] = (1500,3000)
    
  for ivar,var in enumerate(vars):
    h_data = data_file.Get("srbase/h_{0}".format(var)).Clone()
    h_data.Reset()
    GetTotalHistogram(h_data, data_file, var, cr, ht_regs, False)
    h_mc_vec = [f.Get("srbase/h_{0}".format(var)).Clone() for f in mc_files]
    for h,f in zip(h_mc_vec, mc_files):
      GetTotalHistogram(h, f, var, cr, ht_regs, False)
      h.Scale(lumi)
      h.Rebin(rebin[ivar])
    h_RS_temp = rs_file.Get("srbase/h_{0}".format(var)).Clone()
    GetTotalHistogram(h_RS_temp, rs_file, var, cr, ht_regs, True)
    if RSfromMC:
      h_RS_temp.Scale(lumi)

    h_RS = h_RS_temp.Clone()
    # for i in range(1,151):
    #   h_RS.SetBinContent(i, h_RS_temp.GetBinContent(i))
    #   h_RS.SetBinError(i, h_RS_temp.GetBinError(i))

    # h_RS.Scale(lumi)
    h_data.Rebin(rebin[ivar])
    h_RS.Rebin(rebin[ivar])

    if var=="nJet30" or var=="nBJet20":
      if var=="nJet30":
        bins = np.array([0.,2.,4.,7.,10.,15.]) if "H" in ht_regs or "UH" in ht_regs else np.array([0.,2.,4.,7.,15.])
      else:
        bins = np.array([0.,1.,2.,3.,6.])
      h_data = h_data.Rebin(bins.size-1, str(random.random()), bins)
      h_RS = h_RS.Rebin(bins.size-1, str(random.random()), bins)
      for i,h in enumerate(h_mc_vec):
        h_mc_vec[i] = h.Rebin(bins.size-1, str(random.random()), bins)            

    tot_ewk = sum(h.Integral(0,-1) for h in h_mc_vec)
    pctdiff = (h_RS.Integral(0,-1) / (h_data.Integral(0,-1) - tot_ewk) - 1) * 100
    # subLegText = "Overpred: {0:.1f}%".format(pctdiff)
    subLegText = None
        
    saveAs = outdir+"/{0}/{1}".format(cr+ht_name,var)
    exts = [".pdf",".png"]
    for ext in exts:
      ppm.plotDataMC([h_RS]+h_mc_vec[::-1], ["RS QCD Pred", "Z(#nu#nu)+Jets", "W+Jets", "Top"], h_data=h_data,
                     saveAs=saveAs+ext, lumi=lumi, extraEnergyText=str(data_year_txt) if not RSfromMC else None,
                     xAxisTitle=titles[ivar], doMT2Colors=True, xRangeUser=xRangeUser[ivar],
                     doOverflow=False, xAxisUnit=units[ivar], subLegText=subLegText,
                     legCoords=(0.60,0.63,0.87,0.89), cmsTextSize=0.045)



