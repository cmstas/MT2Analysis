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
  
  const char* sela = "run == 1 && lumi == 10 && evt == 7466";
  const char* selb = "run == 1 && lumi == 10 && evt == 7533";
  const char* selc = "run == 1 && lumi == 10 && evt == 7864";
  const char* seld = "run == 1 && lumi == 18 && evt == 13527";
  const char* sele = "run == 1 && lumi == 26 && evt == 20071";
  const char* self = "run == 1 && lumi == 30 && evt == 22853";
  const char* selg = "run == 1 && lumi == 31 && evt == 24384";
  const char* selh = "run == 1 && lumi == 38 && evt == 29109";
  const char* seli = "run == 1 && lumi == 45 && evt == 34943";
  const char* selj = "run == 1 && lumi == 51 && evt == 39806";
  const char* selk = "run == 1 && lumi == 59 && evt == 45536";
  const char* sell = "run == 1 && lumi == 62 && evt == 48748";
  const char* selm = "run == 1 && lumi == 65 && evt == 50410";
  const char* seln = "run == 1 && lumi == 65 && evt == 50571";
  const char* selo = "run == 1 && lumi == 77 && evt == 60021";
  const char* selp = "run == 1 && lumi == 80 && evt == 62492";
  const char* selq = "run == 1 && lumi == 87 && evt == 67448";
  const char* selr = "run == 1 && lumi == 88 && evt == 68817";
  const char* sels = "run == 1 && lumi == 96 && evt == 74504";
  const char* selt = "run == 1 && lumi == 100 && evt == 77984";
  const char* selu = "run == 1 && lumi == 105 && evt == 81855";
  const char* selv = "run == 1 && lumi == 105 && evt == 81933";
  const char* selw = "run == 1 && lumi == 122 && evt == 95092";
  const char* selx = "run == 1 && lumi == 123 && evt == 95452";
  const char* sely = "run == 1 && lumi == 147 && evt == 114514";
  const char* selz = "run == 1 && lumi == 149 && evt == 116292";
  const char* sel0 = "run == 1 && lumi == 153 && evt == 119424";
  const char* sel1 = "run == 1 && lumi == 158 && evt == 123590";
  const char* sel2 = "run == 1 && lumi == 166 && evt == 129183";
  const char* sel3 = "run == 1 && lumi == 167 && evt == 129938";

  const char * selection_string = Form("(%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s) || (%s)",sela,selb,selc,seld,sele,self,selg,selh,seli,selj,selk,sell,selm,seln,selo,selp,selq,selr,sels,selt,selu,selv,selw,selx,sely,selz,sel0,sel1,sel2,sel3);
  
  TFile * outfile = TFile::Open( "testfile_original.root" , "RECREATE" );

  TTree * skimmed = tree_st->CopyTree ( selection_string );

  outfile->cd();
  if (skimmed->GetEntries() > 0) {
    cout << skimmed->GetEntries() << " events found" << endl;
    skimmed->Write();
  } else cout << "No events with short tracks" << endl;
  outfile->Close();

}
