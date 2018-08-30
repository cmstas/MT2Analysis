import glob
import os
import subprocess
import socket
import time
import sys

# tag = "V00-10-01_json_314472-317696_2017C_JECs"
# tag = "V00-10-04_2016fullYear_17Jul2018"
# tag = "V00-10-04_2017fullYear_31Mar2018"
tag = "V00-10-04_2018_json_314472-317696"
# tag = "V00-10-03_94x_2017_mc"
# tag = "V00-10-03_2018_HEmiss_MC"
# tag = "V00-10-03_2018_HEmiss_data_relval"
# tag = "V00-10-01_json_294927-306462_31Mar2018_correctJECs"
# tag = "RebalanceAndSmear_V00-10-01_json_294927-306462_31Mar2018_correctJECs"
# tag = "RebalanceAndSmear_V00-10-01_94X_mc"

submitJobs        = True  # set to False if you've already submitted jobs and just want to monitor, or if you just want to check for missing files
sweeprootExisting = False # set to False to skip sweeprooting on existing files. For if you just want to check for new cms4/resubmit
removeLogs        = False # remove all condor log files after successful completion
doMerge           = True  # set to False to stop after all jobs have been finished/sweeprooted
doSkim            = True  # set to False to stop after merging
doQCDSkim         = True
doRSSkim          = True
doTriggerSkim     = False

MAXLOCALJOBS = 20

def getCompletedFiles():
    global tag, samples
    files = set()
    for s in samples:
        files.update(set(glob.glob("/hadoop/cms/store/user/bemarsh/mt2babies/{0}_{1}/*.root".format(tag,s))))
    return files

def sweepRoot(files):
    global samples, tag

    # cmd = "sweeproot/sweepRoot -b -o mt2 -t mt2"
    cmd = "sweeproot/sweepRoot -b -o mt2"
    for fname in files:
        cmd += " "+fname
        sweeprooted.add(fname)

    # print '\n'+cmd
    p = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE)
    out, err = p.communicate()
    # print out
    for line in out.split('\n'):
        if line.startswith("BAD FILE"):
            print '\n'+line
            sweeprooted.remove(line.split(":")[1].strip())
            subprocess.call("hadoop fs -rm -r "+line.split(":")[1].replace("/hadoop",""), shell=True)
    

def deleteLogFiles():
    global tag
    subprocess.call("rm -r /data/tmp/bemarsh/condor_job_logs/{0}*/*.out".format(tag), shell=True)
    subprocess.call("rm -r /data/tmp/bemarsh/condor_job_logs/{0}*/*.err".format(tag), shell=True)
    subprocess.call("rm -r /data/tmp/bemarsh/condor_submit_logs/{0}*/*.log".format(tag), shell=True)
    
    
def watchCondorJobs(sweeprootWhileWaiting=False):
    global sweeprooted, haveDeletedLogs
    print "* Waiting and watching..."
    NJOBS = 1
    while NJOBS > 0:
        if "uaf-10" in socket.gethostname():
            out = subprocess.check_output(["condor_q", "bemarsh"]).split('\n')[-4].split(":")[1].strip()
            NJOBS = int(out.split()[0])
            nidle = int(out.split(";")[1].split(",")[2].strip().split()[0])
            nrunning = int(out.split(";")[1].split(",")[3].strip().split()[0])
            nheld = int(out.split(";")[1].split(",")[4].strip().split()[0])
        else:
            out = subprocess.check_output(["condor_q", "bemarsh"]).split('\n')[-2]
            NJOBS = int(out.split()[0])
            nidle = int(out.split(";")[1].split(",")[2].strip().split()[0])
            nrunning = int(out.split(";")[1].split(",")[3].strip().split()[0])
            nheld = int(out.split(";")[1].split(",")[4].strip().split()[0])
        print "  {0} jobs left. {1} idle, {2} running, {3} held".format(NJOBS, nidle, nrunning, nheld)
        if nheld > 0:
            print "* Releasing held jobs..."
            os.system("condor_release bemarsh")
        if NJOBS > 0:
            print "  Checking again in 5 min..."
            curTime = time.time()
            if sweeprootWhileWaiting:
                newFiles = getCompletedFiles().difference(sweeprooted)
                if len(newFiles) > 0:
                    print "  Sweeprooting {0} files while waiting...".format(len(newFiles)),
                    sweepRoot(newFiles)
                    if removeLogs:
                        deleteLogFiles()
                    print "Done!"
            remainingTime = 5*60 - (time.time()-curTime)
            # remainingTime = 10 - (time.time()-curTime)
            if remainingTime > 0:
                time.sleep(remainingTime)
    print "* condor jobs finished!"


        
print "* Running babymaker for tag \"{0}\"".format(tag)
samples = []
for fname in glob.glob("configs_{0}/*.cmd".format(tag)):
    fname = fname.split("/")[1]
    pos = fname.find(tag)
    samp = fname[pos+len(tag)+1:].split(".")[0]
    samples.append(samp)

print "* Found {0} samples:".format(len(samples))
for s in samples:
    print "   ", s

if not sweeprootExisting:
    sweeprooted = getCompletedFiles().copy()
else:
    sweeprooted = set()

if submitJobs:
    print "* Submitting condor jobs"
    for s in samples:
        print s
        subprocess.call("condor_submit configs_{0}/condor_{0}_{1}.cmd".format(tag, s), shell=True)


