import ROOT
import os,sys
sys.path.append("../CardMaker")
from math import sqrt
import ResultPlotUtils as utils
from Datacard import Datacard
import ppmUtils
from array import *

ROOT.gROOT.SetBatch(1)

def MakePlot(ht_reg, outdir, userMax=None, doPostfit=False, scalePred=1.0, ratioRange=None, doPull=False,
             drawObs=False, drawSignal=False, sigName="", makeSBplot=False):

    if (not doPostfit or drawSignal) and len(utils.datacards)==0:
        print "ERROR: must load pickled datacards first! (utils.LoadPickledDatacards)"
        return

    if doPostfit and utils.postfit_file is None:
        print "ERROR: must load postfit file first! (utils.LoadPostfitFile)"
        print "       (make with combine -M FitDiagnostics)"
        return

    jbj_regs = utils.GetJBJregions(ht_reg)

    #list of lists, one per jbj region, of low edges of MT2 bins
    mt2bins = utils.GetMT2bins(ht_reg)

    # nBinsTotal = sum([len(bins)-1 for bins in mt2bins]) + 1
    nBinsTotal = sum([(len(bins)-1 if len(bins)>2 else len(bins)) for bins in mt2bins]) # always at least 2 for readability
    nBinsPadding = 2 if ht_reg in ["HT575to1200", "HT1200to1500", "HT1500toInf"] else 0 # extra bins to add to right for readability
    nBinsTotal += nBinsPadding
    bkg_processes = ["zinv","llep","qcd"]
    nBkgs = len(bkg_processes)

    ## setup histograms
    h_data = ROOT.TH1D("h_data","",nBinsTotal,0,nBinsTotal)
    h_sig = ROOT.TH1D("h_sig","",nBinsTotal,0,nBinsTotal)
    h_SB = ROOT.TH1D("h_SB","",nBinsTotal,0,nBinsTotal)
    h_bkg_vec = []
    for i,proc in enumerate(bkg_processes):
        h_bkg_vec.append(ROOT.TH1D("h_"+proc,"",nBinsTotal,0,nBinsTotal))
    g_unc = ROOT.TGraphAsymmErrors() # graph to store prediction uncertainties
    g_unc_ratio = ROOT.TGraphAsymmErrors() # graph to store prediction uncertainties

    ## fill histograms
    ibin = 0
    binLabels = []
    SB_ratio = []
    
    for ijbj, jbj_reg in enumerate(jbj_regs):
        for imt2 in range(len(mt2bins[ijbj])-1):
            ibin += 1
            mt2left = mt2bins[ijbj][imt2]
            mt2right = mt2bins[ijbj][imt2+1]
            mt2name = utils.GetMT2name(mt2left,mt2right)
            
            if ht_reg != "monojet":
                datacard_name = "{0}_{1}_{2}".format(ht_reg,jbj_reg,mt2name)
            else:
                ht_name = "HT{0}to{1}".format(mt2left,mt2right)
                ht_name = ht_name.replace("-1","Inf")
                datacard_name = "{0}_{1}_{2}".format(ht_name,jbj_reg,"m0toInf")

            if not doPostfit:
                dc = utils.datacards[datacard_name]
                
                # get yields. first entry is data, rest are background predictions
                obs = int(round(dc.GetObservation()))
                yields = dc.GetBackgroundRates()
                for bkg in yields:
                    if yields[bkg] < 0:
                        print "WARNING: negative prediction for bkg {0} in card {1}. Setting to 0.0".format(bkg, datacard_name)
                        yields[bkg] = 0.0
                    yields[bkg] *= scalePred
                h_data.SetBinContent(ibin, obs)
                for i,bkg in enumerate(bkg_processes):
                    h_bkg_vec[i].SetBinContent(ibin, yields[bkg])
                tot_pred = sum(yields.values())

                sig = dc.GetSignalRate()
                sigerr = dc.GetTotalUncertainty(signal=True)[0]
                h_sig.SetBinContent(ibin, sig)
                h_sig.SetBinError(ibin, sigerr)

                # get uncertainties
                tot_unc_up, tot_unc_down = dc.GetTotalUncertainty()
                thisPoint = g_unc.GetN()
                g_unc.SetPoint(thisPoint, ibin-0.5, tot_pred)
                g_unc.SetPointError(thisPoint, 0.5, 0.5, tot_unc_down, tot_unc_up)            
                g_unc_ratio.SetPoint(thisPoint, ibin-0.5, 1)
                g_unc_ratio.SetPointError(thisPoint, 0.5, 0.5, tot_unc_down/tot_pred, tot_unc_up/tot_pred)

                # divide S by total error (signal syst + background syst + sqrt(pred) stat uncert on obs) for a rough measure of expected significance
                SB_ratio.append((datacard_name, sig / sqrt(tot_unc_up**2 + sigerr**2 + tot_pred)))
                h_SB.SetBinContent(ibin, SB_ratio[-1][1])

            else:
                d = utils.postfit_file.Get("shapes_fit_b/{0}".format(datacard_name))
                yields = {}
                for bkg in bkg_processes:
                    yields[bkg] = 0.0
                for key in d.GetListOfKeys():
                    name = key.GetName()
                    for bkg in bkg_processes:
                        if name.startswith(bkg):
                            h = d.Get(name)
                            yields[bkg] += h.GetBinContent(1)
                for i,bkg in enumerate(bkg_processes):
                    h_bkg_vec[i].SetBinContent(ibin, yields[bkg])
                tot_pred = sum(yields.values())
                x,y = ROOT.Double(), ROOT.Double()
                d.Get("data").GetPoint(0, x, y)
                obs = int(y)
                h_data.SetBinContent(ibin, obs)
                
                unc = d.Get("total_background").GetBinError(1)
                thisPoint = g_unc.GetN()
                g_unc.SetPoint(thisPoint, ibin-0.5, tot_pred)
                g_unc.SetPointError(thisPoint, 0.5, 0.5, unc, unc)
                g_unc_ratio.SetPoint(thisPoint, ibin-0.5, 1)
                g_unc_ratio.SetPointError(thisPoint, 0.5, 0.5, unc/tot_pred, unc/tot_pred)

                if drawSignal:
                    dc = utils.datacards[datacard_name]
                    sig = dc.GetSignalRate()
                    sigerr = dc.GetTotalUncertainty(signal=True)[0]
                    h_sig.SetBinContent(ibin, sig)
                    h_sig.SetBinError(ibin, sigerr)
                    # divide S by total error (signal syst + background syst + sqrt(pred) stat uncert on obs) for a rough measure of expected significance
                    SB_ratio.append((datacard_name, sig / sqrt(unc**2 + sigerr**2 + tot_pred)))
                    h_SB.SetBinContent(ibin, SB_ratio[-1][1])
                

            binLabels.append(utils.GetMT2label(mt2left,mt2right))

        # add an extra bin if only 1 in this topo reg, for readability
        if len(mt2bins[ijbj])-1==1:
            ibin += 1
            h_data.SetBinContent(ibin, -1)
            binLabels.append("")

    for i in range(nBinsPadding):
        binLabels.append("")
        h_data.SetBinContent(nBinsTotal-i, -1)
                    
        
    # if drawSignal and len(SB_ratio)>0:
    #     print "Bins with highest S/B:"
    #     for name,sb in sorted(SB_ratio, key=lambda x:x[1], reverse=True)[:5]:
    #         print "   {0:.4f} {1}".format(sb, name)

    h_bkg_vec[0].SetFillColor(418)
    h_bkg_vec[1].SetFillColor(ROOT.kAzure+4)
    h_bkg_vec[2].SetFillColor(401)
                
    stack = ROOT.THStack("bkg_stack","")
    for j in range(nBkgs):
        h_bkg_vec[nBkgs-1-j].SetLineWidth(1)
        h_bkg_vec[nBkgs-1-j].SetLineColor(ROOT.kBlack)
        stack.Add(h_bkg_vec[nBkgs-1-j])

    markerSize = min(75.0 / nBinsTotal, 1.0)

    h_data.SetMarkerStyle(20)
    h_data.SetMarkerSize(1.3 * markerSize)
    h_data.SetMarkerColor(ROOT.kBlack)
    h_data.SetLineColor(ROOT.kBlack)

    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetLineWidth(1)

    npads = 3 if (drawSignal and makeSBplot) else 2

    pads = []
    if npads==2:
        c = ROOT.TCanvas("c","c",900,600)
        pads.append(ROOT.TPad("1","1",0.0,0.18,1.0,1.0))
        pads.append(ROOT.TPad("2","2",0.0,0.0,1.0,0.19))

        pads[0].SetTopMargin(0.08)
        pads[0].SetBottomMargin(0.13)
        pads[0].SetRightMargin(0.05)
        pads[0].SetLeftMargin(0.10)
        
        pads[1].SetRightMargin(0.05)
        pads[1].SetLeftMargin(0.10)
    elif npads==3:
        c = ROOT.TCanvas("c","c",900,715)
        pads.append(ROOT.TPad("1","1",0.0, 0.31, 1.0, 1.0))
        pads.append(ROOT.TPad("2","2",0.0, 0.16, 1.0, 0.32))
        pads.append(ROOT.TPad("3","3",0.0, 0.0,  1.0, 0.16))

        pads[0].SetTopMargin(0.08)
        pads[0].SetBottomMargin(0.13)
        pads[0].SetRightMargin(0.05)
        pads[0].SetLeftMargin(0.10)
        
        pads[1].SetRightMargin(0.05)
        pads[1].SetLeftMargin(0.10)

        pads[2].SetRightMargin(0.05)
        pads[2].SetLeftMargin(0.10)

    pads[0].Draw()
    pads[1].Draw()
    if npads>2: pads[2].Draw()

    pads[0].cd()

    pads[0].SetLogy(1)
    # pads[0].SetTickx(1)        
    pads[1].SetTickx(1)
    pads[0].SetTicky(1)
    pads[1].SetTicky(1)
    if npads>2:
        pads[2].SetTickx(1)
        pads[2].SetTicky(1)
        # pads[2].SetLogy(1)
    
    yMin = 1e-2
    if userMax!=None:
        yMax = userMax
    else:
        yMax = h_data.GetMaximum() ** (2.0)
    h_data.GetYaxis().SetRangeUser(yMin,yMax)
    h_data.GetYaxis().SetTitle("Events / bin")
    h_data.GetYaxis().SetTitleOffset(1.2)
    h_data.GetYaxis().SetTickLength(0.02)
    h_data.GetXaxis().SetRangeUser(0,nBinsTotal)
    h_data.GetXaxis().SetNdivisions(nBinsTotal,0,0)
    h_data.GetXaxis().SetLabelSize(0)
    h_data.GetXaxis().SetTickLength(0.015)

    # just draw dots to get axes. Will draw TGraphAsymmErrors on top later
    h_data.SetMarkerStyle(1)
    h_data.Draw("P")

    # draw the backgrounds
    stack.Draw("SAME HIST")
    
    # draw the prediction uncertainties
    g_unc.SetFillStyle(3244)
    g_unc.SetFillColor(ROOT.kGray+3)
    g_unc.Draw("SAME 2")

    # draw the signal, if desired
    if drawSignal:
        h_sig.SetLineColor(ROOT.kRed)
        h_sig.SetLineWidth(2)
        h_sig.Draw("HIST SAME")

    # get a graph using proper asymmetric poissonian errors
    g_data = ROOT.TGraphAsymmErrors()
    ppmUtils.ConvertToPoissonGraph(h_data, g_data, drawZeros=True, drawXerr=False)
    # g_data.SetPointError(g_data.GetN()-1, 0, 0, 0, 0)
    g_data.SetMarkerStyle(20)
    g_data.SetMarkerSize(1.2)
    g_data.SetLineWidth(1)
    # draw with zero marker size so error bars drawn all the way to x axis in the case of 0 content
    g_data_clone = g_data.Clone()
    g_data_clone.SetMarkerSize(0.0)
    
    # draw the graph and then axes again on top
    if drawObs:
        g_data.Draw("SAME P")      
        g_data_clone.Draw("SAME P")      
    h_data.Draw("SAME AXIS")

    # save for later
    left = pads[0].GetLeftMargin()
    right = pads[0].GetRightMargin()
    top = pads[0].GetTopMargin()
    bot = pads[0].GetBottomMargin()

    #draw the x-axis labels
    binWidth = (1.0-right-left)/nBinsTotal
    text = ROOT.TLatex()
    text.SetNDC(1)
    text.SetTextAlign(32)
    text.SetTextAngle(90)
    text.SetTextSize(min(binWidth * 1.6,0.021))
    text.SetTextFont(62)
    for ibin in range(nBinsTotal):
        x = left + (ibin+0.5)*binWidth
        y = pads[0].GetBottomMargin()-0.009
        text.DrawLatex(x,y,binLabels[ibin])
        
    # draw the "Pre/Post-fit background" text
    text.SetTextAlign(13)
    text.SetTextFont(42)
    text.SetTextAngle(0)
    text.SetTextSize(0.05)
    if doPostfit:
        text.DrawLatex(left+0.04,1-top-0.01, "Post-fit background")
    else:
        text.DrawLatex(left+0.04,1-top-0.01, "Pre-fit background")

    # draw the HT bin  in upper middle
    text.SetTextAlign(21)
    text.SetTextFont(62)
    text.SetTextAngle(0)
    text.SetTextSize(0.035)
    text.DrawLatex(left+(1-right-left)*0.5, 1-top-0.01-0.04, utils.GetHTtitle(ht_reg))

    # Draw the CMS and luminosity text
    ppmUtils.DrawCmsText(pads[0],text="CMS",textSize=0.038)
    ppmUtils.DrawLumiText(pads[0],lumi=utils.lumi,textSize=0.038)

    # draw the j/bj region labels
    ibin = 0
    for ijbj,jbj_reg in enumerate(jbj_regs):
        n_bins = len(mt2bins[ijbj])-1
        if n_bins == 1:
            n_bins = 2
        xcenter = left + binWidth*(ibin+(n_bins)*0.5)
        lines = utils.GetJBJtitle(jbj_reg)
        text.SetTextAlign(22)
        text.SetTextFont(62)
        text.SetTextSize(min(0.030, 0.70 * n_bins/nBinsTotal))
        # in the last region, move the text left a bit to avoid overlap with tick marks
        if ijbj==len(jbj_regs)-1:
            text.SetTextAlign(12)
            xcenter = left + binWidth*ibin + 0.007
            xcenter = max(xcenter, 1-right-0.25)
        y = bot+(1-top-bot)*0.83
        if xcenter > 1-right-0.19:
            y = 0.65
        text.DrawLatex(xcenter,y,lines[0])
        text.DrawLatex(xcenter,y-text.GetTextSize()-0.001,lines[1])

        ibin += n_bins

    #draw the lines separating j-bj region
    line = ROOT.TLine()
    line.SetNDC(1)
    line.SetLineStyle(2)
    line.SetLineWidth(1)
    line.SetLineColor(ROOT.kBlack)
    ibin = 0
    for i in range(len(jbj_regs)-1):
        ibin += len(mt2bins[i])-1 if len(mt2bins[i])>2 else 2
        x = left+binWidth*ibin
        line.DrawLineNDC(x,bot,x,bot+(1-top-bot)*0.85)        
    
    # legend
    leg = ROOT.TLegend(1-right-0.175,1-top-0.23,1-right-0.02,1-top-0.01)
    leg.SetBorderSize(1)
    leg.SetCornerRadius(0.3)
    leg.AddEntry(g_data,"Data","pe")
    for i in range(nBkgs):
        leg.AddEntry(h_bkg_vec[i], utils.GetLegendName(bkg_processes[i]),'f')
    if drawSignal:
        leg.AddEntry(h_sig, "{0} ({1},{2})".format(*sigName.split("_")), 'l')
    leg.Draw()


    ####################
    #### RATIO PLOT ####
    ####################

    if ratioRange is None:
        if doPull:
            ratioRange = (-3, 3)
        else:
            ratioRange = (0, 2)
    
    pads[1].cd()
    h_ratio = h_bkg_vec[0].Clone("h_ratio") #h_ratio is just a dummy histogram to draw axes correctly
    h_ratio.Reset()
    g_ratio = ROOT.TGraphAsymmErrors()
    h_pred = h_bkg_vec[0].Clone("h_pred")
    if not doPull:
        for i in range(1,nBkgs):
            h_pred.Add(h_bkg_vec[i])
        ppmUtils.GetPoissonRatioGraph(h_pred, h_data, g_ratio, drawZeros=True, drawXerr=False, useMCErr=False)
    else:
        utils.GetPullPlot(g_data, g_unc, g_ratio)
        g_unc_ratio = ROOT.TGraphAsymmErrors()
        for i in range(nBinsTotal):
            g_unc_ratio.SetPoint(i, i+0.5, 0)
            g_unc_ratio.SetPointError(i, 0.5, 0.5, 1, 1)
    h_ratio.GetYaxis().SetRangeUser(ratioRange[0], ratioRange[1])
    h_ratio.GetYaxis().SetNdivisions(505)
    h_ratio.GetYaxis().SetTitle("Pull" if doPull else "Data/pred.")
    h_ratio.GetYaxis().SetTitleSize(0.16)
    h_ratio.GetYaxis().SetTitleOffset(0.18)
    h_ratio.GetYaxis().SetLabelSize(0.13)
    h_ratio.GetYaxis().CenterTitle()
    h_ratio.GetYaxis().SetTickLength(0.02)
    h_ratio.GetXaxis().SetLabelSize(0)
    h_ratio.GetXaxis().SetTitle("")
    h_ratio.GetXaxis().SetNdivisions(nBinsTotal,0,0)
    h_ratio.GetXaxis().SetTickSize(0.06)
    g_ratio.SetMarkerStyle(20)
    g_ratio.SetMarkerSize(markerSize)
    g_ratio.SetLineWidth(1)

    g_unc_ratio.SetFillStyle(1001)
    g_unc_ratio.SetFillColor(ROOT.kGray)
    
    h_ratio.Draw()
    g_unc_ratio.Draw("SAME 2")

    # draw line at 1
    line = ROOT.TLine()
    line.SetLineStyle(1)
    line.SetLineWidth(1)
    line.SetLineColor(ROOT.kGray+2)
    y = 0 if doPull else 1
    line.DrawLine(0,y,nBinsTotal,y)

    #draw the lines separating j-bj region
    line.SetNDC(1)
    line.SetLineStyle(2)
    line.SetLineWidth(1)
    line.SetLineColor(ROOT.kBlack)
    ibin = 0
    for i in range(len(jbj_regs)-1):
        ibin += len(mt2bins[i])-1 if len(mt2bins[i])>2 else 2
        line.DrawLine(ibin,ratioRange[0],ibin,ratioRange[1])

    
    h_ratio.Draw("SAME AXIS")
    if drawObs:
        g_ratio.Draw("SAME P0")


    if drawSignal and makeSBplot:
        pads[2].cd()
        h_SB.SetLineColor(ROOT.kRed)
        h_SB.SetLineWidth(2)
        # h_SB.GetYaxis().SetRangeUser(1e-2,10)
        h_SB.GetYaxis().SetRangeUser(0,4)
        h_SB.GetYaxis().SetNdivisions(505)
        h_SB.GetYaxis().SetTitle("S/#sigma_{S+B}")
        h_SB.GetYaxis().SetTitleSize(0.16)
        h_SB.GetYaxis().SetTitleOffset(0.25)
        h_SB.GetYaxis().SetLabelSize(0.13)
        h_SB.GetYaxis().CenterTitle()
        h_SB.GetYaxis().SetTickLength(0.02)
        h_SB.GetXaxis().SetLabelSize(0)
        h_SB.GetXaxis().SetTitle("")
        h_SB.GetXaxis().SetNdivisions(nBinsTotal,0,0)
        h_SB.GetXaxis().SetTickSize(0.06)

        h_SB.Draw("HIST")

        line = ROOT.TLine()
        line.SetLineStyle(1)
        line.SetLineWidth(1)
        line.SetLineColor(ROOT.kGray+1)
        # line.DrawLine(0,1e-1,nBinsTotal,1e-1)
        line.DrawLine(0,1e0,nBinsTotal,1e0)

        #draw the lines separating j-bj region
        line.SetNDC(1)
        line.SetLineStyle(2)
        line.SetLineWidth(1)
        line.SetLineColor(ROOT.kBlack)
        ibin = 0
        for i in range(len(jbj_regs)-1):
            ibin += len(mt2bins[i])-1 if len(mt2bins[i])>2 else 2
            # line.DrawLine(ibin,1e-2,ibin,10)
            line.DrawLine(ibin,0,ibin,4)

        
    if drawSignal:
        outdir = os.path.join(outdir,sigName)

    name = "{0}_{1}_{2}".format("postfit" if doPostfit else "prefit", ht_reg, "pull" if doPull else "ratio")
    try:
        os.makedirs(outdir)
    except:
        pass
    c.SaveAs(os.path.join(outdir,name+".pdf"))
    c.SaveAs(os.path.join(outdir,name+".png"))
    
    h_data.Delete()
    g_data.Delete()
    h_ratio.Delete()
    g_ratio.Delete()
    for h in h_bkg_vec:
        h.Delete()
    

