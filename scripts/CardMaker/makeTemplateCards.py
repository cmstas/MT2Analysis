import os
import ROOT as r
from Datacard import *
from nuisances import *
from math import isnan, isinf
import cPickle as pickle
from copy import deepcopy


TAG = "FullRunII_17MCfor18_ttbbWeights_80x_fastsim"

doSuperSignalRegions = False
suppressFirstUHmt2bin = True # start UH at 400
RsfofErr = 0.15
lumi_syst_16 = 0.025
lumi_syst_17 = 0.023
lumi_syst_18 = 0.050

verbose = False

f_zinvDY = {}
f_lostlep = {}
f_qcd = {}
f_sig = {}

f_zinvDY[16] = r.TFile("../../MT2Looper/output/V00-10-10_combined_17MCfor18_ttbbWeights/zinvFromDY_2016.root")
f_zinvDY[17] = r.TFile("../../MT2Looper/output/V00-10-10_combined_17MCfor18_ttbbWeights/zinvFromDY_2017.root")
f_zinvDY[18] = r.TFile("../../MT2Looper/output/V00-10-10_combined_17MCfor18_ttbbWeights/zinvFromDY_2018.root")
f_lostlep[16] = r.TFile("../../MT2Looper/output/V00-10-10_combined_17MCfor18_ttbbWeights/lostlepFromCRs_2016.root")
f_lostlep[17] = r.TFile("../../MT2Looper/output/V00-10-10_combined_17MCfor18_ttbbWeights/lostlepFromCRs_2017.root")
f_lostlep[18] = r.TFile("../../MT2Looper/output/V00-10-10_combined_17MCfor18_ttbbWeights/lostlepFromCRs_2018.root")
f_qcd[16] = r.TFile("../../MT2Looper/output/V00-10-10_combined_17MCfor18_ttbbWeights/qcdFromRS_2016.root")
f_qcd[17] = r.TFile("../../MT2Looper/output/V00-10-10_combined_17MCfor18_ttbbWeights/qcdFromRS_2017.root")
f_qcd[18] = r.TFile("../../MT2Looper/output/V00-10-10_combined_17MCfor18_ttbbWeights/qcdFromRS_2018.root")
f_data = r.TFile("../../MT2Looper/output/V00-10-10_combined_17MCfor18_ttbbWeights/data_RunAll.root")

years = [16, 17, 18]

outdir = "cards_"+TAG
os.system("mkdir -p "+outdir)

# used to set the approximate error when we have no MC SR events
# measure the average MC event weight in a topological region
def getAverageWeight(h, bkg, year, dirname):
    # hardcode some tricky ones
    if (bkg, year, dirname) == ("zinv", 16, "sr29M"):
        return 0.03
    if (bkg, year, dirname) == ("zinv", 17, "sr29M"):
        return 0.005
    if (bkg, year, dirname) == ("zinv", 18, "sr29M"):
        return 0.005
    if (bkg, year, dirname) == ("zinv", 17, "sr28UH"):
        return 0.006
    if (bkg, year, dirname) == ("zinv", 18, "sr28UH"):
        return 0.006
    if (bkg, year, dirname) == ("zinv", 17, "sr28H"):
        return 0.006
    if (bkg, year, dirname) == ("zinv", 18, "sr28H"):
        return 0.006
    if (bkg, year, dirname) == ("zinv", 16, "sr29H"):
        return 0.03
    if (bkg, year, dirname) == ("zinv", 17, "sr29H"):
        return 0.006
    if (bkg, year, dirname) == ("zinv", 18, "sr29H"):
        return 0.006
    if (bkg, year, dirname) == ("zinv", 17, "sr29UH"):
        return 0.006
    if (bkg, year, dirname) == ("zinv", 18, "sr29UH"):
        return 0.006

    if h.Integral() == 0:
        raise Exception("ERROR: cannot get average weight of an empty histogram!")
    n_evts_effective = 0.0
    for i in range(1, h.GetNbinsX()+1):
        cont = h.GetBinContent(i)
        err = h.GetBinError(i)
        if cont > 0.0:
            n_evts_effective += (cont / err)**2
    return h.Integral() / n_evts_effective

