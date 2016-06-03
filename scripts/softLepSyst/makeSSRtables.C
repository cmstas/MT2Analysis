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

bool doPostfit = false;

void makeTable (TFile* fIn, vector<int> srVec, string regionTitle) {

  //get prefit file regardless for signal yields
  TFile * fPre = new TFile("preFitYields.root");

  
  //initialize
  double fakeYield = 0;
  double dilepYield = 0;
  double onelepYield = 0;
  double totalYield = 0;
  double dataYield = 0;
  double fakeErr = 0;
  double dilepErr = 0;
  double onelepErr = 0;
  double totalErr = 0;
  double dataErr = 0;
  
  double t24bdYield = 0;
  double t24bdErr = 0;
  double t5qqqqwwYield = 0;
  double t5qqqqwwErr = 0;

  //load histograms
  TH1F* hFakes = (TH1F*) fIn->Get("h_bkg1")->Clone();
  TH1F* hDilep = (TH1F*) fIn->Get("h_bkg2")->Clone();
  TH1F* hOnelep = (TH1F*) fIn->Get("h_bkg3")->Clone();
  TH1F* hData = (TH1F*) fIn->Get("h_data")->Clone();
  TH1F* hT24bd = (TH1F*) fPre->Get("h_sig4")->Clone();
  TH1F* hT5qqqqww = (TH1F*) fPre->Get("h_sig5")->Clone();
  THStack* hStack = (THStack*) fIn->Get("h_total")->Clone();
  TH1F* hTotal = (TH1F*) hStack->GetStack()->Last()->Clone();
  
  //loop over SRs to get total yields/errors
  for (unsigned int i=0; i < srVec.size(); i++) {
    cout << "Adding region " << srVec[i] << "..." << endl;
    
    //fakes
    fakeYield += hFakes->GetBinContent(srVec[i]);
    fakeErr += hFakes->GetBinError(srVec[i]);

    //dilep
    dilepYield += hDilep->GetBinContent(srVec[i]);
    dilepErr += hDilep->GetBinError(srVec[i]);

    //onelep
    onelepYield += hOnelep->GetBinContent(srVec[i]);
    onelepErr += hOnelep->GetBinError(srVec[i]);

    //t24bd
    t24bdYield += hT24bd->GetBinContent(srVec[i]);
    if (TString(regionTitle).Contains("Comp")){
      float thisBinErr =  hT24bd->GetBinError(srVec[i]);
      t24bdErr += thisBinErr/2; //half error is linearly added
      t24bdErr = sqrt(t24bdErr*t24bdErr + thisBinErr/2 * thisBinErr/2); //half error is added in quad
    }
    else t24bdErr += hT24bd->GetBinError(srVec[i]);

    //t5qqqqww
    t5qqqqwwYield += hT5qqqqww->GetBinContent(srVec[i]);
    if (TString(regionTitle).Contains("Comp")){
      float thisBinErr =  hT5qqqqww->GetBinError(srVec[i]);
      t5qqqqwwErr += thisBinErr/2; //half error is linearly added
      t5qqqqwwErr = sqrt(t5qqqqwwErr*t5qqqqwwErr + thisBinErr/2 * thisBinErr/2); //half error is added in quad
    }
    else t5qqqqwwErr += hT5qqqqww->GetBinError(srVec[i]);

    //total
    totalYield += hTotal->GetBinContent(srVec[i]);
    if (TString(regionTitle).Contains("Comp")){
      float thisBinErr =  hTotal->GetBinError(srVec[i]);
      totalErr += thisBinErr/2; //half error is linearly added
      totalErr = sqrt(totalErr*totalErr + thisBinErr/2 * thisBinErr/2); //half error is added in quad
    }
    else totalErr += hTotal->GetBinError(srVec[i]);
    
    //data
    dataYield += hData->GetBinContent(srVec[i]);
    dataErr += hData->GetBinError(srVec[i]);
  }

  std::cout << "--------BEGIN TABLE--------" << endl << endl;
  std::cout << "\\begin{tabular}{l|c}" << endl;
  std::cout << "\\hline" << endl;
  std::cout << "\\multicolumn{2}{c}{"<< regionTitle << "} \\\\" << endl;
  std::cout << "\\hline" << endl;
  std::cout << "\\hline" << endl;
  std::cout << "Sample & Yield \\\\" << endl;
  std::cout << "\\hline" << endl;
  std::cout << "\\hline" << endl;
  std::cout <<  Form("Fake (Pred) & %.1f \\\\",fakeYield) << endl;
  std::cout <<  Form("2 Lep (Pred) & %.1f \\\\",dilepYield) << endl;
  std::cout <<  Form("1 Lep (Pred) & %.1f \\\\",onelepYield) << endl;
  // std::cout <<  Form("Fake (Pred) & %.1f $\\pm$ %.1f \\\\",fakeYield,fakeErr) << endl;
  // std::cout <<  Form("2 Lep (Pred) & %.1f $\\pm$ %.1f \\\\",dilepYield,dilepErr) << endl;
  // std::cout <<  Form("1 Lep (Pred) & %.1f $\\pm$ %.1f \\\\",onelepYield,onelepErr) << endl;
  std::cout << "\\hline" << endl;
  if (doPostfit) std::cout <<  Form("Total SM Post-Fit & %.1f $\\pm$ %.1f \\\\",totalYield,totalErr) << endl;
  else std::cout <<  Form("Total SM Pre-Fit & %.1f $\\pm$ %.1f \\\\",totalYield,totalErr) << endl;
  std::cout << "\\hline" << endl;
  std::cout <<  Form("Data & %.f \\\\",dataYield) << endl;
  std::cout << "\\hline" << endl;
  std::cout <<  Form("T2-4bd 275,235 & %.f $\\pm$ %.1f \\\\",t24bdYield,t24bdErr) << endl;
  std::cout <<  Form("T5qqqqWW 1100,500 & %.f $\\pm$ %.1f \\\\",t5qqqqwwYield,t5qqqqwwErr) << endl;
  std::cout << "\\hline" << endl;
  std::cout << "\\end{tabular}" << endl;

  std::cout << endl << "--------END TABLE--------" << endl << endl;
  
  return;
}

