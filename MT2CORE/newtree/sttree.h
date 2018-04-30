//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Apr 30 01:34:45 2018 by ROOT version 6.10/09
// from TTree st/A short-track baby-tree edition
// found on file: /hadoop/cms/store/user/dpgilber/Stat1Leps_Friends/TTDL/MT2-10_ST-nonext_friend.root
//////////////////////////////////////////////////////////

#ifndef sttree_h
#define sttree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
//#include <TSelector.h>
#include "mt2tree.h"

// Header file for the classes stored in the TTree if any.

class sttree : public mt2tree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           ntracks;
   Int_t           nshorttracks;
   Int_t           nshorttracks_short;
   Int_t           nshorttracks_trkshort;
   Int_t           nshorttracks_trklong;
   Int_t           track_isshort[55];   //[ntracks]
   Int_t           track_ispixelonly[55];   //[ntracks]
   Int_t           track_istrkshort[55];   //[ntracks]
   Int_t           track_istrklong[55];   //[ntracks]
   Double_t        track_pt[55];   //[ntracks]
   Double_t        track_ptErr[55];   //[ntracks]
   Double_t        track_eta[55];   //[ntracks]
   Double_t        track_phi[55];   //[ntracks]
   Double_t        track_nChi2[55];   //[ntracks]
   Double_t        track_ipSigXY[55];   //[ntracks]
   Double_t        track_ipBSSigXY[55];   //[ntracks]
   Double_t        track_dxy[55];   //[ntracks]
   Double_t        track_dxyBS[55];   //[ntracks]
   Double_t        track_dxyErr[55];   //[ntracks]
   Double_t        track_dz[55];   //[ntracks]
   Double_t        track_dzBS[55];   //[ntracks]
   Double_t        track_dzErr[55];   //[ntracks]
   Int_t           track_valHits[55];   //[ntracks]
   Int_t           track_pixHits[55];   //[ntracks]
   Int_t           track_lostOuterHits[55];   //[ntracks]
   Int_t           track_lostOuterPixHits[55];   //[ntracks]
   Int_t           track_lostInnerHits[55];   //[ntracks]
   Int_t           track_lostInnerPixHits[55];   //[ntracks]
   Int_t           track_pixelLayersWithMeasurement[55];   //[ntracks]
   Int_t           track_trackerLayersWithMeasurement[55];   //[ntracks]
   Int_t           track_highPurity[55];   //[ntracks]
   Int_t           track_charge[55];   //[ntracks]
   Int_t           track_ismatched[55];   //[ntracks]
   Int_t           track_gpidx[55];   //[ntracks]
   Int_t           track_gppdgid[55];   //[ntracks]
   Double_t        track_gpdr[55];   //[ntracks]
   Double_t        track_sumPUChP[55];   //[ntracks]
   Double_t        track_sumChP[55];   //[ntracks]
   Double_t        track_sumChP0p3[55];   //[ntracks]
   Double_t        track_sumChH[55];   //[ntracks]
   Double_t        track_sumChHmini[55];   //[ntracks]
   Double_t        track_sumNeuH[55];   //[ntracks]
   Double_t        track_sumPh[55];   //[ntracks]
   Double_t        track_sumNeuH0p05[55];   //[ntracks]
   Double_t        track_sumPh0p05[55];   //[ntracks]
   Double_t        track_dedx[55];   //[ntracks]
   Double_t        track_dedxPixel[55];   //[ntracks]
   Int_t           track_pfPdgId[55];   //[ntracks]
   Int_t           track_pfCharge[55];   //[ntracks]
   Double_t        track_pfDR[55];   //[ntracks]
   Double_t        track_pfPt[55];   //[ntracks]
   Double_t        track_pfEta[55];   //[ntracks]
   Double_t        track_pfPhi[55];   //[ntracks]
   Int_t           track_anypfPdgId[55];   //[ntracks]
   Int_t           track_anypfCharge[55];   //[ntracks]
   Double_t        track_anypfDR[55];   //[ntracks]
   Double_t        track_anypfPt[55];   //[ntracks]
   Double_t        track_anypfEta[55];   //[ntracks]
   Double_t        track_anypfPhi[55];   //[ntracks]
   Double_t        track_jetDR[55];   //[ntracks]
   Double_t        track_jetPt[55];   //[ntracks]
   Double_t        track_jetEta[55];   //[ntracks]
   Double_t        track_jetPhi[55];   //[ntracks]
   Double_t        track_iso[55];   //[ntracks]
   Double_t        track_isonomin[55];   //[ntracks]
   Double_t        track_reliso[55];   //[ntracks]
   Double_t        track_relisonomin[55];   //[ntracks]
   Double_t        track_iso_nocorr[55];   //[ntracks]
   Double_t        track_reliso_nocorr[55];   //[ntracks]
   Double_t        track_isocorr[55];   //[ntracks]
   Double_t        track_relisocorr[55];   //[ntracks]
   Int_t           ncharginos;
   Int_t           gp_pdgid[1];   //[ncharginos]
   Double_t        gp_pt[1];   //[ncharginos]
   Double_t        gp_eta[1];   //[ncharginos]
   Double_t        gp_phi[1];   //[ncharginos]
   Int_t           gp_status[1];   //[ncharginos]
   Double_t        gp_decayXY[1];   //[ncharginos]

   // List of branches
   TBranch        *b_ntracks;   //!
   TBranch        *b_nshorttracks;   //!
   TBranch        *b_nshorttracks_short;   //!
   TBranch        *b_nshorttracks_trkshort;   //!
   TBranch        *b_nshorttracks_trklong;   //!
   TBranch        *b_track_isshort;   //!
   TBranch        *b_track_ispixelonly;   //!
   TBranch        *b_track_istrkshort;   //!
   TBranch        *b_track_istrklong;   //!
   TBranch        *b_track_pt;   //!
   TBranch        *b_track_ptErr;   //!
   TBranch        *b_track_eta;   //!
   TBranch        *b_track_phi;   //!
   TBranch        *b_track_nChi2;   //!
   TBranch        *b_track_ipSigXY;   //!
   TBranch        *b_track_ipBSSigXY;   //!
   TBranch        *b_track_dxy;   //!
   TBranch        *b_track_dxyBS;   //!
   TBranch        *b_track_dxyErr;   //!
   TBranch        *b_track_dz;   //!
   TBranch        *b_track_dzBS;   //!
   TBranch        *b_track_dzErr;   //!
   TBranch        *b_track_valHits;   //!
   TBranch        *b_track_pixHits;   //!
   TBranch        *b_track_lostOuterHits;   //!
   TBranch        *b_track_lostOuterPixHits;   //!
   TBranch        *b_track_lostInnerHits;   //!
   TBranch        *b_track_lostInnerPixHits;   //!
   TBranch        *b_track_pixelLayersWithMeasurement;   //!
   TBranch        *b_track_trackerLayersWithMeasurement;   //!
   TBranch        *b_track_highPurity;   //!
   TBranch        *b_track_charge;   //!
   TBranch        *b_track_ismatched;   //!
   TBranch        *b_track_gpidx;   //!
   TBranch        *b_track_gppdgid;   //!
   TBranch        *b_track_gpdr;   //!
   TBranch        *b_track_sumPUChP;   //!
   TBranch        *b_track_sumChP;   //!
   TBranch        *b_track_sumChP0p3;   //!
   TBranch        *b_track_sumChH;   //!
   TBranch        *b_track_sumChHmini;   //!
   TBranch        *b_track_sumNeuH;   //!
   TBranch        *b_track_sumPh;   //!
   TBranch        *b_track_sumNeuH0p05;   //!
   TBranch        *b_track_sumPh0p05;   //!
   TBranch        *b_track_dedx;   //!
   TBranch        *b_track_dedxPixel;   //!
   TBranch        *b_track_pfPdgId;   //!
   TBranch        *b_track_pfCharge;   //!
   TBranch        *b_track_pfDR;   //!
   TBranch        *b_track_pfPt;   //!
   TBranch        *b_track_pfEta;   //!
   TBranch        *b_track_pfPhi;   //!
   TBranch        *b_track_anypfPdgId;   //!
   TBranch        *b_track_anypfCharge;   //!
   TBranch        *b_track_anypfDR;   //!
   TBranch        *b_track_anypfPt;   //!
   TBranch        *b_track_anypfEta;   //!
   TBranch        *b_track_anypfPhi;   //!
   TBranch        *b_track_jetDR;   //!
   TBranch        *b_track_jetPt;   //!
   TBranch        *b_track_jetEta;   //!
   TBranch        *b_track_jetPhi;   //!
   TBranch        *b_track_iso;   //!
   TBranch        *b_track_isonomin;   //!
   TBranch        *b_track_reliso;   //!
   TBranch        *b_track_relisonomin;   //!
   TBranch        *b_track_iso_nocorr;   //!
   TBranch        *b_track_reliso_nocorr;   //!
   TBranch        *b_track_isocorr;   //!
   TBranch        *b_track_relisocorr;   //!
   TBranch        *b_ncharginos;   //!
   TBranch        *b_gp_pdgid;   //!
   TBranch        *b_gp_pt;   //!
   TBranch        *b_gp_eta;   //!
   TBranch        *b_gp_phi;   //!
   TBranch        *b_gp_status;   //!
   TBranch        *b_gp_decayXY;   //!

   sttree(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~sttree() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   ClassDef(sttree,0);
};

