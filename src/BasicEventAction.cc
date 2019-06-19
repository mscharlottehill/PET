//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: BasicEventAction.cc 100946 2016-11-03 11:28:08Z gcosmo $
//
/// \file BasicEventAction.cc
/// \brief Implementation of the BasicEventAction class

#include "BasicEventAction.hh"
#include "BasicPETSD.hh"
#include "BasicPETHit.hh"
#include "BasicAnalysis.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BasicEventAction::BasicEventAction()
 : G4UserEventAction(),
   fDetHCID(-1)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BasicEventAction::~BasicEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BasicPETHitsCollection*
BasicEventAction::GetHitsCollection(G4int hcID,
                                  const G4Event* event) const
{
  auto hitsCollection
    = static_cast<BasicPETHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));

  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID;
    G4Exception("BasicEventAction::GetHitsCollection()",
      "MyCode0003", FatalException, msg);
  }

  return hitsCollection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BasicEventAction::PrintEventStatistics(
                              G4double detectorEdep, G4double detectorTrackLength) const
{
  // print event statistics
  G4cout
     << "   Detector: total energy: "
     << std::setw(7) << G4BestUnit(detectorEdep, "Energy")
     << "       total track length: "
     << std::setw(7) << G4BestUnit(detectorTrackLength, "Length")
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BasicEventAction::BeginOfEventAction(const G4Event* /*event*/)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BasicEventAction::EndOfEventAction(const G4Event* event)
{
  // Get hits collections IDs (only once)
  if ( fDetHCID == -1 ) {
    fDetHCID
      = G4SDManager::GetSDMpointer()->GetCollectionID("DetectorHitsCollection");
  }

  // Get hits collections
  auto detHC = GetHitsCollection(fDetHCID, event);

  // Get hit with total values
  auto detHit = (*detHC)[detHC->entries()-1];

  // Print per event (modulo n)
  //
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
    G4cout << "---> End of event: " << eventID << G4endl;

    PrintEventStatistics(
      detHit->GetEdep(), detHit->GetTrackLength());
  }

  // Fill histograms, ntuple
  //

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill histograms
  analysisManager->FillH1(0, detHit->GetEdep());
  analysisManager->FillH1(1, detHit->GetTrackLength());

  analysisManager->FillNtupleDColumn(0, detHit->GetEdep());
  analysisManager->FillNtupleDColumn(1, detHit->GetTrackLength());
  analysisManager->AddNtupleRow();


  // // fill ntuple
  // analysisManager->FillNtupleDColumn(0, absoHit->GetEdep());
  // analysisManager->FillNtupleDColumn(1, gapHit->GetEdep());
  // analysisManager->FillNtupleDColumn(2, absoHit->GetTrackLength());
  // analysisManager->FillNtupleDColumn(3, gapHit->GetTrackLength());
  // analysisManager->AddNtupleRow();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
