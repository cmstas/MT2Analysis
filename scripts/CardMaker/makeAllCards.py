import os
import ROOT as r
from Datacard import *
from nuisances import *

## TODO
#   - why are zinv nuisances for every year the same? Should be different from different zinv MC
#   - cases where MC SR is 0 for llep or zinv
#   - make sure zinv ratio, purity , rsfof errors make sense
#   - why are jec (llep and zinv) and lep_eff (zinv) hard-coded??

f_zinvDY = {}
f_lostlep = {}
f_qcd = {}

f_zinvDY[16] = r.TFile("../../MT2Looper/output/V00-10-10_combined/zinvFromDY_2016.root")
f_zinvDY[17] = r.TFile("../../MT2Looper/output/V00-10-10_combined/zinvFromDY_2017.root")
f_zinvDY[18] = r.TFile("../../MT2Looper/output/V00-10-10_combined/zinvFromDY_2018.root")
f_lostlep[16] = r.TFile("../../MT2Looper/output/V00-10-10_combined/lostlepFromCRs_2016.root")
f_lostlep[17] = r.TFile("../../MT2Looper/output/V00-10-10_combined/lostlepFromCRs_2017.root")
f_lostlep[18] = r.TFile("../../MT2Looper/output/V00-10-10_combined/lostlepFromCRs_2018.root")
f_qcd[16] = r.TFile("../../MT2Looper/output/V00-10-10_combined/qcdFromRS_2016.root")
f_qcd[17] = r.TFile("../../MT2Looper/output/V00-10-10_combined/qcdFromRS_2017.root")
f_qcd[18] = r.TFile("../../MT2Looper/output/V00-10-10_combined/qcdFromRS_2018.root")
f_data = r.TFile("../../MT2Looper/output/V00-10-10_combined/data_unblinded.root")

years = [16, 17, 18]

RsfofErr = 0.15

outdir = "cards_test"
os.system("mkdir -p "+outdir)

