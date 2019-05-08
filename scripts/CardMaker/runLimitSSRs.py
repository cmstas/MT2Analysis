import subprocess
import glob

indir = "cards_V00-10-16_FullRunII_newQCD/ssrs_sigSyst15/"

for f in glob.glob(indir+"/*.txt"):
    ssr = f.split("/")[-1].split(".")[0]
    out = subprocess.check_output("combine -M AsymptoticLimits {0}".format(f).split()).split('\n')
    for line in out:
        if "Observed" in line:
            lim = float(line.split()[-1])
            print ssr, lim
            break
