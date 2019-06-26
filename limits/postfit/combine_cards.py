import os, sys
import glob

template_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/scripts/CardMaker/cards_V00-10-18_FullRunII/templates"
tag = "V00-10-18_FullRunII"
# template_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/scripts/CardMaker/cards_V00-10-16_FullRunII_noDecorrSyst/rpvMonoPhi/rpvMonoPhi_1250_900"
# tag = "V00-10-16_FullRunII_noDecorrSyst_rpvMonoPhi_1250_900"
# template_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/scripts/CardMaker/cards_V00-10-17_FullRunII_withDYContam/T2bt/T2bt_525_325"
# tag = "V00-10-17_FullRunII_T2bt_525_325"
# template_dir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/scripts/CardMaker/cards_V00-10-15_FullRunII_decorrHalfLepEff/T1tttt/T1tttt_2100_0"
# tag = "V00-10-15_FullRunII_decorrHalfLepEff_T1tttt_2100_0"

os.system("mkdir -p "+tag)

files = {}
files["monojet"] = glob.glob(os.path.join(template_dir, "datacard*_j1_*.txt"))
files["VL"] = glob.glob(os.path.join(template_dir, "datacard*_HT250to450_*.txt"))
files["L"] = glob.glob(os.path.join(template_dir, "datacard*_HT450to575_*.txt"))
files["M"] = glob.glob(os.path.join(template_dir, "datacard*_HT575to1200_*.txt"))
files["H"] = glob.glob(os.path.join(template_dir, "datacard*_HT1200to1500_*.txt"))
files["UH"] = glob.glob(os.path.join(template_dir, "datacard*_HT1500toInf_*.txt"))
files["all"] = glob.glob(os.path.join(template_dir, "datacard_*.txt"))

# remove monojet files from non-monojet regions
for reg in files:
    if reg in ["monojet", "all"]:
        continue
    for f in files[reg]:
        if "j1_" in f:
            files[reg].remove(f)

cmds = []
for reg in files:
    cmd = "combineCards.py -S "
    for f in files[reg]:
        name = f.split("/datacard_")[-1].split(".txt")[0]
        cmd += "{0}={1} ".format(name, f)
    cmd += " > {0}/cards_{1}.txt".format(tag, reg)
    cmds.append(cmd)
    
    # print cmd
    # raw_input()

for cmd in cmds:
    os.system(cmd + " &")



