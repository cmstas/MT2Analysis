import ROOT
import os
import pyRootPlotMaker as ppm
import numpy as np

username = os.environ["USER"]

ROOT.gROOT.SetBatch(1)

# tag = "V00-10-01_31Mar2018_usedByJason"
# tag = "V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_RecoBJet_noJERsmear_jetReweight"
# tag = "V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_BTagSFs_noJERsmear_jetReweight"
# tag = "V00-10-04_fullBinning_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma_newJER"
tag = "V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_noJERsmear"

dir = "looper_output/{0}/qcd".format(tag)
# dir_noRS = "../SmearLooper/output/V00-09-06_noRS/"
# dir_noRS = "../SmearLooper/output/V00-10-01_noRS/"
dir_noRS = "../SmearLooper/output/V00-10-04_94x_2017_noRS_fullBinning/"
# dir_noRS = "../SmearLooper/output/test/"

inputs = ['merged_hists.root']

for inp in inputs:

  frse = ROOT.TFile(os.path.join(dir,inp))
  fnrse = ROOT.TFile(os.path.join(dir_noRS,"qcd_ht.root"))

  os.system("mkdir -p /home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/{2}".format(username, tag, inp.replace('.root','')))
  os.system("cp ~/scripts/index.php /home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/{2}".format(username, tag, inp.replace('.root','')))
  
  vars = ["met","ht","mt2","nJet30","nBJet20","J0pt","J1pt","deltaPhiMin","diffMetMhtOverMet"]
  names = ["E_{T}^{miss}","H_{T}","M_{T2}","nJet30","nBJet20","p_{T}(jet1)","p_{T}(jet2)","#Delta#phi_{min}","|met - mht|/met"]
  units = ["GeV","GeV","GeV",None,None,"GeV","GeV",None,None]
  rebin = [2,2,2,1,1,2,2,1,2]
  
  for tdir in ["ht450to1200","ht1200toInf","ht250to1200"]:
#  for tdir in ["ht450to1200","ht1200toInf"]:    

    os.system("mkdir -p /home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/{2}/{3}".format(username, tag, inp.replace('.root',''),tdir))
    os.system("cp ~/scripts/index.php /home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/{2}/{3}/".format(username, tag, inp.replace('.root',''),tdir))
    
    for ivar,var in enumerate(vars):
      
      hrs = frse.Get("{0}/h_{1}".format(tdir, var))
      hnrs = fnrse.Get("{0}/h_{1}".format(tdir, var))
  
      hrs.Rebin(rebin[ivar])
      hnrs.Rebin(rebin[ivar])
  
      for ext in [".pdf",".png",".root"]:
          saveAs = "/home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/{5}/{2}/{2}_{3}{4}".format(username, tag, tdir,var,ext,inp.replace('.root',''))
          ppm.plotDataMC([hnrs], ["QCD MC"], h_data=hrs, dataTitle="R&S from MC", saveAs=saveAs, 
                         xAxisTitle=names[ivar], xAxisUnit=units[ivar])
      
      if "nJet30" in var or "nBJet20" in var:
        newbins = np.array([0.,2.,4.,7.,10.,15.]) if var=="nJet30" else np.array([0.,1.,2.,3.,4.,6.])
        tmp_hrs = hrs.Rebin(5,"tmp_hrs",newbins)
        tmp_hnrs = hnrs.Rebin(5,"tmp_hnrs",newbins)
      
        for ext in [".pdf",".png",".root"]:
          saveAs = "/home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/{5}/{2}/{2}_{3}{4}".format(username, tag, tdir,var+"_rebin",ext,inp.replace('.root',''))
          ppm.plotDataMC([tmp_hnrs], ["QCD MC"], h_data=tmp_hrs, dataTitle="R&S from MC", saveAs=saveAs, 
                         xAxisTitle=names[ivar], xAxisUnit=units[ivar])
