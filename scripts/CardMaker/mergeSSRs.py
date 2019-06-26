import os
from Datacard import *
from nuisances import *
import cPickle as pickle

tag = "V00-10-17_FullRunII"
sig_syst = 1.15
out_tag = "ssrs_sigSyst15"

dcs = pickle.load(open("cards_{0}/templates_ssr/template_datacards.pkl".format(tag), 'rb'))

os.system("mkdir -p cards_{0}/{1}".format(tag, out_tag))

ssrs = {
    "2j_loose" : ["HT1200toInf_j2toInf_b0toInf_m1200toInf"],
    "2j_tight" : ["HT1500toInf_j2toInf_b0toInf_m1400toInf"],
    "4j_loose" : ["HT1200toInf_j4toInf_b0toInf_m1000to1400","HT1200toInf_j4toInf_b0toInf_m1400toInf"],
    "4j_tight" : ["HT1500toInf_j4toInf_b0toInf_m1400toInf"],
    "7j_loose" : ["HT1200toInf_j7toInf_b0toInf_m600to800","HT1200toInf_j7toInf_b0toInf_m800to1000","HT1200toInf_j7toInf_b0toInf_m1000toInf"],
    "7j_tight" : ["HT1500toInf_j7toInf_b0toInf_m800to1000","HT1500toInf_j7toInf_b0toInf_m1000toInf"],
    "10j_loose" : ["HT1200toInf_j10toInf_b0toInf_m400to600","HT1200toInf_j10toInf_b0toInf_m600to800","HT1200toInf_j10toInf_b0toInf_m800to1000","HT1200toInf_j10toInf_b0toInf_m1000toInf"],
    "10j_tight" : ["HT1500toInf_j10toInf_b0toInf_m600to800","HT1500toInf_j10toInf_b0toInf_m800to1000","HT1500toInf_j10toInf_b0toInf_m1000toInf"],
    "2b_loose" : ["HT1200toInf_j2toInf_b2toInf_m600to800","HT1200toInf_j2toInf_b2toInf_m800to1000","HT1200toInf_j2toInf_b2toInf_m1000toInf"],
    "2b_tight" : ["HT1500toInf_j2toInf_b2toInf_m600to800","HT1500toInf_j2toInf_b2toInf_m800toInf"],
    "3b_loose" : ["HT1200toInf_j2toInf_b3toInf_m400to600","HT1200toInf_j2toInf_b3toInf_m600toInf"],
    "3b_tight" : ["HT1500toInf_j2toInf_b3toInf_m400to600","HT1500toInf_j2toInf_b3toInf_m600toInf"],
    "4b_loose" : ["HT1200toInf_j2toInf_b4toInf_m400to600","HT1200toInf_j2toInf_b4toInf_m600toInf"],
    "4b_tight" : ["HT1500toInf_j2toInf_b4toInf_m400to600","HT1500toInf_j2toInf_b4toInf_m600toInf"],
    "7j3b_loose" : ["HT1200toInf_j7toInf_b3toInf_m400to600","HT1200toInf_j2toInf_b3toInf_m600toInf"],
    "7j3b_tight" : ["HT1500toInf_j7toInf_b3toInf_m400toInf"],
    "7j4b_loose" : ["HT1200toInf_j7toInf_b4toInf_m400to600","HT1200toInf_j2toInf_b4toInf_m600toInf"],
    "7j4b_tight" : ["HT1500toInf_j7toInf_b4toInf_m400toInf"],
    "10j4b_loose" : ["HT1200toInf_j10toInf_b4toInf_m400to600","HT1200toInf_j10toInf_b4toInf_m600toInf"],
    "10j4b_tight" : ["HT1500toInf_j10toInf_b4toInf_m400toInf"],
    "MonoPhi" : ["HT250to350_j1_b0_m0toInf", "HT350to450_j1_b0_m0toInf", "HT250to450_j2to3_b0_m200to300"],
}

allmerged = []
for ssr in ssrs:
    cards = [dcs[s] for s in ssrs[ssr]]
    merged = Datacard.MergeCards(cards, ssr)
    merged.SetSignalRate(0.333, 16)
    merged.SetSignalRate(0.333, 17)
    merged.SetSignalRate(0.334, 18)
    merged.AddNuisance("sig_syst","lnN")
    merged.SetNuisanceSignalValue("sig_syst", sig_syst, 16)
    merged.SetNuisanceSignalValue("sig_syst", sig_syst, 17)
    merged.SetNuisanceSignalValue("sig_syst", sig_syst, 18)
    merged.AddNuisance("lumi_syst","lnN")
    merged.SetNuisanceSignalValue("lumi_syst", 1.025, 16)
    merged.SetNuisanceSignalValue("lumi_syst", 1.023, 17)
    merged.SetNuisanceSignalValue("lumi_syst", 1.025, 18)
    allmerged.append(merged)
    merged.Write("cards_{0}/{1}/{2}.txt".format(tag, out_tag, ssr), sortkey=customNuisanceSort)

pickle.dump(allmerged, open("cards_{0}/{1}/ssrs.pkl".format(tag, out_tag), 'wb'))

