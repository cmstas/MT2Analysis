import sys, os
import glob
sys.path.append("ProjectMetis/scripts")
import dis_client

from samples_2016 import samples_2016
from samples_2017 import samples_2017
from samples_2018 import samples_2018

tag = "V00-10-07_2018fullYear_17Sep2018"
# samples = samples_2016
samples = samples_2018

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

nsamps = 0
nfiles = 0
nevts = 0

for type in samples:
    if type not in do:
        continue
    for ds in samples[type]:
        try:
            info = dis_client.query(ds, "snt")["response"]["payload"][0]
        except IndexError:
            print "WARNING: dataset not found!", ds
            continue
        path = info["location"]
        if "2018D" in ds and "v2" in ds:
            path = path.replace("V10-01-00","V10-02-02")

        samp = samples[type][ds][3]

        cmd = "./writeConfig.sh {0} {1}_{2}".format(path, tag, samp)
        # print cmd
        
        nsamps += 1
        nfiles += len(glob.glob(path+"/*.root"))
        nevts += info["nevents_out"]

        os.system(cmd)

os.system("mkdir -p configs_"+tag)
os.system("mv condor_{0}*.cmd configs_{0}".format(tag))

print "Number of samples:", nsamps
print "Number of files:", nfiles
print "Number of events:", nevts



