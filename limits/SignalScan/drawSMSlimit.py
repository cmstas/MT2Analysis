#!/usr/bin/env python
from math import *
import os, commands
from sys import argv,exit
from optparse import OptionParser
import ROOT

print "running:", argv

if len(argv)<2:
    print "Usage: "+argv[0]+" fileWithLimits.txt [doOneFold (default=False)]"
    exit(1)

INPUT = argv[1]
doOneFold = (argv[2]=='True' or argv[2]=='true') if len(argv)>2 else False

# get contours separately for the left and right side of the deltaM=Mtop diagonal (T2tt)
divideTopDiagonal = False

ROOT.gROOT.SetBatch(True)

models   = ["T1bbbb", "T1tttt","T1qqqq","T2qq","T2bb","T2tt","T2cc","T2bW","T2bt","T2WW","extraT2tt","rpvMonoPhi","T5qqqqWW","T5qqqqVV"]
model = "mymodel"
for m in models:
    if m in INPUT:
        model = m

print "model =", model

# xsfile = "SUSYCrossSections13TeVgluglu.root" if "T1" in model else "SUSYCrossSections13TeVstopstop.root" if model=="T2tt" or model=="T2bb" or model=="T2cc" else "SUSYCrossSections13TeVsquarkantisquark.root" if model=="T2qq" else "theXSfile.root"
f_xs = ROOT.TFile("xsec_susy_13tev_run2.root")
xshist = "h_xsec_gluino" if "T1" in model or "T5" in model else "h_xsec_stop" if model in ["T2tt","T2bb","T2cc","T2bW","T2bt","T2WW","extraT2tt"] else "h_xsec_squark" if model=="T2qq" else "h_xsec_gluino"
h_xs = f_xs.Get(xshist)

if model=="rpvMonoPhi":
    xs_map = {}
    with open("xsecs_monophi_NNPDF23_lo_as_0130_qed.txt") as fid:
        for line in fid:
            sp = line.split()
            mass = int(sp[0].strip("."))
            xs = float(sp[1])
            unc_stat = float(sp[2])
            unc_scale = max(float(sp[3]),float(sp[4]))
            unc_pdf = max(float(sp[5]),float(sp[6]))
            unc = sqrt(unc_stat**2 + unc_scale**2 + unc_pdf**2)
            xs_map[mass] = (xs,unc)
    sorted_masses = sorted(float(x) for x in xs_map.keys())
    def getMonoPhiXS(m):        
        if m < sorted_masses[0]:
            xs  =  xs_map[sorted_masses[0]][0] + (xs_map[sorted_masses[0]][0] - xs_map[sorted_masses[1]][0]) * (sorted_masses[0]-m) / (sorted_masses[1]-sorted_masses[0])
            unc =  xs_map[sorted_masses[0]][1] + (xs_map[sorted_masses[0]][1] - xs_map[sorted_masses[1]][1]) * (sorted_masses[0]-m) / (sorted_masses[1]-sorted_masses[0])
        elif m > sorted_masses[-1]:
            xs  =  xs_map[sorted_masses[-1]][0] - (xs_map[sorted_masses[-2]][0] - xs_map[sorted_masses[-1]][0]) * (m-sorted_masses[-1]) / (sorted_masses[-1]-sorted_masses[-2])
            unc =  xs_map[sorted_masses[-1]][1] - (xs_map[sorted_masses[-2]][1] - xs_map[sorted_masses[-1]][1]) * (m-sorted_masses[-1]) / (sorted_masses[-1]-sorted_masses[-2])
        else:
            i = 0
            while m > sorted_masses[i]:
                i += 1
            xs  =  xs_map[sorted_masses[i-1]][0] - (xs_map[sorted_masses[i-1]][0] - xs_map[sorted_masses[i]][0]) * (m-sorted_masses[i-1]) / (sorted_masses[i]-sorted_masses[i-1])
            unc =  xs_map[sorted_masses[i-1]][1] - (xs_map[sorted_masses[i-1]][1] - xs_map[sorted_masses[i]][1]) * (m-sorted_masses[i-1]) / (sorted_masses[i]-sorted_masses[i-1])
        return xs, unc

