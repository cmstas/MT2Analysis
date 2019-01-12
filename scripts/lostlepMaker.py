import sys, os
import numpy as np
import ROOT as r

verbose = False
doHybrid = True  # hybrid estimate: uses CR MT2 binning until the MC CR integral is less than the threshold below
hybrid_nevent_threshold = 50.0  
last_bin_relerr = 0.4   # extrapolation error in last bin. increases linearly until this in last bin

verbose = False

# returns error of C=A*B or C=A/B
def err_mult(A, B, errA, errB, C):
    if A==0.0 and errA==0.0:
        A, errA = 1., 1.
    if B==0.0 and errB==0.0:
        B, errB = 1., 1.
    return C*np.sqrt((errA/A)**2+(errB/B)**2)

if len(sys.argv) < 3:
    print "usage: python lostlepMaker.py <MT2Looper output dir> <data_name> (<lostlepSR>)"
    exit(0)

input_dir = sys.argv[1]
data_tag = sys.argv[2]
lostlepSR = sys.argv[3] if len(sys.argv) >= 4 else "lostlep"

output_name = os.path.join(input_dir, "lostlepFromCRs.root")
print "Writing to file:", output_name

f_data = r.TFile("{0}/{1}.root".format(input_dir, data_tag))
f_lostlep = r.TFile("{0}/lostlep.root".format(input_dir))
f_lostlepSR = r.TFile("{0}/{1}.root".format(input_dir, lostlepSR))

if f_data.IsZombie() or f_lostlep.IsZombie():
    print "Input file does not exist!"
    exit(0)

keep = "sr"
skip = "srbase"

dirs = []

keys = f_lostlep.GetListOfKeys()
for ik in range(keys.Capacity()):
    name = keys.At(ik).GetName()
    if name.startswith(keep) and skip not in name:
        dirs.append(name[2:])

