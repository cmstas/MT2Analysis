import ROOT
from math import sqrt
import re
from sys import argv,exit
import os
import subprocess
import ppmUtils as utils

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)

pi = ROOT.Math.Pi()

h_bar = ROOT.TH2F("h_VetoEtaPhi_bar","#eta-#phi Veto Bins, Barrel",170,-1.4,1.4,360,-pi,pi)
h_ecn = ROOT.TH2F("h_VetoEtaPhi_ecn","#eta-#phi Veto Bins, Neg Endcap",100,-2.4,-1.4,100,-pi,pi)
h_ecp = ROOT.TH2F("h_VetoEtaPhi_ecp","#eta-#phi Veto Bins, Pos Endcap",100,1.4,2.4,100,-pi,pi)

def BarVeto(x1, y1, x2, y2 ):
    for x in xrange(x1,x2+1):
        for y in xrange(y1,y2+1):
            h_bar.SetBinContent(x,y,1)

def ECnVeto(x1, y1, x2, y2 ):
    for x in xrange(x1,x2+1):
        for y in xrange(y1,y2+1):
            h_ecn.SetBinContent(x,y,1)

def ECpVeto(x1, y1, x2, y2 ):
    for x in xrange(x1,x2+1):
        for y in xrange(y1,y2+1):
            h_ecp.SetBinContent(x,y,1)


bar_pts = [ [25,348,35,358], [48,333,65,343], [84,1,86,360], [135,335,150,345], [20,290,35,305], [85,275,100,287], [135,274,150,285], [68,215,85,240], [65,182,79,188], [60,188,77,194],
            [85,226,93,240], [142,215,152,224], [32,134,45,141], [157,148,168,155], [8,105,19,117], [29,80,42,90], [32,47,45,57], [71,28,83,35], [3,23,12,31], [146,1,158,6],
            [1,1,2,360], [169,1,170,360]]
ecn_pts = [ [81,1,100,100], [1,25,23,30], [25,9,45,12], [77,60,83,62] ]
ecp_pts = [ [1,1,20,100], [11,25,23,28], [33,16,50,19], [26,40,40,43], [27,61,45,65] ]


for ptlist in bar_pts:
    BarVeto(ptlist[0],ptlist[1],ptlist[2],ptlist[3])
for ptlist in ecn_pts:
    ECnVeto(ptlist[0],ptlist[1],ptlist[2],ptlist[3])
for ptlist in ecp_pts:
    ECpVeto(ptlist[0],ptlist[1],ptlist[2],ptlist[3])

outfile = ROOT.TFile("VetoHists.root","RECREATE")
outfile.cd()
h_bar.Write()
h_ecn.Write()
h_ecp.Write()
outfile.Close()