#endif

#ifdef sttree_cxx
void sttree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("ntracks", &ntracks, &b_ntracks);
   fChain->SetBranchAddress("nshorttracks", &nshorttracks, &b_nshorttracks);
   fChain->SetBranchAddress("nshorttracks_short", &nshorttracks_short, &b_nshorttracks_short);
   fChain->SetBranchAddress("nshorttracks_trkshort", &nshorttracks_trkshort, &b_nshorttracks_trkshort);
   fChain->SetBranchAddress("nshorttracks_trklong", &nshorttracks_trklong, &b_nshorttracks_trklong);
   fChain->SetBranchAddress("track_isshort", track_isshort, &b_track_isshort);
   fChain->SetBranchAddress("track_ispixelonly", track_ispixelonly, &b_track_ispixelonly);
   fChain->SetBranchAddress("track_istrkshort", track_istrkshort, &b_track_istrkshort);
   fChain->SetBranchAddress("track_istrklong", track_istrklong, &b_track_istrklong);
   fChain->SetBranchAddress("track_pt", track_pt, &b_track_pt);
   fChain->SetBranchAddress("track_ptErr", track_ptErr, &b_track_ptErr);
   fChain->SetBranchAddress("track_eta", track_eta, &b_track_eta);
   fChain->SetBranchAddress("track_phi", track_phi, &b_track_phi);
   fChain->SetBranchAddress("track_nChi2", track_nChi2, &b_track_nChi2);
   fChain->SetBranchAddress("track_ipSigXY", track_ipSigXY, &b_track_ipSigXY);
   fChain->SetBranchAddress("track_ipBSSigXY", track_ipBSSigXY, &b_track_ipBSSigXY);
   fChain->SetBranchAddress("track_dxy", track_dxy, &b_track_dxy);
   fChain->SetBranchAddress("track_dxyBS", track_dxyBS, &b_track_dxyBS);
   fChain->SetBranchAddress("track_dxyErr", track_dxyErr, &b_track_dxyErr);
   fChain->SetBranchAddress("track_dz", track_dz, &b_track_dz);
   fChain->SetBranchAddress("track_dzBS", track_dzBS, &b_track_dzBS);
   fChain->SetBranchAddress("track_dzErr", track_dzErr, &b_track_dzErr);
   fChain->SetBranchAddress("track_valHits", track_valHits, &b_track_valHits);
   fChain->SetBranchAddress("track_pixHits", track_pixHits, &b_track_pixHits);
   fChain->SetBranchAddress("track_lostOuterHits", track_lostOuterHits, &b_track_lostOuterHits);
   fChain->SetBranchAddress("track_lostOuterPixHits", track_lostOuterPixHits, &b_track_lostOuterPixHits);
   fChain->SetBranchAddress("track_lostInnerHits", track_lostInnerHits, &b_track_lostInnerHits);
   fChain->SetBranchAddress("track_lostInnerPixHits", track_lostInnerPixHits, &b_track_lostInnerPixHits);
   fChain->SetBranchAddress("track_pixelLayersWithMeasurement", track_pixelLayersWithMeasurement, &b_track_pixelLayersWithMeasurement);
   fChain->SetBranchAddress("track_trackerLayersWithMeasurement", track_trackerLayersWithMeasurement, &b_track_trackerLayersWithMeasurement);
   fChain->SetBranchAddress("track_highPurity", track_highPurity, &b_track_highPurity);
   fChain->SetBranchAddress("track_charge", track_charge, &b_track_charge);
   fChain->SetBranchAddress("track_ismatched", track_ismatched, &b_track_ismatched);
   fChain->SetBranchAddress("track_gpidx", track_gpidx, &b_track_gpidx);
   fChain->SetBranchAddress("track_gppdgid", track_gppdgid, &b_track_gppdgid);
   fChain->SetBranchAddress("track_gpdr", track_gpdr, &b_track_gpdr);
   fChain->SetBranchAddress("track_sumPUChP", track_sumPUChP, &b_track_sumPUChP);
   fChain->SetBranchAddress("track_sumChP", track_sumChP, &b_track_sumChP);
   fChain->SetBranchAddress("track_sumChP0p3", track_sumChP0p3, &b_track_sumChP0p3);
   fChain->SetBranchAddress("track_sumChH", track_sumChH, &b_track_sumChH);
   fChain->SetBranchAddress("track_sumChHmini", track_sumChHmini, &b_track_sumChHmini);
   fChain->SetBranchAddress("track_sumNeuH", track_sumNeuH, &b_track_sumNeuH);
   fChain->SetBranchAddress("track_sumPh", track_sumPh, &b_track_sumPh);
   fChain->SetBranchAddress("track_sumNeuH0p05", track_sumNeuH0p05, &b_track_sumNeuH0p05);
   fChain->SetBranchAddress("track_sumPh0p05", track_sumPh0p05, &b_track_sumPh0p05);
   fChain->SetBranchAddress("track_dedx", track_dedx, &b_track_dedx);
   fChain->SetBranchAddress("track_dedxPixel", track_dedxPixel, &b_track_dedxPixel);
   fChain->SetBranchAddress("track_pfPdgId", track_pfPdgId, &b_track_pfPdgId);
   fChain->SetBranchAddress("track_pfCharge", track_pfCharge, &b_track_pfCharge);
   fChain->SetBranchAddress("track_pfDR", track_pfDR, &b_track_pfDR);
   fChain->SetBranchAddress("track_pfPt", track_pfPt, &b_track_pfPt);
   fChain->SetBranchAddress("track_pfEta", track_pfEta, &b_track_pfEta);
   fChain->SetBranchAddress("track_pfPhi", track_pfPhi, &b_track_pfPhi);
   fChain->SetBranchAddress("track_anypfPdgId", track_anypfPdgId, &b_track_anypfPdgId);
   fChain->SetBranchAddress("track_anypfCharge", track_anypfCharge, &b_track_anypfCharge);
   fChain->SetBranchAddress("track_anypfDR", track_anypfDR, &b_track_anypfDR);
   fChain->SetBranchAddress("track_anypfPt", track_anypfPt, &b_track_anypfPt);
   fChain->SetBranchAddress("track_anypfEta", track_anypfEta, &b_track_anypfEta);
   fChain->SetBranchAddress("track_anypfPhi", track_anypfPhi, &b_track_anypfPhi);
   fChain->SetBranchAddress("track_jetDR", track_jetDR, &b_track_jetDR);
   fChain->SetBranchAddress("track_jetPt", track_jetPt, &b_track_jetPt);
   fChain->SetBranchAddress("track_jetEta", track_jetEta, &b_track_jetEta);
   fChain->SetBranchAddress("track_jetPhi", track_jetPhi, &b_track_jetPhi);
   fChain->SetBranchAddress("track_iso", track_iso, &b_track_iso);
   fChain->SetBranchAddress("track_isonomin", track_isonomin, &b_track_isonomin);
   fChain->SetBranchAddress("track_reliso", track_reliso, &b_track_reliso);
   fChain->SetBranchAddress("track_relisonomin", track_relisonomin, &b_track_relisonomin);
   fChain->SetBranchAddress("track_iso_nocorr", track_iso_nocorr, &b_track_iso_nocorr);
   fChain->SetBranchAddress("track_reliso_nocorr", track_reliso_nocorr, &b_track_reliso_nocorr);
   fChain->SetBranchAddress("track_isocorr", track_isocorr, &b_track_isocorr);
   fChain->SetBranchAddress("track_relisocorr", track_relisocorr, &b_track_relisocorr);
   fChain->SetBranchAddress("ncharginos", &ncharginos, &b_ncharginos);
   fChain->SetBranchAddress("gp_pdgid", &gp_pdgid, &b_gp_pdgid);
   fChain->SetBranchAddress("gp_pt", &gp_pt, &b_gp_pt);
   fChain->SetBranchAddress("gp_eta", &gp_eta, &b_gp_eta);
   fChain->SetBranchAddress("gp_phi", &gp_phi, &b_gp_phi);
   fChain->SetBranchAddress("gp_status", &gp_status, &b_gp_status);
   fChain->SetBranchAddress("gp_decayXY", &gp_decayXY, &b_gp_decayXY);
}

Bool_t sttree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef sttree_cxx
