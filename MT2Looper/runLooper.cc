#include <string>
#include "TChain.h"
#include "TString.h"

#include "MT2Looper.h"

int main(int argc, char **argv) {

  if (argc < 5) {
    std::cout << "USAGE: runLooper <input_dir> <sample> <configuration_tag> <output_dir>" << std::endl;
    return 1;
  }

  std::string input_dir(argv[1]); 
  std::string sample(argv[2]); 
  std::string config_tag(argv[3]); 
  std::string output_dir(argv[4]);

  TChain *ch = new TChain("mt2"); 

  // T5qqqqWW comes from T5qqqqVV sample; replace here so we can find the correct files
  // BR correction factor is handled in the looper
  std::string modsamp = sample;
  std::string::size_type i = modsamp.find("T5qqqqWW");
  if(i != std::string::npos)
      modsamp.replace(i, 8, "T5qqqqVV");
  i = modsamp.find("T2WW");
  if(i != std::string::npos)
      modsamp.replace(i, 8, "T2bt");
 
  TString infile = Form("%s/%s*.root",input_dir.c_str(),modsamp.c_str());
  // TString infile = Form("%s/%s.root",input_dir.c_str(),sample.c_str());
  ch->Add(infile);
  if (ch->GetEntries() == 0) {
    std::cout << "ERROR: no entries in chain. filename was: " << infile << std::endl;
    return 2;
  }

  MT2Looper *looper = new MT2Looper();
  looper->loop(ch, sample, config_tag, output_dir); 
  return 0;
}