def MakeMacroRegionPlot(macro_reg, datacard_dir, datacard_name, outdir, userMax=None):
    # based on MakePlot, but here we want to sum over several cards

    topo_regs = utils.GetMacroregions(macro_reg) #arbitrary set

    #list of lists, one per maco region, of low edges of MT2 bins
    mt2bins = []
    for topo_reg in topo_regs:
        bins = utils.GetMT2binsFull(topo_reg)
        mt2bins.append(bins)

    bkg_processes = ["zinv","llep","qcd"]
    nBkgs = len(bkg_processes)

    # MT2 binning we want to use: let's take the region with the highest number of bins
    xbins = mt2bins[0] #start with the first region
    for tempmt2bins in mt2bins :
        if (len(xbins) < len(tempmt2bins)) : xbins = tempmt2bins
    xbinsA = array('d', xbins)
    xbinsA[len(xbinsA)-1] = 1800 # set last bin
    nBinsTotal = len(xbinsA)-1

    ## setup histograms
    h_data = ROOT.TH1D("h_data","",len(xbinsA)-1,xbinsA)
    h_bkg_tot = ROOT.TH1D("h_tot","",len(xbinsA)-1,xbinsA)
    h_bkg_vec = []
    for i,proc in enumerate(bkg_processes):
        h_bkg_vec.append(ROOT.TH1D("h_"+proc,"",len(xbinsA)-1,xbinsA))
    h_unc = ROOT.TH1D("h_unc","",len(xbinsA)-1,xbinsA)  #  store prediction uncertainties
    h_unc_ratio = ROOT.TH1D("h_unc_ratio","",len(xbinsA)-1,xbinsA)  #  store prediction uncertainties

    ## fill histograms
    ibin = 0
    binLabels = []
    
