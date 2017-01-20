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
void reweight() {
    
    gStyle->SetOptStat(0);
    gStyle->SetLineWidth(2);
    TCanvas *c1 = new TCanvas("c1","c1",800,600); 
    TLegend *leg = new TLegend(0.6,0.7,0.9,0.9);
    TFile *f_mzp1200 = TFile::Open("gentuple_2HDM_MZp1000_MA0300_MDM100.root");
    TFile *f_mzp1000 = TFile::Open("gentuple_2HDM_MZp1200_MA0300_MDM100.root");
    
    
    TTree *f_mzp1000ma0300 = (TTree*) f_mzp1000->Get("tree/treeMaker");
    TTree *f_mzp1200ma0300 = (TTree*) f_mzp1200->Get("tree/treeMaker");
    
    TH1F *h_mzp1000ma0300_gen = new TH1F("h_mzp1000ma0300_gen","",30,0,1500);
    TH1F *h_mzp1200ma0300_gen = new TH1F("h_mzp1200ma0300_gen","",30,0,1500); 
    //TH1F *h_mzp1000ma0300_gen = new TH1F();
    //TH1F *h_mzp1200ma0300_gen = new TH1F();
    TH1F *ratio = new TH1F("ratio","ratio",30,0,1500);
    //test->Print();
    f_mzp1000ma0300->Draw("genMET_true>>h_mzp1000ma0300_gen");//,"","histe");
    f_mzp1200ma0300->Draw("genMET_true>>h_mzp1200ma0300_gen");
    //h_mzp1000ma0300_gen->Smooth(3);
    //h_mzp1200ma0300_gen->Smooth(3);
    h_mzp1000ma0300_gen->SetLineColor(3);
    h_mzp1200ma0300_gen->SetLineColor(4);
    h_mzp1000ma0300_gen->SetLineWidth(2);
    h_mzp1200ma0300_gen->SetLineWidth(2);
    h_mzp1000ma0300_gen->SetXTitle("p_{T} (GeV)");
    h_mzp1000ma0300_gen->SetYTitle("Events");
    h_mzp1000ma0300_gen->Draw();
    h_mzp1200ma0300_gen->Draw("same");
    leg->AddEntry(h_mzp1000ma0300_gen,"MA0 = 300 GeV, MZp = 1000 GeV");
    leg->AddEntry(h_mzp1200ma0300_gen,"MA0 = 300 GeV, MZp = 1200 GeV");
    leg->Draw();
    c1->SetLogy();
    c1->Print("ratio.pdf");
    /*
    // smooth
    h_mzp1000ma0300_gen->Smooth();
    h_mzp1200ma0300_gen->Smooth();
    h_mzp1000ma0300_gen->SetTitle("smooth"); 
    h_mzp1000ma0300_gen->Draw();
    h_mzp1200ma0300_gen->Draw("same");
    
    c1->Print("ratio.pdf");
    */
    // ratio
    /*
    c1->SetLogy(0);
    TH1F *his2 = (TH1F*)h_mzp1200ma0300_gen->Clone("his2");
    his2->Divide(h_mzp1000ma0300_gen);
    his2->SetLineColor(4);
    his2->SetLineWidth(2);
    his2->SetXTitle("P_{T} (GeV)");
    his2->SetYTitle("Ratio (MZp1200/MZp1000)");
    his2->Draw("hist");
    c1->Print("ratio.pdf");
    
    // 
    c1->SetLogy();
    TH1F *his_1050 = new TH1F("his_1050","MA0300MZp1050",30,0,1500);
    TH1F *his_1100 = new TH1F("his_1100","MA0300MZp1100",30,0,1500);
    TH1F *his_1150 = new TH1F("his_1150","MA0300MZp1150",30,0,1500);
    his_1050->Add(h_mzp1000ma0300_gen,h_mzp1200ma0300_gen,0.75,0.25);
    his_1100->Add(h_mzp1000ma0300_gen,h_mzp1200ma0300_gen,0.5,0.5);
    his_1150->Add(h_mzp1000ma0300_gen,h_mzp1200ma0300_gen,0.25,0.75);
    
    h_mzp1000ma0300_gen->SetLineColor(51);
    his_1050->SetLineColor(61);
    his_1100->SetLineColor(71);
    his_1150->SetLineColor(81);
    h_mzp1200ma0300_gen->SetLineColor(91);
    
    h_mzp1000ma0300_gen->SetLineWidth(2);
    his_1050->SetLineWidth(2);
    his_1100->SetLineWidth(2);
    his_1150->SetLineWidth(2);
    h_mzp1200ma0300_gen->SetLineWidth(2);
    
    h_mzp1000ma0300_gen->Draw();
    his_1050->Draw("same");
    his_1100->Draw("same");
    his_1150->Draw("same");
    h_mzp1200ma0300_gen->Draw("same");

    leg->Clear();
    leg->AddEntry(h_mzp1000ma0300_gen,"MA0 = 300 GeV, MZp = 1000 GeV");
    leg->AddEntry(his_1050,"MA0 = 300 GeV, MZp = 1050 GeV");
    leg->AddEntry(his_1100,"MA0 = 300 GeV, MZp = 1100 GeV");
    leg->AddEntry(his_1150,"MA0 = 300 GeV, MZp = 1150 GeV");
    leg->AddEntry(h_mzp1200ma0300_gen,"MA0 = 300 GeV, MZp = 1200 GeV");
    leg->Draw();
    c1->Update();
    c1->Print("ratio.pdf)");
    */
    /*
    TFile *htest = new TFile("test.root","recreate");
        test->Write();
        h_mzp1000ma0300_gen->Write();
    htest->Close();
*/

}
