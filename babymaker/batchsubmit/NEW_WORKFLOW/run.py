import glob
import os
import subprocess
import socket
import time
import sys
import cPickle as pickle
from utils import *


tag = "V00-10-08_2016fullYear_17Jul2018"
# tag = "V00-10-08_2017fullYear_31Mar2018"
# tag = "V00-10-08_2018fullYear_17Sep2018"
skims = []
skims.append("base")
skims.append("ST")
skims.append("RS")
skims.append("qcd")

doMerge = False
forceRecheckExist = True

if os.path.exists("{0}/progress.pkl".format(tag)):
    d = pickle.load(open("{0}/progress.pkl".format(tag), 'rb'))
else:
    d = {}

sampnames = []
for f in glob.glob("{0}/file_lists/*.txt".format(tag)):
    sampnames.append(os.path.split(f)[1].split(".")[0])

# sampnames = ["qcd_ht1500to2000", "ttsl_powheg", "ttdl_powheg"]

for s in sampnames:
    if s not in d:
        d[s] = Sample(s)
    fl = open("{0}/file_lists/{1}.txt".format(tag, s))
    metadata = fl.readline().strip().split()
    nevents = int(metadata[2])
    d[s].nevents = nevents
    outdir = "/hadoop/cms/store/user/bemarsh/mt2babies/{0}_{1}".format(tag, s)
    job_ids = []
    for line in fl:
        fid = line.strip().split()[0].split(".")[0]
        indir = line.strip().split()[1]
        fidx = int(fid.split("_")[-1])
        fin = os.path.join(indir, fid+".root")
        bid = "mt2_baby_{0}".format(fidx)
        fbaby = os.path.join(outdir, "{0}.root".format(bid))
        exe = "wrapper.sh"
        args = [bid, fin, outdir, "1" if "RebalanceAndSmear" in tag else "0"]
        job_ids.append(bid)
        if bid not in d[s].jobs:
            d[s].add_job(Job(bid, fin, fbaby, exe, args, True, "mt2"))
        elif forceRecheckExist and not os.path.exists(d[s].jobs[bid].output):
            d[s].jobs[bid].done = False
            d[s].jobs[bid].sweeprooted = False

        for skim in skims:
            fskim = os.path.join(outdir, "skim_"+skim, "skim_{0}_{1}.root".format(skim, fidx))
            sid = "skim_{0}_{1}".format(skim, fidx)
            exe = "wrapper_skim.sh"
            args = [sid, fbaby, os.path.join(outdir, "skim_"+skim), skim]
            job_ids.append(sid)
            if sid not in d[s].jobs:
                d[s].add_job(Job(sid, fbaby, fskim, exe, args, False, "mt2"))
            elif forceRecheckExist and not os.path.exists(d[s].jobs[sid].output):
                d[s].jobs[sid].done = False
                d[s].jobs[sid].sweeprooted = False

    for bid in d[s].jobs.keys():
        if bid not in job_ids:
            print "Job deleted! {0} from sample {1}".format(bid, s)
            d[s].remove_job(bid)

savepkl = lambda x : pickle.dump(x, open("{0}/progress.pkl".format(tag), 'wb'))
savepkl(d)

for s in sampnames:
    os.system("mkdir -p {0}/condor_configs".format(tag))

summarydir = "/home/users/bemarsh/public_html/mt2/babymaking_summary"
os.system("mkdir -p "+summarydir)

alldone = False
while not alldone:

    # check for new files to sweeproot
    to_sweeproot = []
    print "Checking for new files..."
    for s in sampnames:
        for jid in d[s].jobs:
            job = d[s].jobs[jid]
            if not job.done and os.path.exists(job.output):
                to_sweeproot.append(job)
    print "Sweeprooting {0} new files...".format(len(to_sweeproot))
    sweeprootJobs(to_sweeproot)

    # get the current condor output
    subprocess.call("condor_release bemarsh > /dev/null", shell=True)
    p = subprocess.Popen("condor_q {0} -nobatch -wide".format(os.environ["USER"]).split(), stdout=subprocess.PIPE)
    out, err = p.communicate()
    condor_out = out.split("\n")

    alldone = True
    N_submitted = 0
    for s in sampnames:
        print "Checking sample", s
        n_babies_newly_done = 0
        n_skims_newly_done = 0
        
        to_submit = []
        d[s].done = 0
        d[s].running = 0
        d[s].waiting = 0
        for jid in d[s].jobs:
            job = d[s].jobs[jid]
            if not job.done and job.sweeprooted and os.path.exists(job.output):
                job.done = True
                if "skim" in job.id:
                    n_skims_newly_done += 1
                else:
                    n_babies_newly_done += 1
            elif not job.done:
                job.sweeprooted = False

            # ignore if we are already done
            if job.done:
                d[s].done += 1
                continue

            alldone = False

            # can't do job yet if input doesn't exist
            if not os.path.exists(job.input):
                d[s].waiting += 1
                continue

            if isCurrentlySubmitted(job, tag, condor_out):
                d[s].running += 1
            else:
                if len(condor_out)+N_submitted < 20000:
                    d[s].running += 1
                    to_submit.append(job)
                else:
                    d[s].waiting += 1

        if n_babies_newly_done > 0 or n_skims_newly_done > 0:
            print "** {0} babies and {1} skims newly finished!".format(n_babies_newly_done, n_skims_newly_done)

        if len(to_submit) > 0:
            print "** To submit {0} more jobs.".format(len(to_submit))
            N_submitted += len(to_submit)
            writeConfig(tag, s, to_submit)
            subprocess.call("condor_submit {0}/condor_configs/{1}.cmd > /dev/null".format(tag, s), shell=True)

    savepkl(d)

    writeSummary(d, tag, summarydir)

    if not alldone:
        print "Checking again in 20 min..."
        time.sleep(20 * 60)

