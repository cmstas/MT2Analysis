
// #include "JetResponseTemplates/JetResponseTemplates/interface/JRTreader.h"
#include "JRTreader.h"
#include <vector>

#include "TFile.h"
#include "TF1.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TCanvas.h"

// stupid c++11
constexpr float JRTreader::pt_bins[];
constexpr float JRTreader::eta_bins[];
constexpr int JRTreader::eta_bin_map[];

JRTreader::JRTreader(const char *fname){
    fits_b = NULL;
    fits_nonb = NULL;
    if(fname != NULL){
        Init(fname);
    }
}

void JRTreader::DeleteFitVec(vector< vector<TH1D*>* > *fits)
{
     if(fits != NULL){
        for(int i=0; i<n_pt_bins; i++){
            if(fits->at(i) != NULL){
                for(int j=0; j<n_eta_bins; j++){
                    if(fits->at(i)->at(j) != NULL)
                        delete fits->at(i)->at(j);
                }
                delete fits->at(i);
            }
        }
        delete fits;
    }
}

JRTreader::~JRTreader(){
    DeleteFitVec(fits_b);
    DeleteFitVec(fits_nonb);
}

int JRTreader::Init(const char *fname, bool correctDataResponse){
    TFile *f = TFile::Open(fname);
    if(f->IsZombie())
        return 0;

    fits_b = new vector< vector<TH1D*>* >;
    fits_nonb = new vector< vector<TH1D*>* >;

    TString prefix = "fit";
    if(!useFits)
        prefix = "JRT";

    TH1D *core, *tail, *fit;

    for(int ipt=0; ipt<n_pt_bins; ipt++){
        fits_b->push_back(new vector<TH1D*>);
        fits_nonb->push_back(new vector<TH1D*>);
        for(int ieta=0; ieta<n_eta_bins; ieta++){
            TString fitname;
            int JRTieta = eta_bin_map[ieta];
            fitname = Form("pt%d/pt%d_eta%d/%s_pt%d_eta%d_bjet",ipt,ipt,JRTieta,prefix.Data(),ipt,JRTieta);
            fit = (TH1D*)f->Get(fitname);
            if(fit != NULL) {
                fits_b->at(ipt)->push_back((TH1D*)fit->Clone(Form("%s_pt%d_eta%d_bjet",prefix.Data(),ipt,ieta)));
                fits_b->at(ipt)->at(ieta)->SetDirectory(0);
                if(useFits){
                    core = (TH1D*)f->Get(fitname+"_core");
                    tail = (TH1D*)f->Get(fitname+"_tail");
                    float corr = correctDataResponse ? GetJERCorrection(eta_bins[ieta]+0.001) : 1.0;
                    coreScale *= corr;
                    TransformFit(fits_b->at(ipt)->at(ieta), core, tail);
                    coreScale /= corr;
                }
            }else{
                fits_b->at(ipt)->push_back(NULL);
            }

            fitname = Form("pt%d/pt%d_eta%d/%s_pt%d_eta%d_nonbjet",ipt,ipt,JRTieta,prefix.Data(),ipt,JRTieta);
            fit = (TH1D*)f->Get(fitname);
            if(fit != NULL){
                fits_nonb->at(ipt)->push_back((TH1D*)fit->Clone(Form("%s_pt%d_eta%d_nonbjet",prefix.Data(),ipt,ieta)));
                fits_nonb->at(ipt)->at(ieta)->SetDirectory(0);
                // TH1D *fit_before = (TH1D*)fits_nonb->at(ipt)->at(ieta)->Clone("fitbefore");
                if(useFits){
                    core = (TH1D*)f->Get(fitname+"_core");
                    tail = (TH1D*)f->Get(fitname+"_tail");
                    float corr = correctDataResponse ? GetJERCorrection(eta_bins[ieta]+0.001) : 1.0;
                    coreScale *= corr;
                    TransformFit(fits_nonb->at(ipt)->at(ieta), core, tail);
                    coreScale /= corr;
                }
                // if(ipt<=19)
                //     DrawComp(fit_before, fits_nonb->at(ipt)->at(ieta), 
                //              Form("JRTplots/fits_comp/nonbjets/pt%02d_eta%02d_nonbjets",ipt,ieta));
                // delete fit_before;
            }else{
                fits_nonb->at(ipt)->push_back(NULL);
            }

        }
    }

    f->Close();
    delete f;
        
    return 1;
}

