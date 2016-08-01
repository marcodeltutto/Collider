
#if !defined(_EndcapFromXML_H_)
#define _EndcapFromXML_H_


//#include "IwGameXoml.h"
#include "CzXoml.h"
#include "GLEndcap.h"
#include "IwGxScene.h"

/**
 @brief Completely analogous to ABoxFromXML, ACylinderFromXML etc.  Dynamically creates a GLEndcap (or GLDonut based on name) geometry of which is defined in XML attributes of XML element that are read. Adds new objects to event-display vector of objects to be rendered (MeshGroup).
*/
class EndcapFromXML: public IzXomlResource{
public:
	EndcapFromXML() : IzXomlResource() {
		setClassType("AEndcap"); 
		zmin = zmax = rmin = rmax = 0;
		name = "";

	};
	
	virtual ~EndcapFromXML(){};

	//float zi, zo, ri, ro, w, eta;
	//int phi[8];

	float rmin, rmax ,zmin,zmax, thickness;
	CzString name;
	void set_zmin(float _zmin){zmin = _zmin;}
	void set_zmax(float _zmax){zmax = _zmax;}
	void set_rmin(float _rmin){rmin = _rmin;}
	void set_rmax(float _rmax){rmax = _rmax;}
	void set_name(CzString _name){name = _name;}
	void set_nameHash(unsigned int _nameHash){nameHash = _nameHash;}

	static unsigned int  pix_hash, sct_hash, trt_hash, ecal_hash, hcal_hash, muon_hash;
	static unsigned int zmin_hash, zmax_hash, rmin_hash, rmax_hash, c_hash, n_hash;
	unsigned int nameHash;


	
	
	virtual void	Init(){};
	virtual void	Release(){};
	virtual int LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node);


};

class EndcapFromXMLCreator : public IzXomlClassCreator
{
public:
	EndcapFromXMLCreator()
	{
		setClassName("AEndcap");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new EndcapFromXML(); }
};


#endif
