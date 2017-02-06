
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
  cout << "file name is " << fname << endl;
  cout << "option = " << option << endl;
  TString Mass;
  if (option==0) Mass=gSystem->GetFromPipe(Form("file=%s; test=${file##*MZp}; test2=${test%%_MA0*}; echo \"$test2\"",input.data()));
  else if (option==1)  Mass=gSystem->GetFromPipe(Form("file=%s; test=${file##*MZp-}; test2=${test%%_MA0*}; echo \"$test2\"",input.data()));
  else return 0;    
  cout << "Mass = " << Mass.Data() << endl;
  int mass = atoi(Mass.Data());
  cout << "mass = " << mass << endl;
	
  TH1D* th1[20];
  
  float xmin=0;
  float xmax= mass>2000? 10000:2500;
  int nbins = xmax/10.;
  TCanvas *c1 = new TCanvas();
  th1[0]=new TH1D("genMET_true","genMET_true",nbins,xmin,xmax);
  th1[1]=new TH1D("Higgs_pt","Higgs_pt",nbins,xmin,xmax);
  th1[2]=new TH1D("b0_pt","b0_pt",nbins,xmin,xmax);
  th1[3]=new TH1D("b1_pt","b1_pt",nbins,xmin,xmax);
  th1[4]=new TH1D("deltaR","deltaR",60,0,6);
  th1[5]=new TH1D("genMET_DM","genMET_DM",nbins,xmin,xmax);
	for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){
	        if (jEntry > 70000) break;
                data.GetEntry(jEntry);
                float genMET_true=data.GetFloat("genMET_true");
		th1[0]->Fill(genMET_true);
		
		TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");
		int nGenPar=data.GetInt("nGenPar");
		int * genParSt=data.GetPtrInt("genParSt");
		int * genParId=data.GetPtrInt("genParId");
		
		int bIndex[2]={-1,-1};
		TLorentzVector* thisJet ,*DM1, *DM2;
		bool findThis=false, findDM1=false, findDM2=false;
		
		int DMId=36;
		for(int i=0;i<nGenPar;i++){
			if(genParId[i]==25&&genParSt[i]==22 ){
				thisJet=(TLorentzVector*)genParP4->At(i);
				th1[1]->Fill(thisJet->Pt());
				findThis=true;
			}
		        // find DM	
			if(genParId[i]==DMId&&genParSt[i]==1 ){
				//cout<<"found";
				DM1=(TLorentzVector*)genParP4->At(i);
				findDM1=true;
			}
			if(genParId[i]==-DMId&&genParSt[i]==1 ){
				//cout<<"found";
				DM2=(TLorentzVector*)genParP4->At(i);
				findDM2=true;
			}
			
			//if(genParSt[i]!=23)continue;
			// find b ID
                        if(fabs(genParId[i])==5 && bIndex[0]==-1 &&genParSt[i]==23){
				bIndex[0]=i;
			}
			else if (fabs(genParId[i])==5 && bIndex[0]!=-1 &&genParSt[i]==23 ){
				bIndex[1]=i;
			}
			
		}
		//if (jEntry==79213) cout << "scanloop" << endl;
		TLorentzVector* bJet0 =(TLorentzVector*)genParP4->At(bIndex[0]);
		TLorentzVector* bJet1 =(TLorentzVector*)genParP4->At(bIndex[1]);
		
		th1[2]->Fill(bJet0->Pt());
		th1[3]->Fill(bJet1->Pt());
		th1[4]->Fill(bJet0->DeltaR(*bJet1)); 
		if(findDM1 && findDM2)	th1[5]->Fill( (*DM1+*DM2).Pt());
		//cout<<findThat<<","<<findThis<<endl;
		// if(findThis && findThat){
		// 	//cout<<"M="<<(*thisJet+*thatJet).M()<<endl;
		// 	th1[5]->Fill((*thisJet+*thatJet).M());
		// }
		
		//TClonesArray* ak4nGenJet = (TClonesArray*) data.GetPtrTObject("ak4nGenJet");
		
		
	}
	system("mkdir -p output");
	TFile* output=new TFile(Form("output/%s",input.data()),"recreate");
	for(int i=0;i<6;i++) {
            th1[i]->Write();
            th1[i]->Draw();
            if (i==0) c1->Print(Form("output/%s.pdf(",fname.data()));
            if (i==5) c1->Print(Form("output/%s.pdf)",fname.data()));
            if (i!=0&&i!=5 ) c1->Print(Form("output/%s.pdf",fname.data()));
        }
	output->Close();
        return 0;
	
	
}

