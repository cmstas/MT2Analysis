from Datacard import *

CORR_ACROSS_ALLBINS = 0
PER_HT_REG = 1
PER_TOPO_REG = 2
PER_TOPO_REG_CRSL = 3
PER_TOPO_REG_CRDY = 4
PER_MT2_BIN = 5

DEFAULT_splitByYear = False

# default type is lnN. Can optionally set "type" to "lnU"

nuisances = {
    "lumi_syst" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : True,
        },
    # "sig_PUsyst" : {
    #     "correlation" : CORR_ACROSS_ALLBINS,
    #     "splitByYear" : False,
    #     },
    "sig_gensyst" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        "type" : "lnN",
        },
    "sig_IsrSyst" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : True,
        },
    "sig_bTagHeavySyst" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "sig_bTagLightSyst" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "sig_MCstat" : {
        "correlation" : PER_MT2_BIN,
        "splitByYear" : True,
        },
    # "sig_MCstat16" : {
    #     "correlation" : PER_MT2_BIN,
    #     "splitByYear" : False,
    #     },
    # "sig_MCstat1718" : {
    #     "correlation" : PER_MT2_BIN,
    #     "splitByYear" : False,
    #     },
    "sig_jec_renorm" : {
        "correlation" : PER_MT2_BIN,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "jec" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "lep_eff" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "lep_eff_decorr" : {
        "correlation" : PER_MT2_BIN,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "zinv_alphaErr" : {
        "correlation" : PER_MT2_BIN,
        "splitByYear" : False, # overwritten in cardmaker anyway (depends on if we're extrapolating or not)
        },
    "zinv_purity" : {
        "correlation" : PER_TOPO_REG_CRDY,
        "splitByYear" : False,
        },
    "zinv_Rsfof" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : False,
        },
    "zinv_shape" : {
        "correlation" : PER_TOPO_REG,
        "splitByYear" : False,
        },
    "zinv_trigeff" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "zinv_renorm" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "llep_mtcut" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "llep_btageff" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "llep_taueff" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "llep_renorm" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "llep_ttHeavyFlavor" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "llep_MCstat" : {
        "correlation" : PER_MT2_BIN,
        "splitByYear" : True,
        },
    "llep_shape" : {
        "correlation" : PER_TOPO_REG_CRSL,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "qcd_RSstat" : {
        "correlation" : PER_MT2_BIN,
        "splitByYear" : True,
        },
    "qcd_HEM" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : False,
        },
    "qcd_JERvar" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "qcd_TailVar" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "qcd_SigmaSoftVar" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "qcd_NJetShape" : {
        "correlation" : PER_HT_REG,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "qcd_NBJetShape" : {
        "correlation" : PER_HT_REG,
        "splitByYear" : DEFAULT_splitByYear,
        },
   
    }


def customNuisanceSort(nuis_name):
    if nuis_name.startswith("lumi"):
        return '0'+nuis_name
    if nuis_name.startswith("sig"):
        return '1'+nuis_name
    if nuis_name.startswith("jec"):
        return '2'+nuis_name
    if nuis_name.startswith("lep"):
        return '3'+nuis_name
    if nuis_name.startswith("zinv"):
        return '4'+nuis_name
    if nuis_name.startswith("llep"):
        return '5'+nuis_name
    if nuis_name.startswith("qcd"):
        return '6'+nuis_name
    return '7' + nuis_name
