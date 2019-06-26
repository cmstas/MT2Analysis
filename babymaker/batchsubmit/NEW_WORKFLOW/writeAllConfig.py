import sys, os
import glob
sys.path.append("ProjectMetis/scripts")
import dis_client
import json

from samples_2016 import samples_2016
from samples_2017 import samples_2017
from samples_2018 import samples_2018

checkForScale1fb = True

# tag = "V00-10-16_2016fullYear"
# tag = "V00-10-16_2017fullYear"
tag = "V00-10-18_2018fullYear"
# tag = "RebalanceAndSmear_V00-10-16_2016fullYear"
# tag = "RebalanceAndSmear_V00-10-16_2017fullYear"
# tag = "RebalanceAndSmear_V00-10-16_2018fullYear"
# samples = samples_2016
# samples = samples_2017
samples = samples_2018

# cms4tags = ["CMS4_V10-02-06", "CMS4_V10-02-05", "CMS4_V10-02-04", "CMS4_V09-04-17", "CMS4_V00-00-02_2017Sep27"]
# cms4tags = ["CMS4_V10-02-06", "CMS4_V10-02-05", "CMS4_V10-02-04", "CMS4_V09-04-19"]
cms4tags = ["CMS4_V10-02-08", "CMS4_V10-02-06", "CMS4_V10-02-05", "CMS4_V10-02-04"]

do = [
    "data",
    "ttbar",
    "wjets",
    "zinv",
    "dyjetsll",
    "qcd_ht",
    "singletop",
    "tttt",
    "gjets",
    "ttv",
    "diboson",
    "signal",
]

sigdir2018 = "/hadoop/cms/store/group/snt/run2_mc2018/"
sigdir2017 = "/hadoop/cms/store/group/snt/run2_mc2017/"
sigdir2016 = "/hadoop/cms/store/group/snt/run2_mc2016_94x/"

if checkForScale1fb:
    fs1fb = open("/home/users/bemarsh/CORE/Tools/datasetinfo/scale1fbs.txt")
    xsecs_in_scale1fb = {}
    for line in fs1fb:
        if line.strip().startswith("#"):
            continue
        ds = line.strip().split()[0]
        cms4tag = line.strip().split()[1]
        xsec = float(line.strip().split()[4])
        xsecs_in_scale1fb[(ds,cms4tag)] = xsec

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

        info = dis_client.query(ds, "snt")["response"]["payload"]
        if len(info)==0:
            if type=="signal":
                if "2016" in tag:
                    sigdir = sigdir2016
                    if "private" in ds:
                        sigdir = "/hadoop/cms/store/group/snt/run2_mc2016_94x_private/"
                if "2017" in tag:
                    sigdir = sigdir2017
                    if "private" in ds:
                        sigdir = "/hadoop/cms/store/group/snt/run2_mc2017_private/"
                if "2018" in tag:
                    sigdir = sigdir2018
                    if "private" in ds:
                        sigdir = "/hadoop/cms/store/group/snt/run2_mc2018_private/"

                cms3tag = "CMS4_V10-02-08" if "2018" in tag and "private" not in ds else "CMS4_V10-02-05"
                        
                if "private" in ds:
                    sigdir += ds.strip("/").replace("/","_").replace("MINIAODSIM","CMS4")+"_v1"
                else:
                    sigdir += ds.strip("/").replace("/","_")+"_" + cms3tag
                # print sigdir

                info = [{ "location" : sigdir,
                          "cms3tag"  : cms3tag,
                          "xsec" : 1.0,
                          "kfactor" : 1.0,
                          "filter_eff" : 1.0,
                          "nevents_out" : 1,
                          }]

            else:
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

        if not os.path.exists(path):
            path = path.rstrip('/')+"_todelete"
        if not os.path.exists(path):
            print "WARNING: path does not exist!", path
            continue

        if checkForScale1fb and type is not "data" and (ds, thisTag) not in xsecs_in_scale1fb:
            print "WARNING: sample does not have scale1fb info!", ds, thisTag

        samp = samples[type][ds][3]

        try:
            metadata = json.load(open(path+"/metadata.json"))
        except:
            if type=="signal":
                metadata = {"ijob_to_nevents" : {1:[1,1]}}
            else:
                print "WARNING! no metadata! Skipping", ds
                continue
        nevts_effective = 0
        for i in metadata["ijob_to_nevents"]:
            nevts_effective += metadata["ijob_to_nevents"][i][1]

        corr = 1.0
        if type not in ["data", "signal"]:
            snt_xsec = info[idx]["xsec"] * info[idx]["kfactor"] * info[idx]["filter_eff"]
            mt2_xsec = samples[type][ds][0] * samples[type][ds][1] * samples[type][ds][2]
            corr = mt2_xsec / snt_xsec
            if (ds, thisTag) in xsecs_in_scale1fb and abs(snt_xsec / xsecs_in_scale1fb[(ds, thisTag)] - 1) > 1e-3:
                print "ERROR: xsec from DIS doesn't match xsec in scale1fb.txt! Skipping", ds
                continue
        if abs(corr-1.0) > 0.2:
            print "WARNING: large xsec correction of {0:.3f} for sample {1}".format(corr, samp)

        fout = open("{0}/{1}.txt".format(outdir, samp), 'w')
        fout.write("# {0} {1} {2} {3} {4} {5}\n".format(ds, nevts_effective, info[idx]["xsec"], info[idx]["kfactor"], info[idx]["filter_eff"], corr))
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



