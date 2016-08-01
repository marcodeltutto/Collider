// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. You must ensure that this whole notice is present in all files and derivatives, so the reader knows its origin.
// If you use this engine in your product then please ensure that you credit IwGame's usage appropriately. Please see http://www.drmop.com/index.php/iwgame-engine/ for licensing details and support
//
//

#if !defined(_IW_GAME_SETTINGS_H_)
#define _IW_GAME_SETTINGS_H_

#include "IwGameUtil.h"

#define	IW_GAME_SETTINGS	CIwGameSettings::getInstance()


//
//
//
//
// CIwGameSettings is a settings class that controls varius facets fof the IwGame engine
// 
//
// 
//
class CIwGameSettings
{
public:
	CDEFINE_SINGLETON(CIwGameSettings)

protected:
	//// Properties
	bool		PreventPowerSaving;

public:
	bool		getPreventPowerSaving() const			{ return PreventPowerSaving; }
	void		setPreventPowerSaving(bool enable)		{ PreventPowerSaving = enable; }
	//// Properties end

protected:

public:
	int		Init();
	void	Release();

};



#endif // _IW_GAME_SETTINGS_H_
