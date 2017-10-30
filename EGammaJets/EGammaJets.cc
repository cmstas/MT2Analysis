#include "EGammaJets.h"

using namespace std;
using namespace mt2;
using namespace duplicate_removal;

void EGammaJets::loop (const TChain * chain, const char* sample, const char* outdir, const int maxEvt) {
  TString outname = Form("%s/%s.root",outdir,sample);
  TFile * outfile = new TFile(outname.Data(), "RECREATE");

  //  const char* lumi17 = "11.379";

  const int nBinsRat = 30;
  const float minRat = 0.0;
  const float maxRat = 3.0;

  const int nPhiBins = 32;
  const float minPhi = 0.0;
  const float maxPhi = 3.2;

  const int nBinsNorm = 30;
  const int minNorm = 0;
  const int maxNorm = 3;

  const int nBinsPt = 30;
  const float minPt = 300;
  const float maxPt = 900;

  const int nBinsMet = 30;
  const float minMet = -300;
  const float maxMet = 300;

  //  const int nMETBins = 10;
  // const float minMET = 0;
  // const float maxMET = 200;

  const float nEFBins = 100;
  const float minEF = 0;
  const float maxEF = 1;

  TH1::SetDefaultSumw2(true);

  TH1F h_ph("h_ph","Photon-Jet Pairs by p_{T}^{#gamma}/p_{T}^{Jet}",nBinsRat,minRat,maxRat);
  TH1F h_el("h_el","Electron-Jet Pairs by p_{T}^{e}/p_{T}^{Jet}",nBinsRat,minRat,maxRat);
  TH1F h_el_id("h_el_id","Electron-Jet Pairs by p_{T}^{e}/p_{T}^{Jet} (Tightened ID)",nBinsRat,minRat,maxRat);
  TH1F h_highNEEF_phi("h_highNEEF_phi","#phi_{Jet} - #phi_{MET} for Jets with NEEF > 0.8",nPhiBins,minPhi,maxPhi);
  TH1F h_lowNEEF_phi("h_lowNEEF_phi","#phi_{Jet} - #phi_{MET} for Jets with NEEF < 0.8",nPhiBins,minPhi,maxPhi);
  TH1F h_METJet_NEEF("h_METJet_NEEF","NEEF of Jets Opposite MET",nEFBins,minEF,maxEF);
  TH1F h_METJet_CEEF("h_METJet_CEEF","CEEF of Jets Opposite MET",nEFBins,minEF,maxEF);
  TH1F h_METJet_NHEF("h_METJet_NHEF","NHEF of Jets Opposite MET",nEFBins,minEF,maxEF);
  TH1F h_METJet_CHEF("h_METJet_CHEF","CHEF of Jets Opposite MET",nEFBins,minEF,maxEF);

  TProfile p_NEEF("p_NEEF","Average NEEF of Jets by #phi_{Jet} - #phi_{MET}",nPhiBins,minPhi,maxPhi);
  TProfile p_CEEF("p_CEEF","Average CEEF of Jets by #phi_{Jet} - #phi_{MET}",nPhiBins,minPhi,maxPhi);
  TProfile p_NHEF("p_NHEF","Average NHEF of Jets by #phi_{Jet} - #phi_{MET}",nPhiBins,minPhi,maxPhi);
  TProfile p_CHEF("p_CHEF","Average CHEF of Jets by #phi_{Jet} - #phi_{MET}",nPhiBins,minPhi,maxPhi);

  TH1F h_el_norm("h_el_norm","Electron-Jet Pairs by (p_{T}^{e}/p_{T}^{Jet}) / Jet CEMF",nBinsNorm,minNorm,maxNorm);
  TH1F h_ph_norm("h_ph_norm","Photon-Jet Pairs by (p_{T}^{#gamma}/p_{T}^{Jet}) / Jet NEMF",nBinsNorm,minNorm,maxNorm);
  TH1F h_el_norm_no0("h_el_norm_no0","Electron-Jet Pairs by (p_{T}^{e}/p_{T}^{Jet}) / Jet CEMF",nBinsNorm,minNorm,maxNorm); 
  TH1F h_ph_norm_no0("h_ph_norm_no0","Photon-Jet Pairs by (p_{T}^{#gamma}/p_{T}^{Jet}) / Jet NEMF",nBinsNorm,minNorm,maxNorm);
  TProfile p_el("p_el","Mean p_{T}^{e}/p_{T}^{Jet} by p_{T}^{Jet}",nBinsPt,minPt,maxPt);
  TProfile p_ph("p_ph","Mean p_{T}^{#gamma}/p_{T}^{Jet} by p_{T}^{Jet}",nBinsPt,minPt,maxPt);

  TH1F h_ph_par("h_ph_par","MET (Anti)Parallel to Photon",nBinsMet,minMet,maxMet);
  TH1F h_ph_per("h_ph_per","MET Perpendicular to Photon",nBinsMet,minMet,maxMet);
  TH1F h_el_par("h_el_par","MET (Anti)Parallel to Electron",nBinsMet,minMet,maxMet);
  TH1F h_el_per("h_el_per","MET Perpendicular to Electron",nBinsMet,minMet,maxMet);

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
      const float jet_eta_ = mt2_tree.jet_eta[iJet];
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
	    h_ph_norm.Fill( (pho_pt_ / jet_pt_) / mt2_tree.jet_NEEF[iJet] );
	    if (mt2_tree.jet_NEEF[iJet] > 0) h_ph_norm_no0.Fill( (pho_pt_ / jet_pt_) / mt2_tree.jet_NEEF[iJet] );
	    p_ph.Fill( jet_pt_, pho_pt_ / jet_pt_ );

	    const float cosphi = cos(pho_phi_ - mt2_tree.met_phi);
	    const float sinphi = sin(pho_phi_ - mt2_tree.met_phi);

	    h_ph_par.Fill( mt2_tree.met_pt * cosphi );
	    h_ph_per.Fill( mt2_tree.met_pt * sinphi );
	  }
      }

      // Because we're using electrons without IDs, there's a chance they're actually photons.
      // If they passed a loose photon ID, let's just say they're photons and not double count them
      bool el_overlap = pho_overlap;
      for ( int ilep = 0; ilep < mt2_tree.nEl50noID && !el_overlap; ilep++) {
	
	// Only electrons

	// Only consider els with pT > 100
	const float el_pt_ = mt2_tree.El50noID_pt[ilep];
	if (el_pt_ < 100)
	  {
	    ilep = mt2_tree.nEl50noID;
	    continue;
	  }
	
	const float el_eta_ = mt2_tree.El50noID_eta[ilep];
	const float el_phi_ = mt2_tree.El50noID_phi[ilep];
	
	el_overlap = DeltaR(el_eta_,jet_eta_,el_phi_,jet_phi_) < 0.3;
	if (el_overlap) 
	  {
	    h_el.Fill(el_pt_ / jet_pt_);
	    h_el_norm.Fill( (el_pt_ / jet_pt_) / mt2_tree.jet_CEEF[iJet] );
	    if (mt2_tree.jet_CEEF[iJet] > 0) h_el_norm_no0.Fill( (el_pt_ / jet_pt_) / mt2_tree.jet_CEEF[iJet] );
	    p_el.Fill( jet_pt_, (el_pt_ / jet_pt_) );

	    const float cosphi = cos(el_phi_ - mt2_tree.met_phi);
	    const float sinphi = sin(el_phi_ - mt2_tree.met_phi);

	    h_el_par.Fill( mt2_tree.met_pt * cosphi );
	    h_el_per.Fill( mt2_tree.met_pt * sinphi );


	    if (fabs(jet_eta_) < 2.4 && mt2_tree.jet_NHEF[iJet] < 0.9 && mt2_tree.jet_NEEF[iJet] < 0.9 && mt2_tree.jet_NM[iJet] > 1 && mt2_tree.jet_CHEF[iJet] > 0 && mt2_tree.jet_CM[iJet] > 0 && mt2_tree.jet_CEEF[iJet] < 0.8)
	      {
		h_el_id.Fill(el_pt_ / jet_pt_);
	      }

	  }
      }
      
      if (el_overlap && pho_overlap) {
	cout << "Double overlap jet!" << endl;
	cout << "Run:lumi:evt = " << mt2_tree.evt << ":" << mt2_tree.lumi << ":" << mt2_tree.evt << endl;
      }

      const float met_pt_ = mt2_tree.met_pt;

      if (met_pt_ < 100) continue;

      const float NEEF = mt2_tree.jet_NEEF[iJet];
      const float CEEF = mt2_tree.jet_CEEF[iJet];
      const float NHEF = mt2_tree.jet_NHEF[iJet];
      const float CHEF = mt2_tree.jet_CHEF[iJet];
      const float met_phi_ = mt2_tree.met_phi;
      float diff = fabs(jet_phi_-met_phi_);
      if (diff > 3.14159) diff = 2 * 3.14159 - diff;
      if (NEEF < 0.8) {	
	h_lowNEEF_phi.Fill( diff );
      } else {
	h_highNEEF_phi.Fill( diff );
      }
      if (3.14159 - diff < 0.3) {
	h_METJet_NEEF.Fill(NEEF);
	h_METJet_CEEF.Fill(CEEF);
	h_METJet_NHEF.Fill(NHEF);
	h_METJet_CHEF.Fill(CHEF);
      }
      p_NEEF.Fill(diff,NEEF);
      p_CEEF.Fill(diff,CEEF);
      p_NHEF.Fill(diff,NHEF);
      p_CHEF.Fill(diff,CHEF);


    } // Jet Loop

  } // Event Loop

  cout << "Finished event loop." << endl;

  h_ph.Write();
  h_el.Write();
  h_el_id.Write();
  h_lowNEEF_phi.Write();
  h_highNEEF_phi.Write();
  h_METJet_NEEF.Write();
  h_METJet_NHEF.Write();
  h_METJet_CEEF.Write();
  h_METJet_CHEF.Write();
  p_NEEF.Write();
  p_NHEF.Write();
  p_CEEF.Write();
  p_CHEF.Write();

  h_el_norm.Write();
  h_ph_norm.Write();
  h_el_norm_no0.Write();
  h_ph_norm_no0.Write();
  p_ph.Write();
  p_el.Write();

  h_ph_par.Write();
  h_ph_per.Write();
  h_el_par.Write();
  h_el_per.Write();

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
