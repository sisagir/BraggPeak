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
// This is the modified version of the example electromagnetic/TestEm7/src/PrimaryGeneratorAction.cc

#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* det)
:G4VUserPrimaryGeneratorAction(),                                              
 fParticleGun(0),
 fDetector(det),
 fRndmBeam(0),
 fInitPosX(0),
 fInitPosY(0),
 fInitPosZ(0),
 fInputFile(),
 fEbeamCumul(0),       
 fGunMessenger(0)
{
  dataLoaded = false;
  fParticleGun  = new G4ParticleGun(1);
  G4ParticleDefinition* particle
           = G4ParticleTable::GetParticleTable()->FindParticle("proton");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleEnergy(160*MeV);  
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1.,0.,0.));
  fParticleGun->SetParticlePosition(G4ThreeVector(-2.*cm,0.,0.));
    
  //create a messenger for this class
  fGunMessenger = new PrimaryGeneratorMessenger(this);  
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fGunMessenger;  
}

void PrimaryGeneratorAction::ReadPICinput()
{
  //Do not allow more than one thread to trigger the file reading
  if (dataLoaded)
    return;
  
  std::cout<<"Reading EPOCH input data : "<<std::endl;
  std::cout<<"                           "<<fInputFile<<std::endl;
  fBeamSourceFile = new TFile(fInputFile);
  fBeamSourceReader.SetTree("ana",fBeamSourceFile);
  TTreeReaderValue<Double_t> theKE(fBeamSourceReader, "b_KE");
  TTreeReaderValue<Int_t>    theNp(fBeamSourceReader, "b_Np");
  TTreeReaderValue<Double_t> thePx(fBeamSourceReader, "b_px");
  TTreeReaderValue<Double_t> thePy(fBeamSourceReader, "b_py");
  TTreeReaderValue<Double_t> thePz(fBeamSourceReader, "b_pz");
  nentries = fBeamSourceReader.GetEntries();
  fBeamSourceReader.SetEntry(1);
  dataLoaded= true;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //this function is called at the begining of event
  if (!dataLoaded) PrimaryGeneratorAction::ReadPICinput(); //Read input beam data if it is not already loaded!

  //Initial particle position
  //G4double x0 = -0.5*(fDetector->GetAbsorSizeX());
  //G4double y0 = 0.*cm, z0 = 0.*cm;
  G4double x0 = fInitPosX, y0 = fInitPosY, z0 = fInitPosZ;
  
  //randomize the beam, if requested.
  if (fRndmBeam > 0.) 
    {
      if (fRndmBeam > fDetector->GetAbsorSizeYZ())
        fRndmBeam = fDetector->GetAbsorSizeYZ(); 
      G4double rbeam = 0.5*fRndmBeam;
      y0 = (2*G4UniformRand()-1.)*rbeam;
      z0 = (2*G4UniformRand()-1.)*rbeam;
    }

  // Draw a proton from the input file with its kinetic energy, momentum, and normalized weight
  TRandom3 rand(0);
  Long64_t coin = rand.Uniform(0,nentries);
  fBeamSourceReader.Restart();
  fBeamSourceReader.SetTree("ana",fBeamSourceFile);
  TTreeReaderValue<Double_t> theKE(fBeamSourceReader, "b_KE");
  TTreeReaderValue<Int_t>    theNp(fBeamSourceReader, "b_Np");
  TTreeReaderValue<Double_t> thePx(fBeamSourceReader, "b_px");
  TTreeReaderValue<Double_t> thePy(fBeamSourceReader, "b_py");
  TTreeReaderValue<Double_t> thePz(fBeamSourceReader, "b_pz");
  fBeamSourceReader.SetEntry(coin);

  auto ke = *theKE;
  auto np = *theNp;
  auto px = *thePx; //Epoch laser beam is in x-direction=!
  auto py = *thePy;
  auto pz = *thePz;
  auto parVec = G4ThreeVector(px, py, pz).unit();
  //auto parVec = G4ThreeVector(1, 0, 0).unit();
  //ke = 62

  fParticleGun->SetNumberOfParticles(np);
  fParticleGun->SetParticleMomentumDirection(parVec);
  fParticleGun->SetParticleEnergy(ke * MeV);
  fParticleGun->GeneratePrimaryVertex(anEvent);
  fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
  
  //DEBUG:
  //std::cout<<"Coin:"<<coin;
  //std::cout<<", ke:"<<ke;
  //std::cout<<", px:"<<parVec.x();
  //std::cout<<", py:"<<parVec.y();
  //std::cout<<", pz:"<<parVec.z()<<std::endl;
  //std::cout<<"Nparticles:"<<fParticleGun->GetNumberOfParticles()<<std::endl;
  //std::cout<<"Momentum:"<<fParticleGun->GetParticleMomentum()<<std::endl;
  //std::cout<<"Initial Position:"<<fParticleGun->GetParticlePosition()<<std::endl;
    
  fEbeamCumul += fParticleGun->GetParticleEnergy(); 
}



