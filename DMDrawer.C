
#include "untuplizer.h"
#include "TH1D.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TFile.h"
#include "TDirectory.h"
#include <iostream>
#include <TSystem.h>
#include <TString.h>
#include <string>
using namespace std;

int DMDrawer(string input,int option=0){
  TreeReader data(input.data());
  string fname = input;
  fname.erase( fname.length()-5,5);
  string mode[] = {"sim","full"};
  //cout << "file name is " << fname << endl;
  cout << "mode = " << mode[option] << endl;
  TString Mass;
    // option=0:  sim-level simulation
  if (option==0) Mass=gSystem->GetFromPipe(Form("file=%s; test=${file##*MZp}; test2=${test%%_MA0*}; echo \"$test2\"",input.data()));
    // option=1:  full simulation
  else if (option==1)  Mass=gSystem->GetFromPipe(Form("file=%s; test=${file##*MZp-}; test2=${test%%_MA0*}; echo \"$test2\"",input.data()));
  else return 1;    
  //cout << "Mass = " << Mass.Data() << endl;
  int mass = atoi(Mass.Data());
  cout << "mass = " << mass << endl;
  TH1D* th1[20];
  TH1D* th1_norm[20];
  float xmin=0;
  float xmax= mass>2000? 10000:2500;
  int nbins = xmax/10.;
  TCanvas *c1 = new TCanvas();
  th1[0]=new TH1D("FATjetPt","FATjetPt",nbins,xmin,xmax);
  th1[1]=new TH1D("Higgs_pt","Higgs_pt",nbins,xmin,xmax);
  th1[2]=new TH1D("rawMET","rawMET",nbins,xmin,xmax);
  th1[3]=new TH1D("corrMET","corrMET",nbins,xmin,xmax);
	for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){
	        if (jEntry > 70000) break;
                data.GetEntry(jEntry);
                //float genMET_true=data.GetFloat("genMET_true");
		//th1[0]->Fill(genMET_true);
		
		TClonesArray* genParP4 = (TClonesArray*)data.GetPtrTObject("genParP4");
		TClonesArray* FATjetP4;
		int nGenPar=data.GetInt("nGenPar");
		int nFatJet;
		int * genParSt=data.GetPtrInt("genParSt");
		int * genParId=data.GetPtrInt("genParId");
		Float_t rawMET,corrMET;
                if (option==1) {
                    rawMET = data.GetFloat("pfMetRawPt");
		    corrMET = data.GetFloat("pfMetCorrPt");
                    FATjetP4 = (TClonesArray*) data.GetPtrTObject("FATjetP4");
		    nFatJet=data.GetInt("FATnJet");
                }
                else {
                    rawMET = data.GetFloat("genMET_true");
		    corrMET = data.GetFloat("genMET_calo");
                }
		TLorentzVector* thisJet, *fatJet;
		int DMId=36;
		for(int i=0,n=0;i<nGenPar;i++){
			if(genParId[i]==25&&genParSt[i]==22 ){
				thisJet=(TLorentzVector*)genParP4->At(i);
				th1[1]->Fill(thisJet->Pt());
                                // only fill MET once
                                if (n==0) { 
                                    th1[2]->Fill(rawMET);
                                    th1[3]->Fill(corrMET);
                                    n++;
                                }
                        }
			
		}
		for(int i=0;i<nFatJet;i++){
                     if (option==1&&jEntry<35000) {
			fatJet=(TLorentzVector*)FATjetP4->At(i);
		        th1[0]->Fill(fatJet->Pt());
                     }
                }
                //TLorentzVector* bJet0 =(TLorentzVector*)genParP4->At(bIndex[0]);
		//TLorentzVector* bJet1 =(TLorentzVector*)genParP4->At(bIndex[1]);
		
		
		
	}
        //th1[3]->Draw("hist");
        // normalize
        for (int i=0;i<4;i++){
            th1_norm[i] = (TH1D*)th1[i]->Clone();
            double integral = th1_norm[i]->Integral();
            if (integral>0) th1_norm[i]->Scale(1/integral);
        }

	//system("mkdir -p output");
	TFile* output=new TFile(Form("output/%s",input.data()),"recreate");
        output->mkdir("normTo1");
	for(int i=0;i<4;i++) {
            th1[i]->Write();
        }
        output->cd("normTo1");
        for(int i=0;i<4;i++) th1_norm[i]->Write();
	output->Close();
        return 0;
	
	
}

