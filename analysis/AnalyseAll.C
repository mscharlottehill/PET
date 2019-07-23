/*
 Based on code provided by:
  gary.smith@ed.ac.uk
  3rd July 2019

 Assumed filename: 
 'AllData.csv'
 
 To run:
 $ root AnalyseAll.C 

*/

#include "TFile.h"
#include "TString.h"
#include "TH1.h"
#include "TTree.h"
#include "TCanvas.h"

void CSVToRoot(Int_t cSize = 500){

  TFile *outFile = new TFile("SenseSurface.root","RECREATE");

  TTree *T = new TTree("Sensitivity","Sensitivity");
  Long64_t nlines;
  TString lineFormat = "Length/F:Thickness/F:Sensitivity/F";
  nlines = T->ReadFile("AllData.csv",lineFormat,',');

  printf(" Sensitivity as a function of length and crystal size \n");

  TCanvas * c1 =  new TCanvas("c1","c1",0,0,cSize,cSize);

  // Draw straight from TTree
  T->Draw("Sensitivity:Thickness:Length","","L*");

  T->Write();

  outFile->Close();
}

void PlotTGraphFromRootData(Int_t cSize = 500){

  // Declaration of leaf types
  Float_t         Length;
  Float_t         Thickness;
  Float_t         Sensitivity;

  // List of branches
  TBranch        *b_Length;
  TBranch        *b_Thickness;
  TBranch        *b_Sensitivity;

  TFile *file  = new TFile("SenseSurface.root");
  TTree *tree;
  file->GetObject("Sensitivity",tree);

  tree->SetBranchAddress("Length", &Length, &b_Length);
  tree->SetBranchAddress("Thickness", &Thickness, &b_Thickness);
  tree->SetBranchAddress("Sensitivity", &Sensitivity, &b_Sensitivity);

  Long64_t nentries = tree->GetEntriesFast();

  Float_t LengthArray[nentries], ThicknessArray[nentries], SensitivityArray[nentries];

  Bool_t comments = kTRUE;

  for (Long64_t iEntry=0; iEntry<nentries;iEntry++) {
    tree->GetEntry(iEntry);

    if(comments){
      cout << endl;
      cout << " entry       = " << iEntry      << endl;
      cout << " Sensitivity = " << Sensitivity << endl;
      cout << " Thickness   = " << Thickness   << endl;
      cout << " Length      = " << Length      << endl;
    }

    LengthArray[iEntry]      = Length;
    ThicknessArray[iEntry]   = Thickness;
    SensitivityArray[iEntry] = Sensitivity;

  }

  TCanvas * c2 =  new TCanvas("c2","c2",cSize,0,cSize,cSize);
  TGraph2D * gr = new TGraph2D(nentries,LengthArray,ThicknessArray,SensitivityArray);
  gr->SetName("TGraph");
  gr->SetTitle("Sensitivity Contour, Positron Source;Length (m);Thickness (cm);Sensitivity (%)");

  // Draw TGraph
  gStyle->SetPalette(67);
//  gr->SetMarkerStyle(50);
  gr->Draw("SURF1");

}

void AnalyseAll(){

  CSVToRoot();
  PlotTGraphFromRootData();

}
