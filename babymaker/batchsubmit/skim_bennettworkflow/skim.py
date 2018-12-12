import sys
import glob
import ROOT as r

if len(sys.argv) < 5:
    print "usage : python skim.py <selection file> <indir> <outdir> <samplename>"
    print "        Selection file is a text file with the skim selection."
    print "        If there are multiple lines they will be appended together as strings."
    print "        Everything after a '//' will be ignored"
    exit(0)

selfile = sys.argv[1]
indir = sys.argv[2]
outdir = sys.argv[3]
samp = sys.argv[4]

with open(selfile) as f:
    sel = ""
    for line in f:
        line = line.split("//")[0].strip()
        if line.startswith("#"):
            samptype = line.split("#")[1]
            if samptype in samp:
                sel += line.split("#")[-1].strip()
        else:
            sel += line

print "Skimming with selection:", sel
print ""

ch = r.TChain("mt2")
infiles = glob.glob("{0}/{1}*.root".format(indir, samp))
for file in infiles:
    print "Adding file", file
    ch.Add(file)

outname = "{0}/{1}.root".format(outdir, samp)
print "\nWriting to:", outname

r.TTree.SetMaxTreeSize(2500000000)

nevt = ch.GetEntries()
print "\nInput has {0} entries".format(nevt)

fout = r.TFile(outname, "RECREATE")
outtree = ch.CopyTree(sel)
nevt_out = outtree.GetEntries()
print "Output has {0} entries".format(nevt_out)
print "Reduction factor of", 1.0*nevt/nevt_out
outtree.Write()
fout.Close()