#    for ijbj, jbj_reg in enumerate(jbj_regs):
    datacard_list = []
    for imt2 in range(len(xbinsA)-1):
        datacard_list.append([])

    for itopo, topo_reg in enumerate(topo_regs):

        ibin = 0
        for imt2 in range(len(mt2bins[itopo])-1):
            ibin += 1
            mt2left = mt2bins[itopo][imt2]
            mt2right = mt2bins[itopo][imt2+1]
            if mt2right == 1800 : mt2right = "Inf"
            mt2name = utils.GetMT2name(mt2left,mt2right)
            datacard_name_fmt = datacard_name.format(topo_reg,mt2name)
            datacard_list[imt2].append(os.path.join(datacard_dir,datacard_name_fmt))

            # get yields. first entry is data, rest are background predictions
            #print datacard_name_fmt
            yields = utils.GetYieldsFromDatacard(os.path.join(datacard_dir,datacard_name_fmt),bkg_processes)
            #print yields[0]
            h_data.SetBinContent(ibin, h_data.GetBinContent(ibin) + yields[0])
            for j in range(1,nBkgs+1):
                h_bkg_vec[j-1].SetBinContent(ibin, h_bkg_vec[j-1].GetBinContent(ibin) + yields[j])
            tot_pred = sum(yields[1:])
            h_bkg_tot.SetBinContent(ibin, h_bkg_tot.GetBinContent(ibin) + tot_pred)
                
            # get uncertainties
            pred_unc = utils.GetUncertaintiesFromDatacard(os.path.join(datacard_dir,datacard_name_fmt),bkg_processes)
            tot_unc_up   = ROOT.TMath.Sqrt(sum([(pred_unc[i][0]*yields[i+1])**2 for i in range(nBkgs)]))
            tot_unc_down = ROOT.TMath.Sqrt(sum([(pred_unc[i][1]*yields[i+1])**2 for i in range(nBkgs)]))
            tot_unc_sym = max(tot_unc_up, tot_unc_down)
#            h_unc.SetBinContent(ibin, h_unc.GetBinContent(ibin) + tot_unc_sym) # treat as fully correlated for now
            h_unc.SetBinContent(ibin, h_unc.GetBinContent(ibin) + tot_unc_sym*tot_unc_sym) # sum in quadrature
            #print tot_pred, tot_unc_up, tot_unc_down

    h_bkg_vec[0].SetFillColor(418)
    h_bkg_vec[1].SetFillColor(ROOT.kAzure+4)
    h_bkg_vec[2].SetFillColor(401)
    

    # Outdated: simple sum in quadrature
    #for imt2 in range(len(xbinsA)-1):
    #    h_unc_ratio.SetBinContent(imt2+1, h_data.GetBinContent(imt2+1)/h_bkg_tot.GetBinContent(imt2+1))
    #    h_unc_ratio.SetBinError(imt2+1, ROOT.TMath.Sqrt(h_unc.GetBinContent(imt2+1))/h_bkg_tot.GetBinContent(imt2+1))
    #    h_unc.SetBinError(imt2+1, ROOT.TMath.Sqrt(h_unc.GetBinContent(imt2+1)))
    #    h_unc.SetBinContent(imt2+1, h_bkg_tot.GetBinContent(imt2+1))

    for imt2 in range(len(xbinsA)-1):
        unc = utils.getMacroRegionUncertainties("test"+str(imt2), datacard_list[imt2])
        h_unc_ratio.SetBinContent(imt2+1, h_data.GetBinContent(imt2+1)/h_bkg_tot.GetBinContent(imt2+1))
        h_unc_ratio.SetBinError(imt2+1, unc/h_bkg_tot.GetBinContent(imt2+1))
        h_unc.SetBinError(imt2+1, unc)
        h_unc.SetBinContent(imt2+1, h_bkg_tot.GetBinContent(imt2+1))
                
    stack = ROOT.THStack("bkg_stack","")
    for j in range(nBkgs):
        h_bkg_vec[nBkgs-1-j].SetLineWidth(1)
        h_bkg_vec[nBkgs-1-j].SetLineColor(ROOT.kBlack)
        stack.Add(h_bkg_vec[nBkgs-1-j])
