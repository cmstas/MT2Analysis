#ifndef JRTREADER
#define JRTREADER

#include <vector>

#include "TROOT.h"
#include "TChain.h"
#include "TTree.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TH1D.h"

using namespace std;

class JRTreader {
 public:
    
    JRTreader(const char *fname=0);
    ~JRTreader();
    int Init(const char *fname, bool correctDataResponse=false, int unc_var=0, string conf_tag="");
    float GetRandomResponse(float pt, float eta, bool isBjet);
    float GetValue(float pt, float eta, bool isBjet, float smearfact);
    float GetJERCorrection(float eta, int unc_var=0);
    void GetModifiedBins(int ptbin, int etabin, bool isBjet, int *new_ptbin, int *new_etabin);
    static int GetPtBin(float pt);
    static int GetEtaBin(float eta);
    void SetCoreScale(float);
    void SetTailScale(float);
    void SetMeanShift(float);
    void TransformFit(TH1D *fit, TH1D *core, TH1D *tail);
    void UseRawHistograms(bool use=true);
    void SetBinWidth(float width);
    void Draw(int,int, TH1D* usethis=NULL, string tag="test");
    void DrawComp(TH1D* h1, TH1D* h2, TString tag="test");

    // void debug(){ cout << "ADDR: " << fits_nonb << " test   " << fits_nonb->size() << endl; }

    // Sorry to anyone else that attempts to read this. Eta bins in the templates do not correspond to the
    // eta bins for the Jet Energy Response data/MC corrections, so I have to use a finer binning that
    // has edges on both. The array eta_bin_map gives the index of the corresponding eta bin in the 
    // jet response template file.
    static const int n_eta_bins = 17;
    static constexpr float eta_bins[n_eta_bins+1] = {0,0.3,0.5,0.8,1.1,1.3,1.4,1.7,1.9,2.1,
                                                     2.3,2.5,2.8,3.0,3.2,4.1,5.0,-1};
    static constexpr int eta_bin_map[n_eta_bins] = {0, 1, 2, 3, 4, 4, 5, 6, 6, 6, 7, 7, 8, 8, 9, 10, 11};
                              // for reference      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14  15  16
    static const int n_pt_bins  = 23;
    static constexpr float pt_bins[n_pt_bins+1]   = {0,20,30,50,80,120,170,230,300,380,470,570,680,800,1000,
                                                     1300,1700,2200,2800,3500,4300,5200,6500,-1};

 private:
    static void DeleteFitVec(vector< vector<TH1D*>* > *fits);

    vector< vector<TH1D*>* > *fits_b;
    vector< vector<TH1D*>* > *fits_nonb;
    string config_tag;
    bool useFits = true;
    float BINWIDTH = 0.02;
    float coreScale = 1.0;
    float tailScale = 1.0;
    float meanShift = 0.0;

};

#endif
