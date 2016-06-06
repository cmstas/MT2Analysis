#include <iostream>
#include <iomanip>
#include <utility>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

#include "TROOT.h"
#include "TLatex.h"
#include "TPad.h"
#include "TString.h"
#include "TH2.h"
#include "THStack.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH1D.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TKey.h"


std::string setBinLabel(int ibin){

  if (ibin==1) return "E_{T}^{miss}>200, 2-3j, 0b";
  if (ibin==2) return "E_{T}^{miss}>200, 2-3j, #geq 1b";
  if (ibin==3) return "E_{T}^{miss}>200, 2-3j, #geq 1B";
  if (ibin==4) return "E_{T}^{miss}>200, 4-5j, 0b";
  if (ibin==5) return "E_{T}^{miss}>200, 4-5j, #geq 1b";
  if (ibin==6) return "E_{T}^{miss}>200, 4-5j, #geq 1B";
  if (ibin==7) return "E_{T}^{miss}>200, 6+j, 0-2B";
  if (ibin==8) return "E_{T}^{miss}>300, 2-3j, 0b";
  if (ibin==9) return "E_{T}^{miss}>300, 2-3j, #geq 1b";
  if (ibin==10) return "E_{T}^{miss}>300, 2-3j, #geq 1B";
  if (ibin==11) return "E_{T}^{miss}>300, 4-5j, 0b";
  if (ibin==12) return "E_{T}^{miss}>300, 4-5j, #geq 1b";
  if (ibin==13) return "E_{T}^{miss}>300, 4-5j, #geq 1B";
  if (ibin==14) return "E_{T}^{miss}>300, 6+j, 0-2B";
  if (ibin==15) return "E_{T}^{miss}>200, 1j, 0b";
  if (ibin==16) return "E_{T}^{miss}>300, 1j, 0b";
  if (ibin==17) return "E_{T}^{miss}>200, 1j, #geq 1b";
  if (ibin==18) return "E_{T}^{miss}>200, 1j, #geq 1B";
  if (ibin==19) return "E_{T}^{miss}>200, #geq 3B";
  if (ibin==20) return "E_{T}^{miss}>500, #geq 2j, 0-2B";
  if (ibin==21) return "H_{T}>1000, #geq 2j, 0-2B";
  
  return "error";
  
}

void setHistParams(TH1D* h, int nlep, int updn){

  h->SetMarkerStyle(20);
  h->SetMarkerColor(kBlack);
  
  if (nlep==2) h->GetYaxis()->SetRangeUser(0.0,0.15);
  else if (nlep==1) h->GetYaxis()->SetRangeUser(0.0,0.05);
  
  if (nlep==2 && updn==2) h->SetTitle("PDF UP Variations;;2L Transfer Factor Variation");
  else if (nlep==2 && updn==1) h->SetTitle("PDF DOWN Variations;;2L Transfer Factor Variation");
  else if (nlep==1 && updn==2) h->SetTitle("PDF UP Variations;;1L Transfer Factor Variation");
  else if (nlep==1 && updn==1) h->SetTitle("PDF DOWN Variations;;1L Transfer Factor Variation");
  
  return;
  
}

void drawHist(TH1D* h, TString name, int nlep, int updn) {

  setHistParams(h, nlep, updn);
  
  for (int ibin = 1; ibin < h->GetNbinsX(); ibin++){
    string binName = setBinLabel(ibin);
    h->GetXaxis()->SetBinLabel(ibin, binName.c_str());
  }

	    
  gStyle->SetOptStat("");
  gStyle->SetCanvasColor(0);
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetFrameBorderMode(0);
  
  TString canvas_name = "c_"+name;
  TCanvas* can = new TCanvas(canvas_name,canvas_name, 1200, 600);
  can->cd();
  gPad->SetRightMargin(0.05);
  gPad->Modified();
  
  // splitting canvas for ratio plots
  TPad* fullpad(0);
  TPad* plotpad(0);

  // master pad
  fullpad = new TPad("fullpad","fullpad",0,0,1,1);
  fullpad->Draw();
  fullpad->cd();
  
  // // main plot pad, for ratio on bottom
  // plotpad = new TPad("plotpad","plotpad",0,0.2,1,0.99);
  // plotpad->SetRightMargin(0.05);
  // //    plotpad->SetBottomMargin(0.12);
  // plotpad->SetBottomMargin(0.26);
  // plotpad->Draw();
  // plotpad->cd();

  h->Draw("");
  
  can->Print("plots/c_"+name+".pdf");

  return;
}

