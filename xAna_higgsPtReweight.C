//root -q -b -l juwu.C++\(\"inputFile\"\,\"outputFile\"\)


#include <vector>
#include <iostream>
#include <algorithm>
#include <TString.h>
#include <map>
#include <TH1D.h>
#include <TFile.h>
#include "untuplizer.h"
#include "standalone_LumiReWeighting.cc"


using namespace std;

double getWeight( TH1F *h_ratio,double higgsPt ) {
    int binWeight = h_ratio->GetXaxis()->FindBin( higgsPt );
    return h_ratio->GetBinContent( binWeight );
}
/*double getWeight(TGraphAsymmErrors *h_ratio, double npv) {    
    return h_ratio->Eval(npv);
}*/
void xAna_higgsPtReweight(string inputFile="ZprimeToA0hToA0chichihbb_2HDM_MZp-1000_MA0-300_13TeV-madgraph.root") {

    //standalone_LumiReWeighting LumiWeights_central(0);
    //get TTree from file ...
    TreeReader data(inputFile.data());
    gStyle->SetOptStat(0);
    gStyle->SetLineWidth(2);
    TCanvas *c1 = new TCanvas("c1","c1",800,600); 
    TLegend *leg = new TLegend(0.6,0.7,0.9,0.9);
    TFile *f_mzp1000_gen = TFile::Open("output/gentuple_2HDM_MZp1000_MA0300_MDM100.root");
    TFile *f_mzp1200_gen = TFile::Open("output/gentuple_2HDM_MZp1200_MA0300_MDM100.root");
    TFile *f_mzp1200_full = TFile::Open("output/ZprimeToA0hToA0chichihbb_2HDM_MZp-1200_MA0-300_13TeV-madgraph.root");
    TFile *f_mzp1000_full = TFile::Open("output/ZprimeToA0hToA0chichihbb_2HDM_MZp-1000_MA0-300_13TeV-madgraph.root");
                
    TH1F *h_mzp1000ma0300_gen_1 = (TH1F*)f_mzp1000_gen->Get("normTo1/Higgs_pt");
    TH1F *h_mzp1200ma0300_gen_1 = (TH1F*)f_mzp1200_gen->Get("normTo1/Higgs_pt"); 
    TH1F *h_mzp1000ma0300_gen = (TH1F*)f_mzp1000_gen->Get("Higgs_pt");
    TH1F *h_mzp1200ma0300_gen = (TH1F*)f_mzp1200_gen->Get("Higgs_pt"); 
    TH1F *h_mzp1200ma0300_full = (TH1F*)f_mzp1200_full->Get("Higgs_pt"); 
    TH1F *h_mzp1000ma0300_full = (TH1F*)f_mzp1000_full->Get("Higgs_pt"); 
    TH1F *h_mzp1200ma0300_reweight = new TH1F("higgsPtRW", "higgsPtRW", 250,0,2500);
    TH1F *h_mzp1000ma0300_refill = new TH1F("higgsPtRF", "higgsPtRF", 250,0,2500);

    Double_t ratioBin[51];
    for (int i=0;i<51;i++)  ratioBin[i] = i*20;
    TH1F *ratio = (TH1F*)h_mzp1200ma0300_gen->Clone("ratio");
    ratio->Divide(h_mzp1000ma0300_gen);
    //TGraphAsymmErrors *ratio_rebin = new TGraphAsymmErrors(h_mzp1200ma0300_gen,h_mzp1000ma0300_gen);
    TH1F *ratio1 = (TH1F*)h_mzp1200ma0300_gen_1->Clone("ratio1");
    ratio1->Divide(h_mzp1000ma0300_gen_1);
    //h_mzp1000ma0300_gen->Draw("hist");
    //h_mzp1200ma0300_gen->Draw("histsame");
    /*ratio1->Draw();
    c1->SaveAs("weight_norm1.png");
    ratio->Draw();
    c1->SaveAs("weight_event.png");
*/
    TH1F *ratio_rebin = (TH1F*)ratio->Rebin(50,"ratio_rebin",ratioBin);
    //ratio_rebin->Draw();
  
    //Event loop
    for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){
        if (jEntry > 70000) break;
        if (jEntry % 50000 == 0) fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());
        data.GetEntry(jEntry); 
        // find Higgs pt
        TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");
        int nGenPar=data.GetInt("nGenPar");
        int *genParSt=data.GetPtrInt("genParSt"); 
        int *genParId=data.GetPtrInt("genParId"); 
        TLorentzVector* thisJet;
        Int_t DMId = 36;
        for(int it=0; it< nGenPar; it++){
	    if(genParId[it]==25&&genParSt[it]==22 ){
	        thisJet=(TLorentzVector*)genParP4->At(it);
                double higgsPt = thisJet->Pt();
	        double eventWeight = 1;
                eventWeight = getWeight(ratio_rebin, higgsPt);
                h_mzp1200ma0300_reweight->Fill(higgsPt,eventWeight);
                h_mzp1000ma0300_refill->Fill(higgsPt);
	    }
        }

    } // end of loop over entries
   /* 
    h_mzp1000ma0300_refill->Draw("hist");
    c1->SaveAs("refill.png");
    h_mzp1000ma0300_full->SetLineColor(6);
    h_mzp1000ma0300_full->Draw("histsame");
    c1->SaveAs("DMD_higgsPt.png");
  */
    Double_t ptbins[] = {0,50,100,150,200,250,300,400,500,600,800,1000};
    TH1F *h_mzp1200ma0300_reweight_rebin = (TH1F*) h_mzp1200ma0300_reweight->Rebin(11,"h_mzp1200ma0300_reweight_rebin",ptbins);
    TH1F *h_mzp1200ma0300_full_rebin = (TH1F*) h_mzp1200ma0300_full->Rebin(11,"h_mzp1200ma0300_full_rebin",ptbins);
    h_mzp1200ma0300_reweight_rebin->SetTitle("higgs pt reweight, event by event");
    h_mzp1200ma0300_reweight_rebin->SetXTitle("p_{T} (GeV)");
    h_mzp1200ma0300_reweight_rebin->SetYTitle("Event");
    h_mzp1200ma0300_reweight_rebin->Draw("hist");
    h_mzp1200ma0300_full_rebin->SetLineColor(6);
    h_mzp1200ma0300_full_rebin->Draw("histsame");
    leg->AddEntry(h_mzp1200ma0300_reweight_rebin,"weight MZp = 1200 GeV");
    leg->AddEntry(h_mzp1200ma0300_full_rebin,"Full MZp = 1200 GeV");
    leg->Draw();

    //save output
    //TString endfix=gSystem->GetFromPipe(Form("file=%s; echo \"${file##*/}\"",inputFile.data()));
    //TString outputFile = "muHisto_" + endfix;
    /*
    TFile* outFile = new TFile("output/eventWeight.root","recreate");
    h_hpt_mzp1000->Write();
    h_hpt_mzp1200->Write();
    outFile->Close();
*/
}
