void skim_custom(const char* indir, const char* outdir, const char* sample){

    // long long max_tree_size = 5000000000LL;
    // TTree::SetMaxTreeSize(max_tree_size);
    
    TChain *c = new TChain("mt2");
    c->Add(Form("%s/%s*.root",indir,sample));
    
    TFile fout(Form("%s/%s.root",outdir,sample), "RECREATE");

    // TTree *t = c->CopyTree("mt2>50 && ht>250 && nlep==0 && diffMetMht/met_pt<0.5");
    // TTree *t = c->CopyTree("mt2>50 && ht>250 && nlep==0");
    TTree *t = c->CopyTree("isGolden && nlep==0 && met_pt>250 && nJet30==2 && jet1_pt>250 && deltaPhiMin<0.3 && diffMetMht/met_pt<0.5");

    t->Write();
    fout.Close();

}
