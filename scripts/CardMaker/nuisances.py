from Datacard import *

CORR_ACROSS_ALLBINS = 0
PER_HT_REG = 1
PER_TOPO_REG = 2
PER_TOPO_REG_CRSL = 3
PER_MT2_BIN = 4

DEFAULT_splitByYear = True

nuisances = {
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
        "splitByYear" : DEFAULT_splitByYear,
        },
    "zinv_purity" : {
        "correlation" : PER_TOPO_REG,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "zinv_Rsfof" : {
        "correlation" : CORR_ACROSS_ALLBINS,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "zinv_shape" : {
        "correlation" : PER_TOPO_REG,
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
    "llep_MCstat" : {
        "correlation" : PER_MT2_BIN,
        "splitByYear" : DEFAULT_splitByYear,
        },
    "llep_shape" : {
        "correlation" : PER_TOPO_REG_CRSL,
        "splitByYear" : DEFAULT_splitByYear,
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