limits = ["obs", "exp", "ep1s", "em1s", "ep2s", "em2s", "op1s", "om1s","sig"]
#limits = ["obs", "exp"]

# coloum-limit map for txt files (n -> column n+1) 
fileMap = {"exp":2, "obs":3, "ep1s":4, "em1s":5, "ep2s":6, "em2s":7, "sig":8}


def getLimitYN ( h_lim_mu, r_exluded=1):
    name = h_lim_mu.GetName().replace("mu","yn")
    h_lim_yn = h_lim_mu.Clone(name)
    for ix in range(1,h_lim_yn.GetNbinsX()+1):
        for iy in range(1,h_lim_yn.GetNbinsY()+1):
            r = h_lim_yn.GetBinContent(ix,iy)
            h_lim_yn.SetBinContent(ix, iy, 1e-3 if r>r_exluded else 1 if r>0 else 0)
    return h_lim_yn
    
def getLimitXS ( h_lim_mu, h_xs):
    name = h_lim_mu.GetName().replace("mu","xs")
    h_lim_xs = h_lim_mu.Clone(name)
    for ix in range(1,h_lim_xs.GetNbinsX()+1):
        m = h_lim_xs.GetXaxis().GetBinCenter(ix)
        for iy in range(1,h_lim_xs.GetNbinsY()+1):
            r = h_lim_xs.GetBinContent(ix,iy)
            if model=="rpvMonoPhi":
                xs = getMonoPhiXS(m)[0]
            else:
                xs  = h_xs.GetBinContent(h_xs.FindBin(m))
            if model == "T2qq":  # xsecs in file are 10-fold. Correct for that here, to 8-fold
                xs = xs*0.8
            h_lim_xs.SetBinContent(ix, iy, r*xs)
    return h_lim_xs
    

def readLimitsFromFile(INPUT, fileMap, h_lims_mu0, h_lims_xs0, h_lims_yn0):
    rlim = {}
    for line in open(INPUT, "r"):
        if line.startswith("#"):
            continue
        m1        = float(line.split()[0])
        m2        = float(line.split()[1])
        for lim,index in fileMap.iteritems():
            rlim[lim]  = float(line.split()[index])

        # get xs for the given mass
        if model=="rpvMonoPhi":
            xs, exs = getMonoPhiXS(m1)
        else:
            xs  = h_xs.GetBinContent(h_xs.FindBin(m1))
            exs = h_xs.GetBinError  (h_xs.FindBin(m1))

        if model == "T2qq":  # xsecs in file are 10-fold. Correct for that here, to 8-fold
            xs, exs = xs*0.8, exs*0.8


        rlim['op1s'] = rlim['obs'] * xs / (xs+exs)
        rlim['om1s'] = rlim['obs'] * xs / (xs-exs)
    
        #fill the 2d limit histos
        binX=h_lims_mu0[lim].GetXaxis().FindBin(m1)
        binY=h_lims_mu0[lim].GetYaxis().FindBin(m2)
    
        for lim in limits:
            if lim != "sig":
                if model == "T2qq":
                    # limits calculated assuming 10-fold xsec. Correct for that here (we want either 8-fold or 1-fold)
                    if doOneFold:
                        rlim[lim] *= 10.0/1.0 # from 10-fold to 1-fold
                    else:
                        rlim[lim] *= 10.0/8.0 # from 10-fold to 8-fold
                if model == "rpvMonoPhi":
                    rlim[lim] *= 1.0 / xs # the xs in cards is hardcoded to 1.0 pb
            h_lims_mu0[lim].SetBinContent(binX, binY, rlim[lim])
            h_lims_xs0[lim].SetBinContent(binX, binY, rlim[lim]*xs)
            h_lims_yn0[lim].SetBinContent(binX, binY, 1 if rlim[lim]<1 else 1e-3)