NresubmitFiles = 1
while NresubmitFiles > 0:
    watchCondorJobs(sweeprootWhileWaiting=True)

    newFiles = getCompletedFiles().difference(sweeprooted)
    if len(newFiles)>0:
        print "* sweeprooting remaining files..."
        sweepRoot(newFiles)
                
    subprocess.call("mkdir -p configs_{0}_resubmit".format(tag), shell=True)
    if os.listdir("configs_{0}_resubmit".format(tag)) != []:
        subprocess.call("rm -r configs_{0}_resubmit/*".format(tag), shell=True)
    subprocess.call("./checkAllConfig.sh configs_{0}".format(tag), shell=True)

    resubmitFiles = glob.glob("configs_{0}_resubmit/*.cmd".format(tag))
    NresubmitFiles = len(resubmitFiles)
    if NresubmitFiles > 0:
        for f in resubmitFiles:
            subprocess.call("condor_submit "+f+" > /dev/null", shell=True)

print "* WOO! All files are good!"

if not doMerge:
    sys.exit(0)

print "* Merging..."

hadoopdir = "/hadoop/cms/store/user/bemarsh/mt2babies/"
outdir = "/nfs-6/userdata/mt2/"+tag
logdir = "mergeLogs"
subprocess.call("mkdir -p "+outdir, shell=True)
subprocess.call("rm -f "+outdir+"/*.root", shell=True)
subprocess.call("mkdir -p "+logdir, shell=True)
subprocess.call("chmod -R a+wrx "+outdir, shell=True)
mergePs = []
current = 0
done = 0
while done < len(samples):
    done = 0    
    running = len(mergePs)
    for i in range(len(mergePs)):
        if mergePs[i].poll() != None:
            done += 1
            running -= 1
    for s in samples[current:current+MAXLOCALJOBS-running]:
        mergePs.append(subprocess.Popen("nice -n 19 root -b -q -l mergeHadoopFiles.C'(\"{0}/{1}_{2}\",\"{3}/{2}.root\")' > {4}/log_merge_{1}_{2}.txt".format(hadoopdir,tag,s,outdir,logdir), shell=True))
        running += 1
        current += 1
    if done < len(samples):
        print "{0}/{1} samples done merging. {2} currently running. Checking again in 5 min...".format(done, len(samples), running)
        time.sleep(5 * 60)

print "* Done Merging!"

if not doSkim and not doQCDSkim and not doRSSkim:
    sys.exit(0)

print "* Skimming..."
skimdir = "/nfs-6/userdata/mt2/{0}_skim".format(tag)
qcdskimdir = "/nfs-6/userdata/mt2/{0}_qcd_skim".format(tag)
RSskimdir = "/nfs-6/userdata/mt2/{0}_RS_skim".format(tag)
triggerskimdir = "/nfs-6/userdata/mt2/{0}_trigger_skim".format(tag)
logdir = "/nfs-6/userdata/mt2/skimLogs_bemarsh"
if doSkim:    
    subprocess.call("mkdir -p "+skimdir, shell=True)
    subprocess.call("rm -f "+skimdir+"/*.root", shell=True)
if doQCDSkim: 
    subprocess.call("mkdir -p "+qcdskimdir, shell=True)
    subprocess.call("rm -f "+qcdskimdir+"/*.root", shell=True)
if doRSSkim:  
    subprocess.call("mkdir -p "+RSskimdir, shell=True)
    subprocess.call("rm -f "+RSskimdir+"/*.root", shell=True)
if doTriggerSkim:  
    subprocess.call("mkdir -p "+Triggerskimdir, shell=True)
    subprocess.call("rm -f "+Triggerskimdir+"/*.root", shell=True)
skim_commands = []
for s in samples:
    if doSkim:
        skim_commands.append("nice -n 19 root -b -q -l skim_bennettworkflow/skim_base.C'(\"{0}\",\"{1}\",\"{2}\")' > {3}/log_{2}.txt".format(outdir,skimdir,s,logdir))
    if doQCDSkim:
        skim_commands.append("nice -n 19 root -b -q -l skim_bennettworkflow/skim_qcd.C'(\"{0}\",\"{1}\",\"{2}\")' > {3}/log_QCD_{2}.txt".format(outdir,qcdskimdir,s,logdir))
    if doRSSkim:
        skim_commands.append("nice -n 19 root -b -q -l skim_bennettworkflow/skim_RS.C'(\"{0}\",\"{1}\",\"{2}\")' > {3}/log_RS_{2}.txt".format(outdir,RSskimdir,s,logdir))
    if doTriggerSkim:
        skim_commands.append("nice -n 19 root -b -q -l skim_bennettworkflow/skim_trigger.C'(\"{0}\",\"{1}\",\"{2}\")' > {3}/log_trigger_{2}.txt".format(outdir,triggerskimdir,s,logdir))
skimPs = []
current = 0
done = 0
while done < len(skim_commands):
    done = 0    
    running = len(skimPs)
    for i in range(len(skimPs)):
        if skimPs[i].poll() != None:
            done += 1
            running -= 1
    for s in skim_commands[current:current+MAXLOCALJOBS-running]:
        skimPs.append(subprocess.Popen(s, shell=True))
        running += 1
        current += 1
    if done < len(skim_commands):
        print "{0}/{1} skims complete. {2} currently running. Checking again in 5 min...".format(done, len(skim_commands), running)
        time.sleep(5 * 60)

print "* Done skimming!"

