
#if !defined(_ABOXFROMXML_H_)
#define _ABOXFROMXML_H_


//#include "IwGameXoml.h"
#include "CzXoml.h"
#include "GLCube.h"
#include "IwGxScene.h"

/** @brief This class is used to instatiate GLCube objects from an xml element (e.g. test_detectors.xml).
 * 	The attributes are used to define the cube's position in world space and the cube is added to the 
 * 	IwGxScene's vector of meshes to be drawn. 
 *  
 *  Since this class is instantiated dynamically by ABoxFromXMLCreator, the memory is freed later as a result of adding this to the IwGxScenes "toBeCleaned" vector.
 *  */
//class ABoxFromXML: public IIwGameXomlResource{
class ABoxFromXML: public IzXomlResource{
public:


	ABoxFromXML() : IzXomlResource() {
 
		setClassType("ABox"); 
		zi = zo = ri = ro = w = eta = 0;
		for (int i=0;i<8;i++) phi[i] = -1;

	};
	virtual ~ABoxFromXML(){};

	/** define the z, and rho parameters for the cubes.
	 */
	float zi, zo, ri, ro, w, eta, dphi;
	int phi[8];

	
	void set_zi(float _zi){zi = _zi;}
	void set_zo(float _zo){zo = _zo;}
	void set_ri(float _ri){ri = _ri;}
	void set_ro(float _ro){ro = _ro;}
	void set_w(float _w){w = _w;}
	void set_eta(float _eta){eta = _eta;}
	void set_dphi(float _dphi){dphi = _dphi;}
	
	static unsigned int zi_hash, zo_hash, ri_hash, ro_hash, w_hash, eta_hash, phi_hash, dphi_hash;
	//callback for setter
	/*void set_value(void (* set_fn)(float node->GetValueAsFloat()), CIwGameXmlNode * node,char * attrib_name ){
		unsigned int name_hash = CIwGameString::CalculateHash(attrib_name);
		if (node->GetName().getHash() == name_hash) set_fn(node->GetValueAsFloat(), attrib_name);
	}*/
	
	
	virtual void	Init(){};
	virtual void	Release(){};

	virtual int LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node);


};

class ABoxFromXMLCreator : public IzXomlClassCreator
{
public:
	ABoxFromXMLCreator()
	{
		setClassName("abox");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new ABoxFromXML(); }
};


#endif