#    h_bkg_tot.Print("all")
#    h_unc.Print("all")
#    h_unc_ratio.Print("all")
    h_data.Print("all")

    h_data.SetMarkerStyle(20)
    h_data.SetMarkerSize(1.3)
    h_data.SetMarkerColor(ROOT.kBlack)
    h_data.SetLineColor(ROOT.kBlack)

    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetLineWidth(1)
    c = ROOT.TCanvas("c","c",600,600)

    pads = []
    pads.append(ROOT.TPad("1","1",0.0,0.18,1.0,1.0))
    pads.append(ROOT.TPad("2","2",0.0,0.0,1.0,0.19))

    pads[0].SetTopMargin(0.08)
    pads[0].SetBottomMargin(0.13)
    pads[0].SetRightMargin(0.05)
    pads[0].SetLeftMargin(0.10)

    pads[1].SetRightMargin(0.05)
    pads[1].SetLeftMargin(0.10)

    pads[0].Draw()
    pads[1].Draw()
    pads[0].cd()

    pads[0].SetLogy(1)
    # pads[0].SetTickx(1)        
    pads[1].SetTickx(1)
    pads[0].SetTicky(1)
    pads[1].SetTicky(1)
    
    yMin = 1e-1
    if userMax!=None:
        yMax = userMax
    else:
        yMax = h_data.GetMaximum() ** (2.0)
    h_data.GetYaxis().SetRangeUser(yMin,yMax)
    h_data.GetYaxis().SetTitle("Events / Bin")
    h_data.GetYaxis().SetTitleOffset(1.2)
    h_data.GetYaxis().SetTickLength(0.02)
    #h_data.GetXaxis().SetLabelSize(0)
    h_data.GetXaxis().SetTickLength(0.015)
    h_data.GetXaxis().SetTitle("MT2 [GeV]")
    h_data.GetXaxis().SetTitleSize(0.04)
    h_data.GetXaxis().SetLabelSize(0.036)
    h_data.GetXaxis().SetTitleOffset(1.2)

    # just draw dots to get axes. Will draw TGraphAsymmErrors on top later
    h_data.SetMarkerStyle(1)
    h_data.Draw("P")
    

    # draw the backgrounds
    stack.Draw("SAME HIST")
    
    # draw the prediction uncertainties
    h_unc.SetFillStyle(3244)
    h_unc.SetFillColor(ROOT.kGray+3)
    h_unc.Draw("SAME E2")

    # get a graph using proper asymmetric poissonian errors
    g_data = ROOT.TGraphAsymmErrors()
    ppmUtils.ConvertToPoissonGraph(h_data, g_data, drawZeros=True)
#    g_data.SetPointError(g_data.GetN()-1, 0, 0, 0, 0)
    g_data.SetMarkerStyle(20)
    g_data.SetMarkerSize(1.2)
    g_data.SetLineWidth(1)
    
    # draw the graph and then axes again on top
    g_data.Draw("SAME P")      
    h_data.Draw("SAME AXIS")

    # save for later
    left = pads[0].GetLeftMargin()
    right = pads[0].GetRightMargin()
    top = pads[0].GetTopMargin()
    bot = pads[0].GetBottomMargin()

    text = ROOT.TLatex()
    text.SetNDC(1)
       
    # draw the "Pre-fit background" text
    text.SetTextAlign(13)
    text.SetTextFont(42)
    text.SetTextAngle(0)
    text.SetTextSize(0.05)
    text.DrawLatex(left+0.04,1-top-0.01, "Pre-fit background")

    # draw the HT bin  in upper middle
    text.SetTextAlign(21)
    text.SetTextFont(62)
    text.SetTextAngle(0)
    text.SetTextSize(0.035)
#    text.DrawLatex(left+(1-right-left)*0.5, 1-top-0.01-0.04, utils.GetHTtitle(ht_reg))
    text.DrawLatex(left+(1-right-left)*0.3, 1-top-0.01-0.09, utils.GetMacroregionTitle(macro_reg))


    # Draw the CMS and luminosity text
    ppmUtils.DrawCmsText(pads[0],text="CMS",textSize=0.038)
    ppmUtils.DrawLumiText(pads[0],lumi=utils.lumi,textSize=0.038)

    
    # legend
    leg = ROOT.TLegend(1-right-0.175,1-top-0.23,1-right-0.02,1-top-0.01)
    leg.SetBorderSize(1)
    leg.SetCornerRadius(0.3)
    leg.AddEntry(g_data,"Data","lp")
    for i in range(nBkgs):
        leg.AddEntry(h_bkg_vec[i], utils.GetLegendName(bkg_processes[i]),'f')
    leg.Draw()


    ####################
    #### RATIO PLOT ####
    ####################
    
    pads[1].cd()
    h_ratio = h_bkg_vec[0].Clone("h_ratio") #h_ratio is just a dummy histogram to draw axes correctly
    h_ratio.Reset()
    g_ratio = ROOT.TGraphAsymmErrors()
    h_pred = h_bkg_tot.Clone("h_pred")
    ppmUtils.GetPoissonRatioGraph(h_pred, h_data, g_ratio, drawZeros=True, useMCErr=False)
    h_ratio.GetYaxis().SetRangeUser(0,2)
    h_ratio.GetYaxis().SetNdivisions(505)
    h_ratio.GetYaxis().SetTitle("Data/Pred.")
    h_ratio.GetYaxis().SetTitleSize(0.16)
    h_ratio.GetYaxis().SetTitleOffset(0.25)
    h_ratio.GetYaxis().SetLabelSize(0.13)
    h_ratio.GetYaxis().CenterTitle()
    h_ratio.GetYaxis().SetTickLength(0.02)
    h_ratio.GetXaxis().SetLabelSize(0)
    h_ratio.GetXaxis().SetTitle("")
    #h_ratio.GetXaxis().SetNdivisions(nBinsTotal,0,0)
    h_ratio.GetXaxis().SetTickSize(0.06)
    g_ratio.SetMarkerStyle(20)
    g_ratio.SetMarkerSize(1.0)
    g_ratio.SetLineWidth(1)

    h_unc_ratio.SetFillStyle(1001)
    h_unc_ratio.SetFillColor(ROOT.kGray)
    h_ratio.Draw()
    h_unc_ratio.Draw("SAME E2")

    # draw line at 1
    line = ROOT.TLine()
    line.SetLineStyle(1)
    line.SetLineWidth(1)
    line.SetLineColor(ROOT.kGray+2)
    line.DrawLine(xbinsA[0],1,xbinsA[len(xbinsA)-1],1)

    
    h_ratio.Draw("SAME AXIS")
    g_ratio.Draw("SAME P0")


    name = "MacroRegion_{0}".format(macro_reg)
    try:
        os.makedirs(outdir)
    except:
        pass
    c.SaveAs(os.path.join(outdir,name+".pdf"))
    c.SaveAs(os.path.join(outdir,name+".png"))
    
    h_data.Delete()
    g_data.Delete()
    h_ratio.Delete()
    g_ratio.Delete()
    for h in h_bkg_vec:
        h.Delete()
    


