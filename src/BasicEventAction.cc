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

#include "BasicEventAction.hh"
#include "BasicRunAction.hh"
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

//

BasicEventAction::BasicEventAction(BasicRunAction* runAction)
 : G4UserEventAction(),
   fRunAction(runAction),
   fDetHCID(-1)
{}

//

BasicEventAction::~BasicEventAction()
{}

//

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

//

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

//

void BasicEventAction::BeginOfEventAction(const G4Event* /*event*/)
{}

//

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

  // get deposited energy
  G4double dep = detHit->GetEdep();

  // defining a Good Event
  G4double EnergyRes = 1.022*0.106;
  G4double Threshold = (1.022 - EnergyRes)*MeV;
  if (dep > Threshold) fRunAction->CountEvent();


  // Print per event (modulo n)
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
    G4cout << "---> End of event: " << eventID << G4endl;

    PrintEventStatistics(
      dep, detHit->GetTrackLength());
  }


  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill histograms
  analysisManager->FillH1(0, dep);
  analysisManager->FillH1(1, detHit->GetTrackLength());

  analysisManager->FillNtupleDColumn(0, dep);
  analysisManager->FillNtupleDColumn(1, detHit->GetTrackLength());
  analysisManager->AddNtupleRow();

}

// energy resolution for LSO is 10.6%
// good event = edep > 89.4% of 1.022MeV
