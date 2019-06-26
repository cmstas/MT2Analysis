import os,sys
import socket
import subprocess
import glob

TAG="V00-10-18_FullRunII"
# models = ["T1tttt","T1bbbb", "T1qqqq", "T2tt","T2bb", "T2qq","T2cc","T2bt","T2bW","rpvMonoPhi", "T5qqqqVV_mChg0p5mGlumLSP", "T5qqqqVV", "TprimeTprime_2xZ", "extraT2tt"]
models = ["T1tttt","T1bbbb", "T1qqqq", "T2tt","T2bb", "T2qq","T2cc","T2bt","T2bW","rpvMonoPhi","extraT2tt"]

write_report = True
if len(sys.argv) > 1 and sys.argv[1]=="1":
    TAG="V00-10-17_FullRunII_withDYContam"
    models = ["T5qqqqVV","T5qqqqWW","T1tttt","T2tt","T2bt","T2bW","extraT2tt"]

os.system("condor_release bemarsh")
cmd = "condor_q {0} -nobatch -wide".format("bemarsh")
if "uaf-1." in socket.gethostname():
    cmd = cmd.replace("-nobatch","")
p = subprocess.Popen(cmd.format("bemarsh").split(), stdout=subprocess.PIPE)
out,err = p.communicate()
condor_out = out.split("\n")

nactuallyrunning = 0
running = []
for line in condor_out:
    if TAG+"X" not in line+"X":
        continue
    if " R " in line:
        nactuallyrunning += 1
    point = line.split()[9]
    running.append(point)

print "CURRENTLY RUNNING:", len(running)

hns = set(line.strip() for line in open("/home/users/bemarsh/scratch/badhosts.txt").readlines())
progress = {}
for MODEL in models:
    print "Checking", MODEL
    points = [l.strip() for l in open("/home/users/bemarsh/analysis/mt2/current/MT2Analysis/scripts/CardMaker/cards_{0}/{1}/points.txt".format(TAG,MODEL)).readlines()]
    ndone = 0
    for point in points:
        if point in running:
            continue
        log = "/hadoop/cms/store/user/bemarsh/combine/limits/{0}_{1}/log_{2}.log".format(MODEL, TAG, point)
        done = False
        if os.path.exists(log):
            done = True
            try:
                text = open(log).read()
                hn = text.split("HOSTNAME")
                if len(hn)>2:
                    hns.add(hn[1])
                if "Observed" not in open(log).read():
                    done = False
                    print "Point {0} looks like it has bad output!".format(point)
                    if point not in ["T2bb_500_0","T2qq_500_0"]:
                        os.system("rm /hadoop/cms/store/user/bemarsh/combine/limits/{0}_{1}/*{2}.*".format(MODEL, TAG, point))
            except IOError:
                pass
        if not done:
            print "Re-submitting", point
            os.system("./submit.sh {0} {1} {2} 0".format(point, TAG, MODEL))
        else:
            ndone += 1

    progress[MODEL] = (ndone, len(points))

if write_report:
    fout = open("/home/users/bemarsh/public_html/mt2/limits/progress{0}.txt".format("" if len(sys.argv)<2 else sys.argv[1]), 'w')
    fout.write("----- LIMIT PROGRESS -----\n")
    for MODEL in models:
        fout.write("{0}: {1}/{2}\n".format(MODEL, *progress[MODEL]))
    fout.write("\n\nJobs running: {0}/{1}\n".format(nactuallyrunning,len(running)))
    fout.write("\n\nFailed hostnames:\n")
    hnout = open("/home/users/bemarsh/scratch/badhosts.txt", 'w')
    for hn in hns:
        fout.write(hn+'\n')
        hnout.write(hn+'\n')

    hnout.close()
    fout.close()
