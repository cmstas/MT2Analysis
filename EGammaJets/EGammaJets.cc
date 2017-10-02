#include "EGammaJets.h"

using namespace std;
using namespace mt2;
using namespace duplicate_removal;

void EGammaJets::loop (const TChain * chain, const char* sample, const char* outdir, const int maxEvt) {
  TString outname = Form("%s/%s.root",outdir,sample);
  TFile * outfile = new TFile(outname.Data(), "RECREATE");

  //  const char* lumi17 = "11.379";

  const int nBins = 30;
  const float minRat = 0;
  const float maxRat = 3;

  TH1::SetDefaultSumw2(true);

  TH1F h_ph("h_ph","Photon-Jet Pairs by p_{T}^{#gamma}/p_{T}^{Jet}",nBins,minRat,maxRat);
  TH1F h_el("h_el","Electron-Jet Pairs by p_{T}^{e}/p_{T}^{Jet}",nBins,minRat,maxRat);
  TTree * tree = (TTree*)chain->Clone("mt2");
  mt2tree mt2_tree;
  mt2_tree.Init(tree);

  int available = tree->GetEntries();
  int max_event = maxEvt < 0 ? available : min(maxEvt, available);
  
  cout << "Running on " << max_event << " events." << endl;
  cout << "Tree contains " << available << " events." << endl;

  const char* json_file = "../babymaker/jsons/Cert_294927-301567_13TeV_PromptReco_Collisions17_JSON_snt.txt";
  set_goodrun_file(json_file);
  
  eventFilter metFilterTxt;
  cout<<"Loading bad event files ..."<<endl;
  // updated lists for full dataset
  metFilterTxt.loadBadEventList("/nfs-6/userdata/mt2utils/csc2015_Dec01.txt");
  metFilterTxt.loadBadEventList("/nfs-6/userdata/mt2utils/ecalscn1043093_Dec01.txt");
  metFilterTxt.loadBadEventList("/nfs-6/userdata/mt2utils/badResolutionTrack_Jan13.txt");
  metFilterTxt.loadBadEventList("/nfs-6/userdata/mt2utils/muonBadTrack_Jan13.txt");
  cout<<" ... finished!"<<endl;

  int nDup = 0;
  outfile->cd();

  for (int event = 0; event < max_event; event++) {
    if ( (event+1) % 100000 == 0) cout << "Processing event " << event+1 << ", " << (100. * event) / max_event << "% complete" << endl;

    mt2_tree.GetEntry(event);

    const int run_ = mt2_tree.run;
    const int evt_ = mt2_tree.evt;
    const int lumi_ = mt2_tree.lumi;

    DorkyEventIdentifier id (run_, evt_, lumi_);
    if (is_duplicate(id)) {
      nDup++;
      continue;
    }

    if (!goodrun(run_, lumi_)) continue;
    if (mt2_tree.nVert == 0) continue;
    
    // Filters
    if (!mt2_tree.Flag_globalSuperTightHalo2016Filter) continue; 
    //    if (!mt2_tree.Flag_badMuonFilterV2) continue;
    if (!mt2_tree.Flag_eeBadScFilter) continue; 

    if (!mt2_tree.Flag_goodVertices) continue;
    if (!mt2_tree.Flag_HBHENoiseFilter) continue;
    if (!mt2_tree.Flag_HBHENoiseIsoFilter) continue;
    if (!mt2_tree.Flag_EcalDeadCellTriggerPrimitiveFilter) continue;
    //    if (!mt2_tree.Flag_badChargedHadronFilterV2) continue; 

    const float ht_ = mt2_tree.ht;

    if (isinf(ht_) || isnan(ht_)) continue;

    if (ht_ < 1200) continue;    

    const int njet_ = mt2_tree.nJet30;
    if (njet_ < 2) continue;

    if (mt2_tree.jet_pt[0] > 13000.) continue;

    const float jet1_pt_ = mt2_tree.jet1_pt;
    const float jet2_pt_ = mt2_tree.jet2_pt;

    if (jet1_pt_ < 300 || jet2_pt_ < 300) continue;
    
    for (int iJet = 0; iJet < mt2_tree.nJet40; iJet++) {

      const float jet_pt_ = mt2_tree.jet_pt[iJet];
      // Don't look at any jets with pt < 300 GeV
      if (jet_pt_ < 300) {
	iJet = mt2_tree.nJet40;
	continue;
      }
      const float jet_eta_ = mt2_tree.jet_phi[iJet];
      const float jet_phi_ = mt2_tree.jet_phi[iJet];
      
      bool pho_overlap = false;
      for (int ipho = 0; ipho < mt2_tree.ngamma && !pho_overlap; ipho++) {

	// Only consider photons with pT > 100
	const float pho_pt_ = mt2_tree.gamma_pt[ipho];
	if (pho_pt_ < 100)
	  {
	    ipho = mt2_tree.ngamma;
	    continue;
	  }
	
	const float pho_eta_ = mt2_tree.gamma_eta[ipho];
	const float pho_phi_ = mt2_tree.gamma_phi[ipho];
	
	pho_overlap = DeltaR(pho_eta_,jet_eta_,pho_phi_,jet_phi_) < 0.3;
	if (pho_overlap) 
	  {
	    h_ph.Fill(pho_pt_ / jet_pt_);
	  }
      }

      // I don't expect to see a hard photon and hard electron in the same jet
      // but just in case, use different  overlap and recheck so as not to bias in favor of photons
      bool el_overlap = false;
      for ( int ilep = 0; ilep < mt2_tree.nlep && !el_overlap; ilep++) {
	
	// Only electrons
	if (abs(mt2_tree.lep_pdgId[ilep]) != 11) continue;

	// Only consider els with pT > 100
	const float el_pt_ = mt2_tree.lep_pt[ilep];
	if (el_pt_ < 100)
	  {
	    ilep = mt2_tree.nlep;
	    continue;
	  }
	
	const float el_eta_ = mt2_tree.lep_eta[ilep];
	const float el_phi_ = mt2_tree.lep_phi[ilep];
	
	el_overlap = DeltaR(el_eta_,jet_eta_,el_phi_,jet_phi_) < 0.3;
	if (el_overlap) 
	  {
	    h_el.Fill(el_pt_ / jet_pt_);
	  }
      }
      
      if (el_overlap && pho_overlap) {
	cout << "Double overlap jet!" << endl;
	cout << "Run:lumi:evt = " << mt2_tree.evt << ":" << mt2_tree.lumi << ":" << mt2_tree.evt << endl;
      }

    }

  } // Event Loop

  cout << "Finished event loop." << endl;

  h_ph.Write();
  h_el.Write();

  outfile->Close();

  cout << "Number of duplicates was: " << nDup << endl;
  
} // loop function


int main (int argc, char ** argv) {
  int maxEvt = -1;
  if (argc > 4) maxEvt = atoi(argv[4]);

  TChain* ch = new TChain("mt2");
  
  TString infile_name = Form("%s/%s*.root", argv[1], argv[2]);
  ch->Add(infile_name);

  EGammaJets * l = new EGammaJets();
  l->loop(ch, argv[2], argv[3], maxEvt);
  return 0;
}

EGammaJets::EGammaJets() {}

EGammaJets::~EGammaJets() {};
