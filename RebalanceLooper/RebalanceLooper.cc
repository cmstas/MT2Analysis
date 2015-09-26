// C++
#include <iostream>
#include <vector>
#include <set>
#include <cmath>

// ROOT
#include "TDirectory.h"
#include "TTreeCache.h"
#include "Math/VectorUtil.h"
#include "TVector2.h"
#include "TBenchmark.h"
#include "TMath.h"
#include "TMinuit.h"
#include "TList.h"
#include "TCollection.h"
#include "TKey.h"
#include "TF1.h"
#include "TStyle.h"
#include "Math/GSLIntegrator.h"
#include "Math/WrappedTF1.h"

#include "../CORE/Tools/utils.h"

// header
#include "RebalanceLooper.h"

//MT2
#include "../MT2CORE/Plotting/PlotUtilities.h"

using namespace std;

class mt2tree;

std::vector<float> jetpt;
std::vector<float> jeteta;
std::vector<float> jetphi;
std::map<std::string, std::vector<double> > responseMap;
std::map<std::string, std::pair<TF1, float> > fitMap;
float sigma_soft;
float pt_soft_x;
float pt_soft_y;

void setup_fits(){
  TFile* fitFile = new TFile("/home/users/jgran/temp/RS_update/MT2Analysis/RebalanceLooper/template_fits.root", "READ");
  TIter it(fitFile->GetListOfKeys());
  TKey* key;
  while ( (key = (TKey *)it()) ) {
    std::string fit_name = (key->GetTitle());
    TF1* f = (TF1*) fitFile->Get(TString(fit_name));
    float f_scale = 0.01/f->Integral(0, 10, 1.e-5);//hist bin size = 0.01;
    float max_scale = 1E10;
    f_scale = min(f_scale, max_scale);
    std::cout << fit_name << std::endl;
    std::cout << "f_scale = " << f_scale << std::endl;
    std::pair<TF1, float> my_pair;
    my_pair.second = f_scale;
    if(!f){
      std::cout << "Could not find fit " << fit_name << "!" << std::endl;
      continue;
    }
    my_pair.first = *f;
    //fitMap[fit_name] = *f;
    fitMap[fit_name] = my_pair;
  }
  fitFile->Close();
  delete fitFile;
}

float GetValue(float xvalue, TH1F &hist){
  float xmax = hist.GetXaxis()->GetXmax()-0.01;
  int xbin   = hist.GetXaxis()->FindBin(min(xvalue, xmax));
  return hist.GetBinContent(xbin);
}

double ComputeGaussianValue(double scale, double mean, double sigma, float x){
  return scale*TMath::Exp(-0.5*((x - mean)/sigma)*((x - mean)/sigma));
}

