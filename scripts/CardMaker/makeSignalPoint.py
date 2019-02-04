import os, sys
import ROOT as r
from Datacard import *
from nuisances import *
from math import isnan, isinf
import cPickle as pickle
from copy import deepcopy

if len(sys.argv) < 5:
    print "usage: {0} <tag> <signame> <M1> <M2>".format(sys.argv[0])
    exit(1)

TAG = sys.argv[1]

verbose = False
doSuperSignalRegions = False
subtractSignalContam = True

sig_PUsyst = 1.046

isScan = True
signame = sys.argv[2]
# set these to None to do a full scan; set to mass values to do a single point
M1 = int(sys.argv[3])
M2 = int(sys.argv[4])

f_sig = {}
f_sig[16] = r.TFile("../../MT2Looper/output/V00-10-10_2016fullYear_v2/{0}.root".format(signame))
f_sig[17] = r.TFile("../../MT2Looper/output/V00-10-10_2017fullYear_v2/{0}.root".format(signame))
f_sig[18] = r.TFile("../../MT2Looper/output/V00-10-10_2017fullYear_v2/{0}.root".format(signame))

years = [16, 17, 18]

# ad-hoc scaling if signals aren't normalized to right lumi
# (e.g. if using 2017 signal for 2018)
sig_scale = {}
sig_scale[16] = 1.0
sig_scale[17] = 1.0
sig_scale[18] = 58.8 / 41.5

outdir = "cards_"+TAG


