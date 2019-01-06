import os
import subprocess
import datetime as dt

class Job:
    def __init__(self, id, input_file, output_file, exe, args=[], err_if_no_input=False, tree_name="mt2", do_sweeproot=True):
        self.done = False
        self.sweeprooted = False
        self.id = id
        self.input = input_file
        self.output = output_file
        self.exe = exe
        self.args = args
        self.err_if_no_input = err_if_no_input
        self.tree_name = tree_name
        self.do_sweeproot = do_sweeproot

class Sample:
    def __init__(self, name):
        self.name = name
        self.jobs = {}

    def add_job(self, job):
        if not isinstance(job, Job):
            raise TypeError("this isn't of type Job!")
        if job.err_if_no_input and not os.path.exists(job.input):
            raise Exception("input file {0} doesn't exist!".format(job.input))
        self.jobs[job.id] = job


def sweeprootJobs(jobs):
    cmds = {}
    for job in jobs:
        tn = job.tree_name
        if tn not in cmds:
            cmds[tn] = "sweeproot/sweepRoot -b -o {0}".format(tn)
        cmds[tn] += " "+job.output

    for tn in cmds:
        p = subprocess.Popen(cmds[tn].split(), stdout=subprocess.PIPE)
        out, err = p.communicate()
        for line in out.split('\n'):
            if line.startswith("BAD FILE"):
                print '\n'+line
                fout = line.split(":")[1].strip()
                subprocess.call("hadoop fs -rm -r "+fout.replace("/hadoop",""), shell=True)

    for job in jobs:
        job.sweeprooted = True

def writeConfig(tag, samp, jobs):
    os.system("mkdir -p {0}/logs/{1}".format(tag, samp))

    fout = open("{0}/condor_configs/{1}.cmd".format(tag, samp), 'w')

    fout.write("""
universe=vanilla
when_to_transfer_output = ON_EXIT
transfer_input_files=job_input/input.tar.xz
+DESIRED_Sites="T2_US_UCSD"
+remote_DESIRED_Sites="T2_US_UCSD"
+Owner = undefined
log={0}/logs/condor_submit.log
output={0}/logs/{1}/1e.$(Cluster).$(Process).out
error ={0}/logs/{1}/1e.$(Cluster).$(Process).err
notification=Never
x509userproxy=/tmp/x509up_u31592

""".format(tag, samp))

    for job in jobs:
        fout.write("executable={0}\n".format(job.exe))
        fout.write("transfer_executable=True\n")
        fout.write("arguments={0}\n".format(" ".join(job.args)))
        fout.write("queue\n\n")
                   
    fout.close()

def isCurrentlySubmitted(job, tag, condor_out):
    for line in condor_out:
        if tag not in line:
            continue
        if job.id not in line:
            continue
        if job.input not in line:
            continue

        return True

    return False

def writeSummary(d, tag, summarydir):
    fout = open(os.path.join(summarydir, "{0}.txt".format(tag)), 'w')
    fout.write("Last updated {0}\n".format(str(dt.datetime.now())))
    for s in sorted(d.keys(), key=lambda x: d[x].done, reverse=True):
        fout.write("{0:40s} : {1:4d} done, {2:4d} running, {3:4d} waiting\n".format(s, d[s].done, d[s].running, d[s].waiting))
    fout.close()