void makeSSRtables() {

  TFile * fIn;
  if (doPostfit) fIn = new TFile("postFitYields.root");
  else fIn = new TFile("preFitYields.root");

  //vector of SRs which make up the SSR
  vector<int> srVec;
  string srTitle = "";
  
  //zero B region
  srTitle = "nB = 0, $M_{T} > 120$";
  srVec.clear();
  srVec.push_back(3);
  srVec.push_back(12);
  srVec.push_back(21);
  srVec.push_back(24);
  srVec.push_back(33);
  srVec.push_back(42);
  srVec.push_back(45);
  srVec.push_back(48);
  srVec.push_back(60);
  srVec.push_back(63);
  makeTable(fIn, srVec, srTitle);

  //with B region
  srTitle = "nB $>$ 0, $M_{T} > 120$";
  srVec.clear();
  srVec.push_back(6);
  srVec.push_back(9);
  srVec.push_back(15);
  srVec.push_back(18);
  srVec.push_back(27);
  srVec.push_back(30);
  srVec.push_back(36);
  srVec.push_back(39);
  srVec.push_back(51);
  srVec.push_back(54);
  srVec.push_back(57);
  makeTable(fIn, srVec, srTitle);

  //Compressed Stop region
  srTitle = "Compressed Stop Region";
  srVec.clear();
  srVec.push_back(25);
  srVec.push_back(26);
  srVec.push_back(27);
  srVec.push_back(49);
  srVec.push_back(50);
  srVec.push_back(51);
  srVec.push_back(59);
  srVec.push_back(60);
  srVec.push_back(47);
  srVec.push_back(48);
  makeTable(fIn, srVec, srTitle);

  //no b-tag ISR region
  srTitle = "Zero B ISR Region";
  srVec.clear();
  srVec.push_back(47);
  srVec.push_back(48);
  srVec.push_back(23);
  srVec.push_back(24);
  srVec.push_back(59);
  srVec.push_back(60);
  makeTable(fIn, srVec, srTitle);
  
  //high MET and high HT region
  srTitle = "Strong Production";
  srVec.clear();
  srVec.push_back(59);
  srVec.push_back(60);
  srVec.push_back(62);
  srVec.push_back(63);
  makeTable(fIn, srVec, srTitle);
  
  return;
}
