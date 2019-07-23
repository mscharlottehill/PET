/*

 Based on code provided by:
  gary.smith@ed.ac.uk
  3rd July 2019

 Assumed filename: 
 'ThickData.csv'
 
 To run:
 $ root AnalyseThickness.C 

*/

#include "TFile.h"
#include "TString.h"
#include "TH1.h"
#include "TTree.h"
#include "TCanvas.h"

void CSVToRoot(Int_t cSize = 500){

  TFile *outFile = new TFile("ThickVSense.root","RECREATE");

  TTree *T = new TTree("Sensitivity","Sensitivity");
  Long64_t nlines;
  TString lineFormat = "Thickness/F:Sensitivity/F";
  nlines = T->ReadFile("ThickData.csv",lineFormat,',');

  printf(" Sensitivity at %lld thicknesses \n",nlines);

  TCanvas * c1 =  new TCanvas("c1","c1",0,0,cSize,cSize);

  // Draw straight from TTree
  T->Draw("Sensitivity:Thickness","","L*");

  T->Write();

  outFile->Close();
}

void PlotTGraphFromRootData(Int_t cSize = 500){

  // Declaration of leaf types
  Float_t         Thickness;
  Float_t         Sensitivity;

  // List of branches
  TBranch        *b_Thickness;
  TBranch        *b_Sensitivity;

  TFile *file  = new TFile("ThickVSense.root");
  TTree *tree;
  file->GetObject("Sensitivity",tree);

  tree->SetBranchAddress("Thickness", &Thickness, &b_Thickness);
  tree->SetBranchAddress("Sensitivity", &Sensitivity, &b_Sensitivity);

  Long64_t nentries = tree->GetEntriesFast();

  Float_t ThicknessArray[nentries], SensitivityArray[nentries];

  Bool_t comments = kTRUE;

  for (Long64_t iEntry=0; iEntry<nentries;iEntry++) {
    tree->GetEntry(iEntry);

    if(comments){
      cout << endl;
      cout << " entry       = " << iEntry      << endl;
      cout << " Sensitivity = " << Sensitivity << endl;
      cout << " Thickness      = " << Thickness      << endl;
    }

    ThicknessArray[iEntry]      = Thickness;
    SensitivityArray[iEntry] = Sensitivity;

  }

  TCanvas * c2 =  new TCanvas("c2","c2",cSize,0,cSize,cSize);
  TGraph * gr = new TGraph(nentries,ThicknessArray,SensitivityArray);
  gr->SetName("TGraph");
  gr->SetTitle("Sensitivity vs Thickness, Positron Source;Thickness (cm);Sensitivity (%)");

  // Draw TGraph
  gr->Draw("APC*");

}

void AnalyseThickness(){

  CSVToRoot();
  PlotTGraphFromRootData();

}
