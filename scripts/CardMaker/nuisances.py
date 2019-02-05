from Datacard import *

CORR_ACROSS_ALLBINS = 0
PER_HT_REG = 1
PER_TOPO_REG = 2
PER_TOPO_REG_CRSL = 3
PER_MT2_BIN = 4

DEFAULT_splitByYear = False

nuisances = {
    "lumi_syst" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : True,
        },
    "sig_PUsyst" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : False,
        },
    "sig_gensyst" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "sig_IsrSyst" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
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
    "sig_jec" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "sig_renorm" : {
        "correlation" : CORR_ACROSS_ALLBINS,
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
    "zinv_alphaErr" : {
        "correlation" : PER_MT2_BIN,
        "splitByYear" : True,
        },
    "zinv_purity" : {
        "correlation" : PER_TOPO_REG,
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
