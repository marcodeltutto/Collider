#if !defined _COLOURSTORE_
#define _COLOURSTORE_

//#include "IwGameString.h"
#include "CzString.h"


/** @brief 
 Used to store some colours used (and modified) by the event-display interface (see data/HUDscene.xml). Each colour is defined by RGBA values between  [0,256].
 */
class ColourStore{

public:
	ColourStore(){};
	~ColourStore(){};

	static float rgba_Pix[4], rgba_SCT[4], rgba_TRT[4],rgba_ECAL[4],
		rgba_HCAL[4],rgba_muonDets[4], rgba_idTracks[4], rgba_muonTracks[4],
		rgba_ecalTowers[4], rgba_hcalTowers[4], rgba_MET[4];
	

};

#endif