def interpolateDiagonal(hist):
    # interpolate in diagonal direction to fill remaining missing holes
    # start from 15 bins away and finish in the diagonal
    Nx = hist.GetNbinsX() 
    Ny = hist.GetNbinsY()
    for i in range(14,-1,-1): # 14...0
        j=0
        while i+j<Nx and j<Ny:
           j+=1
           val1=hist.GetBinContent(i+j,j)
           if val1==0 or hist.GetBinContent(i+j+1,j+1)!=0:
               continue

           n=2
           while hist.GetBinContent(i+j+n,j+n)==0 and i+j+n<Nx and j+n<Ny:
               n+=1
           val2 = hist.GetBinContent(i+j+n,j+n)
           if val2==0:
               continue
           for nn in range(1,n):                    
               hist.SetBinContent(i+j+nn,j+nn,val1+(val2-val1)*nn/n) 


def extractSmoothedContour(hist, nSmooth=1):
    # if name contains "mu" histogram is signal strenght limit, otherwise it's a Yes/No limit
    isMu = "mu" in hist.GetName()
    #ROOT.gStyle.SetNumberContours(4 if isMu else 2)
    shist = hist.Clone(hist.GetName()+"_smoothed")

    # if smoothing a limit from mu, we need to modify the zeros outside the diagonal, otherwise the smoothing fools us in the diagonal transition
    if isMu:
        for ix in range(1, shist.GetNbinsX()):
            for iy in range(shist.GetNbinsY(),0,-1):
                if shist.GetBinContent(ix,iy)==0:
                    for iyy in range(iy, shist.GetNbinsY()):
                        shist.SetBinContent(ix,iyy, shist.GetBinContent(ix,iy-1))
                else:
                    break
        if model=="T2cc": # for T2cc do it also  bottom-up
            # after smoothing a limit from mu, we need to modify the zeros outside the diagonal, otherwise the contours come wrong for the diagonal 
            interpolateDiagonal(hist)
            for ix in range(1, shist.GetNbinsX()):
                for iy in range(0,shist.GetNbinsY()):
                    if shist.GetBinContent(ix,iy)==0:
                        for iyy in range(iy, 0, -1):
                            shist.SetBinContent(ix,iyy, shist.GetBinContent(ix,iy+1))
                    else:
                        break
            

    for s in range(nSmooth):
        #shist.Smooth() # default smoothing algorithm
        shist.Smooth(1,"k3a")  # k3a smoothing algorithm

    # after smoothing a limit from mu, we need to modify the zeros outside the diagonal, otherwise the contours come wrong for the diagonal
    if isMu:
        for ix in range(1,shist.GetNbinsX()):
            for iy in range(1,shist.GetNbinsY()):
                if hist.GetBinContent(ix,iy)==0:
                    shist.SetBinContent(ix,iy, 1.1)
        
    shist.SetMinimum(0)
    shist.SetMaximum(2 if isMu else 1)
    shist.SetContour(4 if isMu else 2)
    canvas = ROOT.TCanvas()
    shist.Draw("contz list")
    ROOT.gPad.Update()
    obj = ROOT.gROOT.GetListOfSpecials().FindObject("contours")
    list = obj.At(1 if isMu else 0)
    ## take largest graph
    #max_points = -1
    #for l in range(list.GetSize()):
    #    gr = list.At(l).Clone()
    #    n_points = gr.GetN()
    #    if n_points > max_points:
    #        graph = gr
    #        max_points = n_points

    graph = []
    for l in range(list.GetSize()):
        gr = list.At(l).Clone()
        n_points = gr.GetN()
        graph.append((n_points,gr))
    graph.sort(reverse=True)

    #graph = list.First().Clone()
    name = "gr_"
    name += shist.GetName()
    #graph.SetName(name)
    for i,g in enumerate(graph):
        g[1].SetName(name+(str(i)if i>0 else ""))
    #graph.Draw("sameC")
    del canvas
    del shist
    del obj
    del list
    if len(graph)==1: graph.append(graph[0])
    return [graph[0][1], graph[1][1]]  # return list of two largest graphs


