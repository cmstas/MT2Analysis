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

#define N_PT_BINS 23
#define N_ETA_BINS 12

using namespace std;

class JRTreader {
 public:
    
    JRTreader(const char *fname=0);
    ~JRTreader();
    int Init(const char *fname);
    float GetRandomResponse(float pt, float eta, bool isBjet, bool correctDataResolution=false);
    float GetValue(float pt, float eta, bool isBjet, float smearfact, bool correctDataResolution=false);
    static float GetJERCorrection(float eta);
    static int GetPtBin(float pt);
    static int GetEtaBin(float eta);
    static void GetModifiedBins(int ptbin, int etabin, bool isBjet, int *new_ptbin, int *new_etabin);
    void SetCoreScale(float);
    void SetTailScale(float);
    void SetMeanShift(float);
    void TransformFit(TH1D *fit, TH1D *core, TH1D *tail);
    void UseRawHistograms(bool use=true);
    void SetBinWidth(float width);
    void Draw(int,int, TH1D* usethis=NULL, string tag="test");

    static const int n_eta_bins = 12;
    static const int n_pt_bins  = 23;
    static constexpr float eta_bins[n_eta_bins+1] = {0,0.3,0.5,0.8,1.1,1.4,1.7,2.3,2.8,3.2,4.1,5.0,-1};
    static constexpr float pt_bins[n_pt_bins+1]   = {0,20,30,50,80,120,170,230,300,380,470,570,680,800,1000,
                                                     1300,1700,2200,2800,3500,4300,5200,6500,-1};

 private:
    static void DeleteFitVec(vector< vector<TH1D*>* > *fits);

    vector< vector<TH1D*>* > *fits_b;
    vector< vector<TH1D*>* > *fits_b_core;
    vector< vector<TH1D*>* > *fits_b_tail;
    vector< vector<TH1D*>* > *fits_nonb;
    vector< vector<TH1D*>* > *fits_nonb_core;
    vector< vector<TH1D*>* > *fits_nonb_tail;
    bool useFits = true;
    float BINWIDTH = 0.02;
    float coreScale = 1.0;
    float tailScale = 1.0;
    float meanShift = 0.0;

};

#endif