float JRTreader::GetRandomResponse(float pt, float eta, bool isBjet){
    
    int ptbin_orig = JRTreader::GetPtBin(pt);
    int etabin_orig = JRTreader::GetEtaBin(fabs(eta));

    int ptbin, etabin;
    JRTreader::GetModifiedBins(ptbin_orig, etabin_orig, isBjet, &ptbin, &etabin);

    TH1D *fit;
    if(isBjet){
        fit  = fits_b->at(ptbin)->at(etabin);
    }else{
        fit  = fits_nonb->at(ptbin)->at(etabin);
    }

    if(fit==NULL){
        cout << "WARNING: could not find fit histogram for bins pt=" << ptbin << " eta=" << etabin << endl;
        return 1.;
    }

    // TString ts = Form("JRTplots/fits/nonbjets_dataJER/pt%02d_eta%02d_nonbjets",ptbin_orig,etabin_orig);
    // string name =string(ts.Data());
    // Draw(0,0,fit,name);
    
    float response = fit->GetRandom();
    
    return response;
}

float JRTreader::GetValue(float pt, float eta, bool isBjet, float smearfact){
    
    int ptbin_orig = JRTreader::GetPtBin(pt);
    int etabin_orig = JRTreader::GetEtaBin(fabs(eta));

    int ptbin, etabin;
    JRTreader::GetModifiedBins(ptbin_orig, etabin_orig, isBjet, &ptbin, &etabin);

    TH1D *fit;
    if(isBjet){
        fit = fits_b->at(ptbin)->at(etabin);
    }else{
        fit = fits_nonb->at(ptbin)->at(etabin);
    }

    if(fit==NULL){
        cout << "WARNING: could not find fit histogram for bins pt=" << ptbin << " eta=" << etabin << endl;
        return 1.;
    }

    float response = fit->GetBinContent(smearfact);

    return response;
}

void JRTreader::UseRawHistograms(bool use){
    useFits = !use;
}

float JRTreader::GetJERCorrection(float eta){
    // jet-energy resolution is larger in data than MC by the following 
    // eta-dependent factors.
    // We broaden the JRT around 1 by the same factor for data
    // numbers from: https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution

    if      (eta >= 0.0 && eta < 0.5) return 1.109;
    else if (eta >= 0.5 && eta < 0.8) return 1.138;
    else if (eta >= 0.8 && eta < 1.1) return 1.114;
    else if (eta >= 1.1 && eta < 1.3) return 1.123;
    else if (eta >= 1.3 && eta < 1.7) return 1.084;
    else if (eta >= 1.7 && eta < 1.9) return 1.082;
    else if (eta >= 1.9 && eta < 2.1) return 1.140;
    else if (eta >= 2.1 && eta < 2.3) return 1.067;
    else if (eta >= 2.3 && eta < 2.5) return 1.177;
    else if (eta >= 2.5 && eta < 2.8) return 1.364;
    else if (eta >= 2.8 && eta < 3.0) return 1.857;
    // else if (eta >= 2.8 && eta < 3.0) return 1.346; //try average of neighbors to see if 1.9 value is affecting things
    else if (eta >= 3.0 && eta < 3.2) return 1.328;
    else                              return 1.160;

}

