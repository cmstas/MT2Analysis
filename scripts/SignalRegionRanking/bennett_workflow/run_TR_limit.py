import os, sys
import glob

if len(sys.argv)<4:
    print "usage: {0} <tag> <region> <point>".format(sys.argv[0])
    exit(1)

TAG = sys.argv[1]
REG = sys.argv[2]
POINT = sys.argv[3]

MODEL = POINT.split("_")[0]

cards = glob.glob("../../CardMaker/cards_{0}/{1}/{2}/*{3}*.txt".format(TAG, MODEL, POINT, REG))

workdir = os.path.join("scratch", TAG, POINT)
os.system("mkdir -p "+workdir)

os.system("combineCards.py -S {0} > {1}/card_{2}.txt".format(" ".join(cards), workdir, REG))
os.system("combine -M AsymptoticLimits {0}/card_{1}.txt -n {1} > {0}/log_{1}.txt".format(workdir, REG))
# os.system("combine -M Significance --uncapped=1 --rMin=-2 {0}/card_{1}.txt -n {1} > {0}/sig_{1}.txt".format(workdir, REG))
os.system("rm *{0}*.root".format(REG))

