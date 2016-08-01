#include "CaloTowers.h"

//hash codes for XML reader
unsigned int CaloTowers::eCalhist_et_hash = CzString::CalculateHash("LAr3DEt");
unsigned int CaloTowers::eCalhist_psi_hash = CzString::CalculateHash("LAr3Dpsi");
unsigned int CaloTowers::eCalhist_phi_hash = CzString::CalculateHash("LAr3Dphi");

unsigned int CaloTowers::hCalhist_et_hash = CzString::CalculateHash("TILE3DEt");
unsigned int CaloTowers::hCalhist_psi_hash = CzString::CalculateHash("TILE3Dpsi");
unsigned int CaloTowers::hCalhist_phi_hash = CzString::CalculateHash("TILE3Dphi");

unsigned int CaloTowers::ecalTowers_hash = CzString::CalculateHash("ecalTowers");
unsigned int CaloTowers::hcalTowers_hash = CzString::CalculateHash("hcalTowers");