def MakeInclusivePlot(outdir, userMax=None, ratioRange=(0,2),
                      drawSignal=False, sigName="", do_ht_regs=None):
    # plot of topo regions summed over mt2 bins

    if len(utils.datacards)==0:
        print "ERROR: must load pickled datacards first! (utils.LoadPickledDatacards)"
        return

    if do_ht_regs is None:
        incl_regs = utils.GetInclusiveRegions(doMonojet=True)
    else:
        incl_regs = utils.GetInclusiveRegions(doMonojet=False, ht_regs=do_ht_regs)

    incl_datacards = utils.GetInclusiveDatacards(None, None, incl_regs, fullPath=False)

    # for i in range(min(len(incl_regs),len(incl_datacards))):
    #     print incl_regs[i]
    #     for j in range(len(incl_datacards[i])):
    #         print "\t", incl_datacards[i][j]

    bkg_processes = ["zinv","llep","qcd"]
    nBkgs = len(bkg_processes)

    nBinsTotal = len(incl_regs)

    ## setup histograms
    h_data = ROOT.TH1D("h_data","",nBinsTotal, 0, nBinsTotal)
    h_bkg_tot = ROOT.TH1D("h_tot","",nBinsTotal, 0, nBinsTotal)
    h_bkg_vec = []
    for i,proc in enumerate(bkg_processes):
        h_bkg_vec.append(ROOT.TH1D("h_"+proc,"",nBinsTotal, 0, nBinsTotal))
    g_unc = ROOT.TGraphAsymmErrors() # graph to store prediction uncertainties
    g_unc_ratio = ROOT.TGraphAsymmErrors() # graph to store prediction uncertainties
    
    h_sig = ROOT.TH1D("h_sig","",nBinsTotal,0,nBinsTotal)
    g_sig_unc = ROOT.TGraphAsymmErrors()

    ## fill histograms
    ibin = 0
    binLabels = []
    
    for ireg, reg in enumerate(incl_regs):
        ibin += 1

        data_yield = 0
        bkg_yields = [0.0 for i in range(nBkgs)]
        sig_yield = 0

        binLabels.append(utils.GetInclusiveBinLabel(reg))
        
        dcs = []
        for idc, datacard_name in enumerate(incl_datacards[ireg]):

            dc = utils.datacards[datacard_name]
            dcs.append(dc)

            data_yield += int(round(dc.GetObservation()))
            yields = dc.GetBackgroundRates()
            for bkg in yields:
                if yields[bkg] < 0:
                    print "WARNING: negative prediction for bkg {0} in card {1}. Setting to 0.0".format(bkg, datacard_name)
                    yields[bkg] = 0.0
            for i,bkg in enumerate(bkg_processes):
                bkg_yields[i] += yields[bkg]

            if drawSignal:
                sig_yield += dc.GetSignalRate()

        tot_pred = sum(bkg_yields)

        h_data.SetBinContent(ibin, data_yield)
        h_bkg_tot.SetBinContent(ibin, tot_pred)
        for i in range(nBkgs):
            h_bkg_vec[i].SetBinContent(ibin, bkg_yields[i])

        bkg_unc = Datacard.GetTotalMergedUncertainty(dcs)
            
        thisPoint = g_unc.GetN()
        g_unc.SetPoint(thisPoint, ibin-0.5, tot_pred)
        g_unc.SetPointError(thisPoint, 0.5, 0.5, bkg_unc[1], bkg_unc[0])
        g_unc_ratio.SetPoint(thisPoint, ibin-0.5, 1)
        g_unc_ratio.SetPointError(thisPoint, 0.5, 0.5, bkg_unc[1]/tot_pred, bkg_unc[0]/tot_pred)
        
        if drawSignal:
            h_sig.SetBinContent(ibin, sig_yield)
            thisPoint = g_sig_unc.GetN()
            g_sig_unc.SetPoint(thisPoint, ibin-0.5, sig_yield)
            g_sig_unc.SetPointError(thisPoint, 0.5, 0.5, sig_unc, sig_unc)

    h_bkg_vec[0].SetFillColor(418)
    h_bkg_vec[1].SetFillColor(ROOT.kAzure+4)
    h_bkg_vec[2].SetFillColor(401)
    
    stack = ROOT.THStack("bkg_stack","")
    for j in range(nBkgs):
        h_bkg_vec[nBkgs-1-j].SetLineWidth(1)
        h_bkg_vec[nBkgs-1-j].SetLineColor(ROOT.kBlack)
        stack.Add(h_bkg_vec[nBkgs-1-j])

    h_data.SetMarkerStyle(20)
    h_data.SetMarkerSize(1.3)
    h_data.SetMarkerColor(ROOT.kBlack)
    h_data.SetLineColor(ROOT.kBlack)

    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetLineWidth(1)
    c = ROOT.TCanvas("c","c",1350 if do_ht_regs is None else 1050,900)

    pads = []
    pads.append(ROOT.TPad("1","1",0.0,0.18,1.0,1.0))
    pads.append(ROOT.TPad("2","2",0.0,0.0,1.0,0.19))

    pads[0].SetTopMargin(0.08)
    pads[0].SetBottomMargin(0.13)
    pads[0].SetRightMargin(0.05)
    pads[0].SetLeftMargin(0.10)

    pads[1].SetRightMargin(0.05)
    pads[1].SetLeftMargin(0.10)

    pads[0].Draw()
    pads[1].Draw()
    pads[0].cd()

    pads[0].SetLogy(1)
    # pads[0].SetTickx(1)        
    pads[1].SetTickx(1)
    pads[0].SetTicky(1)
    pads[1].SetTicky(1)
    
    yMin = 1e-1 if do_ht_regs is None else 0.5
    if userMax!=None:
        yMax = userMax
    else:
        yMax = h_data.GetMaximum() ** (2.0)
    h_data.GetYaxis().SetRangeUser(yMin,yMax)
    h_data.GetYaxis().SetTitle("Events / bin")
    h_data.GetYaxis().SetTitleOffset(1.2)
    h_data.GetYaxis().SetTickLength(0.02)
    h_data.GetXaxis().SetRangeUser(0,nBinsTotal)
    h_data.GetXaxis().SetNdivisions(nBinsTotal,0,0)
    h_data.GetXaxis().SetLabelSize(0)
    h_data.GetXaxis().SetTickLength(0.015)

    # just draw dots to get axes. Will draw TGraphAsymmErrors on top later
    h_data.SetMarkerStyle(1)
    h_data.Draw("P")    

    # draw the backgrounds
    stack.Draw("SAME HIST")
    
    # draw the prediction uncertainties
    g_unc.SetFillStyle(3244)
    g_unc.SetFillColor(ROOT.kGray+3)
    g_unc.Draw("SAME 2")

    # draw the signal if requested
    if drawSignal:
        h_sig.SetLineColor(ROOT.kRed)
        h_sig.SetLineWidth(2)
        h_sig.Draw("HIST SAME")
        g_sig_unc.SetFillStyle(3004)
        g_sig_unc.SetFillColor(ROOT.kRed)
        g_sig_unc.Draw("SAME 2")

    # get a graph using proper asymmetric poissonian errors
    g_data = ROOT.TGraphAsymmErrors()
    ppmUtils.ConvertToPoissonGraph(h_data, g_data, drawZeros=True, drawXerr=False)