def makeTemplate(dirname, imt2):
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

    name = "{0}_{1}_{2}_{3}".format(ht_name, njets_name, nbjets_name, mt2_name).replace("-1","Inf")
    name_per_ht_reg = "{0}".format(ht_name).replace("-1", "Inf")
    name_per_topo_reg = "{0}_{1}_{2}".format(ht_name, njets_name, nbjets_name).replace("-1", "Inf")

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

    crsl_name = "{0}_{1}_{2}_{3}".format(ht_name, njets_name, nbjets_name, mt2_name).replace("-1","Inf")
    crsl_name_per_topo_reg = "{0}_{1}_{2}".format(ht_name, njets_name, nbjets_name).replace("-1", "Inf")

    # get some zinv info
    h_lastbin_hybrid = f_zinvDY[16].Get(dirname+"/h_lastbinHybrid")
    h_lastmt2_hybrid = f_zinvDY[16].Get(dirname+"/h_lastmt2Hybrid")
    crdy_lastbin_hybrid = int(h_lastbin_hybrid.GetBinContent(1))
    crdy_lastmt2_hybrid = int(h_lastmt2_hybrid.GetBinContent(1))

    if nbjets_HI > 4 or crsl_nbjets_HI > 4:
        print "ERROR: looks like h_*_HI/LOW histograms aren's scaled properly. Did you rescale after hadding?"
        exit(1)



    dc = Datacard(name, 3, ["zinv", "llep", "qcd"], years=[16,17,18], split_bkg_by_year=True)

    dc.SetObservation(h_data_mt2bins.GetBinContent(imt2))
    
    # get some common histograms/values
    h_zinv_mt2bins, h_llep_mt2bins, h_qcd_mt2bins = {}, {}, {}
    h_llep_alpha = {}
    h_zinv_purity, h_zinv_ratio = {}, {}
    n_qcd, n_zinv = {}, {}
    for y in years:
        h_llep_mt2bins[y] = f_lostlep[y].Get(dirname+"/h_mt2bins")
        h_llep_alpha[y] = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha")
        h_zinv_mt2bins[y] = f_zinvDY[y].Get(dirname+"/hybridEstimate")
        h_zinv_purity[y] = f_zinvDY[y].Get(dirname+"/purityCard")
        h_zinv_ratio[y] = f_zinvDY[y].Get(dirname+"/ratioCard")
        h_qcd_mt2bins[y] = f_qcd[y].Get(dirname+"/h_mt2bins")

        n_qcd[y] = h_qcd_mt2bins[y].GetBinContent(imt2)
        n_zinv[y] = h_zinv_mt2bins[y].GetBinContent(imt2)
    
        dc.SetBkgRate(h_zinv_mt2bins[y].GetBinContent(imt2), "zinv", y)
        dc.SetBkgRate(h_llep_mt2bins[y].GetBinContent(imt2), "llep", y)
        dc.SetBkgRate(n_qcd[y], "qcd", y)


    # do the gmN CR stat nuisances (combined CR so not separated by year)
    nuis_name = "llep_CRstat_"+crsl_name
    n_llep_CR = int(f_lostlep[16].Get(dirname+"/h_mt2binsCRyieldDatacard").GetBinContent(imt2))
    dc.AddNuisance(nuis_name, "gmN")
    dc.SetGMNNuisanceN(nuis_name, n_llep_CR)
    for y in years:
        dc.SetNuisanceBkgValue(nuis_name, h_llep_alpha[y].GetBinContent(imt2), "llep", y)

    nuis_name = "zinv_CRstat_"+name_per_topo_reg
    n_zinv_CR = int(f_zinvDY[16].Get(dirname+"/CRyieldCard").GetBinContent(imt2))
    dc.AddNuisance(nuis_name, "gmN")
    dc.SetGMNNuisanceN(nuis_name, n_zinv_CR)
    for y in years:
        alpha = h_zinv_purity[y].GetBinContent(imt2) * h_zinv_ratio[y].GetBinContent(imt2)
        dc.SetNuisanceBkgValue(nuis_name, alpha, "zinv", y)


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
                baseline_int = h_llep_alpha[y].Integral(0,-1)
                err_UP = abs(1.0 - h_UP.Integral(0,-1) / baseline_int)
                err_DN = abs(1.0 - h_DN.Integral(0,-1) / baseline_int)
                err = max(err_UP, err_DN)
                direc = 1 if h_UP.Integral(0,-1) < baseline_int else -1
                err = 1.0 + direc*err                
                dc.SetNuisanceBkgValue(nuis_name, err, "llep", y)

                zinv_err = 1.05
                dc.SetNuisanceBkgValue(nuis_name, zinv_err, "zinv", y)

        if nuis=="zinv_alphaErr":
            for y in years:
                val = h_zinv_ratio[y].GetBinContent(imt2)
                err = 1.0
                if val > 0.0:
                    err = 1.0 + h_zinv_ratio[y].GetBinError(imt2) / val                    
                dc.SetNuisanceBkgValue(nuis_name, err, "zinv", y)

        if nuis=="zinv_purity":
            for y in years:
                val = h_zinv_purity[y].GetBinContent(imt2)
                err = 1.0
                if val > 0.0:
                    err = 1.0 + h_zinv_purity[y].GetBinError(imt2) / val
                dc.SetNuisanceBkgValue(nuis_name, err, "zinv", y)

        if nuis=="zinv_Rsfof":
            for y in years:
                err = 1.0 + (1.0 - h_zinv_purity[y].GetBinContent(imt2)) * RsfofErr
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
                            inc += last_bin_relerr * (ibin - crdy_lastbin_hybrid) / n_extrap_bins * n_zinv[y]
                        extrap_err = -inc/h_zinv_mt2bins[y].GetBinContent(imt2)
                    elif imt2 > crdy_lastbin_hybrid:
                        extrap_err = last_bin_relerr * (imt2 - crdy_lastbin_hybrid) / n_extrap_bins
                extrap_err = 1.0 + extrap_err
                dc.SetNuisanceBkgValue(nuis_name, extrap_err, "zinv", y)

        if nuis == "llep_mtcut":
            for y in years:
                h_mtcut = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_mtcut")
                # measure over entire topo region
                err = h_mtcut.Integral(0, -1) / h_llep_alpha[y].Integral(0,-1)
                dc.SetNuisanceBkgValue(nuis_name, err, "llep", y)

        if nuis == "llep_btageff":
            for y in years:
                h_heavy_UP = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_btagsf_heavy_UP")
                h_heavy_DN = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_btagsf_heavy_DN")
                h_light_UP = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_btagsf_light_UP")
                h_light_DN = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_btagsf_light_DN")
                baseline_int = h_llep_alpha[y].Integral(0,-1)
                err_heavy_UP = abs(1.0 - h_heavy_UP.Integral(0,-1) / baseline_int)
                err_heavy_DN = abs(1.0 - h_heavy_DN.Integral(0,-1) / baseline_int)
                err_light_UP = abs(1.0 - h_light_UP.Integral(0,-1) / baseline_int)
                err_light_DN = abs(1.0 - h_light_DN.Integral(0,-1) / baseline_int)
                err = max(max(max(err_heavy_UP, err_heavy_DN), err_light_UP), err_light_DN)
                direc = 1 if h_heavy_UP.Integral(0,-1) >= baseline_int else -1
                err = 1.0 + direc*err
                dc.SetNuisanceBkgValue(nuis_name, err, "llep", y)

        if nuis == "llep_taueff":
            for y in years:
                h_1p_UP = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_tau1p_UP")
                h_1p_DN = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_tau1p_DN")
                h_3p_UP = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_tau3p_UP")
                h_3p_DN = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_tau3p_DN")
                baseline_int = h_llep_alpha[y].Integral(0,-1)
                err_1p_UP = abs(1.0 - h_1p_UP.Integral(0,-1) / baseline_int)
                err_1p_DN = abs(1.0 - h_1p_DN.Integral(0,-1) / baseline_int)
                err_3p_UP = abs(1.0 - h_3p_UP.Integral(0,-1) / baseline_int)
                err_3p_DN = abs(1.0 - h_3p_DN.Integral(0,-1) / baseline_int)
                err = max(max(max(err_1p_UP, err_1p_DN), err_3p_UP), err_3p_DN)
                direc = 1 if h_1p_UP.Integral(0,-1) >= baseline_int else -1
                err = 1.0 + direc*err
                dc.SetNuisanceBkgValue(nuis_name, err, "llep", y)

        if nuis == "llep_renorm":
            for y in years:
                h_UP = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_renorm_UP")
                h_DN = f_lostlep[y].Get(dirname+"/h_mt2binsAlpha_renorm_DN")
                baseline_int = h_llep_alpha[y].Integral(0,-1)
                err_UP = abs(1.0 - h_UP.Integral(0,-1) / baseline_int)
                err_DN = abs(1.0 - h_DN.Integral(0,-1) / baseline_int)
                err = max(err_UP, err_DN)
                direc = 1 if h_UP.Integral(0,-1) >= baseline_int else -1
                err = 1.0 + direc*err
                dc.SetNuisanceBkgValue(nuis_name, err, "llep", y)

        if nuis == "llep_MCstat":
            for y in years:
                h_mcstat = f_lostlep[y].Get(dirname+"/h_mt2binsMCStat")
                val, err = 0.0, 1.0
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
                if n_qcd[y] > 0.0:
                    err_qcd = f_qcd[y].Get(dirname+"/h_mt2bins_JERvar").GetBinContent(imt2) / n_qcd[y]
                dc.SetNuisanceBkgValue(nuis_name, err_qcd, "qcd", y)

        if nuis == "qcd_TailVar":
            for y in years:
                if n_qcd[y] > 0.0:
                    err_qcd = f_qcd[y].Get(dirname+"/h_mt2bins_TailVar").GetBinContent(imt2) / n_qcd[y]
                dc.SetNuisanceBkgValue(nuis_name, err_qcd, "qcd", y)

        if nuis == "qcd_SigmaSoftVar":
            for y in years:
                if n_qcd[y] > 0.0:
                    err_qcd = f_qcd[y].Get(dirname+"/h_mt2bins_SigmaSoftVar").GetBinContent(imt2) / n_qcd[y]
                dc.SetNuisanceBkgValue(nuis_name, err_qcd, "qcd", y)

        if nuis == "qcd_NJetShape":
            for y in years:
                if n_qcd[y] > 0.0:
                    err_qcd = f_qcd[y].Get(dirname+"/h_mt2bins_NJetShape").GetBinContent(imt2) / n_qcd[y]
                dc.SetNuisanceBkgValue(nuis_name, err_qcd, "qcd", y)

        if nuis == "qcd_NBJetShape":
            for y in years:
                if n_qcd[y] > 0.0:
                    err_qcd = f_qcd[y].Get(dirname+"/h_mt2bins_NBJetShape").GetBinContent(imt2) / n_qcd[y]
                dc.SetNuisanceBkgValue(nuis_name, err_qcd, "qcd", y)



    outname = os.path.join(outdir, "datacard_"+name+".txt")
    dc.Write(outname)


dirname = "sr4M"
imt2 = 2
makeTemplate(dirname, imt2)
imt2 = 5
makeTemplate(dirname, imt2)

