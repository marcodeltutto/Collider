#include "IwGxScene.h"

#if !defined (_ANIMATION_)
#define _ANIMATION_




class IwGxScene;

struct animationStep{
	CIwFVec3 rotation;
	float zoom;
	
	int nsteps;
	int currentStep;
	float deltaZoom;
	CIwFVec3 deltaRotation;
	bool triggerEvent;
};




/** @brief This class is used to direct any animation in the event display (move the 3-D scene around, update the tracks
  as they move through the detector with time etc.).
	Algorithm is that a vector of animation steps is moved through sequentially by the main loop.
	At each step the position of the camera (or everything else if in GL_ORTHO projection mode) is 
	updated. 
	of triggerEvent is true for a particular step, when the step is reached the animation instance will try
	to start the event animation.
**/
class Animation{

public:
	//c-tor
	Animation(){};
	//d-tor
	~Animation(){};

	std::vector<animationStep>  proton_path;
	std::vector<animationStep>  circular_path;

	CIwFVec3 * currentPos;
	CIwFVec3 * currentRotation;

	int projectionMode;
	
	float drawRadius;
	//related to Speed of Light
	float deltaRadius;
	//event to animate
	EventFromXML * currentEvent;
	IwGxScene * scene;
	Tracks * idTracks, * muonTracks;
	CaloTowers * ecalHisto, * hcalHisto;
	//when animating from proton's eye view, follow this path
	static std::vector<CIwFVec3> protonPath;

	bool isPlaying;
	bool eventIsPlaying;

	int animation_mode;
	//functions
	void Init(float worldScale,IwGxScene * _scene);
	void Restart(float worldScale);
	void ResetAnimationPath(int mode = -1);
	void UpdateOrthoPosition(CIwFVec3 * rotation, float & zoom,int mode=0);
	void UpdateEvent();
	void UpdateAnimation(CIwFVec3 * rotation, float & zoom);
	//tracks
	void UpdateTracks (Tracks * tracks);
	void InitTrackAnimation(Tracks * tracks);
	void UpdateTrackIndex(Tracks * tracks, int globalTotal, int iTrack, int nPreviousPoints);
	bool ReplaceLastTrackVertex(Tracks * tracks, int iTrack, int iVert);
	void InsertLastTrackVertex(Tracks * tracks, int iTrack, int iVert, float remainingDrawDistance);
	//calo
	void InitCaloTowersAnimation(CaloTowers * towers);
	void UpdateCaloTowers(CaloTowers * towers);
	void UpdateTowerVertices(GLCube * tower, float & currentOuterRadius);
	void SetupPaths();
	void addStep(float rx, float ry, float rz, float z, int ns,std::vector<animationStep> * steps, bool trigger = false);
	//MET
	void UpdateMET();

	void UpdateTrackIndex_2(Tracks * tracks, int globalTotal, int iTrack, int nPreviousPoints);
	bool ReplaceLastTrackVertex_2(Tracks * tracks, int iTrack, int iVert);
	void InsertLastTrackVertex_2(Tracks * tracks, int iTrack, int iVert, float remainingDrawDistance);
	
	
};





#endif