#    g_data.SetPointError(g_data.GetN()-1, 0, 0, 0, 0)
    g_data.SetMarkerStyle(20)
    g_data.SetMarkerSize(1.8)
    g_data.SetLineWidth(1)
    # draw with zero marker size so error bars drawn all the way to x axis in the case of 0 content
    g_data_clone = g_data.Clone()
    g_data_clone.SetMarkerSize(0.0)
    
    # draw the graph and then axes again on top
    g_data.Draw("SAME P")      
    g_data_clone.Draw("SAME P")      
    h_data.Draw("SAME AXIS")

    # save for later
    left = pads[0].GetLeftMargin()
    right = pads[0].GetRightMargin()
    top = pads[0].GetTopMargin()
    bot = pads[0].GetBottomMargin()

    #draw the x-axis labels
    binWidth = (1.0-right-left)/nBinsTotal
    text = ROOT.TLatex()
    text.SetNDC(1)
    text.SetTextAlign(32)
    text.SetTextAngle(90)
    text.SetTextSize(min(binWidth * 1.6, 0.026 if do_ht_regs is None else 0.0262))
    text.SetTextFont(62)
    for ibin in range(nBinsTotal):
        x = left + (ibin+0.5)*binWidth
        y = pads[0].GetBottomMargin()-0.009
        text.DrawLatex(x,y,binLabels[ibin])
       
    # draw the "Pre-fit background" text
    text.SetTextAlign(13)
    text.SetTextFont(42)
    text.SetTextAngle(0)
    text.SetTextSize(0.05 if do_ht_regs is None else 0.04)
    x = left+0.04 if do_ht_regs is None else left+0.02
    text.DrawLatex(x,1-top-0.01, "Pre-fit background")

    # draw the HT bin  in upper middle
    text.SetTextAlign(21)
    text.SetTextFont(62)
    text.SetTextAngle(0)
    text.SetTextSize(0.035)
    y = 1-top-0.01-0.04 if do_ht_regs is None else bot+(1-top-bot)*0.88
    text.DrawLatex(left+(1-right-left)*0.5, y, "Integrated over M_{T2}")

    # Draw the CMS and luminosity text
    ppmUtils.DrawCmsText(pads[0],text="CMS",textSize=0.038)
    ppmUtils.DrawLumiText(pads[0],lumi=utils.lumi,textSize=0.038)

    #draw the lines separating the regions
    line = ROOT.TLine()
    line.SetNDC(1)
    line.SetLineStyle(2)
    line.SetLineWidth(1)
    line.SetLineColor(ROOT.kBlack)
    nbins = []
    if do_ht_regs is None:
        nbins += [len(bins)-1 for bins in utils.GetMT2bins("monojet")]
        ht_regs = ["HT250to450", "HT450to575", "HT575to1200", "HT1200to1500", "HT1500toInf"]
    else:
        ht_regs = do_ht_regs
    nbins += [len(utils.GetJBJregions(ht_reg)) for ht_reg in ht_regs]
    ibin = 0
    for i in nbins:
        ibin += i
        x = left+binWidth*ibin
        line.DrawLineNDC(x,bot,x,bot+(1-top-bot)*0.85)        

    # draw the  region labels
    ibin = 0
    regtext = ["1j, 0b", "1j, 1b", "H_{T} [250,450]" , "H_{T} [450,575]" , "H_{T} [575,1200]" , "H_{T} [1200,1500]" , "H_{T} [1500, #infty]"]
    if do_ht_regs is not None:
        regtext = []
        for ht_reg in do_ht_regs:
            if ht_reg=="HT1200to1500":
                regtext.append("1200 < H_{T} < 1500 GeV")
            else:
                regtext.append(ht_reg)
    for i,nbin in enumerate(nbins):
        xcenter = left + binWidth*(ibin+nbin/2.0)        
        text.SetTextAlign(23)
        text.SetTextFont(62)
        text.SetTextSize(0.025 if do_ht_regs is None else 0.035)
        # in the 2nd-to-last region, move the text left a bit to avoid overlap with legend
        if i==len(regtext)-2:
            xcenter -= 1*binWidth
        y = bot+(1-top-bot)*0.85
        if xcenter > 1-right-0.19:
            y = 0.67
        text.DrawLatex(xcenter,y,regtext[i])

        ibin += nbin

    
    # legend
    x1 = 1-right-0.175 if do_ht_regs is None else 1-right-0.225
    leg = ROOT.TLegend(x1,1-top-0.23,1-right-0.02,1-top-0.01)
    leg.SetBorderSize(1)
    leg.SetCornerRadius(0.3)
    leg.AddEntry(g_data,"Data","pe")
    for i in range(nBkgs):
        leg.AddEntry(h_bkg_vec[i], utils.GetLegendName(bkg_processes[i]),'f')
    if drawSignal:
        leg.AddEntry(h_sig, sigName)
    leg.Draw()


    ####################
    #### RATIO PLOT ####
    ####################
    
    pads[1].cd()
    h_ratio = h_bkg_vec[0].Clone("h_ratio") #h_ratio is just a dummy histogram to draw axes correctly
    h_ratio.Reset()
    g_ratio = ROOT.TGraphAsymmErrors()
    h_pred = h_bkg_tot.Clone("h_pred")
    ppmUtils.GetPoissonRatioGraph(h_pred, h_data, g_ratio, drawZeros=True, drawXerr=False, useMCErr=False)
    h_ratio.GetYaxis().SetRangeUser(*ratioRange)
    h_ratio.GetYaxis().SetNdivisions(505)
    h_ratio.GetYaxis().SetTitle("Data/pred.")
    h_ratio.GetYaxis().SetTitleSize(0.16)
    h_ratio.GetYaxis().SetTitleOffset(0.25)
    h_ratio.GetYaxis().SetLabelSize(0.13 if do_ht_regs is None else 0.16)
    h_ratio.GetYaxis().SetLabelOffset(0.01)
    h_ratio.GetYaxis().CenterTitle()
    h_ratio.GetYaxis().SetTickLength(0.02)
    h_ratio.GetXaxis().SetLabelSize(0)
    h_ratio.GetXaxis().SetTitle("")
    #h_ratio.GetXaxis().SetNdivisions(nBinsTotal,0,0)
    h_ratio.GetXaxis().SetTickSize(0.06)
    g_ratio.SetMarkerStyle(20)
    g_ratio.SetMarkerSize(1.5)
    g_ratio.SetLineWidth(1)

    g_unc_ratio.SetFillStyle(1001)
    g_unc_ratio.SetFillColor(ROOT.kGray)

    h_ratio.Draw()
    g_unc_ratio.Draw("SAME 2")

    # draw line at 1
    line = ROOT.TLine()
    line.SetLineStyle(1)
    line.SetLineWidth(1)
    line.SetLineColor(ROOT.kGray+2)
    line.DrawLine(0,1,nBinsTotal,1)
    
    h_ratio.Draw("SAME AXIS")
    g_ratio.Draw("SAME P0")

    #draw the lines separating the regions
    line = ROOT.TLine()
    line.SetNDC(1)
    line.SetLineStyle(2)
    line.SetLineWidth(1)
    line.SetLineColor(ROOT.kBlack)
    nbins = []
    if do_ht_regs is None:
        nbins += [len(bins)-1 for bins in utils.GetMT2bins("monojet")]
        ht_regs = ["HT250to450", "HT450to575", "HT575to1200", "HT1200to1500", "HT1500toInf"]
    else:
        ht_regs = do_ht_regs
    nbins += [len(utils.GetJBJregions(ht_reg)) for ht_reg in ht_regs]
    ibin = 0
    for i in nbins:
        ibin += i
        line.DrawLine(ibin, ratioRange[0], ibin, ratioRange[1])


    name = "prefit_inclusive"
    if do_ht_regs is not None:
        name += "_" + "_".join(do_ht_regs)
    try:
        os.makedirs(outdir)
    except:
        pass
    c.SaveAs(os.path.join(outdir,name+".pdf"))
    c.SaveAs(os.path.join(outdir,name+".png"))
    
    h_data.Delete()
    g_data.Delete()
    h_ratio.Delete()
    g_ratio.Delete()
    for h in h_bkg_vec:
        h.Delete()


def MakeHTBinPlot(datacard_dir, datacard_name, outdir, userMax=None, ratioRange=(0,2),
                  scalePred=1.0):

    regs = utils.GetInclusiveRegions(doMonojet=True)
    datacards = utils.GetInclusiveDatacards(datacard_dir, datacard_name, regs)

    ht_regs = ["Monojet", "HT250to450","HT450to575","HT575to1200","HT1200to1500","HT1500toInf"]
    ht_reg_datacards = {}
    for ht_reg in ht_regs:
        ht_reg_datacards[ht_reg] = []
    for ireg,reg in enumerate(regs):
        if "j1_" in reg:
            ht_reg = "Monojet"
        else:
            ht_reg = reg.split("_")[0]
        for dc in datacards[ireg]:
            ht_reg_datacards[ht_reg].append(dc)

    bkg_processes = ["zinv","llep","qcd"]
    nBkgs = len(bkg_processes)

    nBinsTotal = len(ht_regs)

    ## setup histograms
    h_data = ROOT.TH1D("h_data","",nBinsTotal, 0, nBinsTotal)
    h_bkg_tot = ROOT.TH1D("h_tot","",nBinsTotal, 0, nBinsTotal)
    h_bkg_vec = []
    for i,proc in enumerate(bkg_processes):
        h_bkg_vec.append(ROOT.TH1D("h_"+proc,"",nBinsTotal, 0, nBinsTotal))
    g_unc = ROOT.TGraphAsymmErrors() # graph to store prediction uncertainties
    g_unc_ratio = ROOT.TGraphAsymmErrors() # graph to store prediction uncertainties

    ibin = 0
    for ireg, ht_reg in enumerate(ht_regs):
        ibin += 1

        data_yield = 0
        bkg_yields = [0.0 for i in range(nBkgs)]

        for idc, dc in enumerate(ht_reg_datacards[ht_reg]):
            # get yields. first entry is data, rest are background predictions
            #print datacard_name_fmt
            yields = utils.GetYieldsFromDatacard(dc, bkg_processes)
            data_yield += yields[0]
            for j in range(1,nBkgs+1):
                bkg_yields[j-1] += yields[j] * scalePred

        tot_pred = sum(bkg_yields)

        print "{0}: {1:d} {2:.2f} {3:.2f} {4:.2f} {5:.2f}".format(ht_reg, data_yield, tot_pred, bkg_yields[0], bkg_yields[1], bkg_yields[2])

        h_data.SetBinContent(ibin, data_yield)
        h_bkg_tot.SetBinContent(ibin, tot_pred)
        for i in range(nBkgs):
            h_bkg_vec[i].SetBinContent(ibin, bkg_yields[i])

        bkg_unc = utils.getMacroRegionUncertainties(ht_reg, ht_reg_datacards[ht_reg])

        thisPoint = g_unc.GetN()
        g_unc.SetPoint(thisPoint, ibin-0.5, tot_pred)
        g_unc.SetPointError(thisPoint, 0.5, 0.5, bkg_unc*scalePred, bkg_unc*scalePred)
        g_unc_ratio.SetPoint(thisPoint, ibin-0.5, 1)
        g_unc_ratio.SetPointError(thisPoint, 0.5, 0.5, bkg_unc/tot_pred*scalePred, bkg_unc/tot_pred*scalePred)

    h_bkg_vec[0].SetFillColor(418)
    h_bkg_vec[1].SetFillColor(ROOT.kAzure+4)
    h_bkg_vec[2].SetFillColor(401)

    stack = ROOT.THStack("bkg_stack","")
    for j in range(nBkgs):
        h_bkg_vec[nBkgs-1-j].SetLineWidth(1)
        h_bkg_vec[nBkgs-1-j].SetLineColor(ROOT.kBlack)
        stack.Add(h_bkg_vec[nBkgs-1-j])

    h_data.SetMarkerStyle(20)
    h_data.SetMarkerSize(1.3)
    h_data.SetMarkerColor(ROOT.kBlack)
    h_data.SetLineColor(ROOT.kBlack)

    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetLineWidth(1)
    c = ROOT.TCanvas("c","c",900,600)

    pads = []
    pads.append(ROOT.TPad("1","1",0.0,0.18,1.0,1.0))
    pads.append(ROOT.TPad("2","2",0.0,0.0,1.0,0.19))

    pads[0].SetTopMargin(0.08)
    pads[0].SetBottomMargin(0.13)
    pads[0].SetRightMargin(0.05)
    pads[0].SetLeftMargin(0.10)

    pads[1].SetRightMargin(0.05)
    pads[1].SetLeftMargin(0.10)

    pads[0].Draw()
    pads[1].Draw()
    pads[0].cd()

    pads[0].SetLogy(1)
    # pads[0].SetTickx(1)        
    pads[1].SetTickx(1)
    pads[0].SetTicky(1)
    pads[1].SetTicky(1)
    
    yMin = 1e0
    if userMax!=None:
        yMax = userMax
    else:
        yMax = h_data.GetMaximum() ** (2.0)
    h_data.GetYaxis().SetRangeUser(yMin,yMax)
    h_data.GetYaxis().SetTitle("Events / Bin")
    h_data.GetYaxis().SetTitleOffset(1.2)
    h_data.GetYaxis().SetTickLength(0.02)
    h_data.GetXaxis().SetRangeUser(0,nBinsTotal)
    h_data.GetXaxis().SetNdivisions(nBinsTotal,0,0)
    h_data.GetXaxis().SetLabelSize(0)
    h_data.GetXaxis().SetTickLength(0.015)

    # just draw dots to get axes. Will draw TGraphAsymmErrors on top later
    h_data.SetMarkerStyle(1)
    h_data.Draw("P")    

    # draw the backgrounds
    stack.Draw("SAME HIST")
    
    # draw the prediction uncertainties
    g_unc.SetFillStyle(3244)
    g_unc.SetFillColor(ROOT.kGray+3)
    g_unc.Draw("SAME 2")

    # get a graph using proper asymmetric poissonian errors
    g_data = ROOT.TGraphAsymmErrors()
    ppmUtils.ConvertToPoissonGraph(h_data, g_data, drawZeros=True)