def makeTemplate(dirname, imt2, use_pred_for_obs=True, template_output_dir=None):
    # make a template datacard, i.e. one with no signal info


    # Get kinematic limits in signal region
    
    h_ht_LOW = f_data.Get(dirname+"/h_ht_LOW")
    h_ht_HI = f_data.Get(dirname+"/h_ht_HI")
    h_njets_LOW = f_data.Get(dirname+"/h_njets_LOW")
    h_njets_HI = f_data.Get(dirname+"/h_njets_HI")
    h_nbjets_LOW = f_data.Get(dirname+"/h_nbjets_LOW")
    h_nbjets_HI = f_data.Get(dirname+"/h_nbjets_HI")

    h_data_mt2bins = f_data.Get(dirname+"/h_mt2bins")
    n_mt2bins = h_data_mt2bins.GetNbinsX()

    ht_LOW = int(h_ht_LOW.GetBinContent(1))
    ht_HI = int(h_ht_HI.GetBinContent(1))
    njets_LOW = int(h_njets_LOW.GetBinContent(1))
    njets_HI = int(h_njets_HI.GetBinContent(1))
    nbjets_LOW = int(h_nbjets_LOW.GetBinContent(1))
    nbjets_HI = int(h_nbjets_HI.GetBinContent(1))
    mt2_LOW = int(h_data_mt2bins.GetXaxis().GetBinLowEdge(imt2))
    mt2_HI = int(h_data_mt2bins.GetXaxis().GetBinUpEdge(imt2))
    if imt2 == h_data_mt2bins.GetNbinsX():
        mt2_HI = -1

    ht_name = "HT{0}to{1}".format(ht_LOW, ht_HI)
    njets_name = "j{0}".format(njets_LOW) if njets_HI-njets_LOW==1 else "j{0}to{1}".format(njets_LOW, njets_HI-1)
    nbjets_name = "b{0}".format(nbjets_LOW) if nbjets_HI-nbjets_LOW==1 else "b{0}to{1}".format(nbjets_LOW, nbjets_HI-1)
    mt2_name = "m{0}to{1}".format(mt2_LOW, mt2_HI)

    name = "{0}_{1}_{2}_{3}".format(ht_name, njets_name, nbjets_name, mt2_name).replace("-1","Inf").replace("-2","Inf")
    name_per_ht_reg = "{0}".format(ht_name).replace("-1", "Inf").replace("-2","Inf")
    name_per_topo_reg = "{0}_{1}_{2}".format(ht_name, njets_name, nbjets_name).replace("-1", "Inf").replace("-2","Inf")

    # Get kinematic limits in SL control region (ht doesn't change)
    
    h_njets_LOW = f_lostlep[16].Get(dirname+"/h_njets_LOW")
    h_njets_HI = f_lostlep[16].Get(dirname+"/h_njets_HI")
    h_nbjets_LOW = f_lostlep[16].Get(dirname+"/h_nbjets_LOW")
    h_nbjets_HI = f_lostlep[16].Get(dirname+"/h_nbjets_HI")
    h_lastbin_hybrid = f_lostlep[16].Get(dirname+"/h_lastbinHybrid")
    h_lastmt2_hybrid = f_lostlep[16].Get(dirname+"/h_lastmt2Hybrid")

    crsl_njets_LOW = int(h_njets_LOW.GetBinContent(1))
    crsl_njets_HI = int(h_njets_HI.GetBinContent(1))
    crsl_nbjets_LOW = int(h_nbjets_LOW.GetBinContent(1))
    crsl_nbjets_HI = int(h_nbjets_HI.GetBinContent(1))
    crsl_lastbin_hybrid = int(h_lastbin_hybrid.GetBinContent(1))
    crsl_lastmt2_hybrid = int(h_lastmt2_hybrid.GetBinContent(1))

    njets_name = "j{0}".format(crsl_njets_LOW) if crsl_njets_HI-crsl_njets_LOW==1 else \
        "j{0}to{1}".format(crsl_njets_LOW, crsl_njets_HI-1)
    nbjets_name = "b{0}".format(crsl_nbjets_LOW) if crsl_nbjets_HI-crsl_nbjets_LOW==1 else \
        "b{0}to{1}".format(crsl_nbjets_LOW, crsl_nbjets_HI-1)
    if imt2 < crsl_lastbin_hybrid:        
        mt2_name = "m{0}to{1}".format(mt2_LOW, mt2_HI)
    else:
        mt2_name = "m{0}toInf".format(crsl_lastmt2_hybrid)

    crsl_name = "{0}_{1}_{2}_{3}".format(ht_name, njets_name, nbjets_name, mt2_name).replace("-1","Inf").replace("-2","Inf")
    crsl_name_per_topo_reg = "{0}_{1}_{2}".format(ht_name, njets_name, nbjets_name).replace("-1", "Inf").replace("-2","Inf")

    # Get kinematic limits in DY control region (ht doesn't change)

    h_njets_LOW = f_zinvDY[16].Get(dirname+"/h_njets_LOW")
    h_njets_HI = f_zinvDY[16].Get(dirname+"/h_njets_HI")
    h_nbjets_LOW = f_zinvDY[16].Get(dirname+"/h_nbjets_LOW")
    h_nbjets_HI = f_zinvDY[16].Get(dirname+"/h_nbjets_HI")
    h_lastbin_hybrid = f_zinvDY[16].Get(dirname+"/h_lastbinHybrid")
    h_lastmt2_hybrid = f_zinvDY[16].Get(dirname+"/h_lastmt2Hybrid")

    crdy_njets_LOW = int(h_njets_LOW.GetBinContent(1))
    crdy_njets_HI = int(h_njets_HI.GetBinContent(1))
    crdy_nbjets_LOW = int(h_nbjets_LOW.GetBinContent(1))
    crdy_nbjets_HI = int(h_nbjets_HI.GetBinContent(1))
    crdy_lastbin_hybrid = int(h_lastbin_hybrid.GetBinContent(1))
    crdy_lastmt2_hybrid = int(h_lastmt2_hybrid.GetBinContent(1))

    njets_name = "j{0}".format(crdy_njets_LOW) if crdy_njets_HI-crdy_njets_LOW==1 else \
        "j{0}to{1}".format(crdy_njets_LOW, crdy_njets_HI-1)
    nbjets_name = "b{0}".format(crdy_nbjets_LOW) if crdy_nbjets_HI-crdy_nbjets_LOW==1 else \
        "b{0}to{1}".format(crdy_nbjets_LOW, crdy_nbjets_HI-1)
    if imt2 < crdy_lastbin_hybrid:        
        mt2_name = "m{0}to{1}".format(mt2_LOW, mt2_HI)
    else:
        mt2_name = "m{0}toInf".format(crdy_lastmt2_hybrid)

    crdy_name = "{0}_{1}_{2}_{3}".format(ht_name, njets_name, nbjets_name, mt2_name).replace("-1","Inf").replace("-2","Inf")
    crdy_name_per_topo_reg = "{0}_{1}_{2}".format(ht_name, njets_name, nbjets_name).replace("-1", "Inf").replace("-2","Inf")



    if nbjets_HI > 4 or crsl_nbjets_HI > 4 or crdy_nbjets_HI > 4:
        print "ERROR: looks like h_*_HI/LOW histograms aren's scaled properly. Did you rescale after hadding?"
        exit(1)



    dc = Datacard(name, 3, ["zinv", "llep", "qcd"], years=[16,17,18], split_bkg_by_year=True, split_sig_by_year=True)
    # use this later in signal part
    dc.info["dirname"] = dirname
    dc.info["imt2"] = imt2
    dc.info["lostlep_lastbin_hybrid"] = crsl_lastbin_hybrid
    dc.info["orig_name"] = name
    dc.info["name_per_ht_reg"] = name_per_ht_reg
    dc.info["name_per_topo_reg"] = name_per_topo_reg
    
    # get some common histograms/values
    h_zinv_mt2bins, h_llep_mt2bins, h_qcd_mt2bins = {}, {}, {}
    h_llep_alpha = {}
    h_zinv_purity, h_zinv_ratio = {}, {}
    n_qcd, n_zinv, n_llep = {}, {}, {}
    no_qcd_hist = {}
    total_estimate = 0.0
    for y in years:
        h_llep_mt2bins[y] = f_lostlep[y].Get(dirname+"/h_mt2bins")
        h_llep_alpha[y] = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha")
        h_zinv_mt2bins[y] = f_zinvDY[y].Get(dirname+"/hybridEstimate")
        h_zinv_purity[y] = f_zinvDY[y].Get(dirname+"/purityCard")
        h_zinv_ratio[y] = f_zinvDY[y].Get(dirname+"/ratioCard")
        h_qcd_mt2bins[y] = f_qcd[y].Get(dirname+"/h_mt2bins")

        n_zinv[y] = h_zinv_mt2bins[y].GetBinContent(imt2)
        n_llep[y] = h_llep_mt2bins[y].GetBinContent(imt2)
        n_qcd[y] = 0.0        
        no_qcd_hist[y] = True
        if h_qcd_mt2bins[y]: 
            n_qcd[y] = h_qcd_mt2bins[y].GetBinContent(imt2)
            no_qcd_hist[y] = False

        total_estimate += n_zinv[y]
        total_estimate += n_llep[y]
        total_estimate += n_qcd[y]

        dc.SetBkgRate(n_zinv[y], "zinv", y)
        dc.SetBkgRate(n_llep[y], "llep", y)
        dc.SetBkgRate(n_qcd[y], "qcd", y)

    # combine doesn't like when prediction is exactly 0
    if total_estimate == 0.0:
        print "WARNING: predicted exactly 0.0 background for directory {0}, imt2 {1}. Setting qcd{2} to 0.01.".format(dirname, imt2, years[0])
        n_qcd[years[0]] = 0.01
        total_estimate  = 0.01
        dc.SetBkgRate(n_qcd[years[0]], "qcd", years[0])


    if use_pred_for_obs:
        dc.SetObservation(total_estimate)
    else:
        dc.SetObservation(h_data_mt2bins.GetBinContent(imt2))

    # do the gmN CR stat nuisances (combined CR so not separated by year)
    n_llep_CR = int(f_lostlep[16].Get(dirname+"/h_mt2binsCRyieldDatacard").GetBinContent(imt2))
    n_llep_MCCR = float(f_lostlep[16].Get(dirname+"/h_lostlepMC_cr").GetBinContent(imt2))
    if imt2 >= crsl_lastbin_hybrid:
        n_llep_MCCR = float(f_lostlep[16].Get(dirname+"/h_lostlepMC_cr").Integral(crsl_lastbin_hybrid, -1))
    llep_zero_alpha = {}
    dc.info["lostlep_alpha"] = {}
    for y in years:
        alpha = h_llep_alpha[y].GetBinContent(imt2)
        dc.info["lostlep_alpha"][y] = alpha

        nuis_name = "llep_CRstat_"+crsl_name                
        llep_zero_alpha[y] = False

        # handle the case where nCR is nonzero but alpha is 0
        # basically make the gmN error represent the error on the 0 MC count
        # set nCR=0, alpha to the average MC weight in this topo region
        if n_llep_CR > 0 and n_llep_MCCR > 0.0 and alpha < 1e-8:
            llep_zero_alpha[y] = True
            avg_weight = getAverageWeight(f_lostlep[y].Get(dirname+"/h_lostlepMC_sr"), "llep", y, dirname)
            alpha = avg_weight * n_llep_CR / n_llep_MCCR
            nuis_name = "llep_CRstat_"+name+"_"+str(y)
            print ("WARNING: llep alpha is 0 for dir {0} imt2 {1}, year {2}. "
            "Decorrelating gmN nuisance, setting nCR to 0, "
            "and setting alpha to average MC SR weight {3}.").format(dirname, imt2, y, alpha)

        if nuis_name not in dc.GetNuisanceNames():
            dc.AddNuisance(nuis_name, "gmN")

        dc.SetGMNNuisanceN(nuis_name, 0 if llep_zero_alpha[y] else n_llep_CR)
        dc.SetNuisanceBkgValue(nuis_name, alpha, "llep", y)

        if isnan(alpha) or isinf(alpha):
            print "ERROR: llep alpha is nan or inf! Directory {0}, imt2 {1}. Put in protections to continue".format(dirname, imt2)
            exit(1)

        if n_llep_CR == 0 and alpha > 0.0:
            print ("WARNING: n_llep_CR is 0 for directory {0}, imt2 {1}, year {2}. "
            "alpha is nonzero so we're proceeding as usual.").format(dirname, imt2, y)

        if n_llep_CR > 0 and alpha==0.0:
            print ("ERROR: llep alpha is 0.0 for directory {0}, imt2 {1}. n_llep_CR is >0."
                   "Figure out what you want to do and put in protection.").format(dirname, imt2)
            exit(1)

        if n_llep_CR == 0 and alpha==0.0 and not llep_zero_alpha[y]:
            print ("ERROR: llep alpha is 0.0 AND n_llep_cr is 0 for directory {0}, imt2 {1}, year {2}. "
            "Figure out what you want to do and put in protection.").format(dirname, imt2, y)
            exit(1)

        if alpha > 10.0:
            print "WARNING: llep alpha is very large ({0}) for directory {1}, imt2 {2}".format(alpha, dirname, imt2)


    n_zinv_CR = int(f_zinvDY[16].Get(dirname+"/CRyieldCard").GetBinContent(imt2))
    h_zinv_CRyieldEM = f_zinvDY[16].Get(dirname+"/CRyieldEMCard")
    n_zinv_CROF = int(h_zinv_CRyieldEM.GetBinContent(imt2)) if h_zinv_CRyieldEM else 0
    n_zinv_MCCR = float(f_zinvDY[16].Get(dirname+"/h_mt2binsMCCR").GetBinContent(imt2))
    if imt2 >= crdy_lastbin_hybrid:
        n_zinv_MCCR = float(f_zinvDY[16].Get(dirname+"/h_mt2binsMCCR").Integral(crdy_lastbin_hybrid, -1))
    zinv_zero_alpha = {}
    for y in years:
        alpha = h_zinv_purity[y].GetBinContent(imt2) * h_zinv_ratio[y].GetBinContent(imt2)

        nuis_name = "zinv_CRstat_"+crdy_name_per_topo_reg
        zinv_zero_alpha[y] = False

        # handle the case where nCR is nonzero but alpha is 0
        # basically make the gmN error represent the error on the 0 MC count
        # set nCR=0, alpha to the average MC weight in this topo region
        if n_zinv_CR > 0 and n_zinv_MCCR > 0.0 and alpha < 1e-8:
            zinv_zero_alpha[y] = True
            avg_weight = getAverageWeight(f_zinvDY[y].Get(dirname+"/h_mt2binsMCSR"), "zinv", y, dirname)
            alpha = avg_weight * n_zinv_CR / n_zinv_MCCR
            nuis_name = "zinv_CRstat_"+name+"_"+str(y)
            print ("WARNING: zinv alpha is 0 for dir {0} imt2 {1}, year {2}. "
            "Decorrelating gmN nuisance, setting nCR to 0, "
            "and setting alpha to average MC SR weight {3}.").format(dirname, imt2, y, alpha)

        if nuis_name not in dc.GetNuisanceNames():
            dc.AddNuisance(nuis_name, "gmN")

        dc.SetGMNNuisanceN(nuis_name, 0 if zinv_zero_alpha[y] else n_zinv_CR)
        dc.SetNuisanceBkgValue(nuis_name, alpha, "zinv", y)


        if isnan(alpha) or isinf(alpha):
            print "ERROR: zinv alpha is nan or inf! Directory {0}, imt2 {1}. Put in protections to continue".format(dirname, imt2)
            exit(1)

        if n_zinv_CR == 0 and alpha > 0.0:
            print ("WARNING: n_zinv_CR is 0 for directory {0}, imt2 {1}, year {2}. "
            "alpha is nonzero so we're proceeding as usual.").format(dirname, imt2, y)

        if n_zinv_CR > 0 and alpha==0.0:
            print ("ERROR: zinv alpha is 0.0 for directory {0}, imt2 {1}. n_zinv_CR is >0. "
                   "Figure out what you want to do and put in protection.").format(dirname, imt2)
            exit(1)

        if n_zinv_CR == 0 and alpha==0.0 and not zinv_zero_alpha:
            print ("ERROR: zinv alpha is 0.0 AND n_zinv_cr is 0 for directory {0}, imt2 {1}. "
            "Figure out what you want to do and put in protection.").format(dirname, imt2)
            exit(1)

        if alpha > 10.0:
            print "WARNING: zinv alpha is very large ({0}) for directory {1}, imt2 {2}".format(alpha, dirname, imt2)

    lowmt2bin = 2 if (suppressFirstUHmt2bin and "UH" in dirname) else 1

    # now do the lnN nuisances
    for nuis in nuisances:
        corr = nuisances[nuis]["correlation"]
        split_by_year = nuisances[nuis]["splitByYear"]

        nuis_name = nuis
        if corr == PER_HT_REG:
            nuis_name += "_"+name_per_ht_reg
        if corr == PER_TOPO_REG:
            nuis_name += "_"+name_per_topo_reg
        if corr == PER_TOPO_REG_CRSL:
            nuis_name += "_"+crsl_name_per_topo_reg
        if corr == PER_MT2_BIN:
            nuis_name += "_"+name

        dc.AddNuisance(nuis_name, "lnN", split_by_year=split_by_year)

        if nuis == "lumi_syst":
            dc.SetNuisanceSignalValue(nuis_name, 1.0 + lumi_syst_16, 16)
            dc.SetNuisanceSignalValue(nuis_name, 1.0 + lumi_syst_17, 17)
            dc.SetNuisanceSignalValue(nuis_name, 1.0 + lumi_syst_18, 18)

        if nuis == "jec":
            llep_val = 1.02
            zinv_val = 1.05 if ht_HI==450 else 1.02            
            for y in years:
                dc.SetNuisanceBkgValue(nuis_name, llep_val, "llep", y)
                dc.SetNuisanceBkgValue(nuis_name, zinv_val, "zinv", y)

        if nuis == "lep_eff":
            for y in years:
                # lostlep
                h_UP = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_lepeff_UP")
                h_DN = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_lepeff_DN")
                baseline_int = h_llep_alpha[y].Integral(lowmt2bin,-1)
                err_UP = abs(1.0 - h_UP.Integral(lowmt2bin,-1) / baseline_int)
                err_DN = abs(1.0 - h_DN.Integral(lowmt2bin,-1) / baseline_int)
                err = max(err_UP, err_DN)
                direc = 1 if h_UP.Integral(lowmt2bin,-1) > baseline_int else -1
                err = 1.0 + direc*err                
                dc.SetNuisanceBkgValue(nuis_name, err, "llep", y)

                zinv_err = 0.95
                dc.SetNuisanceBkgValue(nuis_name, zinv_err, "zinv", y)

        if nuis=="zinv_alphaErr":
            for y in years:
                err = 1.0
                if zinv_zero_alpha[y]:
                    err = 2.0
                else:
                    val = h_zinv_ratio[y].GetBinContent(imt2)
                    if val < 0.0:
                        print "ERROR: negative zinv alpha! Investigate and put in a fix before proceeding."
                        exit(1)
                    if val > 0.0:
                        err = 1.0 + h_zinv_ratio[y].GetBinError(imt2) / val                    
                dc.SetNuisanceBkgValue(nuis_name, err, "zinv", y)

        if nuis=="zinv_purity":
            for y in years:
                val = h_zinv_purity[y].GetBinContent(imt2)
                if val < 0.0:
                    print "ERROR: negative zinv purity! Investigate and put in a fix before proceeding."
                    exit(1)
                err_up, erf_down = 1.0, 1.0
                if val > 0.0:
                    err = 1.0 + h_zinv_purity[y].GetBinError(imt2) / val
                    err_up = min(err, 1.0 / val)
                    err_down = 1.0 / err
                dc.SetNuisanceBkgValue(nuis_name, (err_up, err_down), "zinv", y)

        if nuis=="zinv_Rsfof":
            # this error is the OF*dR term of the error on (SF-R*OF)  (dSF covered by gmN error, R*dOF covered by purity error)
            # we want relative error OF*dR/(SF-R*OF) = dR / purity * OF/SF 
            for y in years:
                err = 1.0
                if h_zinv_purity[y].GetBinContent(imt2) > 0.0 and n_zinv_CR > 0:
                    err = 1.0 + RsfofErr / h_zinv_purity[y].GetBinContent(imt2) * n_zinv_CROF / n_zinv_CR
                dc.SetNuisanceBkgValue(nuis_name, err, "zinv", y)

        if nuis=="zinv_shape":
            for y in years:
                last_bin_relerr = 0.4
                n_extrap_bins = n_mt2bins - crdy_lastbin_hybrid
                extrap_err = 0.0
                if n_extrap_bins > 0:
                    if imt2 == crdy_lastbin_hybrid and n_zinv[y] > 0.0:
                        inc = 0.0
                        for ibin in range(crdy_lastbin_hybrid+1, n_mt2bins+1):
                            inc += last_bin_relerr * (ibin - crdy_lastbin_hybrid) / n_extrap_bins * h_zinv_mt2bins[y].GetBinContent(ibin)
                        extrap_err = -inc/h_zinv_mt2bins[y].GetBinContent(imt2)
                    elif imt2 > crdy_lastbin_hybrid:
                        extrap_err = last_bin_relerr * (imt2 - crdy_lastbin_hybrid) / n_extrap_bins
                extrap_err = 1.0 + extrap_err
                dc.SetNuisanceBkgValue(nuis_name, extrap_err, "zinv", y)

        if nuis == "llep_mtcut":
            for y in years:
                h_mtcut = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_mtcut")
                # measure over entire topo region
                err = h_mtcut.Integral(lowmt2bin, -1) / h_llep_alpha[y].Integral(lowmt2bin,-1)
                dc.SetNuisanceBkgValue(nuis_name, err, "llep", y)

        if nuis == "llep_btageff":
            for y in years:
                h_heavy_UP = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_btagsf_heavy_UP")
                h_heavy_DN = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_btagsf_heavy_DN")
                h_light_UP = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_btagsf_light_UP")
                h_light_DN = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_btagsf_light_DN")
                baseline_int = h_llep_alpha[y].Integral(lowmt2bin,-1)
                err_heavy_UP = abs(1.0 - h_heavy_UP.Integral(lowmt2bin,-1) / baseline_int)
                err_heavy_DN = abs(1.0 - h_heavy_DN.Integral(lowmt2bin,-1) / baseline_int)
                err_light_UP = abs(1.0 - h_light_UP.Integral(lowmt2bin,-1) / baseline_int)
                err_light_DN = abs(1.0 - h_light_DN.Integral(lowmt2bin,-1) / baseline_int)
                err = max(max(max(err_heavy_UP, err_heavy_DN), err_light_UP), err_light_DN)
                direc = 1 if h_heavy_UP.Integral(lowmt2bin,-1) >= baseline_int else -1
                err = 1.0 + direc*err
                dc.SetNuisanceBkgValue(nuis_name, err, "llep", y)

        if nuis == "llep_taueff":
            for y in years:
                h_1p_UP = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_tau1p_UP")
                h_1p_DN = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_tau1p_DN")
                h_3p_UP = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_tau3p_UP")
                h_3p_DN = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_tau3p_DN")
                baseline_int = h_llep_alpha[y].Integral(lowmt2bin,-1)
                err_1p_UP = abs(1.0 - h_1p_UP.Integral(lowmt2bin,-1) / baseline_int)
                err_1p_DN = abs(1.0 - h_1p_DN.Integral(lowmt2bin,-1) / baseline_int)
                err_3p_UP = abs(1.0 - h_3p_UP.Integral(lowmt2bin,-1) / baseline_int)
                err_3p_DN = abs(1.0 - h_3p_DN.Integral(lowmt2bin,-1) / baseline_int)
                err = max(max(max(err_1p_UP, err_1p_DN), err_3p_UP), err_3p_DN)
                direc = 1 if h_1p_UP.Integral(lowmt2bin,-1) >= baseline_int else -1
                err = 1.0 + direc*err
                dc.SetNuisanceBkgValue(nuis_name, err, "llep", y)

        if nuis == "llep_renorm":
            for y in years:
                h_UP = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_renorm_UP")
                h_DN = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_renorm_DN")
                baseline_int = h_llep_alpha[y].Integral(lowmt2bin,-1)
                err_UP = abs(1.0 - h_UP.Integral(lowmt2bin,-1) / baseline_int)
                err_DN = abs(1.0 - h_DN.Integral(lowmt2bin,-1) / baseline_int)
                err = max(err_UP, err_DN)
                direc = 1 if h_UP.Integral(lowmt2bin,-1) >= baseline_int else -1
                err = 1.0 + direc*err
                dc.SetNuisanceBkgValue(nuis_name, err, "llep", y)

        if nuis == "llep_ttHeavyFlavor":
            for y in years:
                h_UP = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_TTHF_UP")
                h_DN = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_TTHF_DN")
                baseline_int = h_llep_alpha[y].Integral(lowmt2bin,-1)
                err_UP = abs(1.0 - h_UP.Integral(lowmt2bin,-1) / baseline_int)
                err_DN = abs(1.0 - h_DN.Integral(lowmt2bin,-1) / baseline_int)
                err = max(err_UP, err_DN)
                direc = 1 if h_UP.Integral(lowmt2bin,-1) >= baseline_int else -1
                err = 1.0 + direc*err
                dc.SetNuisanceBkgValue(nuis_name, err, "llep", y)

        if nuis == "llep_MCstat":
            for y in years:
                val, err = 0.0, 1.0
                if llep_zero_alpha[y]: # if we're doing the ad-hoc procedure, just give it a 100% error
                    err = 2.0
                else:
                    h_mcstat = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha")
                    val = h_mcstat.GetBinContent(imt2)
                    if val > 0.0:
                        err = 1.0 + h_mcstat.GetBinError(imt2) / val
                dc.SetNuisanceBkgValue(nuis_name, err, "llep", y)

        if nuis == "llep_shape":
            for y in years:
                h_shape = f_lostlep[y].Get(dirname+"/h_mt2binsMCExtrap")
                val, err = 0.0, 1.0
                val = h_llep_mt2bins[y].GetBinContent(imt2)
                if val > 0.0:
                    err = h_shape.GetBinContent(imt2) / val
                dc.SetNuisanceBkgValue(nuis_name, err, "llep", y)

        if nuis == "qcd_JERvar":
            for y in years:
                err_qcd = 1.0
                if not no_qcd_hist[y] and n_qcd[y] > 0.0:
                    err_qcd = f_qcd[y].Get(dirname+"/h_mt2bins_JERvar").GetBinContent(imt2) / n_qcd[y]
                dc.SetNuisanceBkgValue(nuis_name, err_qcd, "qcd", y)

        if nuis == "qcd_RSstat":
            for y in years:
                err_qcd = 1.0
                if not no_qcd_hist[y] and n_qcd[y] > 0.0:
                    err_qcd = 1.0 + h_qcd_mt2bins[y].GetBinError(imt2) / n_qcd[y]
                dc.SetNuisanceBkgValue(nuis_name, err_qcd, "qcd", y)

        if nuis == "qcd_TailVar":
            for y in years:
                err_qcd = 1.0
                if not no_qcd_hist[y] and n_qcd[y] > 0.0:
                    err_qcd = f_qcd[y].Get(dirname+"/h_mt2bins_TailVar").GetBinContent(imt2) / n_qcd[y]
                dc.SetNuisanceBkgValue(nuis_name, err_qcd, "qcd", y)

        if nuis == "qcd_SigmaSoftVar":
            for y in years:
                err_qcd = 1.0
                if not no_qcd_hist[y] and n_qcd[y] > 0.0:
                    err_qcd = f_qcd[y].Get(dirname+"/h_mt2bins_SigmaSoftVar").GetBinContent(imt2) / n_qcd[y]
                dc.SetNuisanceBkgValue(nuis_name, err_qcd, "qcd", y)

        if nuis == "qcd_NJetShape":
            for y in years:
                err_qcd = 1.0
                if not no_qcd_hist[y] and n_qcd[y] > 0.0:
                    err_qcd = f_qcd[y].Get(dirname+"/h_mt2bins_NJetShape").GetBinContent(imt2) / n_qcd[y]
                dc.SetNuisanceBkgValue(nuis_name, err_qcd, "qcd", y)

        if nuis == "qcd_NBJetShape":
            for y in years:
                err_qcd = 1.0
                if not no_qcd_hist[y] and n_qcd[y] > 0.0:
                    err_qcd = f_qcd[y].Get(dirname+"/h_mt2bins_NBJetShape").GetBinContent(imt2) / n_qcd[y]
                dc.SetNuisanceBkgValue(nuis_name, err_qcd, "qcd", y)



    if template_output_dir is not None:
        outname = os.path.join(template_output_dir, "datacard_"+name+".txt")
        dc.Write(outname, sortkey=customNuisanceSort)

    return dc



template_dir = os.path.join(outdir, "templates")
os.system("mkdir -p "+template_dir)

template_datacards = {}
signal_points = set()
iterator = f_lostlep[16].GetListOfKeys()
keep = "sr"
skip = "srbase"
for key in iterator:
    dirname = key.GetTitle()
    if not dirname.startswith(keep):
        continue
    if dirname.startswith(skip):
        continue
    # if "H" not in dirname:
    #     continue
    isSSR = dirname[-1].isdigit()
    if isSSR and not doSuperSignalRegions:
        continue
    if not isSSR and doSuperSignalRegions:
        continue

    # print dirname
    # if dirname != "sr25UH":
    #     continue

    h_mt2bins = f_lostlep[16].Get(dirname+"/h_mt2bins")
    for imt2 in range(1, h_mt2bins.GetNbinsX()+1):
        if suppressFirstUHmt2bin and "UH" in dirname and imt2==1:
            continue
        dc = makeTemplate(dirname, imt2, template_output_dir=template_dir, use_pred_for_obs=False)
        template_datacards[dc.GetName()] = dc

pickle.dump(template_datacards, open(os.path.join(template_dir, "template_datacards.pkl"), 'wb'))



