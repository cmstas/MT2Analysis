// A looper to skim MCs for low Dphi evts

#include <iostream>
#include <string>

#include "../CORE/Tools/utils.h"

#include "TFile.h"
#include "TTree.h"

using namespace std;

int main (int argc, char ** argv)
{
  if (argc < 3)
    {
      cout << "Usage: ./ShortTrackOnlySkim.exe <input_directory> <sample>" << endl;
      return 0;
    }

  TFile f( Form("%s/%s.root", argv[1], argv[2]) );
  TTree *tree_mt2 = (TTree*)f.Get("mt2st");
  TTree *tree_st = (TTree*)f.Get("st");
  tree_st->AddFriend(tree_mt2);
  
  const char * selection_string = "Sum$(jet_pt) > 100";
  
  TFile * outfile = TFile::Open( Form("/nfs-6/userdata/dpgilber/MT2ST_skim_HT100/%s.root",argv[2]) , "RECREATE" );

  TTree * skimmed = tree_st->CopyTree ( selection_string );

  outfile->cd();
  if (skimmed->GetEntries() > 0) {
    cout << skimmed->GetEntries() << " events found" << endl;
    skimmed->Write();
  } else cout << "No events with short tracks" << endl;
  outfile->Close();

}