float GetProb(float pt, float eta, double par){
  std::string pt_string;
  if(pt < 20.0) pt_string = "Pt0";
  else if(pt < 30.0)   pt_string = "Pt1";
  else if(pt < 50.0)   pt_string = "Pt2";
  else if(pt < 80.0)   pt_string = "Pt3";
  else if(pt < 120.0)  pt_string = "Pt4";
  else if(pt < 170.0)  pt_string = "Pt5";
  else if(pt < 230.0)  pt_string = "Pt6";
  else if(pt < 300.0)  pt_string = "Pt7";
  else if(pt < 380.0)  pt_string = "Pt8";
  else if(pt < 470.0)  pt_string = "Pt9";
  else if(pt < 570.0)  pt_string = "Pt10";
  else if(pt < 680.0)  pt_string = "Pt11";
  else if(pt < 800.0)  pt_string = "Pt12";
  else pt_string = "Pt13";
/*
  else if(pt < 1000.0) pt_string = "Pt13";
  else if(pt < 1300.0) pt_string = "Pt14";
  else if(pt < 1700.0) pt_string = "Pt15";
  else if(pt < 2200.0) pt_string = "Pt16";
  else if(pt < 2800.0) pt_string = "Pt17";
  else if(pt < 3500.0) pt_string = "Pt18";
  else if(pt < 4300.0) pt_string = "Pt19";
  else if(pt < 5200.0) pt_string = "Pt20";
  else pt_string = "Pt21";
*/
  //else if(pt < 6500.0) pt_string = "Pt21";

  std::string eta_string;
  if(eta < 0.3) eta_string = "Eta0";
  else if(eta < 0.5)   eta_string = "Eta1";
  else if(eta < 0.8)   eta_string = "Eta2";
  else if(eta < 1.1)   eta_string = "Eta3";
  else if(eta < 1.4)  eta_string = "Eta4";
  else if(eta < 1.7)  eta_string = "Eta5";
  else if(eta < 2.3)  eta_string = "Eta6";
  else if(eta < 2.8)  eta_string = "Eta7";
  else eta_string = "Eta8";
/*
  else if(eta < 3.2)  eta_string = "Eta8";
  else if(eta < 4.1)  eta_string = "Eta9";
  else if(eta < 5.0)  eta_string = "Eta10";
  else eta_string = "Eta11";
*/

  std::string fit_name = "fit_h_tot_JetAll_ResponsePt_" + pt_string + "_" + eta_string;
  //std::cout << "Looking up prob of " << par << " using fit " << fit_name << std::endl;
  double g_scale = fitMap.at(fit_name).first.GetParameter(0);
  double g_mean  = fitMap.at(fit_name).first.GetParameter(1);
  double g_sigma = fitMap.at(fit_name).first.GetParameter(2);
  
  double my_prob = ComputeGaussianValue(g_scale, g_mean, g_sigma, par);
  double my_scale = fitMap.at(fit_name).second;
  return my_prob*my_scale;
}

void minuitFunction(int& nDim, double* gout, double& result, double par[], int flg) {
  float likelihood = 0;
  float pt_constrained_x = 0.0;
  float pt_constrained_y = 0.0;
  float min_prob = 1E-20;;
  for(unsigned int i=0; i<jetpt.size(); i++){
    float prob = GetProb((jetpt.at(i))/par[i], fabs(jeteta.at(i)), par[i]);
    prob = max(prob, min_prob);
    likelihood += log(prob);
    pt_constrained_x -= (jetpt.at(i))*cos(jetphi.at(i))/par[i];
    pt_constrained_y -= (jetpt.at(i))*sin(jetphi.at(i))/par[i];
  }
  float x1 = (pt_soft_x - pt_constrained_x)/sigma_soft;
  float x2 = (pt_soft_y - pt_constrained_y)/sigma_soft;
  likelihood += -x1*x1/2;
  likelihood += -x2*x2/2;

  result = -likelihood;
}

RebalanceLooper::RebalanceLooper(){};
RebalanceLooper::~RebalanceLooper(){};