// certain high-eta bins have no statistics and hence no template. So decrease eta bin
// until we get to a valid template. These are just hard-coded by looking at templates,
// will need to change if templates are updated.
void JRTreader::GetModifiedBins(int ptbin, int etabin, bool isBjet, int *new_ptbin, int *new_etabin){

    if(isBjet){
        if(ptbin > 19) ptbin = 19;

        if(ptbin>=0 && ptbin<=1)
            ;
        if(ptbin>=2 && ptbin<=4)
            if(etabin >= 16) etabin = 15;
        if(ptbin>=5 && ptbin<=7)
            if(etabin >= 15) etabin = 13;
        if(ptbin>=8 && ptbin<=9)
            if(etabin >= 14) etabin = 13;
        if(ptbin>=10 && ptbin<=12)
            if(etabin >= 12) etabin = 11;
        if(ptbin>=13 && ptbin<=15)
            if(etabin >= 10) etabin = 9;
        if(ptbin>=16 && ptbin<=17)
            if(etabin >= 6) etabin = 5;
        if(ptbin>=18 && ptbin<=19)
            if(etabin >=4) etabin = 3;

    }
    if(!isBjet){
        if(ptbin > 19) ptbin = 19;

        if(ptbin>=0 && ptbin<=3)
            ;
        if(ptbin>=4 && ptbin<=5)
            if(etabin >= 16) etabin = 15;
        if(ptbin>=6 && ptbin<=8)
            if(etabin >= 15) etabin = 14;
        if(ptbin>=9 && ptbin<=10)
            if(etabin >=14) etabin = 13;
        if(ptbin>=11 && ptbin<=13)
            if(etabin >= 12) etabin = 11;
        if(ptbin>=14 && ptbin<=15)
            if(etabin >= 10) etabin = 8;
        if(ptbin>=16 && ptbin<=17)
            if(etabin >= 7) etabin = 6;
        if(ptbin>=18 && ptbin<=18)
            if(etabin >=6) etabin = 5;
        if(ptbin>=19 && ptbin<=19)
            if(etabin >=4) etabin = 3;

    }

    *new_ptbin = ptbin;
    *new_etabin = etabin;
    
}

int JRTreader::GetPtBin(float pt){

    for(int i=0; i<JRTreader::n_pt_bins-1; i++){
        if(pt >= JRTreader::pt_bins[i] && pt < JRTreader::pt_bins[i+1])
            return i;
    }

    return JRTreader::n_pt_bins-1;
}

int JRTreader::GetEtaBin(float eta){

    for(int i=0; i<JRTreader::n_eta_bins-1; i++){
        if(eta >= JRTreader::eta_bins[i] && eta < JRTreader::eta_bins[i+1])
            return i;
    }

    return JRTreader::n_eta_bins-1;
}

void JRTreader::SetBinWidth(float width){
    BINWIDTH = width;
}

void JRTreader::SetCoreScale(float scale){
    coreScale = scale;
}

void JRTreader::SetTailScale(float scale){
    tailScale = scale;
}

void JRTreader::SetMeanShift(float shift){
    meanShift = shift;
}

void JRTreader::TransformFit(TH1D *fit, TH1D *core, TH1D *tail){


    for(int ibin=1; ibin <= fit->GetNbinsX(); ibin++){

        float x = fit->GetBinCenter(ibin);
        x -= meanShift;
        float corex = (x-1)/coreScale + 1;
        
        float val = core->GetBinContent(core->FindBin(corex))/coreScale + 
            tailScale*tail->GetBinContent(tail->FindBin(x));

        if(val<0)
            val = 0;

        fit->SetBinContent(ibin, val);

    }

    fit->Scale(1.0/(fit->Integral()*fit->GetBinWidth(1)));

}

void JRTreader::Draw(int ipt, int ieta, TH1D* usethis, string tag)
{
    //for debugging purposes

    TCanvas *c = new TCanvas();

    TH1D* h;
    if(usethis != NULL)
        h = usethis;
    else
        h = fits_nonb->at(ipt)->at(ieta);

    h->Draw("L");

    c->SaveAs(("/home/users/bemarsh/public_html/"+tag+".png").c_str());
    c->SaveAs(("/home/users/bemarsh/public_html/"+tag+".pdf").c_str());

}

void JRTreader::DrawComp(TH1D* h1, TH1D *h2, TString tag)
{
    //for debugging purposes

    TCanvas *c = new TCanvas();

    h1->SetLineColor(kRed);
    h2->SetLineColor(kBlue);

    h1->Draw("L");
    h2->Draw("L SAME");

    TLatex text;
    text.SetNDC(1);
    text.SetTextColor(kRed);
    text.DrawLatex(0.65,0.6,Form("%.2f",h1->Integral(0,-1)*h1->GetBinWidth(1)));
    text.SetTextColor(kBlue);
    text.DrawLatex(0.65,0.55,Form("%.2f",h2->Integral(0,-1)*h2->GetBinWidth(1)));

    c->SaveAs("/home/users/bemarsh/public_html/"+tag+".png");
    c->SaveAs("/home/users/bemarsh/public_html/"+tag+".pdf");

}
