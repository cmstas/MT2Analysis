import sys, os
import glob
sys.path.append("ProjectMetis/scripts")
import dis_client

from samples_2016 import samples_2016
from samples_2017 import samples_2017
from samples_2018 import samples_2018

checkForScale1fb = True

# tag = "V00-10-08_2016fullYear_17Jul2018"
tag = "V00-10-08_2017fullYear_31Mar2018"
# tag = "V00-10-08_2018fullYear_17Sep2018"
# tag = "RebalanceAndSmear_V00-10-07_2016fullYear"
# samples = samples_2016
samples = samples_2017
# samples = samples_2018

# cms4tags = ["CMS4_V10-02-04", "CMS4_V09-04-17"]
cms4tags = ["CMS4_V10-02-05", "CMS4_V10-02-04", "CMS4_V09-04-19"]
# cms4tags = ["CMS4_V10-02-04", "CMS4_V10-02-02", "CMS4_V10-02-01"]

do = [
    "data",
    "ttbar",
    "wjets",
    "zinv",
    "dyjetsll",
    "qcd_ht",
    "singletop",
    "gjets",
    "ttv",
    "diboson",
]

if checkForScale1fb:
    fs1fb = open("/home/users/bemarsh/CORE/Tools/datasetinfo/scale1fbs.txt")
    hasScale1fb = []
    for line in fs1fb:
        if line.strip().startswith("#"):
            continue
        hasScale1fb.append((line.strip().split()[0], line.strip().split()[1]))
    fs1fb.close()

nsamps = 0
nfiles = 0
nevts = 0

outdir = "{0}/file_lists".format(tag)
os.system("mkdir -p "+outdir)

for type in samples:
    if type not in do:
        continue
    for ds in samples[type]:
        if "RebalanceAndSmear" in tag and type=="data" and "JetHT" not in ds:
            continue
        try:
            info = dis_client.query(ds, "snt")["response"]["payload"]
        except IndexError:
            print "WARNING: dataset not found!", ds
            continue

        idx = -1
        if cms4tags is not None:
            for cms4tag in cms4tags:
                for i in range(len(info)):
                    if info[i]["cms3tag"] == cms4tag:
                        idx = i
                        break
                if idx >= 0:
                    break
        else:
            idx = 0
        if idx < 0:
            print "WARNING: dataset not found with tag {0}!".format(cms4tag), ds
            continue

        thisTag = info[idx]["cms3tag"]

        path = info[idx]["location"]
        if "2018D" in ds and "v2" in ds:
            path = path.replace("V10-01-00","V10-02-02")

        if not os.path.exists(path):
            path = path.rstrip('/')+"_todelete"
        if not os.path.exists(path):
            print "WARNING: path does not exist!", path
            continue

        if checkForScale1fb and type is not "data" and (ds, thisTag) not in hasScale1fb:
            print "WARNING: sample does not have scale1fb info!", ds

        samp = samples[type][ds][3]

        fout = open("{0}/{1}.txt".format(outdir, samp), 'w')
        fout.write("# {0} {1} {2} {3} {4}\n".format(ds, info[idx]["nevents_out"], info[idx]["xsec"], info[idx]["kfactor"], info[idx]["filter_eff"]))
        for f in glob.glob(path+"/*.root"):
            indir, fname = os.path.split(f)
            fout.write("{0} {1}\n".format(fname, indir))
        fout.close()
        
        nsamps += 1
        nfiles += len(glob.glob(path+"/*.root"))
        nevts += info[idx]["nevents_out"]


print "Number of samples:", nsamps
print "Number of files:", nfiles
print "Number of events:", nevts



