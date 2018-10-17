import operator
import sys
import ROOT

ROOT.gROOT.SetBatch(True)

lumi=150 # /fb

signal_point = str(sys.argv[1])
m1 = signal_point.split("_")[1]

filename="Ranking_"+signal_point+".txt"

xsecFileName="../..//babymaker/data/xsec_susy_13tev.root"

if "T1" in signal_point:
    hname = "h_xsec_gluino"
elif "T2tt" in signal_point:
    hname="h_xsec_stop"
else:
    hname="h_xsec_squark"

print "m1="+m1
print "pulling xsec from: ", xsecFileName, hname

tfile=ROOT.TFile.Open(xsecFileName,"r")
h_xsec=tfile.Get(hname)
xsec = h_xsec.GetBinContent(h_xsec.FindBin(int(m1)))
print "xsec="+str(xsec)

exp={}
obs={}
sigyield={}

# limits are listed as expect obs
for line in open(filename):
    exp[(line.split()[0])]=round(float(line.split()[1]),3)
    obs[(line.split()[0])]=round(float(line.split()[2]),3)
    sigyield[(line.split()[0])]=round(float(line.split()[3]),3)

output=open('Ranking_'+signal_point+'_ranked.txt', 'w+')
limits=sorted(exp.items(), key=operator.itemgetter(1))    
for l in limits:
    output.write(str(l[0])+' & '+str(l[1])+' & '+str(obs[l[0]]) + ' & ' + str(sigyield[l[0]]) + ' & ' + ('%.3f' % (float(sigyield[l[0]])/(xsec*1000*lumi)*100.)) +'\\\\\n')

print "Percentage of ALL signal events falling into best bin:", str(float(sigyield[l[0]])/(xsec*1000*lumi)*100.)+"%" 
print "Signal yield in best bin:", sigyield[l[0]] 
print "Signal cross section:", str(xsec)
