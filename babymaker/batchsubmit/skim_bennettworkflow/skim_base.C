#include <fstream>
#include <sstream>
#include <iostream>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TCut.h>

using namespace std;

void skim_base(string inpath = "/nfs-6/userdata/mt2/V00-00-03", string outpath = "/nfs-6/userdata/mt2/V00-00-03_skim_nj2_ht450_mt2gt50", string tag = "ttall_msdecays") {
  
  //--------------------------------------------------
  // cut for output files
  //--------------------------------------------------
 
    string sel = "(ht > 250. && nJet30 >= 2 && mt2 > 200. && met_pt > 30.)";                      //baseline SR
    // sel += " || (htJECup > 250. && nJet30JECup >= 2 && mt2JECup > 200. && met_ptJECup > 30.)";      // base SR w/ JEC variation
    // sel += " || (htJECdn > 250. && nJet30JECdn >= 2 && mt2JECdn > 200. && met_ptJECdn > 30.)";      // base SR w/ JEC variation
    sel += " || (ht > 250. && nJet30 >= 1 && nJet30 <= 2 && met_pt > 200.)";                        // monojet SR
    // sel += " || (htJECup > 250. && nJet30JECup >= 1 && nJet30JECup <= 2 && met_ptJECup > 200.)";    // monojet SR w/ JEC var
    // sel += " || (htJECdn > 250. && nJet30JECdn >= 1 && nJet30JECdn <= 2 && met_ptJECdn > 200.)";    // monojet SR w/ JEC var
    sel += " || (gamma_ht > 250. && gamma_nJet30 >= 2 && gamma_mt2 > 200. && gamma_met_pt > 30.)";  // gamma+jet CR
    sel += " || (gamma_ht > 250. && gamma_nJet30 == 1 && gamma_met_pt > 200.)";                     // gamma_jet monojet CR
    sel += " || (zll_ht > 250. && nJet30 >= 2 && zll_mt2 > 200. && zll_met_pt > 30.)";              // DY CR
    sel += " || (zll_ht > 250. && nJet30 == 1 && zll_met_pt > 200.)";                               // DY monojet CR
    sel += " || (rl_ht > 250. && nJet30 >= 2 && rl_mt2 > 200. && rl_met_pt > 30.)";                 // removed lep CR
    sel += " || (rl_ht > 250. && nJet30 == 1 && rl_met_pt > 200.)";                                 // removed lep monojet CR
    // sel += " || (ht > 250. && nJet30 >= 2 && mt2_genmet > 200. && met_genPt > 30.)";                // some study using genmet
    // sel += " || (ht > 250. && nJet30 >= 1 && nJet30 <= 2 && met_genPt > 200.)";

  cout << "Skimming with selection : "<< sel<<endl;

  //--------------------------------------------------
  // input and output file
  //--------------------------------------------------
  
  const char* infilename = Form("%s/%s*.root",inpath.c_str(),tag.c_str());
  const char* outfilename = Form("%s/%s.root",outpath.c_str(),tag.c_str());
  
  //--------------------------------------------------
  // cout stuff
  //--------------------------------------------------
  
  cout << "Reading in : " << infilename << endl;
  cout << "Writing to : " << outfilename << endl;
  cout << "Selection : " << sel << endl;
  
  //--------------------------------------------------
  // read input file, write to output files
  //--------------------------------------------------
  
  //long long max_tree_size = 5000000000LL; // 5GB
  long long max_tree_size = 2500000000LL; // 2.5GB
  TTree::SetMaxTreeSize(max_tree_size);
  
  TChain *chain = new TChain("mt2");
  chain->Add(infilename);

  unsigned int input_entries = chain->GetEntries();
  cout << "Input tree has entries: " << input_entries << endl;
  
  //-------------------
  // skim
  //-------------------
  
  TFile *outfile = TFile::Open(outfilename, "RECREATE");
  assert( outfile != 0 );
  TTree* outtree = chain->CopyTree( sel.c_str() );
  unsigned int output_entries = outtree->GetEntries();
  cout << "Output tree has entries: " << output_entries << endl
       << "Reduction factor of: " << double(input_entries)/double(output_entries) << endl;
  outtree->Write();
  outfile->Close();

}