void makePDFVariationPlots(TString inputDir="../../SoftLepLooper/output/test_pdf/"){

  TFile* fOut = new TFile("pdfVars.root","RECREATE");
  TFile* fIn = new TFile(inputDir+"/add.root");

  fOut->cd();

  //make TFs and save them to output file
  for (unsigned int ipdf = 0; ipdf < 110; ipdf++) {
    TH1D* h_sr1L = (TH1D*) fIn->Get(Form("h_srLep_mtbinsPDFvar%dOnelep",ipdf))->Clone();
    TH1D* h_sr2L = (TH1D*) fIn->Get(Form("h_srLep_mtbinsPDFvar%dDilepton",ipdf))->Clone();
    TH1D* h_cr1L = (TH1D*) fIn->Get(Form("h_cr1L_mtbinsPDFvar%dOnelep",ipdf))->Clone();
    TH1D* h_cr2L = (TH1D*) fIn->Get(Form("h_cr2L_mtbinsPDFvar%dDilepton",ipdf))->Clone();

    TH1D* h_TF1L = (TH1D*) h_sr1L->Clone(Form("h_TF1L_PDFvar%d",ipdf));
    TH1D* h_TF2L = (TH1D*) h_sr2L->Clone(Form("h_TF2L_PDFvar%d",ipdf));

    h_TF1L->Reset();
    h_TF2L->Reset();

    h_TF1L->Divide(h_sr1L,h_cr1L);
    h_TF2L->Divide(h_sr2L,h_cr2L);

    h_TF1L->Write();
    h_TF2L->Write();

    h_sr1L->Delete();
    h_sr2L->Delete();
    h_cr1L->Delete();
    h_cr2L->Delete();
    h_TF1L->Delete();
    h_TF2L->Delete();
  }

  //initialize money plots
  TH1D* h_TF1L_nominal = (TH1D*) fOut->Get("h_TF1L_PDFvar0")->Clone("h_TF1L_nominal");
  TH1D* h_TF1L_pdfVarSigmaUP = (TH1D*) fIn->Get("h_srLep_mtbinsPDFvar0")->Clone("h_TF1L_pdfVarSigmaUP");
  TH1D* h_TF1L_pdfVarSigmaDN = (TH1D*) fIn->Get("h_srLep_mtbinsPDFvar0")->Clone("h_TF1L_pdfVarSigmaDN");
  TH1D* h_TF1L_pdfVarUP = (TH1D*) fIn->Get("h_srLep_mtbinsPDFvar0")->Clone("h_TF1L_pdfVarUP");
  TH1D* h_TF1L_pdfVarDN = (TH1D*) fIn->Get("h_srLep_mtbinsPDFvar0")->Clone("h_TF1L_pdfVarDN");
  
  TH1D* h_TF2L_nominal = (TH1D*) fOut->Get("h_TF2L_PDFvar0")->Clone("h_TF2L_nominal");
  TH1D* h_TF2L_pdfVarSigmaUP = (TH1D*) fIn->Get("h_srLep_mtbinsPDFvar0")->Clone("h_TF2L_pdfVarSigmaUP");
  TH1D* h_TF2L_pdfVarSigmaDN = (TH1D*) fIn->Get("h_srLep_mtbinsPDFvar0")->Clone("h_TF2L_pdfVarSigmaDN");
  TH1D* h_TF2L_pdfVarUP = (TH1D*) fIn->Get("h_srLep_mtbinsPDFvar0")->Clone("h_TF2L_pdfVarUP");
  TH1D* h_TF2L_pdfVarDN = (TH1D*) fIn->Get("h_srLep_mtbinsPDFvar0")->Clone("h_TF2L_pdfVarDN");

  h_TF1L_pdfVarUP->Reset();
  h_TF2L_pdfVarUP->Reset();
  h_TF1L_pdfVarDN->Reset();
  h_TF2L_pdfVarDN->Reset();
  h_TF1L_pdfVarSigmaUP->Reset();
  h_TF2L_pdfVarSigmaUP->Reset();
  h_TF1L_pdfVarSigmaDN->Reset();
  h_TF2L_pdfVarSigmaDN->Reset();

  //1L, loop over bins
  float dA_plus1L(0), dA_minus1L(0);
  float N_plus1L(0), N_minus1L(0);
  for (unsigned int ibin = 1; ibin < 22; ibin++) {
    for (unsigned int ipdf = 10; ipdf < 110; ipdf += 2) {

      TH1D* h_up = (TH1D*) fOut->Get(Form("h_TF1L_PDFvar%d",ipdf))->Clone();
      TH1D* h_dn = (TH1D*) fOut->Get(Form("h_TF1L_PDFvar%d",ipdf+1))->Clone();

      float pdfcentral(0), pdf1(0), pdf2(0);
      
      if( h_TF1L_nominal->GetBinContent(ibin) > 0 ) {
	pdfcentral = h_TF1L_nominal->GetBinContent(ibin);
	pdf1 = h_up->GetBinContent(ibin);
	pdf2 = h_dn->GetBinContent(ibin);
      }
      else {
	continue;
      }

      if( pdf1-pdfcentral > 0 || pdf2 - pdfcentral > 0 ){
	dA_plus1L += pow( max( (pdf1 - pdfcentral), (pdf2 - pdfcentral) ), 2);
	N_plus1L++;
      }

      if( pdf1-pdfcentral < 0 || pdf2 - pdfcentral < 0 ){
	dA_minus1L += pow( max( (pdfcentral - pdf1), (pdfcentral - pdf2) ), 2);
	N_minus1L++;
      }
    }//loop over pdf variations
    h_TF1L_pdfVarSigmaUP->SetBinContent(ibin, sqrt(1./(N_plus1L*1.-1.)*dA_plus1L));
    h_TF1L_pdfVarSigmaDN->SetBinContent(ibin, sqrt(1./(N_minus1L*1.-1.)*dA_minus1L));
    h_TF1L_pdfVarUP->SetBinContent(ibin, h_TF1L_pdfVarSigmaUP->GetBinContent(ibin)/h_TF1L_nominal->GetBinContent(ibin));
    h_TF1L_pdfVarDN->SetBinContent(ibin, h_TF1L_pdfVarSigmaDN->GetBinContent(ibin)/h_TF1L_nominal->GetBinContent(ibin));    
  }//loop over bins

  h_TF1L_pdfVarSigmaUP->Write();
  h_TF1L_pdfVarSigmaDN->Write();
  h_TF1L_pdfVarUP->Write();
  h_TF1L_pdfVarDN->Write();

  //2L, loop over bins
  float dA_plus2L(0), dA_minus2L(0);
  float N_plus2L(0), N_minus2L(0);
  for (unsigned int ibin = 1; ibin < 22; ibin++) {
    for (unsigned int ipdf = 10; ipdf < 110; ipdf += 2) {

      TH1D* h_up = (TH1D*) fOut->Get(Form("h_TF2L_PDFvar%d",ipdf))->Clone();
      TH1D* h_dn = (TH1D*) fOut->Get(Form("h_TF2L_PDFvar%d",ipdf+1))->Clone();

      float pdfcentral(0), pdf1(0), pdf2(0);
      
      if( h_TF2L_nominal->GetBinContent(ibin) > 0 ) {
	pdfcentral = h_TF2L_nominal->GetBinContent(ibin);
	pdf1 = h_up->GetBinContent(ibin);
	pdf2 = h_dn->GetBinContent(ibin);
      }
      else {
	continue;
      }

      if( pdf1-pdfcentral > 0 || pdf2 - pdfcentral > 0 ){
	dA_plus2L += pow( max( (pdf1 - pdfcentral), (pdf2 - pdfcentral) ), 2);
	N_plus2L++;
      }

      if( pdf1-pdfcentral < 0 || pdf2 - pdfcentral < 0 ){
	dA_minus2L += pow( max( (pdfcentral - pdf1), (pdfcentral - pdf2) ), 2);
	N_minus2L++;
      }
    }//loop over pdf variations
    h_TF2L_pdfVarSigmaUP->SetBinContent(ibin, sqrt(1./(N_plus2L*1.-1.)*dA_plus2L));
    h_TF2L_pdfVarSigmaDN->SetBinContent(ibin, sqrt(1./(N_minus2L*1.-1.)*dA_minus2L));
    h_TF2L_pdfVarUP->SetBinContent(ibin, h_TF2L_pdfVarSigmaUP->GetBinContent(ibin)/h_TF2L_nominal->GetBinContent(ibin));
    h_TF2L_pdfVarDN->SetBinContent(ibin, h_TF2L_pdfVarSigmaDN->GetBinContent(ibin)/h_TF2L_nominal->GetBinContent(ibin));    
  }//loop over bins

  h_TF2L_pdfVarSigmaUP->Write();
  h_TF2L_pdfVarSigmaDN->Write();
  h_TF2L_pdfVarUP->Write();
  h_TF2L_pdfVarDN->Write();

  //-----------------------------
  //-----make nice histos--------
  //-----------------------------

  drawHist(h_TF2L_pdfVarDN,"TF2L_pdfVarDN",2,1);
  drawHist(h_TF2L_pdfVarUP,"TF2L_pdfVarUP",2,2);
  drawHist(h_TF1L_pdfVarDN,"TF1L_pdfVarDN",1,1);
  drawHist(h_TF1L_pdfVarUP,"TF1L_pdfVarUP",1,2);
  
  return;
  
}

