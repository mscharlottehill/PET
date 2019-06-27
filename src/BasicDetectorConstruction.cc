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

#include "BasicDetectorConstruction.hh"
#include "BasicPETSD.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
/*
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSDoseDeposit.hh"
*/
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4AutoDelete.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4PSEnergyDeposit.hh"

#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//

//G4ThreadLocal

//

BasicDetectorConstruction::BasicDetectorConstruction()
 : G4VUserDetectorConstruction(),
   fCheckOverlaps(true)
{
}

//

BasicDetectorConstruction::~BasicDetectorConstruction()
{
}

//

G4VPhysicalVolume* BasicDetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//

void BasicDetectorConstruction::DefineMaterials()
{
  G4NistManager* man = G4NistManager::Instance();

  G4bool isotopes = false;

  G4Element*  O = man->FindOrBuildElement("O" , isotopes);
  G4Element* Si = man->FindOrBuildElement("Si", isotopes);
  G4Element* Lu = man->FindOrBuildElement("Lu", isotopes);

  G4Material* LSO = new G4Material("Lu2SiO5", 7.4*g/cm3, 3);
  LSO->AddElement(Lu, 2);
  LSO->AddElement(Si, 1);
  LSO->AddElement(O , 5);
}

//

G4VPhysicalVolume* BasicDetectorConstruction::DefineVolumes()
{
  // we'll need some air
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* default_mat = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* tube_mat   = nist->FindOrBuildMaterial("Lu2SiO5");

  // PET dimensions
  G4double PET_in_rad = 0.4*m, PET_out_rad = 0.45*m, PET_length = 2.0*m; // vary these

  // world size
  G4double world_dim = 2.5*m;

  //
  // World
  //
  G4Box* solidWorld =
  new G4Box("World",
    0.5*world_dim, 0.5*world_dim, 0.5*world_dim);

// fill it with air

  G4LogicalVolume* logicWorld =
  new G4LogicalVolume(solidWorld,
                      default_mat,
                      "World");

// put it somewhere

  G4VPhysicalVolume* physWorld =
  new G4PVPlacement(0,  // no rotation
                      G4ThreeVector(),  // set origin
                      logicWorld,  // logical volume
                      "World",    // name
                      0,    // mother volume
                      false,  // boolean
                      0,    // copy number
                      fCheckOverlaps);    // checks for volume overlaps

// make cylinder

  G4Tubs* solidTube =
    new G4Tubs("Tube", PET_in_rad, PET_out_rad, 0.5*PET_length, 0., twopi);

// fill it with LSO

G4LogicalVolume* logicTube =
  new G4LogicalVolume(solidTube,
                      tube_mat,
                      "Tube");

// put it in the world

new G4PVPlacement(0,                       // no rotation
                  G4ThreeVector(),         // set origin
                  logicTube,           // logical volume
                  "Tube",              // name
                  logicWorld,              // mother  volume
                  false,                   // boolean operation
                  0,                       // number
                  fCheckOverlaps);         // checking overlaps

  //
  // print parameters
  //
  G4cout
    << G4endl
    << "------------------------------------------------------------" << G4endl
    << "---> The PET scanner has bore radius " << PET_in_rad/m << "m and is "
    << PET_length/m << "m long."
    << "------------------------------------------------------------" << G4endl;


  return physWorld;
}

//

void BasicDetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  //
  // Sensitive detectors
  //
  auto detectorSD
    = new BasicPETSD("detectorSD", "DetectorHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(detectorSD);
  SetSensitiveDetector("Tube",detectorSD);
}

//
