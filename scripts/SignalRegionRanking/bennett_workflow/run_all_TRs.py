import os, sys
import glob
import subprocess
import time

MAXLOCALJOBS = 20

if len(sys.argv) < 3:
    print "usage: {0} <tag> <point>".format(sys.argv[0])
    exit(1)

TAG = sys.argv[1]
POINT = sys.argv[2]
MODEL = POINT.split("_")[0]

os.system("mkdir -p scratch/{0}/{1}".format(TAG, POINT))

trs = set()
for card in glob.glob("../../CardMaker/cards_{0}/{1}/{2}/*.txt".format(TAG,MODEL,POINT)):
    tr = card.split("datacard_")[-1].split("_m")[0]
    trs.add(tr)

cmds = []
for tr in trs:
    cmds.append("nice -n19 python run_TR_limit.py {0} {1} {2} &>> scratch/{0}/{2}/log.txt".format(TAG, tr, POINT))

ps = []
done = 0
current = 0
lasttime = 0
while done < len(trs):
    done = 0
    running = len(ps)
    for i in range(len(ps)):
        res = ps[i].poll()
        if res is not None:
            done += 1
            running -= 1

    for cmd in cmds[current:current+MAXLOCALJOBS-running]:
        ps.append(subprocess.Popen(cmd, shell=True))
        running += 1
        current += 1
    if done < len(trs):
        thistime = time.time()
        if thistime > lasttime + 20:
            lasttime = thistime
            print "{0}/{1} TRs done. {2} currently running. Checking again in 20 seconds...".format(done, len(trs), running)
        time.sleep(1)


