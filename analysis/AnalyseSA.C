/*

 Based on code provided by:
  gary.smith@ed.ac.uk
  3rd July 2019

 Assumed filename: 
 'SolidAngleData.csv'
 
 To run:
 $ root AnalyseSA.C 

*/

#include "TFile.h"
#include "TString.h"
#include "TH1.h"
#include "TTree.h"
#include "TCanvas.h"

void CSVToRoot(Int_t cSize = 500){

  TFile *outFile = new TFile("AngleVSense.root","RECREATE");

  TTree *T = new TTree("Sensitivity","Sensitivity");
  Long64_t nlines;
  TString lineFormat = "Angle/F:Sensitivity/F";
  nlines = T->ReadFile("SolidAngleData.csv",lineFormat,',');

  printf(" Sensitivity at %lld angles \n",nlines);

  TCanvas * c1 =  new TCanvas("c1","c1",0,0,cSize,cSize);

  // Draw straight from TTree
  T->Draw("Sensitivity:Angle","","L*");

  T->Write();

  outFile->Close();
}

void PlotTGraphFromRootData(Int_t cSize = 500){

  // Declaration of leaf types
  Float_t         Angle;
  Float_t         Sensitivity;

  // List of branches
  TBranch        *b_Angle;
  TBranch        *b_Sensitivity;

  TFile *file  = new TFile("AngleVSense.root");
  TTree *tree;
  file->GetObject("Sensitivity",tree);

  tree->SetBranchAddress("Angle", &Angle, &b_Angle);
  tree->SetBranchAddress("Sensitivity", &Sensitivity, &b_Sensitivity);

  Long64_t nentries = tree->GetEntriesFast();

  Float_t AngleArray[nentries], SensitivityArray[nentries];

  Bool_t comments = kTRUE;

  for (Long64_t iEntry=0; iEntry<nentries;iEntry++) {
    tree->GetEntry(iEntry);

    if(comments){
      cout << endl;
      cout << " entry       = " << iEntry      << endl;
      cout << " Sensitivity = " << Sensitivity << endl;
      cout << " Solid Angle      = " << Angle      << endl;
    }

    AngleArray[iEntry]      = Angle;
    SensitivityArray[iEntry] = Sensitivity;

  }

  TCanvas * c2 =  new TCanvas("c2","c2",cSize,0,cSize,cSize);
  TGraph * gr = new TGraph(nentries,AngleArray,SensitivityArray);
  gr->SetName("TGraph");
  gr->SetTitle("Sensitivity vs Solid Angle, Positron Source;Solid Angle (sr);Sensitivity (%)");

  // Draw TGraph
  gr->Draw("APC*");

}

void AnalyseSA(){

  CSVToRoot();
  PlotTGraphFromRootData();

}
