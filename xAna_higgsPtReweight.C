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
Double_t getYMax(TH1F *h1, TH1F *h2) {
    Double_t h1Max = h1->GetMaximum();
    Double_t h2Max = h2->GetMaximum();
    if (h2Max>=h1Max) return h2Max;
    else return h1Max;
}
TH1F *getRatio(TH1F *h_num,TH1F *h_den,Int_t binNum, Double_t h_bins[]) {
    if (h_num->Integral()>1.001 or h_num->Integral()<(1-0.001)) h_num->Scale(1./(h_num->Integral()));
    if (h_den->Integral()>1.001 or h_den->Integral()<(1-0.001)) h_den->Scale(1./(h_den->Integral()));
    TH1F *h_num_rebin = (TH1F*)h_num->Rebin(binNum,"h_num_rebin",h_bins);
    TH1F *h_den_rebin = (TH1F*)h_den->Rebin(binNum,"h_den_rebin",h_bins);
    h_num_rebin->Divide(h_den_rebin);
    return h_num_rebin;
}

void xAna_higgsPtReweight(string inputFile="ZprimeToA0hToA0chichihbb_2HDM_MZp-1000_MA0-300_13TeV-madgraph.root") {

    //get TTree from file ...
    TreeReader data(inputFile.data());
    gStyle->SetOptStat(0);
    gStyle->SetLineWidth(2);
    TCanvas *c1 = new TCanvas("c1","c1",800,600); 
    TLegend *leg = new TLegend(0.6,0.7,0.9,0.9);
    TFile *file[4];
    file[0] = TFile::Open("output/gentuple_2HDM_MZp1000_MA0300_MDM100.root");
    file[1] = TFile::Open("output/gentuple_2HDM_MZp1200_MA0300_MDM100.root");
    file[2] = TFile::Open("output/ZprimeToA0hToA0chichihbb_2HDM_MZp-1000_MA0-300_13TeV-madgraph.root");
    file[3] = TFile::Open("output/ZprimeToA0hToA0chichihbb_2HDM_MZp-1200_MA0-300_13TeV-madgraph.root");
    string hName[4] = {"FATjetPt","Higgs_pt","rawMET","corrMET"};
    TH1F *hist[4][8];
    TH1F *h_reweight[8], *ratio[4];
    for (int f=0;f<4;f++) {
        for (int h=0;h<4;h++) {
            string hPath = "normTo1/" + hName[h];
            hist[f][h] = (TH1F*)file[f]->Get(hPath.data());
        }
    }
    for (int i=0;i<8;i++) {
        string hTitle;
        if (i<4) hTitle = hName[i%4] + "RW";
        else hTitle = hName[i%4] + "RWRebin";
        h_reweight[i] = new TH1F(hTitle.data(),hTitle.data(),250,0,2500);
    }
    Double_t ratioBin[51];
    for (int i=0;i<51;i++)  ratioBin[i] = i*20;
    //ratio[1] = getRatio(hist[1][1],hist[0][1],50,ratioBin);

    for (int i=0;i<4;i++)  ratio[i] = getRatio(hist[1][i],hist[0][i],50,ratioBin);
    /*
    // test root bug
    ratio[0]->Draw();
    //c1->Print("test.pdf");
    c1->SaveAs("test0.png");
    c1->Clear();
    c1->cd();
    ratio[1]->Draw();
    //c1->Print("test1.pdf");
    c1->SaveAs("test1.png");
    */
    //Event loop
    for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){
        if (jEntry > 70000) break;
        if (jEntry % 35000 == 0) fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());
        data.GetEntry(jEntry); 
        // find Higgs pt
        TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");
	TClonesArray* FATjetP4;
	int nGenPar=data.GetInt("nGenPar");
	int nFatJet;
	int *genParSt=data.GetPtrInt("genParSt");
	int *genParId=data.GetPtrInt("genParId");
	Float_t rawMET,corrMET;
        rawMET = data.GetFloat("pfMetRawPt");
	corrMET = data.GetFloat("pfMetCorrPt");
        FATjetP4 = (TClonesArray*) data.GetPtrTObject("FATjetP4");
	nFatJet=data.GetInt("FATnJet");
	TLorentzVector* thisJet, *fatJet;
	int DMId=36;
        if (jEntry>35000) {
	    for(int i=0;i<nFatJet;i++){
		fatJet=(TLorentzVector*)FATjetP4->At(i);
                Double_t fatJetRatio = getWeight(ratio[0],fatJet->Pt());
	        h_reweight[0]->Fill(fatJet->Pt(),fatJetRatio);
            }
        }
        for(int it=0, n=0; it< nGenPar; it++){
	    if(genParId[it]==25&&genParSt[it]==22 ){
	        thisJet=(TLorentzVector*)genParP4->At(it);
                double higgsPt = thisJet->Pt();
                Double_t eventRatio = getWeight(ratio[1], higgsPt);
                h_reweight[1]->Fill(higgsPt,eventRatio);
                // only fill MET once
                if (n==0) { 
                    Double_t rawMETRatio = getWeight(ratio[2], rawMET);
                    Double_t corrMETRatio = getWeight(ratio[3], corrMET);
                    h_reweight[2]->Fill(rawMET,rawMETRatio);
                    h_reweight[3]->Fill(corrMET,corrMETRatio);
                    n++;
                }
            }
        }
    } // end of loop over entries
    Double_t ptbins[] = {0,50,100,150,200,250,300,400,500,600,800,1000};
    // histogram setting
    for (int i=0;i<4;i++) {    
        leg->Clear();
        h_reweight[i+4] = (TH1F*) h_reweight[i]->Rebin(11,Form("h_reweight[%d]",i+4),ptbins);
        hist[3][i+4] = (TH1F*) hist[3][i]->Rebin(11,Form("hist[3][%d]",i+4),ptbins);
        h_reweight[i+4]->Scale(1./(h_reweight[i]->Integral()));
        if (i==0 or i==1) h_reweight[i+4]->SetXTitle("p_{T} (GeV)");
        if (i==2 or i==3) h_reweight[i+4]->SetXTitle("E_{T}^{miss} (GeV)");
        h_reweight[i+4]->SetMaximum(getYMax(h_reweight[i+4],hist[3][i+4])*1.1);
        h_reweight[i+4]->SetYTitle("normalize to 1");
        h_reweight[i+4]->GetYaxis()->SetTitleOffset(1.3);
        h_reweight[i+4]->SetLineColor(1);
        h_reweight[i+4]->SetLineWidth(2);
        h_reweight[i+4]->Draw("hist");
        hist[3][i+4]->SetLineColor(6);
        hist[3][i+4]->SetLineWidth(2);
        hist[3][i+4]->Draw("histsame");
        leg->AddEntry(h_reweight[i+4],"weight MZp = 1200 GeV");
        leg->AddEntry(hist[3][i],"Full MZp = 1200 GeV");
        leg->Draw();
        c1->SaveAs(Form("overlap%d.png",i));
    }
    //c1->SaveAs("output/event_higgsPtReweight.png");
    //c1->Print("output/event_higgsPtReweight.pdf");
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
