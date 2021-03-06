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
// This is the modified version of the example electromagnetic/TestEm7/include/PrimaryGeneratorAction.hh

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "vector"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

class G4Event;
class DetectorConstruction;
class PrimaryGeneratorMessenger;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(DetectorConstruction*);    
   ~PrimaryGeneratorAction();

  public:  
    void SetRndmBeam(G4double val)  {fRndmBeam = val;} 
    void SetInitPosXBeam(G4double val)  {fInitPosX = val;} 
    void SetInitPosYBeam(G4double val)  {fInitPosY = val;} 
    void SetInitPosZBeam(G4double val)  {fInitPosZ = val;} 
    void SetInputFile(G4String file)  {fInputFile = file;}  
    virtual void GeneratePrimaries(G4Event*);
    virtual void ReadPICinput();
    
    void   ResetEbeamCumul() {fEbeamCumul = 0.;}
    G4double GetEbeamCumul() {return fEbeamCumul;}
     
    G4ParticleGun* GetParticleGun() {return fParticleGun;}

    G4int nentries;
    TTreeReader fBeamSourceReader;
    
  private:
    G4ParticleGun*             fParticleGun;
    DetectorConstruction*      fDetector;
    G4double                   fRndmBeam;
    G4double                   fInitPosX;
    G4double                   fInitPosY;
    G4double                   fInitPosZ;
    G4String                   fInputFile;
    G4double                   fEbeamCumul;       
    PrimaryGeneratorMessenger* fGunMessenger;
    G4bool                     dataLoaded;
    TFile*                     fBeamSourceFile;    
};

#endif


