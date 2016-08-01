#include "Tracks.h"

//string hash codes
unsigned int Tracks::pt_hash = CzString::CalculateHash("pt");
unsigned int Tracks::phi0_hash = CzString::CalculateHash("phi0");
unsigned int Tracks::cotTheta_hash= CzString::CalculateHash("cottheta");
unsigned int Tracks::z0_hash = CzString::CalculateHash("z0");
unsigned int Tracks::d0_hash = CzString::CalculateHash("d0");

unsigned int Tracks::npix_hash = CzString::CalculateHash("npixhits");
unsigned int Tracks::nsct_hash = CzString::CalculateHash("nscthits");
unsigned int Tracks::ntrt_hash = CzString::CalculateHash("ntrthits");

unsigned int Tracks::polylineX_hash = CzString::CalculateHash("polylineX");
unsigned int Tracks::polylineY_hash = CzString::CalculateHash("polylineY");
unsigned int Tracks::polylineZ_hash = CzString::CalculateHash("polylineZ");
unsigned int Tracks::numpolyline_hash = CzString::CalculateHash("numpolyline");

unsigned int Tracks::idTracks_hash = CzString::CalculateHash("idtracks");
unsigned int Tracks::muonTracks_hash = CzString::CalculateHash("muontracks");

unsigned short Tracks::indices_2[256] = {0};
