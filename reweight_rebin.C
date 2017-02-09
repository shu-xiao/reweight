#include <TCanvas.h>
#include <TFile.h>
#include "iostream"
#include <TTree.h>
#include <TH1F.h>
#include <TH1.h>
#include <TDirectory.h>
#include "TLegend.h"
#include "TAttLine.h"

using namespace std;
void reweight_rebin() {
    
    gStyle->SetOptStat(0);
    gStyle->SetLineWidth(2);
    TCanvas *c1 = new TCanvas("c1","c1",800,600); 
    TLegend *leg = new TLegend(0.6,0.7,0.9,0.9);
    TFile *f_mzp1000_gen = TFile::Open("output/gentuple_2HDM_MZp1000_MA0300_MDM100.root");
    TFile *f_mzp1200_gen = TFile::Open("output/gentuple_2HDM_MZp1200_MA0300_MDM100.root");
    TH1F *h_mzp1000ma0300_gen = (TH1F*)f_mzp1000_gen->Get("Higgs_pt");
    TH1F *h_mzp1200ma0300_gen = (TH1F*)f_mzp1200_gen->Get("Higgs_pt"); 
    
    // ratio with smooth
    /*
    h_mzp1000ma0300_gen->Smooth();
    h_mzp1200ma0300_gen->Smooth();
    h_mzp1000ma0300_gen->SetTitle("smooth"); 
    h_mzp1000ma0300_gen->Draw();
    h_mzp1200ma0300_gen->Draw("same");
    
    c1->Print("ratio.pdf");
    */

    // ratio
    TH1F *weight = (TH1F*)h_mzp1200ma0300_gen->Clone("weight");
    weight->Divide(h_mzp1000ma0300_gen);
    
    //apply on full simulation
    //c1->SetLogy();
    TFile *f_mzp1000_full = TFile::Open("output/ZprimeToA0hToA0chichihbb_2HDM_MZp-1000_MA0-300_13TeV-madgraph.root");
    TFile *f_mzp1200_full = TFile::Open("output/ZprimeToA0hToA0chichihbb_2HDM_MZp-1200_MA0-300_13TeV-madgraph.root");
    TH1F *h_mzp1000ma0300_full = (TH1F*)f_mzp1000_full->Get("Higgs_pt");
    TH1F *h_mzp1200ma0300_full = (TH1F*)f_mzp1200_full->Get("Higgs_pt");
    
    const Double_t ptbins[] = {0,50,100,150,200,250,300,400,500,600,800,1000};
    const Int_t nbins1=sizeof(ptbins)/sizeof(ptbins[0])-1; 
    TH1F *h_mzp1200ma0300_weight = (TH1F*)h_mzp1000ma0300_full->Clone(); 
    h_mzp1200ma0300_weight->Multiply(weight);
    //h_mzp1200ma0300_weight->Rebin(nbins1,ptbins);

    TH1F *h_mzp1200ma0300_weight_rebin = (TH1F*)(h_mzp1200ma0300_weight->Rebin(11,"h_mzp1200ma0300_weight_rebin",ptbins));
    TH1F *h_mzp1200ma0300_full_rebin = (TH1F*)(h_mzp1200ma0300_full->Rebin(11,"h_mzp1200ma0300_full_rebin",ptbins));
     
    h_mzp1200ma0300_weight_rebin->SetXTitle("p_{T} (GeV)");
    h_mzp1200ma0300_weight_rebin->SetYTitle("Event");
    h_mzp1200ma0300_weight_rebin->SetLineWidth(2);
    h_mzp1200ma0300_full_rebin->SetLineWidth(2);
    h_mzp1200ma0300_weight_rebin->SetLineColor(4);
    h_mzp1200ma0300_full_rebin->SetLineColor(7);
    
    h_mzp1200ma0300_weight_rebin->SetTitle("Higgs p_{T} reweight");
    h_mzp1200ma0300_weight_rebin->GetYaxis()->SetTitleOffset(1.5);
    h_mzp1200ma0300_weight_rebin->Draw();
    h_mzp1200ma0300_full_rebin->Draw("same");
    
    leg->Clear(); 
    leg->AddEntry(h_mzp1200ma0300_weight_rebin,"Reweight MZp = 1200 GeV");
    leg->AddEntry(h_mzp1200ma0300_full_rebin,"Full sim MZp = 1200 GeV");
    leg->Draw();
    c1->SaveAs("output/ratio_rebin.png");
    c1->Print("output/ratio_rebin.pdf");

}