#    g_data.SetPointError(g_data.GetN()-1, 0, 0, 0, 0)
    g_data.SetMarkerStyle(20)
    g_data.SetMarkerSize(1.2)
    g_data.SetLineWidth(1)
    
    # draw the graph and then axes again on top
    g_data.Draw("SAME P")      
    h_data.Draw("SAME AXIS")

    # save for later
    left = pads[0].GetLeftMargin()
    right = pads[0].GetRightMargin()
    top = pads[0].GetTopMargin()
    bot = pads[0].GetBottomMargin()

    #draw the x-axis labels
    binWidth = (1.0-right-left)/nBinsTotal
    text = ROOT.TLatex()
    text.SetNDC(1)
    text.SetTextAlign(22)
    text.SetTextAngle(0)
    text.SetTextSize(0.04)
    text.SetTextFont(62)
    for ibin in range(nBinsTotal):
        x = left + (ibin+0.5)*binWidth
        y = pads[0].GetBottomMargin()-0.025
        text.DrawLatex(x,y,ht_regs[ibin])

    # draw the "Pre-fit background" text
    text.SetTextAlign(13)
    text.SetTextFont(42)
    text.SetTextAngle(0)
    text.SetTextSize(0.05)
    text.DrawLatex(left+0.03,1-top-0.03, "Pre-fit background")

    # Draw the CMS and luminosity text
    ppmUtils.DrawCmsText(pads[0],text="CMS",textSize=0.044)
    ppmUtils.DrawLumiText(pads[0],lumi=utils.lumi,textSize=0.044)

    #draw the lines separating the regions
    line = ROOT.TLine()
    line.SetNDC(1)
    line.SetLineStyle(2)
    line.SetLineWidth(1)
    line.SetLineColor(ROOT.kBlack)
    for i in range(1,nBinsTotal):
        x = left+binWidth*i
        line.DrawLineNDC(x,bot,x,bot+(1-top-bot)*0.85)        

    # legend
    leg = ROOT.TLegend(1-right-0.175,1-top-0.23,1-right-0.02,1-top-0.01)
    leg.SetBorderSize(1)
    leg.SetCornerRadius(0.3)
    leg.AddEntry(g_data,"Data","lp")
    for i in range(nBkgs):
        leg.AddEntry(h_bkg_vec[i], utils.GetLegendName(bkg_processes[i]),'f')
    leg.Draw()

    ####################
    #### RATIO PLOT ####
    ####################
    
    pads[1].cd()
    h_ratio = h_bkg_vec[0].Clone("h_ratio") #h_ratio is just a dummy histogram to draw axes correctly
    h_ratio.Reset()
    g_ratio = ROOT.TGraphAsymmErrors()
    h_pred = h_bkg_tot.Clone("h_pred")
    ppmUtils.GetPoissonRatioGraph(h_pred, h_data, g_ratio, drawZeros=True, useMCErr=False)
    h_ratio.GetYaxis().SetRangeUser(ratioRange[0], ratioRange[1])
    h_ratio.GetYaxis().SetNdivisions(505)
    h_ratio.GetYaxis().SetTitle("Data/Pred.")
    h_ratio.GetYaxis().SetTitleSize(0.16)
    h_ratio.GetYaxis().SetTitleOffset(0.25)
    h_ratio.GetYaxis().SetLabelSize(0.13)
    h_ratio.GetYaxis().CenterTitle()
    h_ratio.GetYaxis().SetTickLength(0.02)
    h_ratio.GetXaxis().SetLabelSize(0)
    h_ratio.GetXaxis().SetTitle("")
    #h_ratio.GetXaxis().SetNdivisions(nBinsTotal,0,0)
    h_ratio.GetXaxis().SetTickSize(0.06)
    g_ratio.SetMarkerStyle(20)
    g_ratio.SetMarkerSize(1.0)
    g_ratio.SetLineWidth(1)

    g_unc_ratio.SetFillStyle(1001)
    g_unc_ratio.SetFillColor(ROOT.kGray)

    h_ratio.Draw()
    g_unc_ratio.Draw("SAME 2")

    # draw line at 1
    line = ROOT.TLine()
    line.SetLineStyle(1)
    line.SetLineWidth(1)
    line.SetLineColor(ROOT.kGray+2)
    line.DrawLine(0,1,nBinsTotal,1)
    line.SetLineColor(ROOT.kBlack)
    line.SetLineStyle(2)
    for i in range(1,nBinsTotal):
        line.DrawLine(i,ratioRange[0],i,ratioRange[1])
    
    h_ratio.Draw("SAME AXIS")
    g_ratio.Draw("SAME P0")

    name = "test_justht"
    try:
        os.makedirs(outdir)
    except:
        pass
    c.SaveAs(os.path.join(outdir,name+".pdf"))
    c.SaveAs(os.path.join(outdir,name+".png"))



