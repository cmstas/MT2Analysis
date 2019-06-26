import ROOT as r
import os, sys
import time
import subprocess
# this assumes you've already made template datacards for this tag! (makeTemplateCards.py)

TAG = "V00-10-18_FullRunII"
MODEL = "extraT2tt"

MAXLOCALJOBS=20

if not os.path.exists(os.path.join("cards_"+TAG, "templates", "template_datacards.pkl")):
    print "ERROR! you must make template datacards first!"
    exit(1)

os.system("mkdir -p cards_{0}/logs".format(TAG))

if "80x" in TAG:
    f_nsig = r.TFile("../../babymaker/data/nsig_weights_{0}.root".format(MODEL))
elif "_10" in MODEL or "_50" in MODEL or "_200" in MODEL:
    # short-track
    f_nsig = r.TFile("../../babymaker/data/short_track/nsig_weights_{0}.root".format(MODEL))
else:
    f_nsig = r.TFile("../../babymaker/data/sigweights_Fall17/nsig_weights_{0}.root".format(MODEL).replace("T5qqqqWW","T5qqqqVV"))
h_nsig = f_nsig.Get("h_nsig")

if not h_nsig:
    print "ERROR! couldn't open nsig histogram!"
    exit(1)

y_binwidth = 25
y_max = 2100
x_binwidth = 25
x_max = 3000
points = []
cmds = []
if "T2cc" in MODEL:
    y_binwidth = 5
    y_max = 1100
if MODEL=="T2tt":
    y_binwidth = 12.5
    y_max = 1150
    x_binwidth = 25.0/3
    x_max = 2000
print "Searching for valid points..."
for im1 in range(0, int(round(x_max/x_binwidth))+1):
    for im2 in range(0, int(round(y_max/y_binwidth))+1):
        m1 = int(round(im1 * x_binwidth))
        m2 = int(round(im2 * y_binwidth))
        if m1%25 == 8:
            m1 += 1
        xbin = h_nsig.GetXaxis().FindBin(m1)
        ybin = h_nsig.GetYaxis().FindBin(m2)
        if h_nsig.GetBinContent(xbin, ybin) < 1:
            continue

        logname = "cards_{0}/logs/log_{1}_{2}_{3}".format(TAG, MODEL, m1, m2)
        cmd = "nice -n 19 python -u makeSignalPoint.py {0} {1} {2} {3} {4} &>> {5}".format(TAG, MODEL, m1, m2, 0, logname)
        points.append((m1,m2))
        cmds.append(cmd)

print "Found {0} points. Submitting jobs.".format(len(points))

success_points = set()
failed_points = set()
ps = []
done = 0
current = 0
lasttime = 0
while done < len(points):
    done = 0
    running = len(ps)
    for i in range(len(ps)):
        res = ps[i].poll()
        if res is not None:
            done += 1
            running -= 1
            if res==0:
                success_points.add(points[i])
            else:
                failed_points.add(points[i])
    for cmd in cmds[current:current+MAXLOCALJOBS-running]:
        ps.append(subprocess.Popen(cmd, shell=True))
        running += 1
        current += 1
    if done < len(points):
        thistime = time.time()
        if thistime > lasttime + 20:
            lasttime = thistime
            print "{0}/{1} points done. {2} currently running. Checking again in 20 seconds...".format(done, len(points), running)
        time.sleep(1)


fout = open(os.path.join("cards_"+TAG, MODEL, "points.txt"), 'w')
for m1,m2 in sorted(list(success_points)):
    fout.write("{0}_{1}_{2}\n".format(MODEL,m1,m2))
fout.close()

fout = open(os.path.join("cards_"+TAG, MODEL, "failed_points.txt"), 'w')
for m1,m2 in sorted(list(failed_points)):
    fout.write("{0}_{1}_{2}\n".format(MODEL,m1,m2))
fout.close()
