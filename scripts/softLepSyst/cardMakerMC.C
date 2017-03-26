#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <set>

#include "TROOT.h"
#include "TLatex.h"
#include "TString.h"
#include "TFile.h"
#include "TH1.h"
#include "TH3.h"
#include "TList.h"
#include "TCollection.h"
#include "TKey.h"
#include "TBenchmark.h"

using namespace std;

TFile* f_ttsl = 0;
TFile* f_ttdl = 0;
TFile* f_wjets = 0;
TFile* f_zjets = 0;
TFile* f_sig = 0;
TFile* f_data = 0;

const bool verbose = false;

const bool suppressZeroBins = false;

const bool suppressZeroTRs = false;

const float dummy_alpha = 1.; // dummy value for gmN when there are no SR events

const bool uncorrelatedZGratio = false; // treat ZGratio uncertainty as fully uncorrelated

const bool fourNuisancesPerBinZGratio = true;

const bool integratedZinvEstimate = true;

const bool doDummySignalSyst = false;

double last_dilep_transfer = 1.0;
double last_onelep_transfer = 1.0;

//_______________________________________________________________________________
void ReplaceString(std::string& subject, const std::string& search, const std::string& replace) {
    size_t pos = 0;
    while((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}

//_______________________________________________________________________________
double round(double d)
{
  return floor(d + 0.5);
}

//_______________________________________________________________________________
int printCard( string dir_str , int mt2bin , string signal, string output_dir, int scanM1 = -1, int scanM2 = -1) {

  // read off yields from h_mt2bins hist in each topological region
  if (verbose) cout<<"Looking at region "<<dir_str<<", mt bin "<<mt2bin<<endl;
  TString dir = TString(dir_str);
  TString fullhistname = dir + "/h_mtbins";
  TString fullhistnameScan  = fullhistname+"_sigscan";


  TString signame(signal);
  if (scanM1 >= 0 && scanM2 >= 0) {
    signame = Form("%s_%d_%d",signal.c_str(),scanM1,scanM2);
  }
  bool isSignalWithLeptons = bool(signame.Contains("T1tttt") || signame.Contains("T2tt"));

  double n_ttsl(0.);
  //  double n_ttsl_cr(0.); // If we want to have a gamma, but why bother for this exercise?
  double n_ttdl(0.);
  double n_wjets(0.);
  double n_zjets(0.);

  double n_bkg(0.);
  double n_data(0.);

  double n_sig(0.);

  TH1D* h_sig(0);
  // pick point out of signal scan
  if (scanM1 >= 0 && scanM2 >= 0) {
    TH3D* h_sigscan = (TH3D*) f_sig->Get(fullhistnameScan);
    if (!h_sigscan) return 0;
    int bin1 = h_sigscan->GetYaxis()->FindBin(scanM1);
    int bin2 = h_sigscan->GetZaxis()->FindBin(scanM2);
    h_sig = h_sigscan->ProjectionX(Form("h_mt2bins_%d_%d_%s",scanM1,scanM2,dir_str.c_str()),bin1,bin1,bin2,bin2);
  }
  // single point sample
  else {
    h_sig = (TH1D*) f_sig->Get(fullhistname);
    // Trick to print out monojet regions even when running on signal without monojet events
    //    if (fullhistname.Contains("J")) 
    //      h_sig = (TH1D*) f_sig->Get("sr6M/h_mt2bins");
  }

  if (!h_sig) {
    if (verbose) cout<<"No signal found in this region"<<endl;
    return 0;
  }
  n_sig = h_sig->GetBinContent(mt2bin);
  
  //Get variable boundaries for signal region.
  //Used to create datacard name.
  TH1D* h_ht_LOW = (TH1D*) f_sig->Get(dir+"/h_ht_LOW");
  TH1D* h_ht_HI = (TH1D*) f_sig->Get(dir+"/h_ht_HI");
  int ht_LOW = h_ht_LOW->GetBinContent(1);
  int ht_HI = h_ht_HI->GetBinContent(1);
  
  TH1D* h_met_LOW = (TH1D*) f_sig->Get(dir+"/h_met_LOW");
  TH1D* h_met_HI = (TH1D*) f_sig->Get(dir+"/h_met_HI");
  int met_LOW = h_met_LOW->GetBinContent(1);
  int met_HI = h_met_HI->GetBinContent(1);
  
  TH1D* h_nbjets_LOW = (TH1D*) f_sig->Get(dir+"/h_nbjets_LOW");
  TH1D* h_nbjets_HI = (TH1D*) f_sig->Get(dir+"/h_nbjets_HI");
  int nbjets_LOW = h_nbjets_LOW->GetBinContent(1);
  int nbjets_HI = h_nbjets_HI->GetBinContent(1);
  
  TH1D* h_nbjetshard_LOW = (TH1D*) f_sig->Get(dir+"/h_nbjetshard_LOW");
  TH1D* h_nbjetshard_HI = (TH1D*) f_sig->Get(dir+"/h_nbjetshard_HI");
  int nbjetshard_LOW = h_nbjetshard_LOW->GetBinContent(1);
  int nbjetshard_HI = h_nbjetshard_HI->GetBinContent(1);
  
  TH1D* h_njets_LOW = (TH1D*) f_sig->Get(dir+"/h_njets_LOW");
  TH1D* h_njets_HI = (TH1D*) f_sig->Get(dir+"/h_njets_HI");
  int njets_LOW = h_njets_LOW->GetBinContent(1);
  int njets_HI = h_njets_HI->GetBinContent(1);

  int mt2_LOW = h_sig->GetBinLowEdge(mt2bin);
  int mt2_HI = mt2_LOW + h_sig->GetBinWidth(mt2bin);
  // hardcode the current edge of our highest bin..
  if (mt2_HI == 300) mt2_HI = -1;

  int nbjets_HI_mod = nbjets_HI;
  int nbjetshard_HI_mod = nbjetshard_HI;
  int njets_HI_mod = njets_HI;
  if(nbjets_HI != -1) nbjets_HI_mod--;
  if(nbjetshard_HI != -1) nbjetshard_HI_mod--;
  if(njets_HI != -1) njets_HI_mod--;

  std::string ht_str = "HT" + to_string(ht_LOW) + "to" + to_string(ht_HI);
  std::string met_str = "MET" + to_string(met_LOW) + "to" + to_string(met_HI);
  std::string jet_str = (njets_HI_mod == njets_LOW) ? "j" + to_string(njets_LOW) : "j" + to_string(njets_LOW) + "to" + to_string(njets_HI_mod);
  std::string bjet_str = (nbjets_HI_mod == nbjets_LOW) ? "b" + to_string(nbjets_LOW) : "b" + to_string(nbjets_LOW) + "to" + to_string(nbjets_HI_mod);
  std::string bjethard_str = (nbjetshard_HI_mod == nbjetshard_LOW) ? "Hb" + to_string(nbjetshard_LOW) : "Hb" + to_string(nbjetshard_LOW) + "to" + to_string(nbjetshard_HI_mod);
  std::string mt2_str = "m" + to_string(mt2_LOW) + "to" + to_string(mt2_HI);
  
  //Replace instances of "-1" with "inf" for variables with no upper bound.
  ReplaceString(ht_str, "-1", "Inf");
  ReplaceString(met_str, "-1", "Inf");
  ReplaceString(jet_str, "-1", "Inf");
  ReplaceString(bjet_str, "-1", "Inf");
  ReplaceString(bjethard_str, "-1", "Inf");
  ReplaceString(mt2_str, "-1", "Inf");

  std::string channel = ht_str + "_" + met_str + "_" + jet_str + "_" + bjet_str + "_" + bjethard_str + "_" + mt2_str;
  std::string topologicalR = ht_str + "_" + met_str + "_" + jet_str + "_" + bjet_str + "_" + bjethard_str;
  TString perChannel(channel.c_str());
  TString perTopoRegion(topologicalR.c_str());
  
  TString cardname = Form("%s/datacard_%s_%s.txt",output_dir.c_str(),channel.c_str(),signame.Data());

  //  if (suppressZeroBins && ((n_sig < 0.1) || (n_sig/n_bkg < 0.02))) {
  if ( (suppressZeroBins && (n_sig < 0.001)) || (suppressZeroTRs && (n_sig < 0.001)) ) {
    if (verbose) std::cout << "Zero signal, card not printed: " << cardname << std::endl;
    return 0;
  }

  
  // -------TTSL-------
  // get all the numbers we need from the histograms we have
  // !!!!! HACK: set zero bins to 0.01 for now to make combine happy
  // PREDICTION
  int n_mt2bins = 1;
  TH1D* h_ttslpred = (TH1D*) f_ttsl->Get(fullhistname);
  if (h_ttslpred != 0) {
    n_ttsl = h_ttslpred->GetBinContent(mt2bin);
    n_mt2bins = h_ttslpred->GetNbinsX();
  }
  TH1D* h_ttdlpred = (TH1D*) f_ttdl->Get(fullhistname);
  if (h_ttdlpred != 0) {
    n_ttdl = h_ttdlpred->GetBinContent(mt2bin);
  }
  TH1D* h_wjetspred = (TH1D*) f_wjets->Get(fullhistname);
  if (h_wjetspred != 0) {
    n_wjets = h_wjetspred->GetBinContent(mt2bin);
  }
  TH1D* h_zjetspred = (TH1D*) f_zjets->Get(fullhistname);
  if (h_zjetspred != 0) {
    n_zjets = h_zjetspred->GetBinContent(mt2bin);
  }
//  TH1D* h_fakepred = (TH1D*) f_fake->Get(fullhistnameFakes);
//  if (h_fakepred != 0) {
//    n_fakes = h_fakepred->GetBinContent(mt2bin);
//  }

  n_bkg = n_ttsl + n_ttdl + n_wjets + n_zjets;

  if (f_data) {
    // n_data is 0 by default
    TH1D* h_data = (TH1D*) f_data->Get(fullhistname);
    if (h_data != 0) {
      n_data = h_data->GetBinContent(mt2bin);
    } 
  } else {
    // using only MC: take observation == total bkg
    n_data = n_bkg;
  }

  
  int n_syst = 0;
  // ----- lost lepton bkg uncertainties
  double ttsl_syst = 1.20;
  double ttdl_syst = 1.20;
  double wjets_syst = 1.20;
  double zjets_syst = 1.20;
  double ttsl_TF = 1.;  // ~ same stats in SR and CR
  double ttdl_TF = 1.;  
  double wjets_TF = 1.; 
  double ttsl_CRstat =  round(n_ttsl  * 1.0 / ttsl_TF); // Set CR yield so that SR=CR*TF
  double ttdl_CRstat =  round(n_ttdl  * 1.0 / ttsl_TF);
  double wjets_CRstat = round(n_wjets * 1.0 / ttsl_TF);
  double ttsl_shape_bin2 = 1.15;
  double ttsl_shape_bin3 = 1.05;
  double ttdl_shape_bin2 = 1.30;
  double ttdl_shape_bin3 = 1.50;
  double wjets_shape_bin2 = 1.15;
  double wjets_shape_bin3 = 1.15;

  // Set SR yield so that it is based on CR yield
  n_ttsl = ttsl_CRstat * ttsl_TF;
  n_ttdl = ttdl_CRstat * ttdl_TF;
  n_wjets = wjets_CRstat * wjets_TF;

  TString name_ttsl_syst = "ttsl_syst";
  TString name_ttdl_syst = "ttdl_syst";
  TString name_wjets_syst = "wjets_syst";
  TString name_zjets_syst = "zjets_syst";
  TString name_ttsl_CRstat  = Form("ttsl_CRstat_%s_%s_%s_%s_%s", ht_str.c_str(), met_str.c_str(), jet_str.c_str(), bjet_str.c_str(), bjethard_str.c_str());
  TString name_ttdl_CRstat  = Form("ttdl_CRstat_%s_%s_%s_%s_%s", ht_str.c_str(), met_str.c_str(), jet_str.c_str(), bjet_str.c_str(), bjethard_str.c_str());
  TString name_wjets_CRstat = Form("wjets_CRstat_%s_%s_%s_%s_%s", ht_str.c_str(), met_str.c_str(), jet_str.c_str(), bjet_str.c_str(), bjethard_str.c_str());
  TString name_ttsl_shape = "ttsl_shape";
  TString name_ttdl_shape = "ttdl_shape";
  TString name_wjets_shape = "wjets_shape";

  n_syst += 10; 


  // ----- sig uncertainties
  double sig_syst         = 1.20; // dummy 20% from early MC studies
 
  // fully correlated for lumi, btagsf, lepeff, isr.  Fully uncorrelated for stats and other systs
  TString name_sig_syst         = "sig_syst";
  
  n_syst += 1; 


  ofstream ofile;
  ofile.open(cardname);

  ofile <<  "imax 1  number of channels"                                                    << endl;
  ofile <<  "jmax 4  number of backgrounds"                                                 << endl;
  ofile <<  "kmax *"                                                                        << endl;
  ofile <<  "------------"                                                                  << endl;
  ofile <<  Form("bin         %s",channel.c_str())                                          << endl;
  ofile <<  Form("observation %.0f",n_data)                                                 << endl;
  ofile <<  "------------"                                                                  << endl;
  ofile <<  Form("bin             %s   %s   %s   %s   %s",channel.c_str(),channel.c_str(),channel.c_str(),channel.c_str(),channel.c_str()) << endl;
  ofile <<  "process          sig     ttsl     ttdl    wjets    zjets"                                      << endl;
  ofile <<  "process           0         1           2         3         4"                                      << endl;
  ofile <<  Form("rate            %.3f    %.3f      %.3f      %.3f      %.3f",n_sig,n_ttsl,n_ttdl,n_wjets,n_zjets) << endl;
  ofile <<  "------------"                                                                  << endl;
 
  // ----  systs
  ofile <<  Form("%s     lnN    %.3f    -      -      -      - ",name_sig_syst.Data(),sig_syst)  << endl;
  ofile <<  Form("%s     lnN      -    %.3f    -      -      - ",name_ttsl_syst.Data(),ttsl_syst)  << endl;
  ofile <<  Form("%s     gmN %.0f -    %.3f    -      -      - ",name_ttsl_CRstat.Data(),ttsl_CRstat,ttsl_TF )  << endl;
  ofile <<  Form("%s     lnN      -     -    %.3f     -      - ",name_ttdl_syst.Data(),ttdl_syst)  << endl;
  ofile <<  Form("%s     gmN %.0f -     -    %.3f     -      - ",name_ttdl_CRstat.Data(),ttdl_CRstat,ttdl_TF)  << endl;
  ofile <<  Form("%s     lnN      -     -      -     %.3f    - ",name_wjets_syst.Data(),wjets_syst)  << endl;
  ofile <<  Form("%s     gmN %.0f -     -      -     %.3f    - ",name_wjets_CRstat.Data(),wjets_CRstat,wjets_TF)  << endl;
  ofile <<  Form("%s     lnN      -     -      -      -    %.3f",name_zjets_syst.Data(),zjets_syst)  << endl;

  if (n_mt2bins == 2) {
    ofile <<  Form("%s     lnN      -    %.3f    -      -      - ",name_ttsl_shape.Data(),ttsl_shape_bin2)  << endl;
    ofile <<  Form("%s     lnN      -     -    %.3f     -      - ",name_ttdl_shape.Data(),ttdl_shape_bin2)  << endl;
    ofile <<  Form("%s     lnN      -     -      -     %.3f    - ",name_wjets_shape.Data(),wjets_shape_bin2)  << endl;
  }
  if (n_mt2bins == 3) {
    ofile <<  Form("%s     lnN      -    %.3f    -      -      - ",name_ttsl_shape.Data(),ttsl_shape_bin3)  << endl;
    ofile <<  Form("%s     lnN      -     -    %.3f     -      - ",name_ttdl_shape.Data(),ttdl_shape_bin3)  << endl;
    ofile <<  Form("%s     lnN      -     -      -     %.3f    - ",name_wjets_shape.Data(),wjets_shape_bin3)  << endl;
  }
  
  ofile.close();

  if (verbose) std::cout << "Wrote card: " << cardname << std::endl;

  return 1;
}

//_______________________________________________________________________________
void cardMakerMC(string signal, string input_dir, string output_dir, bool isScan = false, bool doData = false){

  // Benchmark
  TBenchmark *bmark = new TBenchmark();
  bmark->Start("benchmark");

  // ----------------------------------------
  //  samples definition
  // ----------------------------------------

  // set input files
  f_ttsl = new TFile(Form("%s/ttsl.root",input_dir.c_str()));
  f_ttdl = new TFile(Form("%s/ttdl.root",input_dir.c_str()));
  f_wjets = new TFile(Form("%s/wjets_ht.root",input_dir.c_str()));
  f_zjets = new TFile(Form("%s/dyjetsll.root",input_dir.c_str())); // important for Ztautau at low MT
  //  f_fake = new TFile(Form("%s/allBkg.root",input_dir.c_str())); // Negligible

  f_sig = new TFile(Form("%s/%s.root",input_dir.c_str(),signal.c_str()));

  if (doData) f_data = new TFile(Form("%s/data_Run2015CD.root",input_dir.c_str()));

  if( f_ttsl->IsZombie() || f_ttdl->IsZombie()  || f_wjets->IsZombie()  || f_zjets->IsZombie()  || f_sig->IsZombie() || (doData && f_data->IsZombie()) ) {
    std::cout << "Input file does not exist" << std::endl;
    return;
  }

  // ----------------------------------------
  //  cards definitions
  // ----------------------------------------

  set<pair<int, int> > signal_points;  
  //Loop through list of every directory in the signal file.
  //if directory begins with "sr", excluding "srbase", make cards for it.
  TIter it(f_sig->GetListOfKeys());
  TKey* k;
  string keep = "srLep";
  string skip = "srLepbase";
  while ((k = (TKey *)it())) {
    if (strncmp (k->GetTitle(), skip.c_str(), skip.length()) == 0) continue;
    if (strncmp (k->GetTitle(), keep.c_str(), keep.length()) == 0) {//it is a signal region
      string mt2_hist_name = (k->GetTitle());
      mt2_hist_name += "/h_n_mtbins";
      TH1D* h_n_mt2bins = (TH1D*) f_sig->Get(TString(mt2_hist_name));
      int n_mt2bins = h_n_mt2bins->GetBinContent(1);
      for (int imt2 = 1; imt2 <= n_mt2bins; ++imt2) {//Make a separate card for each MT2 bin.
	if (isScan) {
	  int y_binwidth = 25;
	  int y_max = 1600;
	  if (signal.find("T2cc") != std::string::npos || signal.find("T2-4bd") != std::string::npos) {
	    y_binwidth = 5;
	    y_max = 800;
	  }
	  for (int im1 = 0; im1 <= 2000; im1 += 25) {
	    for (int im2 = 0; im2 <= y_max; im2 += y_binwidth) {
	      int result = printCard(k->GetTitle(), imt2, signal, output_dir, im1, im2);   //MT2 and scan bins with no entries are handled by printCard function.
	      if (result > 0) signal_points.insert( make_pair(im1,im2) ); 
	    } // scanM2 loop
	  } // scanM1 loop
	} // if isScan
        else {
	  printCard(k->GetTitle(), imt2, signal, output_dir);   //MT2 bins with no entries are handled by printCard function.
	}
      }
    }
  }

  if (isScan) {
    TString filename = Form("%s/points_%s.txt",output_dir.c_str(),signal.c_str());
    ofstream ofile;
    ofile.open(filename);
    cout << "--------------------------------------------" << endl
	 << "- saw nonzero signal entries for the following points: " << endl;
    for (set<pair<int,int> >::iterator it=signal_points.begin(); it!=signal_points.end(); ++it) {
      cout << signal << "_" << (*it).first << "_" << (*it).second << endl;
      ofile << signal << "_" << (*it).first << "_" << (*it).second << endl;
    }
    ofile.close();
  }
  
  bmark->Stop("benchmark");
  cout << endl;
  cout << "------------------------------" << endl;
  cout << "CPU  Time:	" << Form( "%.01f s", bmark->GetCpuTime("benchmark")  ) << endl;
  cout << "Real Time:	" << Form( "%.01f s", bmark->GetRealTime("benchmark") ) << endl;
  cout << endl;
  delete bmark;

}