def extractSmoothedContourRL(hist, nSmooth=1):
    hR, hL = hist.Clone("muR"), hist.Clone("muL")
    for ix in range(1,hist.GetNbinsX()+1):
        for iy in range(1,hist.GetNbinsY()+1):
            m1, m2 = hist.GetXaxis().GetBinLowEdge(ix), hist.GetYaxis().GetBinLowEdge(iy)
            #if m1-m2>=175:
            #    hL.SetBinContent(ix,iy, 0)
            #if m1-m2<=175:
            #    hR.SetBinContent(ix,iy, 0)
            if m1-m2>=150:
                hL.SetBinContent(ix,iy, 0)
            if m1-m2<=200:
                hR.SetBinContent(ix,iy, 0)

    # check if graph 0 makes sense, otherwise take graph 1
    nL = 0
    g = extractSmoothedContour(hL, nSmooth)
    x,y = ROOT.Double(0), ROOT.Double(0)
    g[0].GetPoint(8,x,y)
    if (x-y)>175: nL=1

    gR,gL = extractSmoothedContour(hR, nSmooth)[0], g[nL]
    gR.SetName("gr_"+hist.GetName()+"_smoothed")
    gL.SetName("gr_"+hist.GetName()+"_smoothed1")
    return [gR,gL]
    

def unexcludeDiagonal(hist, mSplit=175): 
    for ix in range(1,hist.GetNbinsX()+1):
        for iy in range(1,hist.GetNbinsY()+1):
            m1, m2 = hist.GetXaxis().GetBinLowEdge(ix), hist.GetYaxis().GetBinLowEdge(iy)
            val = hist.GetBinContent(ix,iy)
            if m1-m2==mSplit:
                hist.SetBinContent(ix,iy, max(1.5,val))

def mergeGraphs(graphs):
    mg = ROOT.TMultiGraph()
    mg.Add(graphs[0])
    mg.Add(graphs[1])
    g = ROOT.TGraph()
    g.Merge(mg.GetListOfGraphs())
    g.SetName(graphs[0].GetName())
    return g


h_lims_mu0 = {} # limits in signal-strength, original binning
h_lims_yn0 = {} # limits in excluded/non-exluded, original binning
h_lims_xs0 = {} # limits in cross-section, original binning

h_lims_mu   = {} # limits in signal-strength, interpolated
h_lims_yn   = {} # limits in excluded/non-exluded, interpolated
h_lims_xs   = {} # limits in cross-section, interpolated
g2_lims_mu  = {} # TGraph2D limits in signal-strength, automatic interpolation

m1min, m1max = 0, 2800
m2min, m2max = 0, 2400
xbinSize = 5 if model=="T2cc" else 25.0/3 if model=="T2tt" else 25
ybinSize = 5 if model=="T2cc" else 25.0/2 if model=="T2tt" else 25

mass1 = "mGlu" if "T1" in model else "mSq" if model=="T2qq" else "mSb" if model=="T2bb" else "mSt" if "T2tt" in model else "m1"
mass2 = "mLSP"

# create histos
for lim in limits:
    # uniform 25 GeV binning
    #h_lims_mu0[lim] = ROOT.TH2F(lim+"_mu0", model, (m1max-m1min+binSize)/binSize, m1min-binSize/2., m1max+binSize/2., (m2max-m2min+binSize)/binSize, m2min-binSize/2., m2max+binSize/2.)
    h_lims_mu0[lim] = ROOT.TH2F(lim+"_mu0", model, int((m1max-m1min+xbinSize)/xbinSize), m1min-xbinSize/2., m1max+xbinSize/2., int((m2max-m2min+2*ybinSize)/(ybinSize)), m2min-3*ybinSize/2., m2max+ybinSize/2.)
    h_lims_mu0[lim].SetXTitle(mass1)    
    h_lims_mu0[lim].SetYTitle(mass2)

    h_lims_yn0[lim] = h_lims_mu0[lim].Clone(h_lims_mu0[lim].GetName().replace("mu","yn"))
    h_lims_xs0[lim] = h_lims_mu0[lim].Clone(h_lims_mu0[lim].GetName().replace("mu","xs"))


