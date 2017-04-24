import ROOT
import os
import sys
import ResultPlotUtils as utils
import ppmUtils

def MakePlotFromTablecards(card_dir, outdir, userMax=None, printData=True):
    card_names = [f for f in os.listdir(card_dir) if f[:5]=="table" and f[9:13]!="base"]
    card_names.sort()

    nBinsTotal = len(card_names)
    bkg_processes = ["zinv","llep","qcd"]
    nBkgs = len(bkg_processes)

    ## setup histograms
    h_data = ROOT.TH1D("h_data","",nBinsTotal,0,nBinsTotal)
    h_pred = ROOT.TH1D("h_pred","",nBinsTotal,0,nBinsTotal) # dummy hist for setting yMax
    h_bkg_vec = []
    for i,proc in enumerate(bkg_processes):
        h_bkg_vec.append(ROOT.TH1D("h_"+proc,"",nBinsTotal,0,nBinsTotal))
    g_unc = ROOT.TGraphAsymmErrors() # graph to store prediction uncertainties
    g_unc_ratio = ROOT.TGraphAsymmErrors() # graph to store prediction uncertainties

    srnames  = []
    ## fill histograms
    binLabels = []
    for ibin, cardname in enumerate(card_names):
        sr_name, ht_name, jbj_name, mt2name = utils.GetRegsFromCardName(cardname)
        srnames.append(sr_name)
        ibin += 1
        mt2left = mt2name.split("to")[0][1:]
        mt2right = mt2name.split("to")[1]
        if mt2right == "Inf": mt2right = "#infty"

        # get yields. first entry is data, rest are background predictions
        procs = utils.GetEverythingFromTablecard(os.path.join(card_dir,cardname))

        if "data" in procs:
            h_data.SetBinContent(ibin, procs["data"][0])
        for j in range(nBkgs):
            h_bkg_vec[j].SetBinContent(ibin, procs[bkg_processes[j]][0])

        # get uncertainties
        tot_pred, tot_unc_up, tot_unc_down = utils.GetTotalBackgroundNumbers(procs, bkg_processes)
        h_pred.SetBinContent(ibin, tot_pred)
        thisPoint = g_unc.GetN()
        g_unc.SetPoint(thisPoint, ibin-0.5, tot_pred)
        g_unc.SetPointError(thisPoint, 0.5, 0.5, tot_unc_down, tot_unc_up)
        g_unc_ratio.SetPoint(thisPoint, ibin-0.5, 1)
        g_unc_ratio.SetPointError(thisPoint, 0.5, 0.5, tot_unc_down/tot_pred, tot_unc_up/tot_pred)

        binLabels.append(mt2name[1:])

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
    if printData:
        pads.append(ROOT.TPad("1","1",0.0,0.18,1.0,1.0))
        pads.append(ROOT.TPad("2","2",0.0,0.0,1.0,0.19))
    else:
        pads.append(ROOT.TPad("1","1",0.0,0.07,1.0,1.0))
        pads.append(ROOT.TPad("2","2",0.0,0.0,1.0,0.0))

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

    yMin = 1e-3
    if userMax!=None:
        yMax = userMax
    else:
        yMax = h_data.GetMaximum() ** (2.0)
    h_pred.GetYaxis().SetRangeUser(yMin,yMax)
    h_pred.GetYaxis().SetTitle("Events / Bin")
    h_pred.GetYaxis().SetTitleOffset(1.2)
    h_pred.GetYaxis().SetTickLength(0.02)
    h_pred.GetXaxis().SetRangeUser(0,nBinsTotal)
    h_pred.GetXaxis().SetNdivisions(nBinsTotal,0,0)
    h_pred.GetXaxis().SetLabelSize(0)
    h_pred.GetXaxis().SetTickLength(0.015)

    # just draw dots to get axes. Will draw TGraphAsymmErrors on top later
    h_pred.SetMarkerStyle(1)
    h_pred.Draw("P")

    # draw the backgrounds
    stack.Draw("SAME HIST")

    # draw the prediction uncertainties
    g_unc.SetFillStyle(3244)
    g_unc.SetFillColor(ROOT.kGray+3)
    g_unc.Draw("SAME 2")

    # get a graph using proper asymmetric poissonian errors
    g_data = ROOT.TGraphAsymmErrors()
    ppmUtils.ConvertToPoissonGraph(h_data, g_data, drawZeros=True)
    g_data.SetPointError(g_data.GetN()-1, 0, 0, 0, 0)
    g_data.SetMarkerStyle(20)
    g_data.SetMarkerSize(1.2)
    g_data.SetLineWidth(1)

    # draw the graph and then axes again on top
    if printData:
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
    text.SetTextAlign(32)
    text.SetTextAngle(90)
    text.SetTextSize(min(binWidth * 1.2,0.026))
    text.SetTextFont(62)
    for ibin in range(nBinsTotal):
        x = left + (ibin+0.5)*binWidth
        y = pads[0].GetBottomMargin()-0.009
        text.DrawLatex(x,y,binLabels[ibin])

    # draw the "Pre-fit background" text
    text.SetTextAlign(13)
    text.SetTextFont(42)
    text.SetTextAngle(0)
    text.SetTextSize(0.05)
    text.DrawLatex(left+0.04,1-top-0.01, "Pre-fit background")

    # draw the HT bin  in upper middle
    text.SetTextAlign(23)
    text.SetTextFont(62)
    text.SetTextAngle(0)
    text.SetTextSize(0.035)
    text.DrawLatex(left+(1-right-left)*0.5, 1-top-0.01-0.04, srnames[0][:3])

    # Draw the CMS and luminosity text
    ppmUtils.DrawCmsText(pads[0],text="CMS Preliminary",textSize=0.038)
    ppmUtils.DrawLumiText(pads[0],lumi=utils.lumi,textSize=0.038)

    # draw the ht_j/bj region labels and line
    line = ROOT.TLine()
    line.SetNDC(1)
    line.SetLineStyle(2)
    line.SetLineWidth(1)
    line.SetLineColor(ROOT.kBlack)

    srcount = []
    counted = []
    for x in srnames:
        if x in counted:
            srcount[counted.index(x)] += 1
        else:
            counted.append(x)
            srcount.append(1)
    ibin = 0
    for isr, srreg in enumerate(counted):
        xcenter = left + binWidth*(ibin+(srcount[isr])*0.5)
        lines = srreg[3:]
        text.SetTextAlign(21)
        text.SetTextFont(62)
        text.SetTextSize(0.030)
        # in the last region, move the text left a bit to avoid overlap with tick marks
        if isr==len(srcount)-1:
            text.SetTextAlign(13)
            xcenter = left + binWidth*ibin + 0.007
            xcenter = max(xcenter, 1-right-0.25)
        y = bot+(1-top-bot)*0.85
        if xcenter > 1-right-0.19:
            y = 0.67
        text.DrawLatex(xcenter,y,lines)
        # text.DrawLatex(xcenter,y-text.GetTextSize()-0.001,lines[2:])

        x = left+binWidth*ibin
        line.DrawLineNDC(x,bot,x,bot+(1-top-bot)*0.85)
        ibin += srcount[isr]

    # legend
    leg = ROOT.TLegend(1-right-0.175,1-top-0.23,1-right-0.02,1-top-0.01)
    leg.SetBorderSize(1)
    leg.SetCornerRadius(0.3)
    if printData:
        leg.AddEntry(g_data,"Data","lp")
    for i in range(nBkgs):
        leg.AddEntry(h_bkg_vec[i], utils.GetLegendName(bkg_processes[i]),'f')
    leg.Draw()


    ####################
    #### RATIO PLOT ####
    ####################

    if printData:
        pads[1].cd()
        h_ratio = h_bkg_vec[0].Clone("h_ratio") #h_ratio is just a dummy histogram to draw axes correctly
        h_ratio.Reset()
        g_ratio = ROOT.TGraphAsymmErrors()
        h_pred = h_bkg_vec[0].Clone("h_pred")
        for i in range(1,nBkgs):
            h_pred.Add(h_bkg_vec[i])
        ppmUtils.GetPoissonRatioGraph(h_pred, h_data, g_ratio, drawZeros=True, useMCErr=False)
        h_ratio.GetYaxis().SetRangeUser(0,2)
        h_ratio.GetYaxis().SetNdivisions(505)
        h_ratio.GetYaxis().SetTitle("Data/Pred.")
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

        #draw the lines separating j-bj region
        line.SetNDC(1)
        line.SetLineStyle(2)
        line.SetLineWidth(1)
        line.SetLineColor(ROOT.kBlack)
        ibin = 0
        for i in range(len(jbj_regs)-1):
            ibin += len(mt2bins[i])-1
            line.DrawLine(ibin,0,ibin,2)

        h_ratio.Draw("SAME AXIS")
        g_ratio.Draw("SAME P0")

    # name = "prefit_{0}".format(ht_reg)
    name = "test"
    try:
        os.makedirs(outdir)
    except:
        pass
    c.SaveAs(os.path.join(outdir,name+"_"+srnames[0][:3]+".pdf"))
    # c.SaveAs(os.path.join(outdir,name+".png"))

    h_data.Delete()
    g_data.Delete()
    # h_ratio.Delete()
    # g_ratio.Delete()
    for h in h_bkg_vec:
        h.Delete()


