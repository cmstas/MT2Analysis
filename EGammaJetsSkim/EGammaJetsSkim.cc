// A looper to skim MCs for low Dphi events

#include <iostream>

#include "../CORE/Tools/utils.h"

#include "../MT2CORE/mt2tree.h"

using namespace std;

int main (int argc, char ** argv)
{
  if (argc < 4)
    {
      cout << "Usage: ./EGammaJetsSkim <input_directory> <sample> <output_directory>" << endl;
      return 0;
    }

  TChain* chain = new TChain("mt2");
  TString inputs = Form("%s/%s*.root", argv[1], argv[2]);
  chain->Add(inputs);
  
  // Skim

  TString output_name = Form("%s%s_zero.root", argv[3], argv[2]);
  TFile * outfile = TFile::Open( output_name , "RECREATE" );
  
  // run on unskimmed babies to apply lower mt2 cut and no MET cut

  const char* htCut = "ht > 1200";
  const char* j1Cut = "jet1_pt > 300";
  const char* j2Cut = "jet2_pt > 300";

  const char * selection_string = Form("%s && %s && %s", htCut, j1Cut, j2Cut);

  TTree * skimmed = chain->CopyTree ( selection_string );
  skimmed->Write();
  outfile->Close();
}
