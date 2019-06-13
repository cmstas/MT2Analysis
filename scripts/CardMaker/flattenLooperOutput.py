import sys, os
import ROOT as r

if len(sys.argv) < 3:
    print "usage: {0} <indir> <model>".format(sys.argv[0])
    exit(1)

indir = sys.argv[1]
MODEL = sys.argv[2]

outdir = os.path.join(indir, "flattened_signal", MODEL)
# if os.path.exists(outdir):
#     print "looks like this has already been done for this model!"
#     exit(0)

os.system("mkdir -p "+outdir)

if "80x" in indir:
    f_nsig = r.TFile("../../babymaker/data/nsig_weights_{0}.root".format(MODEL))
elif "_10" in MODEL or "_50" in MODEL or "_200" in MODEL:
    # short-track
    f_nsig = r.TFile("../../babymaker/data/short_track/nsig_weights_{0}.root".format(MODEL))
else:
    f_nsig = r.TFile("../../babymaker/data/sigweights_Fall17/nsig_weights_{0}.root".format(MODEL))
h_nsig = f_nsig.Get("h_nsig")

if not h_nsig:
    print "ERROR! couldn't open nsig histogram!"
    exit(1)

fin = r.TFile(os.path.join(indir, MODEL+".root"))

y_binwidth = 25
y_max = 2100
x_binwidth = 25
x_max = 3000
points = []
cmds = []
if "T2cc" in MODEL:
    y_binwidth = 5
    y_max = 800
if MODEL=="T2tt":
    y_binwidth = 12.5
    y_max = 1150
    x_binwidth = 25.0/3
    x_max = 2000

found_some = False
points = []
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
        if os.path.exists(os.path.join(outdir, "{0}_{1}_{2}.root".format(MODEL,m1,m2))):
            found_some = True
            continue
        points.append((m1,m2))
print "Found {0} points".format(len(points))
if len(points)==0:
    exit(0)
if found_some:
    print "Some points already exist, so we're skipping them!"

iterator = fin.GetListOfKeys()
for key in iterator:
    dirname = key.GetTitle()
    if "sr" not in dirname and "crsl" not in dirname and "crdy" not in dirname:
        continue
    if "T5qqqqVV" not in MODEL and "crdy" in dirname:
        continue
    if "t" not in MODEL and "T5qqqq" not in MODEL and "W" not in MODEL and "crsl" in dirname:
        continue
    if "base" in dirname or "ttbar" in dirname or "wjets" in dirname or "mu" in dirname or "el" in dirname:
        continue

    print "Doing "+dirname

    d = fin.Get(dirname)
    hists = d.GetListOfKeys()
    for hkey in hists:
        hist = hkey.GetName()
        if "sigscan" not in hist:
            continue
        h = d.Get(hist)
        if type(h) != r.TH3D:
            continue

        for m1, m2 in points:
            point = "{0}_{1}_{2}".format(MODEL, m1, m2)
            bin1 = h.GetYaxis().FindBin(m1)
            bin2 = h.GetZaxis().FindBin(m2)
            fout = r.TFile(os.path.join(outdir, point+".root"), "UPDATE")
            dout = fout.Get(dirname)
            if not dout:
                dout = fout.mkdir(dirname)
            dout.cd()
            hout = h.ProjectionX(hist.replace("_sigscan",""), bin1, bin1, bin2, bin2)
            hout.Write()
            fout.Close()

        del h





