import os
import glob

tag = "V00-10-18_FullRunII"
# tag = "V00-10-15_FullRunII_decorrHalfLepEff_T2qq_700_350"
# tag = "V00-10-17_FullRunII_T2bt_525_325"

for f in glob.glob("{0}/*.txt".format(tag)):
    cmd = "text2workspace.py {0} -b -o {1} &> logs/blah.txt".format(f, f.replace(".txt", ".root"))
    os.system(cmd+ " &")