fout = r.TFile(output_name, "RECREATE")
fout.cd()
for idir, dsuff in enumerate(dirs):
    if verbose: print "Running on dir:", dsuff

    srdir = "sr"+dsuff
    mt2_histname = srdir + "/h_mt2bins"
    mt2_histname_fine = srdir + "/h_mt2"
    n_mt2bins_histname = srdir + "/h_n_mt2bins"
    crdir = "crsl"+dsuff
    mt2_histname_SL = crdir + "/h_mt2bins"
    mt2_histname_SLall = crdir + "/h_mt2binsAll"
    mt2_histname_SLfine = crdir + "/h_mt2"
    ht_histname_SL = crdir + "/h_htbins"
    nj_histname_SL = crdir + "/h_njbins"
    nbj_histname_SL = crdir + "/h_nbjbins"

    histMap = {}
    histMap["h_lostlepMC_sr"] = f_lostlepSR.Get(mt2_histname)
    histMap["h_lostlepMC_sr_lepeff_UP"] = f_lostlepSR.Get(mt2_histname+"_lepeff_UP")
    histMap["h_lostlepMC_sr_lepeff_DN"] = f_lostlepSR.Get(mt2_histname+"_lepeff_DN")
    histMap["h_lostlepMC_sr_jec_UP"] = f_lostlepSR.Get(mt2_histname+"_jec_UP")
    histMap["h_lostlepMC_sr_jec_DN"] = f_lostlepSR.Get(mt2_histname+"_jec_DN")
    histMap["h_lostlepMC_sr_renorm_UP"] = f_lostlepSR.Get(mt2_histname+"_renorm_UP")
    histMap["h_lostlepMC_sr_renorm_DN"] = f_lostlepSR.Get(mt2_histname+"_renorm_DN")
    histMap["h_lostlepMC_sr_mtcut"] = f_lostlepSR.Get(mt2_histname+"_mtcut")
    histMap["h_lostlepMC_sr_tau1p_UP"] = f_lostlepSR.Get(mt2_histname+"_tau1p_UP")
    histMap["h_lostlepMC_sr_tau1p_DN"] = f_lostlepSR.Get(mt2_histname+"_tau1p_DN")
    histMap["h_lostlepMC_sr_tau3p_UP"] = f_lostlepSR.Get(mt2_histname+"_tau3p_UP")
    histMap["h_lostlepMC_sr_tau3p_DN"] = f_lostlepSR.Get(mt2_histname+"_tau3p_DN")
    histMap["h_lostlepMC_sr_btagsf_heavy_UP"] = f_lostlepSR.Get(mt2_histname+"_btagsf_heavy_UP")
    histMap["h_lostlepMC_sr_btagsf_heavy_DN"] = f_lostlepSR.Get(mt2_histname+"_btagsf_heavy_DN")
    histMap["h_lostlepMC_sr_btagsf_light_UP"] = f_lostlepSR.Get(mt2_histname+"_btagsf_light_UP")
    histMap["h_lostlepMC_sr_btagsf_light_DN"] = f_lostlepSR.Get(mt2_histname+"_btagsf_light_DN")


    # make sure everything exists; if not, set to nominal histogram
    for h in histMap:
        if not histMap[h]:
            if verbose: print "WARNING: hist {0} does not exist! Setting to nominal...".format(h)
            histMap[h] = f_lostlepSR.Get(mt2_histname).Clone()
        histMap[h].SetName(h)

    # get the MT2 binning
    mt2bins = []
    n_mt2bins = histMap["h_lostlepMC_sr"].GetNbinsX()
    for i in range(n_mt2bins+1):
        mt2bins.append(histMap["h_lostlepMC_sr"].GetBinLowEdge(i+1))
    mt2bins = np.array(mt2bins, dtype=float)

    alphaMap = {}
    if histMap["h_lostlepMC_sr"]:
        h_lostlepDD_sr = histMap["h_lostlepMC_sr"].Clone("h_mt2binsSR")
        # create copies of hists to store alpha values
        for h in histMap:
            name = h.replace("h_lostlepMC_sr", "alphaHist")
            nameHist = h.replace("h_lostlepMC_sr", "h_mt2binsAlpha")        
            alphaMap[name] = histMap[h].Clone(nameHist)
    else:
        print "couldn't find lostlep MC SR hist:", mt2_histname
        h_lostlepDD_sr = r.TH1D("h_mt2binsSR", "h_mt2binsSR", n_mt2bins, mt2bins)
        # create copies of hists to store alpha values
        for h in histMap:
            name = h.replace("h_lostlepMC_sr", "alphaHist")
            nameHist = h.replace("h_lostlepMC_sr", "h_mt2binsAlpha")        
            alphaMap[name] = r.TH1D(nameHist, nameHist, n_mt2bins, mt2bins)


    # get "finebinned" hists
    h_lostlepMC_sr_finebin = f_lostlepSR.Get(mt2_histname_fine)
    if h_lostlepMC_sr_finebin:
        h_lostlepDD_sr_finebin = h_lostlepMC_sr_finebin.Clone("h_mt2SR")
    else:
        print "couldn't find lostlep MC SR finebin hist:", mt2_histname
        h_lostlepDD_sr_finebin = r.TH1D("h_mt2SR", "h_mt2SR", 150, 0, 1500)

    # get the CR hists
    histMapCR = {}
    for h in histMap:
        nameCR = h.replace("h_lostlepMC_sr", "h_lostlepMC_cr")
        nameSuf = h.replace("h_lostlepMC_sr","")
        histMapCR[nameCR] = f_lostlep.Get(mt2_histname_SL+nameSuf)

    # again make sure they exist
    for h in histMapCR:
        if not histMapCR[h]:
            if verbose: print "WARNING: hist", h, "does not exist! Setting to nominal..."
            histMapCR[h] = f_lostlep.Get(mt2_histname_SL).Clone()
        histMapCR[h].SetName(h)

    lastbin_hybrid = 1  # hybrid method: will integrate all MT2 bins STARTING with this one
    lastmt2val_hybrid = 200.;
    h_data_cr = f_data.Get(mt2_histname_SL)
    if not histMapCR["h_lostlepMC_cr"]:
        print "couldn't find lostlep MC CR hist:", mt2_histname_SL, "SHOULDN'T GET HERE"
    elif not h_data_cr:
        print "couldn't find lostlep data CR hist:", mt2_histname_SL, "SHOULDN'T GET HERE"
    else:
        if doHybrid:
            for ibin in range(n_mt2bins, 0, -1):
                if histMapCR["h_lostlepMC_cr"].Integral(ibin, -1) >= hybrid_nevent_threshold:
                    lastbin_hybrid = ibin
                    lastmt2val_hybrid = h_data_cr.GetBinLowEdge(ibin)
                    break

        for h in histMap:
            name = h.replace("h_lostlepMC_sr","alphaHist")
            nameCR = h.replace("h_lostlepMC_sr","h_lostlepMC_cr")
            if doHybrid:
                for ibin in range(1, n_mt2bins+1):
                    if ibin < lastbin_hybrid:
                        cr_yield = histMapCR[nameCR].GetBinContent(ibin)
                        err_cr_yield = histMapCR[nameCR].GetBinError(ibin)
                    else:
                        err_cr_yield = r.Double(0.0)
                        cr_yield = histMapCR[nameCR].IntegralAndError(lastbin_hybrid, -1, err_cr_yield)
                    alpha_val = alphaMap[name].GetBinContent(ibin) / cr_yield
                    alpha_err = err_mult(cr_yield, alphaMap[name].GetBinContent(ibin), err_cr_yield, alphaMap[name].GetBinError(ibin), alpha_val)
                    alphaMap[name].SetBinContent(ibin, alpha_val)
                    alphaMap[name].SetBinError(ibin, alpha_err)
            else:
                cr_yield = histMapCR[nameCR].Integral(0,-1)
                alphaMap[name].Scale(1/cr_yield)

    # hack; hardcode this variation to 3%
    alphaMap["alphaHist_mtcut"].Scale(1.03)

    data_cr_totalyield = 0;
    if h_data_cr:
        h_lostlepDD_cr = h_data_cr.Clone("h_mt2binsCRyield")
        h_lostlepDD_cr_datacard = h_data_cr.Clone("h_mt2binsCRyieldDatacard")
        data_cr_totalyield = h_lostlepDD_cr.Integral(0,-1)
        err_cr_yield = r.Double(0.0)
        cr_yield = h_lostlepDD_cr.IntegralAndError(lastbin_hybrid, -1, err_cr_yield)
        for ibin in range(lastbin_hybrid, n_mt2bins+1):
            h_lostlepDD_cr_datacard.SetBinContent(ibin, cr_yield)
            h_lostlepDD_cr_datacard.SetBinError(ibin, err_cr_yield)
    else:
        print "couldn't find data CR hist: " << mt2_histname_SL
        h_lostlepDD_cr = r.TH1D("h_mt2binsCRyield", "h_mt2binsCRyield", n_mt2bins, mt2bins)
        h_lostlepDD_cr_datacard = r.TH1D("h_mt2binsCRyieldDatacard", "h_mt2binsCRyieldDatacard", n_mt2bins, mt2bins)


    h_lostlepMC_cr_finebin = f_lostlep.Get(mt2_histname_SLfine)
    if h_lostlepMC_cr_finebin:
        h_lostlepMC_rescaled_cr_finebin = h_lostlepMC_cr_finebin.Clone("h_mt2CRMCrescaled")
    else:
        print "couldn't find lostlep MC CR finebin hist:", mt2_histname_SLfine
        h_lostlepMC_rescaled_cr_finebin = r.TH1D("h_mt2", "h_mt2", 150, 0, 1500)

    h_lostlepMC_cr_allbins = f_lostlep.Get(mt2_histname_SLall)
    if h_lostlepMC_cr_allbins:
        h_lostlepMC_rescaled_cr_allbins = h_lostlepMC_cr_allbins.Clone("h_mt2binsAllCRMChybrid")
    else:
        print "couldn't find lostlep MC CR allbins hist:", mt2_histname_SLall
        h_lostlepMC_rescaled_cr_allbins = r.TH1D("h_mt2binsAll", "h_mt2binsAll", 150, 0, 1500)

    h_htbins_lostlepMC_cr = f_lostlep.Get(ht_histname_SL)
    if h_htbins_lostlepMC_cr:
        h_htbins_lostlepMC_rescaled_cr = h_htbins_lostlepMC_cr.Clone("h_htbinsCRMCrescaled")
    else:
        print "couldn't find lostlep MC CR ht hist:", ht_histname_SL
        h_htbins_lostlepMC_rescaled_cr = r.TH1D("h_htbins", "h_htbins", 150, 0, 1500)

    h_njbins_lostlepMC_cr = f_lostlep.Get(nj_histname_SL)
    if h_njbins_lostlepMC_cr:
        h_njbins_lostlepMC_rescaled_cr = h_njbins_lostlepMC_cr.Clone("h_njbinsCRMCrescaled")
    else:
        print "couldn't find lostlep MC CR nj hist:", nj_histname_SL
        h_njbins_lostlepMC_rescaled_cr = r.TH1D("h_njbins", "h_njbins", 150, 0, 1500)

    h_nbjbins_lostlepMC_cr = f_lostlep.Get(nbj_histname_SL)
    if h_nbjbins_lostlepMC_cr:
        h_nbjbins_lostlepMC_rescaled_cr = h_nbjbins_lostlepMC_cr.Clone("h_nbjbinsCRMCrescaled")
    else:
        print "couldn't find lostlep MC CR nbj hist:", nbj_histname_SL
        h_nbjbins_lostlepMC_rescaled_cr = r.TH1D("h_nbjbins", "h_nbjbins", 150, 0, 1500)

    h_data_cr_finebin = f_data.Get(mt2_histname_SLfine)
    if h_data_cr_finebin:
        h_data_cr_finebin_save = h_data_cr_finebin.Clone("h_mt2CRyield")
    else:
        print "couldn't find lostlep data CR finebin hist:", mt2_histname_SLfine
        h_data_cr_finebin_save = r.TH1D("h_mt2CRyield", "h_mt2CRyield", 150, 0, 1500)

    h_htbins_data_cr = f_data.Get(ht_histname_SL)
    if h_htbins_data_cr:
        h_htbins_data_cr_save = h_htbins_data_cr.Clone("h_htbinsCRyield")
    else:
        print "couldn't find lostlep data CR ht hist:", ht_histname_SL
        h_htbins_data_cr_save = r.TH1D("h_htbinsCRyield", "h_htbinsCRyield", 150, 0, 1500)

    h_njbins_data_cr = f_data.Get(nj_histname_SL)
    if h_njbins_data_cr:
        h_njbins_data_cr_save = h_njbins_data_cr.Clone("h_njbinsCRyield")
    else:
        print "couldn't find lostlep data CR nj hist:", nj_histname_SL
        h_njbins_data_cr_save = r.TH1D("h_njbinsCRyield", "h_njbinsCRyield", 150, 0, 1500)

    h_nbjbins_data_cr = f_data.Get(nbj_histname_SL)
    if h_nbjbins_data_cr:
        h_nbjbins_data_cr_save = h_nbjbins_data_cr.Clone("h_nbjbinsCRyield")
    else:
        print "couldn't find lostlep data CR nj hist:", nbj_histname_SL
        h_nbjbins_data_cr_save = r.TH1D("h_nbjbinsCRyield", "h_nbjbinsCRyield", 150, 0, 1500)


    # data-driven part: use data to normalize MC SR prediction
    if h_lostlepDD_cr_datacard and alphaMap["alphaHist"]:
        h_lostlepDD_sr.Multiply(h_lostlepDD_cr_datacard, alphaMap["alphaHist"], 1., 1.)

    # others: normalize based on total yield in topo region
    norm = 1.0
    if histMapCR["h_lostlepMC_cr"]:
        norm = data_cr_totalyield / histMapCR["h_lostlepMC_cr"].Integral(0,-1)
    h_lostlepDD_sr_finebin.Scale(norm)
    h_lostlepMC_rescaled_cr_finebin.Scale(norm)
    h_htbins_lostlepMC_rescaled_cr.Scale(norm)
    h_njbins_lostlepMC_rescaled_cr.Scale(norm)
    h_nbjbins_lostlepMC_rescaled_cr.Scale(norm)
    

    # mt2binsAll: normalize MC to data below lastmt2val_hybrid, then MC to the data integral above
    h_data_cr_allbins = f_data.Get(mt2_histname_SLall)
    if(h_data_cr_allbins):
        lastbin_hybrid_allbins = h_data_cr_allbins.FindBin(lastmt2val_hybrid)
        for ibin in range(1, h_data_cr_allbins.GetNbinsX()+1):
            if ibin < lastbin_hybrid_allbins:
                data_val = h_data_cr_allbins.GetBinContent(ibin)
                mc_val = h_lostlepMC_cr_allbins.GetBinContent(ibin)
                mc_err = h_lostlepMC_cr_allbins.GetBinError(ibin)
                h_lostlepMC_rescaled_cr_allbins.SetBinContent(ibin, data_val)
                if(mc_val > 0.): 
                    h_lostlepMC_rescaled_cr_allbins.SetBinError(ibin, mc_err*data_val/mc_val)
            else:
                data_int = h_data_cr_allbins.Integral(lastbin_hybrid_allbins, -1)
                mc_int = h_lostlepMC_cr_allbins.Integral(lastbin_hybrid_allbins, -1)
                mc_val = h_lostlepMC_cr_allbins.GetBinContent(ibin)
                mc_err = h_lostlepMC_cr_allbins.GetBinError(ibin)
                if(mc_int > 0.):
                    h_lostlepMC_rescaled_cr_allbins.SetBinContent(ibin, mc_val * data_int/mc_int)
                    h_lostlepMC_rescaled_cr_allbins.SetBinError(ibin, mc_err * data_int/mc_int)
    else:
        print "WARNING: didn't find histogram in data:", mt2_histname_SLall

        
    Syst = h_lostlepDD_sr.Clone("h_mt2binsSyst")
    MCStat = h_lostlepDD_sr.Clone("h_mt2binsMCStat")
    pred = h_lostlepDD_sr.Clone("h_mt2bins")
    pred_finebin = h_lostlepDD_sr_finebin.Clone("h_mt2")

    h_ht_LOW = f_lostlep.Get(crdir+"/h_ht_LOW");
    h_ht_HI = f_lostlep.Get(crdir+"/h_ht_HI");
    h_nbjets_LOW = f_lostlep.Get(crdir+"/h_nbjets_LOW");
    h_nbjets_HI = f_lostlep.Get(crdir+"/h_nbjets_HI");
    h_njets_LOW = f_lostlep.Get(crdir+"/h_njets_LOW");
    h_njets_HI = f_lostlep.Get(crdir+"/h_njets_HI");

    if h_ht_LOW:
        ht_LOW = h_ht_LOW.GetBinContent(1)
    else:
        ht_LOW = -1.0
    if h_ht_HI:
        ht_HI = h_ht_HI.GetBinContent(1)
    else:
        ht_HI = -1.0
    if h_njets_LOW:
        njets_LOW = h_njets_LOW.GetBinContent(1)
    else:
        njets_LOW = -1.0
    if h_njets_HI:
        njets_HI = h_njets_HI.GetBinContent(1)
    else:
        njets_HI = -1.0
    if h_nbjets_LOW:
        nbjets_LOW = h_nbjets_LOW.GetBinContent(1)
    else:
        nbjets_LOW = -1.0
    if h_nbjets_HI:
        nbjets_HI = h_nbjets_HI.GetBinContent(1)
    else:
        nbjets_HI = -1.0

    # uncertainties on finebin. WHY??
    for ibin in range(1, pred_finebin.GetNbinsX()+1):
        val = pred_finebin.GetBinContent(ibin)
        if val<=0: continue
        err_mcstat = h_lostlepMC_sr_finebin.GetBinError(ibin) / h_lostlepMC_sr_finebin.GetBinContent(ibin)
        err_datastat = np.sqrt(data_cr_totalyield)/data_cr_totalyield if data_cr_totalyield>0 else 0.0
        quadrature = err_mcstat*err_mcstat + err_datastat*err_datastat + 0.15**2 + 0.05**2
        if njets_LOW>=7 and nbjets_LOW>=1: quadrature += 0.2**2
        pred_finebin.SetBinError(ibin, val*np.sqrt(quadrature))

    # MC extrapolation systematic error
    n_extrap_bins = n_mt2bins - lastbin_hybrid
    MCExtrap = h_lostlepDD_sr.Clone("h_mt2binsMCExtrap")
    for ibin in range(lastbin_hybrid, n_mt2bins+1):
        if ibin == lastbin_hybrid:
            increment = 0.0
            for jbin in range(lastbin_hybrid+1, n_mt2bins+1):
                increment += last_bin_relerr / (n_extrap_bins) * (jbin - lastbin_hybrid) * pred.GetBinContent(jbin)
            MCExtrap.SetBinContent(ibin, pred.GetBinContent(ibin) - increment)
        else:
            relvar = last_bin_relerr / n_extrap_bins * (ibin - lastbin_hybrid)
            MCExtrap.SetBinContent(ibin, pred.GetBinContent(ibin) * (1. + relvar))


    #mt2binsAll with extrapolation error
    h_lostlepMC_rescaled_cr_allbins_MCExtrapErr = h_lostlepMC_cr_allbins.Clone("h_mt2binsAllCRMChybridExtrapErr")
    for ibin in range(1, h_lostlepMC_cr_allbins.GetNbinsX()+1):
        sig_mt2bin = pred.FindBin(h_lostlepMC_cr_allbins.GetBinLowEdge(ibin))
        relvar = MCExtrap.GetBinContent(sig_mt2bin) / pred.GetBinContent(sig_mt2bin) if pred.GetBinContent(sig_mt2bin) > 0 else 0.0
        h_lostlepMC_rescaled_cr_allbins_MCExtrapErr.SetBinContent(ibin, h_lostlepMC_rescaled_cr_allbins.GetBinContent(ibin) * relvar)


    h_lastbinHybrid = r.TH1D("h_lastbinHybrid", ";last bin", 1, 0, 1)
    h_lastbinHybrid.SetBinContent(1, lastbin_hybrid)
    h_lastmt2Hybrid = r.TH1D("h_lastmt2Hybrid", ";last bin", 1, 0, 1)
    h_lastmt2Hybrid.SetBinContent(1, lastmt2val_hybrid)


    d = fout.mkdir(srdir)
    d.cd()

    pred.Write()
    pred_finebin.Write()
    Syst.Write()
    h_lostlepDD_cr.Write()
    h_lostlepDD_cr_datacard.Write()
    MCStat.Write()
    MCExtrap.Write()
    for h in histMap:
        histMap[h].Write()
    for h in histMapCR:
        histMapCR[h].Write()
    for h in alphaMap:
        alphaMap[h].Write()
    h_lostlepMC_rescaled_cr_finebin.Write()
    h_data_cr_finebin_save.Write()
    h_lostlepMC_rescaled_cr_allbins.Write()
    h_lostlepMC_rescaled_cr_allbins_MCExtrapErr.Write()
    h_htbins_lostlepMC_rescaled_cr.Write()
    h_njbins_lostlepMC_rescaled_cr.Write()
    h_nbjbins_lostlepMC_rescaled_cr.Write()
    h_htbins_data_cr_save.Write()
    h_njbins_data_cr_save.Write()
    h_nbjbins_data_cr_save.Write()
    h_ht_LOW.Write()
    h_ht_HI.Write()
    h_njets_LOW.Write()
    h_njets_HI.Write()
    h_nbjets_LOW.Write()
    h_nbjets_HI.Write()
    h_lastbinHybrid.Write()
    h_lastmt2Hybrid.Write()

fout.Close()