void RebalanceLooper::loop(TChain* chain, std::string output_name){
  
  setup_fits();

/*
  TFile* sf_file = new TFile("/home/users/jgran/temp/V00-00-12/MT2Analysis/RebalanceLooper/rebalance_scale_factor.root", "READ");
  TH1F* h_sf = (TH1F*) sf_file->Get("sf");
  if(!h_sf){
    std::cout << "Couldn't open rebalance scale factor hist." << std::endl; 
    return;
  }
*/

  // Benchmark
  TBenchmark *bmark = new TBenchmark();
  bmark->Start("benchmark");

  MakeNtuple( Form("%s", output_name.c_str()) );

  // File Loop
  int nDuplicates = 0;
  int nEvents = chain->GetEntries();
  unsigned int nEventsChain = nEvents;
  cout << "[RebalanceLooper::loop] running on " << nEventsChain << " events" << endl;
  unsigned int nEventsTotal = 0;
  TObjArray *listOfFiles = chain->GetListOfFiles();
  TIter fileIter(listOfFiles);
  TFile *currentFile = 0;
  while ( (currentFile = (TFile*)fileIter.Next()) ) {
    cout << "[RebalanceLooper::loop] running on file: " << currentFile->GetTitle() << endl;

    // Get File Content
    TFile f( currentFile->GetTitle() );
    TTree *tree = (TTree*)f.Get("mt2");
    TTreeCache::SetLearnEntries(10);
    tree->SetCacheSize(128*1024*1024);

    t.Init(tree);

    // Event Loop
    unsigned int nEventsTree = tree->GetEntriesFast();
    for( unsigned int event = 0; event < nEventsTree; ++event) {
    //for( unsigned int event = 0; event < 1000; ++event) {
    //for( unsigned int event = 0; event < (nEventsTree/100); ++event) {

      t.GetEntry(event);
      
      //---------------------
      // bookkeeping and progress report
      //---------------------
      ++nEventsTotal;
      if (nEventsTotal%10000==0) {
        ULong64_t i_permille = (int)floor(1000 * nEventsTotal / float(nEventsChain));
        if (isatty(1)) {
          printf("\015\033[32m ---> \033[1m\033[31m%4.1f%%"
              "\033[0m\033[32m <---\033[0m\015", i_permille/10.);
          fflush(stdout);
        }
        else {
          cout<<i_permille/10.<<" ";
          if (nEventsTotal%100000==0) cout<<endl;
        }
      }

      InitNtuple();

      if(t.njet < 2){
        status = -1;
        FillNtuple(); 
        continue;
      }

      //---------------------
      // skip duplicates -- will need this eventually
      //---------------------
      //if( isData ) {
      //  DorkyEventIdentifier id = {stopt.run(), stopt.event(), stopt.lumi() };
      //  if (is_duplicate(id, already_seen) ){
      //    continue;
      //  }
      //}
      //

      ////////////////////////// rebalance //////////////////////////

      jetpt.clear();
      jeteta.clear();
      jetphi.clear();

      float met_x = (t.met_pt)*cos(t.met_phi);
      float met_y = (t.met_pt)*sin(t.met_phi);

      float jet_x = 0;
      float jet_y = 0;
      for(int i=0; i<t.njet; i++){
/*
        if(t.jet_pt[i] < 20.0){
          useJet.push_back(0);
          continue;
        }
*/
        if(t.jet_pt[i] < 100.0 && t.jet_puId[i] < 1){
          useJet.push_back(0);
          met_x += (t.jet_pt[i])*cos(t.jet_phi[i]);//FIXME
          met_y += (t.jet_pt[i])*sin(t.jet_phi[i]);//FIXME
          continue;
        }
        jet_x += (t.jet_pt[i])*cos(t.jet_phi[i]);
        jet_y += (t.jet_pt[i])*sin(t.jet_phi[i]);
        useJet.push_back(1);
        //float sf = GetValue(t.jet_pt[i], *h_sf);
        //float corr_pt = (t.jet_pt[i])/sf;
        //jetpt.push_back(corr_pt);
        jetpt.push_back(t.jet_pt[i]);
        jeteta.push_back(t.jet_eta[i]);
        jetphi.push_back(t.jet_phi[i]);
      }

      if(jetpt.size() < 2){
        for(int i=0; i<jetpt.size(); i++){
          rebalanceFactors.push_back(1.0);
        }
        status = -1;
        FillNtuple(); 
        continue;
      }

      pt_soft_x = -met_x - jet_x;
      pt_soft_y = -met_y - jet_y;
      float pt_soft = sqrt(pt_soft_x*pt_soft_x + pt_soft_y*pt_soft_y);
      sigma_soft = 25.0;
      //sigma_soft = 1.5*t.nVert;
      //sigma_soft = 5.0*sqrt(t.nVert);

      TMinuit* minimizer = new TMinuit(jetpt.size());
      minimizer->SetFCN(minuitFunction);
      int iflag = 0;
      Double_t arglist[10];

      arglist[0] = -1;
      minimizer->mnexcm("SET PRI", arglist, 1, iflag);

      arglist[0] = 1;
      minimizer->mnexcm("SET STRATEGY", arglist, 1, iflag);

      minimizer->SetErrorDef(0.5);

      for(int i=0; i<jetpt.size(); i++){
        std::string name = Form("c%d", i);
        minimizer->mnparm(i,name,1.0,0.05,0,0,iflag);
      }

      arglist[0] = 10000;
      arglist[1] = 1.0;

      minimizer->mnexcm("MIGRAD", arglist, 2, iflag);
      std::cout << "MIGRAD iflag = " << iflag << std::endl;
      status = iflag;
      
      if(iflag !=0){
        arglist[1] = 10.0;//easier threshold for convergence
        minimizer->mnexcm("MIGRAD", arglist, 2, iflag);
        std::cout << "second MIGRAD iflag = " << iflag << std::endl;
        status = iflag;
      }

      arglist[0] = 5000;
      arglist[1] = 0;
      arglist[2] = 1;

      if(iflag !=0){
        minimizer->mnexcm("MINOS", arglist, 3, iflag);
        status = iflag;
      }
         
      for(int i=0; i<jetpt.size(); i++){
        double par_value;
        double par_error;
        minimizer->GetParameter(i, par_value, par_error);
        par_value = 1.0/par_value;
        jetpt.at(i) *= par_value;
        rebalanceFactors.push_back(par_value); 
      }

      delete minimizer;

      jet_x = 0;
      jet_y = 0;
      for(int i=0; i<jetpt.size(); i++){
        jet_x += (jetpt.at(i))*cos(jetphi.at(i));
        jet_y += (jetpt.at(i))*sin(jetphi.at(i));
      }
      float new_met_x = -pt_soft_x - jet_x;
      float new_met_y = -pt_soft_y - jet_y;
      float new_met = sqrt(new_met_x*new_met_x + new_met_y*new_met_y);
      float new_mht = sqrt(jet_x*jet_x + jet_y*jet_y);
     
      FillNtuple();

    }//end loop on events in a file

    delete tree;
    f.Close();
    }//end loop on files

    cout << "[RebalanceLooper::loop] processed " << nEventsTotal << " events" << endl;
    if ( nEventsChain != nEventsTotal ) {
      std::cout << "ERROR: number of events from files is not equal to total number of events" << std::endl;
    }

    cout << nDuplicates << " duplicate events were skipped." << endl;

    CloseNtuple();

    bmark->Stop("benchmark");
    cout << endl;
    cout << nEventsTotal << " Events Processed" << endl;
    cout << "------------------------------" << endl;
    cout << "CPU  Time:	" << Form( "%.01f s", bmark->GetCpuTime("benchmark")  ) << endl;
    cout << "Real Time:	" << Form( "%.01f s", bmark->GetRealTime("benchmark") ) << endl;
    cout << endl;
    delete bmark;

    //delete h_sf;
    //sf_file->Close();
    //delete sf_file;

    return;
}

void RebalanceLooper::MakeNtuple(const char *Filename){
  outfile_ = new TFile(Form("%s", Filename), "RECREATE");
  outfile_->cd();
  outTree_ = new TTree("rebalance", "rebalance");

  outTree_->Branch("rebalanceFactors", &rebalanceFactors );
  outTree_->Branch("useJet", &useJet );
  outTree_->Branch("status", &status );

  return;
}

void RebalanceLooper::InitNtuple () {
  rebalanceFactors.clear();
  useJet.clear();
  status = -999;
  return;
}

void RebalanceLooper::FillNtuple(){
  outTree_->Fill();
  return;
}

void RebalanceLooper::CloseNtuple(){
  outfile_->cd();
  outTree_->Write();
  outfile_->Close();
  return;
}