print "WOO! You made it! All {0} jobs are done!".format(sum([len(d[s].jobs) for s in d]))

if not doMerge:
    exit(0)

print "Merging the skims now."

MAXLOCALJOBS = 25

basedir = "/nfs-7/userdata/mt2/"
logdir = "mergeLogs/"
os.system("mkdir -p "+logdir)

cmds = []
for s in sampnames:
    for skim in skims:
        hadoopdir = "/hadoop/cms/store/user/bemarsh/mt2babies/{0}_{1}/skim_{2}".format(tag, s, skim)
        nfsdir = os.path.join(basedir, "{0}_skim_{1}".format(tag, skim))
        os.system("mkdir -p "+nfsdir)
        os.system("chmod -R a+wrx "+nfsdir)
        cmds.append("nice -n 19 root -b -q -l mergeHadoopFiles.C'(\"{0}\",\"{1}/{2}.root\")' > {3}/log_merge_{4}_{2}.txt".format(hadoopdir,nfsdir,s,logdir,tag))

mergePs = []
current = 0
done = 0
while done < len(cmds):
    done = 0
    running = len(mergePs)
    for i in range(len(mergePs)):
        if mergePs[i].poll() != None:
            done += 1
            running -= 1
    for cmd in cmds[current:current+MAXLOCALJOBS-running]:
        mergePs.append(subprocess.Popen(cmd, shell=True))
        running += 1
        current += 1
    if done < len(cmds):
        print "{0}/{1} skims done merging. {2} currently running. Checking again in 5 min...".format(done, len(cmds), running)
        time.sleep(5 * 60)        

print "Done merging!"

# check if we need an extension merge
samp_counts = {}
samp_events = {}
maxcount = -1
for s in sampnames:
    base = s.split("_ext")[0]
    if base not in samp_counts:
        samp_counts[base] = 1
        samp_events[base] = d[s].nevents
    else:
        samp_counts[base] += 1
        samp_events[base] += d[s].nevents
    maxcount = max(maxcount, samp_counts[base])

if maxcount <= 1:
    sys.exit(0)

print "* Needs an extension merge!"
logdir = "/nfs-7/userdata/mt2/extmergeLogs_bemarsh"
subprocess.call("mkdir -p "+logdir, shell=True)
for skimname in skims:
    print "** Doing {0} skim".format(skimname)
    skimdir = "{0}/{1}_skim_{2}".format(basedir, tag, skimname)
    extmergedir = "{0}/extmerge".format(skimdir)
    subprocess.call("mkdir -p "+extmergedir, shell=True)
    subprocess.call("rm -f {0}/*.root".format(extmergedir), shell=True)

    ps = []
    for s in samp_counts:
        if samp_counts[s] == 1:
            for f in glob.glob("{0}/{1}*.root".format(skimdir, s)):
                subprocess.call("ln -s -t {0} {1}".format(extmergedir, f), shell=True)
        else:
            ps.append(subprocess.Popen("nice -n 19 python skim_bennettworkflow/mergeFixScale1fb.py {0} {1} {2} {3} > {4}/log_{5}_{3}.txt".format(samp_events[s], skimdir, extmergedir, s, logdir, skimname), shell=True))

    Nleft = 1
    while Nleft > 0:
        Nleft = 0
        for p in ps:
            if p.poll() is None:
                Nleft += 1
        if Nleft > 0:
            print "** Waiting on {0} samples. Checking again in 5 min...".format(Nleft)
            time.sleep(5 * 60)
