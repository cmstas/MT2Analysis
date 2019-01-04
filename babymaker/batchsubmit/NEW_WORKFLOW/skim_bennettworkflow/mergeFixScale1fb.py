import sys
import glob
import numpy as np
import ROOT as r

if len(sys.argv) < 5:
    print "usage : python skim.py <# unskimmed events> <skimmed_dir> <extmerge_dir> <samp>"
    exit(0)

nevt_orig = int(sys.argv[1])
skimdir = sys.argv[2]
extmergedir = sys.argv[3]
samp = sys.argv[4]

ch = r.TChain("mt2")
infiles = glob.glob("{0}/{1}*.root".format(skimdir, samp))
for file in infiles:
    print "Adding file", file
    ch.Add(file)
xsec_min = ch.GetMinimum("evt_xsec")
xsec_max = ch.GetMaximum("evt_xsec")
kfactor = ch.GetMinimum("evt_kfactor")
filteff = ch.GetMinimum("evt_filter")
scale1fb_min = ch.GetMinimum("evt_scale1fb")
scale1fb_max = ch.GetMaximum("evt_scale1fb")

if (xsec_max-xsec_min)/xsec_min > 1e-2:
    print "ERROR: xsec varies across inputs! Min: {0}; Max: {1}".format(xsec_min, xsec_max)
    exit(1)

scale1fb = 1000.* xsec_min * kfactor * filteff / nevt_orig
print "input events:", nevt_orig
print "xsec*kfactor*filteff:", xsec_min*kfactor*filteff
print "new scale1fb:", scale1fb
print "old scale1fbs:", scale1fb_min, scale1fb_max

ch.SetBranchStatus("*", 1)
ch.SetBranchStatus("evt_scale1fb", 0)
ch.SetBranchStatus("evt_nEvts", 0)

outname = "{0}/{1}.root".format(extmergedir, samp)
print "\nWriting to:", outname
fout = r.TFile(outname, "RECREATE")

clone = ch.CloneTree(-1, "fast")
np_scale1fb = np.array([scale1fb], dtype=np.float32)
np_nEvts = np.array([int(nevt_orig)], dtype=int)

b_evt_scale1fb = clone.Branch("evt_scale1fb", np_scale1fb, "evt_scale1fb/F")
b_evt_nEvts = clone.Branch("evt_nEvts", np_nEvts, "evt_nEvts/l")

for i in range(clone.GetEntries()):
    clone.GetEntry(i)

    b_evt_scale1fb.Fill()
    b_evt_nEvts.Fill()

clone.SetMaxTreeSize(5000000000)

clone.Write()
fout.Close()

