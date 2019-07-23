C. HILL 
JUN-JUL 2019

Code for GEANT4 simulation of simple PET scanner geometry.

IN THIS BRANCH: A human patient modelled as a cylinder of water, acting as an isotropic source of positrons.
                Sensitvity and scatter fraction are calculated.
                Axial hit positions in the detector are recorded.
                Output is in ROOT.
                
IF CHANGING THE GEOMETRY: Detector geometry should be changed in BasicPET.cc.
                          Patient geometry should be changed in BasicDetectorConstruction.cc.
                          If patient geometry is changed, source geometry must be changed - it is hard coded in
                          BasicPrimaryGeneratorAction.cc.
                       
GEANT4 basic examples and EdMedPhc code used as templates.