# read txt file with limits (map defined above)
print "reading file..."
readLimitsFromFile(INPUT, fileMap, h_lims_mu0, h_lims_xs0, h_lims_yn0)

# so graph goes below mLSP=0 and there is no cut off above zero
def fillHorizontalBelowZero(hist):
    for ix in range(1,hist.GetNbinsX()+1):
        hist.SetBinContent( ix,1,hist.GetBinContent(ix,2) )

output = INPUT.replace(".txt", ("-OneFold" if (model=="T2qq" and doOneFold) else "") + ".root")
fout = ROOT.TFile(output, "RECREATE")
fout.cd()

print "interpolating..."
for lim in limits:
    fillHorizontalBelowZero(h_lims_mu0[lim])
    # interpolation done automatically by TGraph2D using Delaunay method
    g2_lims_mu[lim] = ROOT.TGraph2D(h_lims_mu0[lim])
    xbinSize_inter = xbinSize/2.
    #xbinSize_inter = xbinSize/2. if model!='T2cc' else ybinSize # bin size of interpolation graph (12.5 GeV as decided in dec7 meeting @ R40) 
    ybinSize_inter = ybinSize/2. if model!='T2cc' else ybinSize/2 # bin size of interpolation graph (12.5 GeV as decided in dec7 meeting @ R40) 
    g2_lims_mu[lim].SetNpx( int((g2_lims_mu[lim].GetXmax()-g2_lims_mu[lim].GetXmin())/xbinSize_inter) )
    g2_lims_mu[lim].SetNpy( int((g2_lims_mu[lim].GetYmax()-g2_lims_mu[lim].GetYmin())/ybinSize_inter) )
    h_lims_mu[lim] = g2_lims_mu[lim].GetHistogram()
    h_lims_mu[lim].SetName( h_lims_mu0[lim].GetName().replace("mu0","mu") )
             
    if lim=="sig":
        continue
    #remove negative or nan bins that appear in T2qq for no apparent reason
    for ix in range(1,h_lims_mu[lim].GetNbinsX()+1):
        for iy in range(1,h_lims_mu[lim].GetNbinsY()+1):
            if h_lims_mu[lim].GetBinContent(ix,iy) < 0: #if negative set to zero
                h_lims_mu[lim].SetBinContent(ix,iy,0)
            if isnan(h_lims_mu[lim].GetBinContent(ix,iy)): #if nan set to neighbour average
                val = (h_lims_mu[lim].GetBinContent(ix+1,iy) + h_lims_mu[lim].GetBinContent(ix-1,iy) + h_lims_mu[lim].GetBinContent(ix,iy+1) + h_lims_mu[lim].GetBinContent(ix,iy-1) )/4.0
                h_lims_mu[lim].SetBinContent(ix,iy,val)

    # hard-code here because the interpolation is being weird
    if lim.startswith("o") and model=="T1qqqq":
        h_lims_mu[lim].SetBinContent(42,90,0.98)
        h_lims_mu[lim].SetBinContent(43,91,0.98)
        h_lims_mu[lim].SetBinContent(44,92,0.98)



print "translating to x-sec and yes/no limits and saving 2d histos..."
for lim in limits:
    #if model=="T2tt":
    #    unexcludeDiagonal( h_lims_mu[lim])
    #if model=="T2bb":  # do this for summary plot as per FKW request
    #    unexcludeDiagonal( h_lims_mu[lim],25 )    
    #    unexcludeDiagonal( h_lims_mu[lim],37.5 )    
    
    h_lims_yn[lim] = getLimitYN ( h_lims_mu[lim] )
    h_lims_xs[lim] = getLimitXS ( h_lims_mu[lim], h_xs )
    
    h_lims_mu0[lim].Write()
    h_lims_xs0[lim].Write()
    h_lims_yn0[lim].Write()
    h_lims_mu [lim].Write()
    h_lims_xs [lim].Write()
    h_lims_yn [lim].Write()


graphs0 = {}
graphs1 = {}  # smoothed

