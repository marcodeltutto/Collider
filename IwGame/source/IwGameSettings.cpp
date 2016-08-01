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

#include "s3e.h"
#include "Iw2D.h"
#include "IwGx.h"
#include "IwGL.h"
#include "IwResManager.h"
#include "IwGxFont.h"
#include "s3eOSExec.h"

#include "IwGameUtil.h"
#include "IwGameSettings.h"
#include "IwGameXml.h"

CDECLARE_SINGLETON(CIwGameSettings)

//
//
//
// IwGame implementation
//
//
//
int CIwGameSettings::Init()
{
	PreventPowerSaving = false;

	CIwGameXmlParser*	xml = new CIwGameXmlParser();
	if (xml->Parse("AppEasy.xml") == eXMLParserError_None)
	{
		CIwGameXmlNode* root = xml->GetRoot()->GetFirstNode();
		if (root != NULL)
		{
			// Get debug trace info level
			CIwGameXmlNode* node = root->GetFirstNamedNode(IW_GAME_HASH("debug"));
			if (node != NULL)
			{
				CIwGameXmlAttribute* attrib = node->GetAttribute(IW_GAME_HASH("level"));
				if (attrib != NULL)
					g_ShowTracing = attrib->GetValueAsInt();
			}
		}
	}

	delete xml;

	return 1;
}

void CIwGameSettings::Release()
{
}


