import glob
import os
import subprocess
import fnmatch
import re


#samples = ["datav2_data_Run2016[B-H]_(HTMHT|JetHT|MET)"]
#samples = ["datav2_data_Run2016[B-H]_(HTMHT|MET)"]
# samples = ["datav2_data_Run2016[B-H]_JetHT"]
#samples = ["datav2_data_Run2016[B-H]_MET"]
# samples = ["qcd_ht", "zinv_ht", "wjets_ht", "ttdl", "ttsl", "singletop", "ttw", "ttz"]
# samples = ["zinv_ht", "wjets_ht", "ttdl", "ttsl", "singletop", "ttw", "ttz"]
#samples = ["data_rereco_data_Run2016[B-G]_(HTMHT|JetHT|MET)", "datav2_data_Run2016H_(HTMHT|JetHT|MET)"]
# samples = ["data_rereco_data_Run2016[B-G]_JetHT", "datav2_data_Run2016H_JetHT"]
# samples = ["datav2_data_Run2016H_JetHT"]
# samples = ["data_rereco_data_Run2016[B-G]_JetHT"]
#samples = ["data_rereco_data_Run2016[E-G]_(HTMHT|MET)", "datav2_data_Run2016H_(HTMHT|MET)"]
#samples = ["data_rereco_data_Run2016[E-G]_JetHT", "datav2_data_Run2016H_JetHT"]
#samples = ["data_rereco_data_Run2016[B-D]_JetHT"]
#samples = ["data_rereco_data_Run2016C_MET"]
#samples = ["data_rereco_data_Run2016C_HTMHT"]
#samples = ["data_rereco_data_Run2016[B-D]_(JetHT|HTMHT|MET)"]
# samples = ["qcd_ht", "tt", "singletop", "wjets_ht", "zinv_ht"]
# samples = ["qcd_ht"]
samples = ["data_Run2017[B-F]_JetHT_31Mar2018"]
# samples = ["data_Run2016[B-H]_JetHT_17Jul2018"]

test = False
version="V00-10-04"
doRebalanceAndSmear = True
# tag="V00-10-01_json_294927-306462_31Mar2018_correctJECs"
tag="V00-10-04"
# tag="V00-10-01_94X_mc"
jrttag = "ptBinned_94x_JetID_PUID_BTagSFs_core2sigma"
# jrttag = "2016JRTs"
extratag = ""
# extratag = "_noBResp"
# extratag = "_core25"
# extratag = "_jetReweight"
# extratag = "_withMonojet_v2"
# extratag = "_noJERsmear_v2"
# extratag = "_noJERsmear_HEM2veto_pt20"
extratag = "_jerDOWN"
username = os.environ["USER"]
use_b_resp = True
make_baby = False
use_raw_hists = False
core_scale = 1.0
mean_shift = 0.0
tail_scale = 1.0
# cut_level = 2 if "qcd" in samples[0].lower() else 1
cut_level = 1
apply_weights = True
do_jet_reweighting = False
indir = "/hadoop/cms/store/user/bemarsh/mt2babies/"
numFilesPerJob = 4
append=False

isData = ("data" in samples[0].lower() and "noJERsmear" not in extratag)

x509file = subprocess.check_output(["find","/tmp/", "-maxdepth", "1", "-type", "f", "-user", username, "-regex", "^.*x509.*$"])
if not x509file:
    print "Could not find x509 file for user {0}. Please run voms-proxy-init -voms cms -valid 240:00 to obtain a valid proxy.  Exiting.".format(username)
    quit()

options = ""
if make_baby: options += "-b "
if use_raw_hists: options += "-h "
if not use_b_resp: options += "-j "
if doRebalanceAndSmear: options += "-r "
if apply_weights: options += "-w "
if isData: options += "-d "
options += "-c {0} -m {1} -t {2} -l {3} ".format(core_scale, mean_shift, tail_scale, cut_level)
options += "-f JetResponseTemplates_{0} ".format(jrttag)
if do_jet_reweighting:
    options += "-q njet_reweighting/weights_{0}_{1}.root ".format(version, jrttag+extratag.replace("_jetReweight",""))
if "jerup" in extratag.lower():
    options += "-u 1 "
if "jerdown" in extratag.lower():
    options += "-u -1 "

if "Run2016" in samples[0]:
    config_tag = "data_2016_94x"
elif "Run2017" in samples[0]:
    config_tag = "data_2017_31Mar2018"
else:
    config_tag = "mc_94x_Fall17"

options += "-g {0} ".format(config_tag)

os.system("mkdir -p configs_{0}".format(version))

suffix = ""
is_first = True
for sample in samples:
  re_sample = re.compile("RebalanceAndSmear_"+tag+"_"+sample)
  if "data" in sample:
    if "Run2016" in sample:
      suffix = "_data2016"
    if "Run2017" in sample:
      suffix = "_data2017"
    if "Run2018" in sample:
      suffix = "_data2018"
  else:
    suffix = "_"+sample.replace('_ht','').replace('_pt','')
  if not doRebalanceAndSmear:
    suffix += "_noRS"

  if not test:
    mode = 'w'
    if append and not is_first: mode = 'a'
    fid = open("configs_{0}/config{1}_{2}.cmd".format(version, suffix, jrttag+extratag),mode)

    if (not append) or is_first:
      fid.write("""
universe=Vanilla
when_to_transfer_output = ON_EXIT
transfer_input_files=wrapper.sh, job_input_{0}/input.tar.gz
+DESIRED_Sites="T2_US_UCSD"
+remote_DESIRED_Sites="T2_US_UCSD"
+Owner = undefined
log=/data/tmp/{1}/condor_submit_logs/smearing/condor_12_01_16.log
output=/data/tmp/{1}/condor_job_logs/smearing/1e.$(Cluster).$(Process).out
error =/data/tmp/{1}/condor_job_logs/smearing/1e.$(Cluster).$(Process).err
notification=Never
x509userproxy={2}

""".format(version, username, x509file))    
    
  for dirname in os.listdir(indir):
    m = re_sample.match(dirname)
    if not m: continue
    if test or not test:
      print "Writing {0} to config{1}.cmd".format(dirname,suffix)
    if not test:
      list_of_files = []
      njobs = 0
      for f in glob.glob(os.path.join("{0}/{1}".format(indir, dirname), "*.root")):        
        bn = f.split("/")[-1].split(".")[0]
        list_of_files.append(bn+".")
        if len(list_of_files) == numFilesPerJob:
          fid.write("executable=wrapper.sh\n")
          fid.write("transfer_executable=True\n")
          fid.write("arguments={0} {1} {2} /hadoop/cms/store/user/{3}/smearoutput/{4}_{5}/{6}/{7}\n".format(options, "/".join(f.split("/")[:-1]), ",".join(list_of_files), username, version, jrttag+extratag, suffix.strip("_"), dirname))
          fid.write("queue\n\n")
          njobs += 1
          list_of_files = []
      ## now, if there are still files left in the list, write them to config
      if len(list_of_files):
        fid.write("executable=wrapper.sh\n")
        fid.write("transfer_executable=True\n")
        fid.write("arguments={0} {1} {2} /hadoop/cms/store/user/{3}/smearoutput/{4}_{5}/{6}/{7}\n".format(options, "/".join(f.split("/")[:-1]), ",".join(list_of_files), username, version, jrttag+extratag, suffix.strip("_"), dirname))
        fid.write("queue\n\n")
        njobs += 1
        list_of_files = []          
      print "    {0} jobs".format(njobs)
  is_first = False
  

if not test:
    fid.close()