print "extracting contours and saving graphs..."
for lim in limits:
    if lim=="sig":
        continue
    # get contour. choose the one with maximum number of points
    g_list = g2_lims_mu[lim].GetContourList(1.0)
    graphs = []
    for il in range(g_list.GetSize()):
        gr = g_list.At(il)
        n_points = gr.GetN()
        graphs.append((n_points,gr))
    graphs.sort(reverse=True)
    graphs0[lim] = graphs[0][1]
    #if model=='T2tt' and (lim=='ep2s' or lim=='ep1s'): # two unconnected contours are obtained for these two guys
    #if model=='T2tt' and (lim=='ep2s' or lim=='ep1s' or lim=='exp'): # two unconnected contours are obtained for these two guys
#    if model=='T2tt' and (lim=='ep2s' or lim=='ep1s' or lim=='op1s' or lim=='om1s' or lim=='em1s' or lim=='exp'  or lim=='obs'): # two unconnected contours are obtained for these two guys
#        graphs0[lim]=mergeGraphs([graphs[0][1],graphs[1][1]])
    graphs0[lim].SetName("gr_"+lim)
    graphs0[lim].Write()
    

print "smoothing..."
for lim in limits:
    if lim=="sig":
        continue
    nSmooth = 1 if model=="T2tt" else 2 if model!="T2qq" else 3
    if model!="T2tt" or not divideTopDiagonal:
        graphs = extractSmoothedContour(h_lims_mu[lim], nSmooth)
        graphs1[lim]=graphs[0]
        #if model=='T2tt' and (lim=='ep2s' or lim=='ep1s'): # two unconnected contours are obtained for these two guys
        #if model=='T2tt' and (lim=='ep2s' or lim=='ep1s' or lim=='exp'): # two unconnected contours are obtained for these two guys
        if model=='T2tt' and (lim=='ep2s' or lim=='om1s' or lim=='exp'  or lim=='ep1s'): # two unconnected contours are obtained for these two guys
            graphs1[lim]=mergeGraphs(graphs)
    else:
        graphs = extractSmoothedContourRL(h_lims_mu[lim], nSmooth)
        graphs1[lim]=mergeGraphs(graphs)
    #if model!="T2tt":
    #    graphs1[lim]=graphs[0]
    #else:
    #    graphs1[lim]=mergeGraphs(graphs)

    graphs1[lim].SetName( graphs1[lim].GetName().replace("_mu","") ) 
    graphs1[lim].Write()


print "saving x-check plots"
plotsDir = "xcheckPlots"
can = ROOT.TCanvas("can","can",600,600)
if( not os.path.isdir(plotsDir) ):
    os.system("mkdir "+plotsDir)
for lim in limits:
    if lim=="sig":
        continue
    ROOT.gStyle.SetNumberContours( 100 )
    xmin = 600 if "T1" in model else 150 if model=="T2tt" or model=='T2cc' else 300 if model=="T2bb" else 300 if model=="T2qq" else 0
    xmax = 1400 if model=="T2tt"  or model=="T2bb" else 1600 if model=="T2qq" else 800 if model=='T2cc' else 2800 if model=="T1bbbb" else 2800
    ymax = 700  if model=="T2tt" else 800 if model=="T2bb" or model=="T2cc" else 1200 if model=="T2qq" else 1800
    h_lims_yn0[lim].GetXaxis().SetRangeUser(xmin, xmax)
    h_lims_yn0[lim].GetYaxis().SetRangeUser(0   , ymax)
    h_lims_yn0[lim].Draw("colz")
    graphs0[lim].SetLineWidth(2)
    graphs0[lim].Draw("same")
    graphs1[lim].SetLineWidth(2)
    graphs1[lim].SetLineColor(2)
    graphs1[lim].Draw("same")
    can.SaveAs(plotsDir+"/" + model + "_" + lim + ".eps")
    can.SaveAs(plotsDir+"/" + model + "_" + lim + ".png")
    can.SaveAs(plotsDir+"/" + model + "_" + lim + ".pdf")


print "file "+output+" saved"
#fout.Close()
