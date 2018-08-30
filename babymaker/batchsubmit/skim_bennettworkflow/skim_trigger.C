#include <fstream>
#include <sstream>
#include <iostream>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TCut.h>

using namespace std;

void skim_trigger(string inpath = "/nfs-6/userdata/mt2/V00-00-03", string outpath = "/nfs-6/userdata/mt2/V00-00-03_skim_nj2_ht450_mt2gt50", string tag = "ttall_msdecays") {
  
    //--------------------------------------------------
    // cut for output files
    //--------------------------------------------------
    
    string sel;
    if(tag.find("MET") != string::npos){
        sel = "nJet30>=2 && (HLT_PFMET120_PFMHT120!=0 || HLT_PFMETNoMu120_PFMHTNoMu120!=0) && met_pt>250 && nlep==0";
    }else if(tag.find("JetHT") != string::npos){
        sel = "nJet30>=2 && ht > 500";
    }else if(tag.find("EGamma") != string::npos || tag.find("SingleElectron") != string::npos){
        sel = "nJet30>=2 && HLT_SingleEl!=0 && nlep==1 && abs(lep_pdgId[0])==11 && lep_pt[0]>35";
    }else if(tag.find("SingleMuon") != string::npos){
        sel = "nJet30>=2 && HLT_SingleMu!=0 && nlep==1 && abs(lep_pdgId[0])==13 && lep_pt[0]>28";
    }else{
        cout << "ERROR! could not identify input dataset" << endl;
        return;
    }


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
