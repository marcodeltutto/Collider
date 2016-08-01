
#if !defined(_ACYLINDERFROMXML_H_)
#define _ACYLINDERFROMXML_H_


//#include "IwGameXoml.h"
#include "CzXoml.h"
#include "GLCylinder.h"
#include "GLDonut.h"
#include "IwGxScene.h"
//#include "CzScene.h"

/** @brief This class is used to instatiate GLCylinder and GLDonut objects from an xml element (e.g. test_detectors.xml).
 * 
 * Completely analogous to ABoxFRomXML class in the way it is used. Geometry of cylinder (such as Pixel barrel layer) defined by zmin,zmax,thickness.
 * 
 * */
class ACylinderFromXML: public IzXomlResource{
public:
	ACylinderFromXML() : IzXomlResource() { 
		setClassType("ADisc"); 
		zmin = zmax = r0 = thickness = 0;
		nameHash = 0;

	};
	
	virtual ~ACylinderFromXML(){};


	float r0,zmin,zmax, thickness;
	unsigned int nameHash;
	void set_zmin(float _zmin){zmin = _zmin;}
	void set_zmax(float _zmax){zmax = _zmax;}
	void set_r0(float _r0){r0 = _r0;}
	void set_thickness(float _thickness){thickness = _thickness;}
	void set_nameHash(unsigned int _nameHash){nameHash = _nameHash;}
	static unsigned int  ecalName_hash, hcalName_hash, trtName_hash, pixName_hash, sctName_hash, muonDet_hash;
	static unsigned int zmin_hash, zmax_hash, r0_hash, thickness_hash, n_hash, tileEcap_hash, LArEcap_hash;
	
	
	virtual void	Init(){};
	virtual void	Release(){};
	virtual int     LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node);


};

class ACylinderFromXMLCreator : public IzXomlClassCreator
{
public:
	ACylinderFromXMLCreator()
	{
		setClassName("ADisc");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new ACylinderFromXML(); }
};


#endif
