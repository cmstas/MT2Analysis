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

TFile* f_1lep = 0;
TFile* f_2lep = 0;
TFile* f_fake = 0;
TFile* f_sig = 0;
TFile* f_data = 0;

const bool verbose = false;

const bool suppressZeroBins = false;

const bool suppressZeroTRs = false;

const float dummy_alpha = 1.; // dummy value for gmN when there are no SR events

const bool uncorrelatedZGratio = false; // treat ZGratio uncertainty as fully uncorrelated

const bool fourNuisancesPerBinZGratio = true;

const bool integratedZinvEstimate = true;

const bool doDummySignalSyst = true;

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
  TString fullhistnamePred = dir + "/h_mtbins";
  TString fullhistnameFakes = dir + "/h_mtbinsFake";
  TString fullhistnameScan  = fullhistname+"_sigscan";
  TString fullhistnameScanBtagsfHeavy  = fullhistname+"_sigscan_btagsf_heavy_UP";
  TString fullhistnameScanBtagsfLight  = fullhistname+"_sigscan_btagsf_light_UP";
  TString fullhistnameScanLepeff  = fullhistname+"_sigscan_lepeff_UP";
  TString fullhistnameScanIsr  = fullhistname+"_sigscan_isr_UP";
  //  TString fullhistnameStat  = fullhistname+"Stat";
  TString fullhistnameMCStat  = dir + "/h_crMCInt";
  TString fullhistnameCRyield  = dir + "/h_crDataInt";
  TString fullhistnameRatio  = dir + "/h_ratio";
  TString fullhistnameRatioInt  = dir + "/h_ratioInt";
  TString fullhistnamePurity = dir + "/h_mt2binspurityFailSieieData";
  TString fullhistnamePurityInt = dir + "/h_mt2binspurityIntFailSieieData";

  TString signame(signal);
  if (scanM1 >= 0 && scanM2 >= 0) {
    signame = Form("%s_%d_%d",signal.c_str(),scanM1,scanM2);
  }
  bool isSignalWithLeptons = bool(signame.Contains("T1tttt") || signame.Contains("T2tt"));

  double n_onelep(0.);
  double n_onelep_cr(0.);
  double onelep_polW_syst(0.);
  double onelep_TopW_syst(0.);
  double onelep_btag_syst(0.);
  double onelep_jec_syst(0.);
  double onelep_renorm_syst(0.);
  double err_onelep_mcstat(0.);
  double n_dilep(0.);
  double n_dilep_cr(0.);
  double dilep_dyUPDN_syst(0.);
  double dilep_btag_syst(0.);
  double dilep_jec_syst(0.);
  double dilep_renorm_syst(0.);
  double err_dilep_mcstat(0.);
  double n_fakes(0.);
  double n_bkg(0.);
  double n_data(0.);

  double n_sig(0.);
  double n_sig_TR(0.);
  double err_sig_mcstat(0.);
  double n_sig_btagsf_heavy_UP(0.);
  double n_sig_btagsf_light_UP(0.);
  double n_sig_lepeff_UP(0.);
  double n_sig_isr_UP(0.);

  TH1D* h_sig(0);
  TH1D* h_sig_btagsf_heavy_UP(0);
  TH1D* h_sig_btagsf_light_UP(0);
  TH1D* h_sig_lepeff_UP(0);
  TH1D* h_sig_isr_UP(0);
  // pick point out of signal scan
  if (scanM1 >= 0 && scanM2 >= 0) {
    TH3D* h_sigscan = (TH3D*) f_sig->Get(fullhistnameScan);
    if (!h_sigscan) return 0;
    int bin1 = h_sigscan->GetYaxis()->FindBin(scanM1);
    int bin2 = h_sigscan->GetZaxis()->FindBin(scanM2);
    h_sig = h_sigscan->ProjectionX(Form("h_mt2bins_%d_%d_%s",scanM1,scanM2,dir_str.c_str()),bin1,bin1,bin2,bin2);
    TH3D* h_sigscan_btagsf_heavy_UP = (TH3D*) f_sig->Get(fullhistnameScanBtagsfHeavy);
    if (h_sigscan_btagsf_heavy_UP) h_sig_btagsf_heavy_UP = h_sigscan_btagsf_heavy_UP->ProjectionX(Form("h_mt2bins_btagsf_heavy_UP_%d_%d_%s",scanM1,scanM2,dir_str.c_str()),bin1,bin1,bin2,bin2);
    TH3D* h_sigscan_btagsf_light_UP = (TH3D*) f_sig->Get(fullhistnameScanBtagsfLight);
    if (h_sigscan_btagsf_light_UP) h_sig_btagsf_light_UP = h_sigscan_btagsf_light_UP->ProjectionX(Form("h_mt2bins_btagsf_light_UP_%d_%d_%s",scanM1,scanM2,dir_str.c_str()),bin1,bin1,bin2,bin2);
    TH3D* h_sigscan_lepeff_UP = (TH3D*) f_sig->Get(fullhistnameScanLepeff);
    if (h_sigscan_lepeff_UP) h_sig_lepeff_UP = h_sigscan_lepeff_UP->ProjectionX(Form("h_mt2bins_lepeff_UP_%d_%d_%s",scanM1,scanM2,dir_str.c_str()),bin1,bin1,bin2,bin2);
    TH3D* h_sigscan_isr_UP = (TH3D*) f_sig->Get(fullhistnameScanIsr);
    if (h_sigscan_isr_UP) h_sig_isr_UP = h_sigscan_isr_UP->ProjectionX(Form("h_mt2bins_isr_UP_%d_%d_%s",scanM1,scanM2,dir_str.c_str()),bin1,bin1,bin2,bin2);
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
  n_sig_TR = h_sig->Integral(0,-1);
  err_sig_mcstat = h_sig->GetBinError(mt2bin);
  if (h_sig_btagsf_heavy_UP) n_sig_btagsf_heavy_UP = h_sig_btagsf_heavy_UP->GetBinContent(mt2bin);
  if (h_sig_btagsf_light_UP) n_sig_btagsf_light_UP = h_sig_btagsf_light_UP->GetBinContent(mt2bin);
  if (h_sig_lepeff_UP) n_sig_lepeff_UP = h_sig_lepeff_UP->GetBinContent(mt2bin);
  if (h_sig_isr_UP) n_sig_isr_UP = h_sig_isr_UP->GetBinContent(mt2bin);
  
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
 
  // get all the numbers we need from the histograms we have
  // !!!!! HACK: set zero bins to 0.01 for now to make combine happy
  // PREDICTION
  int n_mt2bins = 1;
  TH1D* h_1lpred = (TH1D*) f_1lep->Get(fullhistnamePred);
  if (h_1lpred != 0) {
    n_onelep = h_1lpred->GetBinContent(mt2bin);
    n_mt2bins = h_1lpred->GetNbinsX();
  }
  //W polarization ratio uncertainty
  TH1D* h_1lratio_polW = (TH1D*) f_1lep->Get(fullhistnameRatioInt+"Syst");
  if (h_1lratio_polW != 0) {
    onelep_polW_syst = h_1lratio_polW->GetBinError(1)/h_1lratio_polW->GetBinContent(1);
  }
  //Top/W composition uncertainty
  TH1D* h_1lratio_TopW = (TH1D*) f_1lep->Get(fullhistnameRatioInt+"TopW");
  if (h_1lratio_TopW != 0) {
    onelep_TopW_syst = h_1lratio_TopW->GetBinError(1)/h_1lratio_TopW->GetBinContent(1);
  }
  //btag uncertainty, only in soft-b regions
  if (nbjets_LOW != 0 && nbjetshard_HI == 1) {
    onelep_btag_syst = 0.05;
  }
  else onelep_btag_syst = 0;
  //JEC uncertainty, 10% across all regions
  onelep_jec_syst = 0.10;
  //Renorm uncertainty
  TH1D* h_1lratio_Renorm = (TH1D*) f_1lep->Get(fullhistnameRatioInt+"Renorm");
  if (h_1lratio_Renorm != 0) {
    onelep_renorm_syst = h_1lratio_Renorm->GetBinError(1)/h_1lratio_Renorm->GetBinContent(1);
  }
  
  // MC STAT UNC
  TH1D* h_onelep_mcstat = (TH1D*) f_1lep->Get(fullhistnameRatioInt);
  if (h_onelep_mcstat != 0) {
    err_onelep_mcstat = h_onelep_mcstat->GetBinError(1)/h_onelep_mcstat->GetBinContent(1);
  }
  
  // CR YIELD
  TH1D* h_1l_cryield = (TH1D*) f_1lep->Get(fullhistnameCRyield);
  n_onelep_cr = 0;
  if (h_1l_cryield != 0)
    n_onelep_cr = round(h_1l_cryield->Integral(0,-1));
  // K_MT Extrap. Uncertainty
  // K_HT Extrap. Uncert
  // Purity uncertainty
  // Composition uncertainty
  
  // DILEPTONS
  TH1D* h_2lpred = (TH1D*) f_2lep->Get(fullhistnamePred);
  if (h_2lpred != 0) {
    n_dilep = h_2lpred->GetBinContent(mt2bin);
  }
  //DY UP/DN  uncertainty
  // TH1D* h_2lratio_dyUPDN = (TH1D*) f_1lep->Get(fullhistnameRatioInt+"dyUPDN");
  // if (h_2lratio_dyUPDN != 0) {
  //   dilep_dyUPDN_syst = h_2lratio_dyUPDN->GetBinError(1)/h_2lratio_dyUPDN->GetBinContent(1);
  // }
  //5% in multijet 0b and 10% in monojet 0b/1b regions
  if (nbjets_HI == 1 && njets_LOW != 1) {
    dilep_dyUPDN_syst = 0.05;
  }
  else if (njets_LOW == 1 && (nbjets_LOW == 1 || nbjets_LOW == 0)) {
    dilep_dyUPDN_syst = 0.10;
  }
  else dilep_dyUPDN_syst = 0;
  //JEC uncertainty, 20% in monojet & tails, 5% elsewhere
  if (njets_LOW == 1 || nbjets_LOW == 3 || met_LOW == 500 || ht_LOW == 1000) {
    dilep_jec_syst = 0.20;
  }
  else dilep_jec_syst = 0.05;
  //Renorm  uncertainty
  TH1D* h_2lratio_Renorm = (TH1D*) f_2lep->Get(fullhistnameRatioInt+"Renorm");
  if (h_2lratio_Renorm != 0) {
    dilep_renorm_syst = h_2lratio_Renorm->GetBinError(1)/h_2lratio_Renorm->GetBinContent(1);
  }
  TH1D* h_2l_cryield = (TH1D*) f_2lep->Get(fullhistnameCRyield);
  n_dilep_cr = 0;
  if (h_2l_cryield != 0)
    n_dilep_cr = round(h_2l_cryield->Integral(0,-1));
  // MC STAT UNC
  TH1D* h_dilep_mcstat = (TH1D*) f_2lep->Get(fullhistnameRatioInt);
  if (h_dilep_mcstat != 0) {
    err_dilep_mcstat = h_dilep_mcstat->GetBinError(1)/h_dilep_mcstat->GetBinContent(1);
  }
  
  // FAKES
  TH1D* h_fakepred = (TH1D*) f_fake->Get(fullhistnameFakes);
  if (h_fakepred != 0) {
    n_fakes = h_fakepred->GetBinContent(mt2bin);
  }
  
//  TH1D* h_2l_cryield = (TH1D*) f_2lep->Get(fullhistnameCRyield);
//  n_dilep_cr = 0;
//  if (h_2l_cryield != 0)
//    n_dilep_cr = round(h_2l_cryield->Integral(0,-1));
  
  
  n_bkg = n_onelep + n_dilep + n_fakes;

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
  double onelep_shape = 1.0;
  double onelep_alpha  = 1; // transfer factor
  double onelep_mcstat = 1. + err_onelep_mcstat; // transfer factor stat uncertainty
  double onelep_alphaerr = 1. + 0.05; // transfer factor syst uncertainty
  double onelep_polW = 1. + onelep_polW_syst; // transfer factor syst uncertainty due to W polarization
  double onelep_TopW = 1. + onelep_TopW_syst; // transfer factor syst uncertainty due to Top/W composition
  double onelep_btag = 1. + onelep_btag_syst; // transfer factor syst uncertainty due to btag SF
  double onelep_jec = 1. + onelep_jec_syst; // transfer factor syst uncertainty due to JEC variations
  double onelep_renorm = 1. + onelep_renorm_syst; // transfer factor syst uncertainty due to renormalization/factorization scale
  double onelep_lepeff = 1.10;
  double onelep_bTag = 1.2; // special for 7jets with b-tags
 
  // want this to be correlated either (1) among all bins or (2) for all bins sharing the same CR bin
  TString name_onelep_shape = Form("onelep_shape_%s_%s", bjet_str.c_str(), bjethard_str.c_str());
  TString name_onelep_crstat = Form("onelep_CRstat_%s_%s_%s_%s_%s", ht_str.c_str(), met_str.c_str(), jet_str.c_str(), bjet_str.c_str(), bjethard_str.c_str());
  TString name_onelep_mcstat = Form("onelep_MCstat_%s", channel.c_str());
  //TString name_onelep_alphaerr = Form("onelep_alpha_%s_%s_%s_%s", ht_str.c_str(), jet_str.c_str(), bjet_str.c_str(), bjethard_str.c_str());
  TString name_onelep_polW = "onelep_polW";
  TString name_onelep_TopW =  Form("onelep_TopW_%s_%s_%s_%s_%s", ht_str.c_str(), met_str.c_str(), jet_str.c_str(), bjet_str.c_str(), bjethard_str.c_str());
  TString name_onelep_btag = "onelep_btag"; 
  TString name_onelep_dilep_jec = "onelep_dilep_jec"; //correlated across onelep AND dilep
  TString name_onelep_dilep_renorm = "onelep_dilep_renorm"; //correlated across onelep AND dilep
  TString name_onelep_lepeff = "onelep_lepeff";
  //TString name_onelep_bTag = Form("onelep_bTag_%s_%s_%s_%s", ht_str.c_str(), jet_str.c_str(), bjet_str.c_str(), bjethard_str.c_str());

  if (n_onelep_cr > 0.) {
    onelep_alpha = n_onelep / n_onelep_cr > 0 ? n_onelep / n_onelep_cr : last_onelep_transfer;
    if (onelep_alpha > 2.) onelep_alpha = 2.; // hard bound to avoid statistical fluctuations
    if (onelep_alpha > 0.) last_onelep_transfer = onelep_alpha;
  }
  else {
    onelep_alpha = last_onelep_transfer;
  }
  n_syst += 8; // onelep_crstat, onelep_mcstat, onelep_polW, onelep_TopW, onelep_btag, onelep_dilep_jec, onelep_dilep_renorm, onelep_lepeff

  if (n_mt2bins > 1) {
    if (mt2bin == 1 && n_onelep > 0.) {
      // first bin needs to compensate normalization from the rest
      float increment = 0.;
      for (int ibin=1; ibin<h_1lpred->GetNbinsX(); ibin++)
	increment += 0.2 / (n_mt2bins - 1) * (ibin - 1) * h_1lpred->GetBinContent(ibin);
      onelep_shape = 1. - increment/n_onelep;
      if (onelep_shape < 0) onelep_shape = 0.1; // protection against huge oscillations
    }
    else
      onelep_shape = 1. + 0.2 / (n_mt2bins - 1) * (mt2bin - 1);
    n_syst++;  // onelep_shape
  }
  n_onelep = n_onelep_cr * onelep_alpha; // don't use onelep prediction as central value any more, since it has to be consistent with CR*alpha
  
  // ----- lost lepton bkg uncertainties
  double dilep_shape = 1.0;
  double dilep_alpha  = 1; // transfer factor
  double dilep_mcstat = 1. + err_dilep_mcstat; // transfer factor stat uncertainty
  double dilep_alphaerr = 1. + 0.05; // transfer factor syst uncertainty
  double dilep_dyUPDN = 1 + dilep_dyUPDN_syst; // transfer factor sys uncertainty due to DY UP/DN variation
  double dilep_jec = 1 + dilep_jec_syst; // transfer factor sys uncertainty due to JEC variations
  double dilep_renorm = 1 + dilep_renorm_syst; // transfer factor syst uncertainty due to renormalization/factorization scale
  double dilep_lepeff = 1.15;
  double dilep_bTag = 1.2; // special for 7jets with b-tags
  
  // want this to be correlated either (1) among all bins or (2) for all bins sharing the same CR bin
  TString name_dilep_shape = Form("dilep_shape_%s_%s", bjet_str.c_str(), bjethard_str.c_str());
  TString name_dilep_crstat = Form("dilep_CRstat_%s_%s_%s_%s", ht_str.c_str(), jet_str.c_str(), bjet_str.c_str(), bjethard_str.c_str());
  TString name_dilep_mcstat = Form("dilep_MCstat_%s", channel.c_str());
  //TString name_dilep_alphaerr = Form("dilep_alpha_%s_%s_%s_%s", ht_str.c_str(), jet_str.c_str(), bjet_str.c_str(), bjethard_str.c_str());
  TString name_dilep_dyUPDN = "dilep_dyUPDN";
  TString name_dilep_lepeff = "dilep_lepeff";
  TString name_dilep_bTag = Form("dilep_bTag_%s_%s_%s_%s", ht_str.c_str(), jet_str.c_str(), bjet_str.c_str(), bjethard_str.c_str());
  
  if (n_dilep_cr > 0.) {
    dilep_alpha = n_dilep / n_dilep_cr > 0 ? n_dilep / n_dilep_cr : last_dilep_transfer;
    if (dilep_alpha > 2.) dilep_alpha = 2.; // hard bound to avoid statistical fluctuations
    if (dilep_alpha > 0.) last_dilep_transfer = dilep_alpha;
  }
  else {
    dilep_alpha = last_dilep_transfer;
  }
  n_syst += 4; // dilep_crstat, dilep_mcstat, dilep_dyUPDN, dilep_lepeff
  
  if (n_mt2bins > 1) {
    if (mt2bin == 1 && n_dilep > 0.) {
      // first bin needs to compensate normalization from the rest
      float increment = 0.;
      for (int ibin=1; ibin<h_2lpred->GetNbinsX(); ibin++)
        increment += 0.2 / (n_mt2bins - 1) * (ibin - 1) * h_2lpred->GetBinContent(ibin);
      dilep_shape = 1. - increment/n_dilep;
      if (dilep_shape < 0) dilep_shape = 0.1; // protection against huge oscillations
    }
    else
      dilep_shape = 1. + 0.2 / (n_mt2bins - 1) * (mt2bin - 1);
    n_syst++;  // dilep_shape
  }
  n_dilep = n_dilep_cr * dilep_alpha; // don't use dilep prediction as central value any more, since it has to be consistent with CR*alpha


  
  
  TString name_fake_syst = Form("fake_syst_%s_%s_%s", ht_str.c_str(), jet_str.c_str(), bjet_str.c_str());
  double fake_syst = 1.99;
  n_syst++; //fake_syst

  // ----- sig uncertainties
  double sig_syst         = 1.20; // dummy 10% from early MC studies
  double sig_lumi         = 1.046; // 4.6% lumi uncertainty, end of 2015
  double sig_mcstat       = (n_sig > 0.) ? 1. + sqrt(pow(err_sig_mcstat/n_sig,2) + 0.005) : 1.071; // MC stat err +  quadrature sum of 5% for JES, 5% for renorm/fact scales
  double sig_btagsf_heavy = (n_sig > 0.) ? n_sig_btagsf_heavy_UP/n_sig : 1.00; // btagsf heavy, eff UP
  double sig_btagsf_light = (n_sig > 0.) ? n_sig_btagsf_light_UP/n_sig : 1.00; // btagsf light, eff UP
  double sig_lepeff       = (n_sig > 0.) ? n_sig_lepeff_UP/n_sig : 1.00; // lepton eff UP
  double sig_isr          = (n_sig > 0.) ? n_sig_isr_UP/n_sig : 1.00; // isr weight UP

  // fully correlated for lumi, btagsf, lepeff, isr.  Fully uncorrelated for stats and other systs
  TString name_sig_syst         = "sig_syst";
  TString name_sig_lumi         = "lumi_syst";
  TString name_sig_mcstat       = "sig_MCstat_"+perChannel;
  TString name_sig_isr          = "sig_isrSyst";
  TString name_sig_btagsf_heavy = "sig_bTagHeavySyst";
  TString name_sig_btagsf_light = "sig_bTagLightSyst";
  TString name_sig_lepeff       = "sig_lepEffSyst";
  
  if (doDummySignalSyst) {
    // dummy: just 1 nuisance, correlated
    ++n_syst;
  }
  // otherwise do "real" signal systematics
  else {
    n_syst += 5; // mcstat (including gen scales and JEC), lumi, btagsf_heavy, btagsf_light, isr
    if (isSignalWithLeptons) ++n_syst; // lepeff
  }

  ofstream ofile;
  ofile.open(cardname);

  ofile <<  "imax 1  number of channels"                                                    << endl;
  ofile <<  "jmax 3  number of backgrounds"                                                 << endl;
  ofile <<  "kmax *"                                                                        << endl;
  ofile <<  "------------"                                                                  << endl;
  ofile <<  Form("bin         %s",channel.c_str())                                          << endl;
  ofile <<  Form("observation %.0f",n_data)                                                 << endl;
  ofile <<  "------------"                                                                  << endl;
  ofile <<  Form("bin             %s   %s   %s   %s",channel.c_str(),channel.c_str(),channel.c_str(),channel.c_str()) << endl;
  ofile <<  "process          sig     onelep     dilepton    fakes"                                      << endl;
  ofile <<  "process           0         1           2         3"                                      << endl;
  ofile <<  Form("rate            %.3f    %.3f      %.3f      %.3f",n_sig,n_onelep,n_dilep,n_fakes) << endl;
  ofile <<  "------------"                                                                  << endl;
 
  // ---- sig systs
  if (doDummySignalSyst) {
    ofile <<  Form("%s                                    lnN   %.3f    -      -     - ",name_sig_syst.Data(),sig_syst)  << endl;
  }
  // full signal systematics
  else {
    ofile <<  Form("%s                    lnN    %.3f   -    -    - ",name_sig_lumi.Data(),sig_lumi)  << endl;
    ofile <<  Form("%s     lnN    %.3f   -    -    - ",name_sig_mcstat.Data(),sig_mcstat)  << endl;
    ofile <<  Form("%s                  lnN    %.3f   -    -    - ",name_sig_isr.Data(),sig_isr)  << endl;
    ofile <<  Form("%s            lnN    %.3f   -    -    - ",name_sig_btagsf_heavy.Data(),sig_btagsf_heavy)  << endl;
    ofile <<  Form("%s            lnN    %.3f   -    -    - ",name_sig_btagsf_light.Data(),sig_btagsf_light)  << endl;
    if (isSignalWithLeptons) ofile <<  Form("%s               lnN    %.3f   -    -    - ",name_sig_lepeff.Data(),sig_lepeff)  << endl;
  }


  // ---- onelep systs
  ofile <<  Form("%s   \t\t\t    lnN    -  %.3f  -     - ",name_onelep_lepeff.Data(),onelep_lepeff)  << endl;
  ofile <<  Form("%s    gmN %.0f    -  %.5f -     - ",name_onelep_crstat.Data(),n_onelep_cr,onelep_alpha)  << endl;
  ofile <<  Form("%s        lnN    -    %.3f    -    - ",name_onelep_mcstat.Data(),onelep_mcstat)  << endl;
  if (n_mt2bins > 1)
    ofile <<  Form("%s   \t\t     lnN    -   %.3f    -     - ",name_onelep_shape.Data(),onelep_shape)  << endl;
    ofile <<  Form("%s   \t\t\t\t     lnN    -    %.3f    -    - ",name_onelep_polW.Data(),onelep_polW)  << endl;
    ofile <<  Form("%s   \t     lnN    -    %.3f    -    - ",name_onelep_TopW.Data(),onelep_TopW)  << endl;
    ofile <<  Form("%s   \t\t\t\t     lnN    -    %.3f    -    - ",name_onelep_btag.Data(),onelep_btag)  << endl;
    ofile <<  Form("%s   \t\t\t     lnN    -    %.3f    %.3f    - ",name_onelep_dilep_jec.Data(),onelep_jec,dilep_jec)  << endl;
    ofile <<  Form("%s   \t\t\t     lnN    -    %.3f    %.3f    - ",name_onelep_dilep_renorm.Data(),onelep_renorm,dilep_renorm)  << endl;
  //ofile <<  Form("%s        lnN    -    %.3f    -    - ",name_onelep_alphaerr.Data(),onelep_alphaerr)  << endl;

  // ---- dilep systs
  ofile <<  Form("%s   \t\t\t\t    lnN    -    -    %.3f    - ",name_dilep_lepeff.Data(),dilep_lepeff)  << endl;
  ofile <<  Form("%s     gmN %.0f    -    -    %.5f     - ",name_dilep_crstat.Data(),n_dilep_cr,dilep_alpha)  << endl;
  ofile <<  Form("%s        lnN    -    -    %.3f    - ",name_dilep_mcstat.Data(),dilep_mcstat)  << endl;
  if (n_mt2bins > 1)
    ofile <<  Form("%s    \t\t     lnN    -    -   %.3f     - ",name_dilep_shape.Data(),dilep_shape)  << endl;
    ofile <<  Form("%s   \t\t\t\t     lnN    -    -   %.3f     - ",name_dilep_dyUPDN.Data(),dilep_dyUPDN)  << endl;
  //  ofile <<  Form("%s        lnN    -    -    %.3f    - ",name_dilep_alphaerr.Data(),dilep_alphaerr)  << endl;


  
  ofile <<  Form("%s           lnN    -    -    -    %.3f ",name_fake_syst.Data(),fake_syst)  << endl;

  
  ofile.close();

  if (verbose) std::cout << "Wrote card: " << cardname << std::endl;

  return 1;
}

//_______________________________________________________________________________
void cardMaker(string signal, string input_dir, string output_dir, bool isScan = false, bool doData = false){

  // Benchmark
  TBenchmark *bmark = new TBenchmark();
  bmark->Start("benchmark");

  // ----------------------------------------
  //  samples definition
  // ----------------------------------------

  // set input files
  f_1lep = new TFile(Form("%s/pred_CR1L.root",input_dir.c_str()));
  f_2lep = new TFile(Form("%s/pred_CR2L.root",input_dir.c_str()));
  f_fake = new TFile(Form("%s/allBkg.root",input_dir.c_str()));

  f_sig = new TFile(Form("%s/%s.root",input_dir.c_str(),signal.c_str()));

  if (doData) f_data = new TFile(Form("%s/data_Run2015CD.root",input_dir.c_str()));

  if( f_1lep->IsZombie() || f_2lep->IsZombie()  || f_sig->IsZombie() || (doData && f_data->IsZombie()) ) {
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