def MakeTablesFromTablecards(card_dir, outdir, userMax=None, printData=True):
    card_names = [f for f in os.listdir(card_dir) if f[:5]=="table"]
    card_names.sort()

    cardname = card_names[0]

    table = printTableFromCard(card_dir, cardname)
    print table


def printTableFromCard(card_dir, cardname, printData=True):
    content = ["zinv", "llep", "qcd"]
    ncols = 1
    text  = "\\begin{table}[!ht]\n"
    text += "\\caption{"+"}\n"
    text += "\\centering\n"
    text += "\\begin{tabular}{r" + "|c"*ncols + "}\n"
    text += "\\hline\n"

    srname, htname, jbjname, mt2name = utils.GetRegsFromCardName(cardname)
    procs = utils.GetEverythingFromTablecard(os.path.join(card_dir,cardname))

    # title = srname + htname + jbjname
    title = srname
    binName = mt2name.split("to")[0][1:]+" $<$ mt2 $<$ "+mt2name.split("to")[1]

    text += "\\hline \n"
    text += "\\multicolumn{{{0}}}{{c}}{{{1}}} \\\\ \n".format(ncols+1, title)
    text += "\\hline \n\\hline \n"
    text += "Sample & "+binName+" \\\\ \n"

    for bkg in content:
        yields = procs[bkg][0]
        stat_up = procs[bkg][1][0]
        stat_dn = procs[bkg][1][1]
        syst_up = procs[bkg][1][2]
        syst_dn = procs[bkg][1][3]

        text += bkg+" & "
        text += "{0}".format(yields)
        if stat_up == stat_dn:
            text += "\\pm {0}(\\rm{{stat}})".format(stat_up)
        else:
            text += "^{{+{0}}}_{{-{1}}}(\\rm{{stat}})".format(stat_up, stat_dn)
        if syst_up == syst_dn:
            text += "\\pm {0}(\\rm{{syst}})".format(syst_up)
        else:
            text += "^{{+{0}}}_{{-{1}}}(\\rm{{syst}})".format(syst_up, syst_dn)
        text += " \\\\ \n"

    # text += "\\hline \n"
    text += "\\hline \\hline \n"
    text += "\end{tabular}\n"
    text += "\end{table}\n"

    return text
