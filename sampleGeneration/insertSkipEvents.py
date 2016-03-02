from sys import argv,exit

print "running:", argv

if len(argv)<2:
    print "Usage: "+argv[0]+" N1 N2"
    print "N1: events per job"
    print "N2: job number"
    exit(1)

N1 = int(argv[1])
N2 = int(argv[2])

print N1, "total number of jobs per file, job number: ", N2

SKIPNUMBER = int(N1 * N2)

line_skip = "    secondaryFileNames = cms.untracked.vstring()\n"
skip = "    skipEvents=cms.untracked.uint32(%d),\n"% SKIPNUMBER +line_skip 



template = "cfg_step2_AOD.py"
myCfg = ""
for line in open(template, "r"):
    if line == line_skip:
        line  = skip

    myCfg += line

f = open("cfgAOD_withSkip.py","w")
f.write(myCfg)
f.close()
          