def MakeComparison(ht_reg, outdir, doWhat="data", h1_name="h1", h2_name="h2", userMax=None, ratioRange=None):
    #doWhat is one of data,zinv,llep,qcd,bkg

    if len(utils.datacards)==0:
        print "ERROR: must load pickled datacards first! (utils.LoadPickledDatacards)"
        return
    if len(utils.datacards2)==0:
        print "ERROR: must load pickled second set of datacards first! (utils.LoadPickledDatacards)"
        return

    jbj_regs = utils.GetJBJregions(ht_reg)

    #list of lists, one per jbj region, of low edges of MT2 bins
    mt2bins = utils.GetMT2bins(ht_reg)

    # nBinsTotal = sum([len(bins)-1 for bins in mt2bins]) + 1
    nBinsTotal = sum([(len(bins)-1 if len(bins)>2 else len(bins)) for bins in mt2bins]) # always at least 2 for readability
    nBinsPadding = 2 if ht_reg in ["HT575to1200", "HT1200to1500", "HT1500toInf"] else 0 # extra bins to add to right for readability
    nBinsTotal += nBinsPadding
    bkg_processes = ["zinv","llep","qcd"]
    nBkgs = len(bkg_processes)

    ## setup histograms
    h1 = ROOT.TH1D("h1","",nBinsTotal,0,nBinsTotal)
    h2 = ROOT.TH1D("h2","",nBinsTotal,0,nBinsTotal)

    ## fill histograms
    ibin = 0
    binLabels = []
    
    for ijbj, jbj_reg in enumerate(jbj_regs):
        for imt2 in range(len(mt2bins[ijbj])-1):
            ibin += 1
            mt2left = mt2bins[ijbj][imt2]
            mt2right = mt2bins[ijbj][imt2+1]
            mt2name = utils.GetMT2name(mt2left,mt2right)
            
            if ht_reg != "monojet":
                datacard_name = "{0}_{1}_{2}".format(ht_reg,jbj_reg,mt2name)
            else:
                ht_name = "HT{0}to{1}".format(mt2left,mt2right)
                ht_name = ht_name.replace("-1","Inf")
                datacard_name = "{0}_{1}_{2}".format(ht_name,jbj_reg,"m0toInf")

            dc1 = utils.datacards[datacard_name]
            dc2 = utils.datacards2[datacard_name]
                
            # get yields. first entry is data, rest are background predictions
            obs1 = int(round(dc1.GetObservation()))
            obs2 = int(round(dc2.GetObservation()))
            yields1 = dc1.GetBackgroundRates()
            yields2 = dc2.GetBackgroundRates()
            for bkg in yields1:
                if yields1[bkg] < 0:
                    print "WARNING: negative prediction for bkg {0} in card {1}. Setting to 0.0".format(bkg, datacard_name)
                    yields1[bkg] = 0.0
                if yields2[bkg] < 0:
                    print "WARNING: negative prediction for bkg {0} in card {1}. Setting to 0.0".format(bkg, datacard_name)
                    yields2[bkg] = 0.0
            tot_pred1 = sum(yields1.values())
            tot_pred2 = sum(yields2.values())
            tot_unc_up1, tot_unc_down1 = dc1.GetTotalUncertainty()
            tot_unc_up2, tot_unc_down2 = dc2.GetTotalUncertainty()

            if doWhat=="data":
                h1.SetBinContent(ibin, obs1)
                h2.SetBinContent(ibin, obs2)
                h1.SetBinError(ibin, sqrt(obs1))
                h2.SetBinError(ibin, sqrt(obs2))
            elif doWhat=="bkg":
                h1.SetBinContent(ibin, tot_pred1)
                h2.SetBinContent(ibin, tot_pred2)
                h1.SetBinError(ibin, tot_unc_up1)
                h2.SetBinError(ibin, tot_unc_up2)
            else:
                h1.SetBinContent(ibin, yields1[doWhat])
                h2.SetBinContent(ibin, yields2[doWhat])
                
            binLabels.append(utils.GetMT2label(mt2left,mt2right))

        # add an extra bin if only 1 in this topo reg, for readability
        if len(mt2bins[ijbj])-1==1:
            ibin += 1
            h1.SetBinContent(ibin, -1)
            h2.SetBinContent(ibin, -1)
            binLabels.append("")

    for i in range(nBinsPadding):
        binLabels.append("")
        h1.SetBinContent(nBinsTotal-i, -1)
        h2.SetBinContent(nBinsTotal-i, -1)


    h1.SetMarkerStyle(20)
    h1.SetMarkerSize(0.5)
    h1.SetMarkerColor(ROOT.kBlack)
    h1.SetLineColor(ROOT.kBlack)

    h2.SetMarkerStyle(20)
    h2.SetMarkerSize(0.5)
    h2.SetMarkerColor(ROOT.kRed)
    h2.SetLineColor(ROOT.kRed)

    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetLineWidth(1)
    c = ROOT.TCanvas("c","c",900,600)

    pads = []
    pads.append(ROOT.TPad("1","1",0.0,0.18,1.0,1.0))
    pads.append(ROOT.TPad("2","2",0.0,0.0,1.0,0.19))

    pads[0].SetTopMargin(0.08)
    pads[0].SetBottomMargin(0.13)
    pads[0].SetRightMargin(0.05)
    pads[0].SetLeftMargin(0.10)

    pads[1].SetRightMargin(0.05)
    pads[1].SetLeftMargin(0.10)

    pads[0].Draw()
    pads[1].Draw()
    pads[0].cd()

    pads[0].SetLogy(1)
    # pads[0].SetTickx(1)        
    pads[1].SetTickx(1)
    pads[0].SetTicky(1)
    pads[1].SetTicky(1)
    
    yMin = 1e-1
    if userMax!=None:
        yMax = userMax
    else:
        yMax = h1.GetMaximum() ** (1.5)
    h1.GetYaxis().SetRangeUser(yMin,yMax)
    h1.GetYaxis().SetTitle("Events / Bin")
    h1.GetYaxis().SetTitleOffset(1.2)
    h1.GetYaxis().SetTickLength(0.02)
    h1.GetXaxis().SetRangeUser(0,nBinsTotal)
    h1.GetXaxis().SetNdivisions(nBinsTotal,0,0)
    h1.GetXaxis().SetLabelSize(0)
    h1.GetXaxis().SetTickLength(0.015)

    h1.Draw("PE")
    h2.Draw("SAME PE")

    # save for later
    left = pads[0].GetLeftMargin()
    right = pads[0].GetRightMargin()
    top = pads[0].GetTopMargin()
    bot = pads[0].GetBottomMargin()

    #draw the x-axis labels
    binWidth = (1.0-right-left)/nBinsTotal
    text = ROOT.TLatex()
    text.SetNDC(1)
    text.SetTextAlign(32)
    text.SetTextAngle(90)
    text.SetTextSize(min(binWidth * 1.6,0.021))
    text.SetTextFont(62)
    for ibin in range(nBinsTotal):
        x = left + (ibin+0.5)*binWidth
        y = pads[0].GetBottomMargin()-0.009
        text.DrawLatex(x,y,binLabels[ibin])
        
    # draw the "Pre/Post-fit background" text
    text.SetTextAlign(13)
    text.SetTextFont(42)
    text.SetTextAngle(0)
    text.SetTextSize(0.05)
    text.DrawLatex(left+0.04,1-top-0.01, doWhat+" comparison")

    # draw the HT bin  in upper middle
    text.SetTextAlign(21)
    text.SetTextFont(62)
    text.SetTextAngle(0)
    text.SetTextSize(0.035)
    text.DrawLatex(left+(1-right-left)*0.5, 1-top-0.01-0.04, utils.GetHTtitle(ht_reg))

    # Draw the CMS and luminosity text
    ppmUtils.DrawCmsText(pads[0],text="CMS",textSize=0.038)
    ppmUtils.DrawLumiText(pads[0],lumi=utils.lumi,textSize=0.038)


    # draw the j/bj region labels
    ibin = 0
    for ijbj,jbj_reg in enumerate(jbj_regs):
        n_bins = len(mt2bins[ijbj])-1
        if n_bins == 1:
            n_bins = 2
        xcenter = left + binWidth*(ibin+(n_bins)*0.5)
        lines = utils.GetJBJtitle(jbj_reg)
        text.SetTextAlign(22)
        text.SetTextFont(62)
        text.SetTextSize(min(0.030, 0.70 * n_bins/nBinsTotal))
        # in the last region, move the text left a bit to avoid overlap with tick marks
        if ijbj==len(jbj_regs)-1:
            text.SetTextAlign(12)
            xcenter = left + binWidth*ibin + 0.007
            xcenter = max(xcenter, 1-right-0.25)
        y = bot+(1-top-bot)*0.83
        if xcenter > 1-right-0.19:
            y = 0.65
        text.DrawLatex(xcenter,y,lines[0])
        text.DrawLatex(xcenter,y-text.GetTextSize()-0.001,lines[1])

        ibin += n_bins

    #draw the lines separating j-bj region
    line = ROOT.TLine()
    line.SetNDC(1)
    line.SetLineStyle(2)
    line.SetLineWidth(1)
    line.SetLineColor(ROOT.kBlack)
    ibin = 0
    for i in range(len(jbj_regs)-1):
        ibin += len(mt2bins[i])-1 if len(mt2bins[i])>2 else 2
        x = left+binWidth*ibin
        line.DrawLineNDC(x,bot,x,bot+(1-top-bot)*0.85)        
    
    # legend
    leg = ROOT.TLegend(1-right-0.175,1-top-0.21,1-right-0.02,1-top-0.04)
    leg.SetBorderSize(1)
    leg.AddEntry(h1,h1_name,'pel')
    leg.AddEntry(h2,h2_name,'pel')
    leg.Draw()


    ####################
    #### RATIO PLOT ####
    ####################
    
    pads[1].cd()
    h_ratio = h2.Clone("h_ratio") #h_ratio is just a dummy histogram to draw axes correctly
    h_ratio.Divide(h1)
    for ibin in range(1,h_ratio.GetNbinsX()+1):
        h_ratio.SetBinError(ibin,0.0001)
    h_ratio.GetYaxis().SetRangeUser(ratioRange[0], ratioRange[1])
    h_ratio.GetYaxis().SetNdivisions(505)
    h_ratio.GetYaxis().SetTitle(h2_name+"/"+h1_name)
    h_ratio.GetYaxis().SetTitleSize(0.16)
    h_ratio.GetYaxis().SetTitleOffset(0.18)
    h_ratio.GetYaxis().SetLabelSize(0.13)
    h_ratio.GetYaxis().CenterTitle()
    h_ratio.GetYaxis().SetTickLength(0.02)
    h_ratio.GetXaxis().SetLabelSize(0)
    h_ratio.GetXaxis().SetTitle("")
    h_ratio.GetXaxis().SetNdivisions(nBinsTotal,0,0)
    h_ratio.GetXaxis().SetTickSize(0.06)
    
    h_ratio.Draw("PE")

    # draw line at 1
    line = ROOT.TLine()
    line.SetLineStyle(1)
    line.SetLineWidth(1)
    line.SetLineColor(ROOT.kGray+2)
    line.DrawLine(0,1,nBinsTotal,1)

    #draw the lines separating j-bj region
    line.SetNDC(1)
    line.SetLineStyle(2)
    line.SetLineWidth(1)
    line.SetLineColor(ROOT.kBlack)
    ibin = 0
    for i in range(len(jbj_regs)-1):
        ibin += len(mt2bins[i])-1 if len(mt2bins[i])>2 else 2
        line.DrawLine(ibin,ratioRange[0],ibin,ratioRange[1])
    
    h_ratio.Draw("SAME PE")
    h_ratio.Draw("SAME AXIS")

    name = "compare_{0}_{1}".format(doWhat, ht_reg)
    try:
        os.makedirs(outdir)
    except:
        pass
    c.SaveAs(os.path.join(outdir,name+".pdf"))
    c.SaveAs(os.path.join(outdir,name+".png"))
    
    h1.Delete()
    h2.Delete()
    h_ratio.Delete()

    