def printFullCard(signal_dc, im1, im2, output_dir):
    dc = signal_dc

    dirname = dc.info["dirname"]
    imt2 = dc.info["imt2"]

    isSignalWithLeptons = "T1tttt" in signame or "T2tt" in signame

    name = "{0}_{1}_{2}_{3}".format(dc.GetName(), signame, im1, im2)
    dc.SetName(name)

    aux_names = ["genmet", "btagsf_heavy_UP", "btagsf_heavy_DN",
                 "btagsf_light_UP", "btagsf_light_DN",
                 "lepeff_UP", "lepeff_DN", "isr_UP", "isr_DN"]

    h_sig = {}
    h_aux = {}
    h_crsl_sig = {}
    h_crsl_aux = {}
    for an in aux_names:
        h_aux[an] = {}
        h_crsl_aux[an] = {}
        for y in years:
            h_aux[an][y] = None
            h_crsl_aux[an][y] = None
    if im1 >= 0 and im2 >= 0:
        # doing a full scan (or one point of one)
        for y in years:
            h_sigscan = f_sig[y].Get(dirname+"/h_mt2bins_sigscan")
            h_sig[y] = None
            h_crsl_sig[y] = None
            if not h_sigscan:
                if verbose: print "Could not get h_mt2bins_sigscan for dir {0}, year {1}, skipping".format(dirname, y)
                continue
            bin1 = h_sigscan.GetYaxis().FindBin(im1)
            bin2 = h_sigscan.GetZaxis().FindBin(im2)
            h_sig[y] = h_sigscan.ProjectionX("h_mt2bins_{0}_{1}_{2}_{3}_{4}".format(y, dirname, im1, im2, imt2), bin1, bin1, bin2, bin2)
            for an in aux_names:
                h_sigscan = f_sig[y].Get(dirname+"/h_mt2bins_sigscan_"+an)
                h_aux[an][y] = 0
                if h_sigscan:
                    h_aux[an][y] = h_sigscan.ProjectionX("h_mt2bins_{0}_{1}_{2}_{3}_{4}_{5}".format(an, y, dirname, im1, im2, imt2),
                                                         bin1, bin1, bin2, bin2)
            if subtractSignalContam:
                h_sigscan = f_sig[y].Get(dirname.replace("sr","crsl")+"/h_mt2bins_sigscan")
                if h_sigscan:
                    h_crsl_sig[y] = h_sigscan.ProjectionX("h_mt2bins_{0}_{1}_{2}_{3}_{4}".format(y, dirname+"sl", im1, im2, imt2),
                                                          bin1, bin1, bin2, bin2)
                for an in aux_names:
                    h_sigscan = f_sig[y].Get(dirname.replace("sr","crsl")+"/h_mt2bins_sigscan_"+an)
                    h_crsl_aux[an][y] = None
                    if h_sigscan and an=="genmet":
                        h_crsl_aux[an][y] = h_sigscan.ProjectionX("h_mt2bins_{0}_{1}_{2}_{3}_{4}_{5}".format(an, y, dirname+"sl", 
                                                                                                         im1, im2, imt2),
                                                                  bin1, bin1, bin2, bin2)
    else:
        # doint a single-point sample
        # not implemented yet
        raise Exception("non-scan samples not implemented yet!")


    allnone = True
    for y in years:
        if h_sig[y] is not None:
            allnone = False
    if allnone:
        if verbose: print "Couldn't find signal for any year, skipping"
        return False

    # loop over years and get histogram contents. Skip if no histo for any year
    n_sig = {}
    n_sig_topo = {}
    err_sig_mcstat = {}
    err_sig_mcstat_rel = {}
    n_aux = {}
    for an in aux_names:
        n_aux[an] = {}
    totsig = 0.0
    for y in years:
        n_sig[y] = 0.0
        n_sig_topo[y] = 0.0
        err_sig_mcstat[y] = 0.0
        err_sig_mcstat_rel[y] = 0.0

        if h_sig[y]:
            if not h_aux["genmet"][y]:
                print "genmet histogram doesn't exist for {0}. This is strange and may indicate something went wrong in the looper. We'll assume that the gen counts are 0 for a good reason, but you may want to check this.".format(name)
                h_aux["genmet"][y] = h_sig[y].Clone("h_mt2bins_genmet_{0}_{1}_{2}".format(im1, im2, dirname))
                h_aux["genmet"][y].Scale(0.0)
            n_sig[y] = h_sig[y].GetBinContent(imt2)
            n_sig_topo[y] = h_sig[y].Integral(0,-1)
            err_sig_mcstat[y] = h_sig[y].GetBinContent(imt2)
            if n_sig[y] > 0:
                err_sig_mcstat_rel[y] = err_sig_mcstat[y] / n_sig[y] - 1.0

        for an in aux_names:
            n_aux[an][y] = n_sig[y]
            if h_aux[an][y]:
                n_aux[an][y] = h_aux[an][y].GetBinContent(imt2)

        totsig += (n_sig[y] + n_aux["genmet"][y]) / 2.0

    if totsig < 0.001:
        if verbose: print "No signal found for {0} point {1},{2}, dir {3}. Skipping".format(signame, im1, im2, dirname)
        return False


    # do reco/gen averaging
    n_sig_recogenaverage = {}
    err_sig_recogenaverage = {}
    for y in years:
        n_sig_recogenaverage[y] = (n_sig[y] + n_aux["genmet"][y]) / 2.0
        err_sig_recogenaverage[y] = 0.0
        if n_sig_recogenaverage[y] > 0.0:
            err_sig_recogenaverage[y] = (n_sig[y] - n_aux["genmet"][y]) / 2.0 / n_sig_recogenaverage[y]

    # subtract signal contamination
    n_sig_cor = {}
    n_sig_cor_recogenaverage = {}
    n_aux_cor = {}
    n_sig_crsl = {}
    n_aux_crsl = {}
    for an in aux_names:
        n_aux_cor[an] = {}
        n_aux_crsl[an] = {}
    for y in years:
        n_sig_cor[y] = n_sig[y]
        n_sig_crsl[y] = 0.0
        n_sig_cor_recogenaverage[y] = n_sig_recogenaverage[y]
        for an in aux_names:
            n_aux_cor[an][y] = n_aux[an][y]
            n_aux_crsl[an][y] = 0.0

        if subtractSignalContam:
            if h_crsl_sig[y]:
                if imt2 >= dc.info["lostlep_lastbin_hybrid"]:
                    n_sig_crsl[y] = h_crsl_sig[y].Integral(dc.info["lostlep_lastbin_hybrid"], -1)
                else:
                    n_sig_crsl[y] = h_crsl_sig[y].GetBinContent(imt2)
                for an in aux_names:
                    if not h_crsl_aux[an][y]:
                        continue
                    if imt2 >= dc.info["lostlep_lastbin_hybrid"]:
                        n_aux_crsl[an][y] = h_crsl_aux[an][y].Integral(dc.info["lostlep_lastbin_hybrid"], -1)
                    else:
                        n_aux_crsl[an][y] = h_crsl_aux[an][y].GetBinContent(imt2)

                nextra = n_sig_crsl[y] * dc.info["lostlep_alpha"][y]
                nextra_genmet = n_aux_crsl["genmet"][y] * dc.info["lostlep_alpha"][y]
                nextra_recogenaverage = (nextra + nextra_genmet) / 2.0
                n_sig_cor[y] = max(0.0, n_sig[y] - nextra)
                n_sig_cor_recogenaverage[y] = max(0.0, n_sig_recogenaverage[y] - nextra_recogenaverage)
                err_sig_recogenaverage[y] = 0.0
                if n_sig_cor_recogenaverage[y] > 0:
                    err_sig_recogenaverage[y] = abs(n_sig_cor[y] - n_sig_cor_recogenaverage[y]) / 2.0 / n_sig_cor_recogenaverage[y]

            elif isSignalWithLeptons:
                pass

    totsig = 0.0
    for y in years:
        n_sig_cor_recogenaverage[y] *= 1.0 * sig_scale[y]
        totsig += n_sig_cor_recogenaverage[y]

    if totsig < 0.001:
        if verbose: print "Skipping {0}, imt2 {1} for point {2},{3} due to 0 bin content ".format(dirname, imt2, im1, im2)
        return False

    for y in years:
        dc.SetSignalRate(n_sig_cor_recogenaverage[y], y)

    for nuis in nuisances:
        corr = nuisances[nuis]["correlation"]
        split_by_year = nuisances[nuis]["splitByYear"]

        nuis_name = nuis
        if corr == PER_HT_REG:
            nuis_name += "_" + dc.info["name_per_ht_reg"]
        if corr == PER_TOPO_REG:
            nuis_name += "_" + dc.info["name_per_topo_reg"]
        if corr == PER_MT2_BIN:
            nuis_name += "_" + dc.info["orig_name"]

        if nuis == "sig_PUsyst":
            for y in years:
                dc.SetNuisanceSignalValue(nuis_name, sig_PUsyst, y)

        if nuis == "sig_gensyst":
            for y in years:
                dc.SetNuisanceSignalValue(nuis_name, 1.0 + err_sig_recogenaverage[y], y)

        if nuis == "sig_IsrSyst":
            for y in years:
                err = 1.0
                if n_sig[y] > 0.0:
                    errup = (n_aux["isr_UP"][y] / n_sig[y] - 1.0)
                    errdn = (n_aux["isr_DN"][y] / n_sig[y] - 1.0)
                    err = max(abs(errup), abs(errdn))
                    err = 1.0+err if errup>0 else 1.0-err
                dc.SetNuisanceSignalValue(nuis_name, err, y)

        if nuis == "sig_bTagHeavySyst":
            for y in years:
                err = 1.0
                if n_sig[y] > 0.0:
                    errup = (n_aux["btagsf_heavy_UP"][y] / n_sig[y] - 1.0)
                    errdn = (n_aux["btagsf_heavy_DN"][y] / n_sig[y] - 1.0)
                    err = max(abs(errup), abs(errdn))
                    err = 1.0+err if errup>0 else 1.0-err
                dc.SetNuisanceSignalValue(nuis_name, err, y)

        if nuis == "sig_bTagLightSyst":
            for y in years:
                err = 1.0
                if n_sig[y] > 0.0:
                    errup = (n_aux["btagsf_light_UP"][y] / n_sig[y] - 1.0)
                    errdn = (n_aux["btagsf_light_DN"][y] / n_sig[y] - 1.0)
                    err = max(abs(errup), abs(errdn))
                    err = 1.0+err if errup>0 else 1.0-err
                dc.SetNuisanceSignalValue(nuis_name, err, y)

        if nuis == "sig_MCstat":
            for y in years:
                err = 1.0 + ((err_sig_mcstat_rel[y])**2 + 0.005)**0.5
                dc.SetNuisanceSignalValue(nuis_name, err, y)

        if nuis== "lep_eff":
            for y in years:
                err = 1.0
                if n_sig[y] > 0.0:
                    errup = (n_aux["lepeff_UP"][y] / n_sig[y] - 1.0)
                    errdn = (n_aux["lepeff_DN"][y] / n_sig[y] - 1.0)
                    err = max(abs(errup), abs(errdn))
                    err = 1.0+err if errup>0 else 1.0-err
                dc.SetNuisanceSignalValue(nuis_name, err, y)


    if output_dir is not None:
        os.system("mkdir -p "+output_dir)
        outname = os.path.join(output_dir, "datacard_"+name+".txt")
        dc.Write(outname, sortkey=customNuisanceSort)

    return True



templates = pickle.load(open(os.path.join(outdir, "templates", "template_datacards.pkl"), 'rb'))

allfalse = True
for dc in templates.values():
    full_outdir = os.path.join(outdir, signame, "{0}_{1}_{2}".format(signame, M1, M2))

    result = printFullCard(dc, M1, M2, full_outdir)
    if result:
        allfalse = False

exit(allfalse)

