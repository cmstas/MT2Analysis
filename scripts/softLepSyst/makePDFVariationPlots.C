#include <iostream>
#include <iomanip>
#include <utility>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

#include "TROOT.h"
#include "TLatex.h"
#include "TString.h"
#include "TH2.h"
#include "THStack.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TKey.h"

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
  
  // TH1D* h_TF2L_nominal = (TH1D*) fOut->Get("h_TF2L_PDFvar0")->Clone("h_TF2L_nominal");
  // TH1D* h_TF2L_pdfVarSigmaUP = (TH1D*) fIn->Get("h_srLep_mtbinsPDFvar0")->Clone("h_TF2L_pdfVarSigmaUP");
  // TH1D* h_TF2L_pdfVarSigmaDN = (TH1D*) fIn->Get("h_srLep_mtbinsPDFvar0")->Clone("h_TF2L_pdfVarSigmaDN");
  // TH1D* h_TF2L_pdfVarUP = (TH1D*) fIn->Get("h_srLep_mtbinsPDFvar0")->Clone("h_TF2L_pdfVarUP");
  // TH1D* h_TF2L_pdfVarDN = (TH1D*) fIn->Get("h_srLep_mtbinsPDFvar0")->Clone("h_TF2L_pdfVarDN");

  h_TF1L_pdfVarUP->Reset();
  //  h_TF2L_pdfVarUP->Reset();
  h_TF1L_pdfVarDN->Reset();
  //h_TF2L_pdfVarDN->Reset();
  h_TF1L_pdfVarSigmaUP->Reset();
  //h_TF2L_pdfVarSigmaUP->Reset();
  h_TF1L_pdfVarSigmaDN->Reset();
  //h_TF2L_pdfVarSigmaDN->Reset();

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

  
  return;
  
}
