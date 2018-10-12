import numpy as np
import ROOT as r

tag = "V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_BTagSFs_noJERsmear"
# tag = "V00-10-01_31Mar2018_2016JRTs"

frs  = r.TFile("../../plotting/looper_output/{0}/qcd/merged_hists.root".format(tag))
fnrs = r.TFile("../output/V00-10-01_noRS/qcd_ht.root")

ht_regs = ["ht250to450","ht450to575","ht575to1200","ht1200to1500","ht1500to2000"]

fout = r.TFile("weights_{0}.root".format(tag), "RECREATE")

for ht_reg in ht_regs:
    dir = fout.mkdir(ht_reg)
    dir.cd()

    hrs  = frs.Get("{0}/h_nJet30".format(ht_reg))
    hnrs = fnrs.Get("{0}/h_nJet30".format(ht_reg))

    hrs.Scale(1./hrs.Integral(0,-1))
    hnrs.Scale(1./hnrs.Integral(0,-1))

    hweights = hnrs.Clone("h_weights")
    hweights.Divide(hrs)

    hrs.Write("h_nJet30_RS")
    hnrs.Write("h_nJet30_noRS")
    hweights.Write("h_weights")

    ## weights in SR jet binning
    bins = np.array([0.,2.,4.,7.,15.]) if ht_reg!="ht250to450" else np.array([0.,2.,4.,15.])
    hrs_bin = hrs.Rebin(bins.size-1, "hrs_bin_"+ht_reg, bins)
    hnrs_bin = hnrs.Rebin(bins.size-1, "hnrs_bin_"+ht_reg, bins)
    hweights_bin = hnrs_bin.Clone("h_weights_binned")
    hweights_bin.Divide(hrs_bin)

    hweights_bin.Write("h_weights_binned")
    
fout.Close()

