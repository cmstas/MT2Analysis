// Skim MT2ST files

#include <iostream>
#include <string>

#include "../CORE/Tools/utils.h"

#include "TFile.h"
#include "TTree.h"

using namespace std;

int main (int argc, char ** argv)
{
  if (argc < 4)
    {
      cout << "Usage: ./ShortTrackSkim.exe <indir> <sample> <outdir>" << endl;
      return 0;
    }

  TFile f( Form("%s/%s.root", argv[1], argv[2]) );
  TTree *tree_mt2 = (TTree*)f.Get("mt2st");
  TTree *tree_st = (TTree*)f.Get("st");
  tree_st->AddFriend(tree_mt2);
  tree_mt2->AddFriend(tree_st);

  // HT100LooseSTC
  const char * selection_string = "Sum$(jet_pt) > 100 && Sum$(track_pt > 15 && fabs(track_eta)<2.4 && track_iso < 100 && track_reliso < 1 && track_sumChP < 100 && track_sumNeuH0p05 + track_sumPh0p05 < 100 && track_sumChP / track_pt < 1 && (track_sumNeuH0p05 + track_sumPh0p05) / track_pt < 100) > 0";
  
  TFile * outfile = TFile::Open( Form("%s/%s.root",argv[3],argv[2]) , "RECREATE" );

  TTree * skimmed_mt2 = tree_mt2->CopyTree ( selection_string );
  TTree * skimmed_st = tree_st->CopyTree ( selection_string );

  if (skimmed_mt2->GetEntries() > 0 && skimmed_mt2->GetEntries() == skimmed_st->GetEntries()) {
    outfile->cd();
    cout << skimmed_mt2->GetEntries() << " events found" << endl;
    skimmed_mt2->Write();
    skimmed_st->Write();
  } else cout << "No events passed selections" << endl;
  outfile->Close();

}
