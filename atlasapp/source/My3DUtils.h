#include <math.h>
//#include "IwGameUtil.h"


void CrossProd(float x1, float y1, float z1, float x2, float y2, float z2, float res[3]) 
{ 

res[0] = y1*z2 - y2*z1; 
res[1] = x2*z1 - x1*z2; 
res[2] = x1*y2 - x2*y1; 

} 



void ChrisGluLookAt_v3(CIwFVec3 * eye, CIwFVec3 * lookAt, CIwFVec3 * up){ 

static CIwFVec3 forward;
static CIwFVec3 reUp;
static CIwFVec3 side;



forward = *lookAt - *eye;

//CIwGameError::LogError("forward_x = ",CIwGameString(forward.x).c_str());


forward.Normalise();
up->Normalise();

side = forward ^ *up;
//CIwGameError::LogError("side_x = ",CIwGameString(side.x).c_str());
side.Normalise();
reUp = side ^ forward;

float M[]= 
{ 
side.x, reUp.x,-forward.x,	0.0,
side.y, reUp.y,-forward.y,	0.0,
side.z, reUp.z,-forward.z,	0.0,
0.0,	0.0,	0.0,		1.0
};

glMultMatrixf(M);
glTranslatef (-eye->x, -eye->y, -eye->z); 
 
}

void ChrisGluLookAt_v2(float eyeX, float eyeY, float eyeZ, float lookAtX, float lookAtY, float lookAtZ, float upX, float upY, float upZ) 
{ 

CIwFVec3 forward, lookAt, eye, up, reUp, side;
eye = CIwFVec3(eyeX,eyeY,eyeZ);
lookAt = CIwFVec3(lookAtX,lookAtY,lookAtZ);
up = CIwFVec3(upX,upY,upZ);

//CIwGameError::LogError("up_x = ",CIwGameString(up.x).c_str());
forward = lookAt - eye;

//CIwGameError::LogError("forward_x = ",CIwGameString(forward.x).c_str());


forward.Normalise();
up.Normalise();

side = forward ^ up;
//CIwGameError::LogError("side_x = ",CIwGameString(side.x).c_str());
side.Normalise();
reUp = side ^ forward;


/*float M[]= 
{ 
side.x, side.y, side.z, 0, 
reUp.x, reUp.y, reUp.z, 0, 
-forward.x, -forward.y, -forward.z, 0, 
0, 0, 0, 1 
};*/

float M[]= 
{ 
side.x, reUp.x,-forward.x,0.0,
side.y, reUp.y,-forward.y,0.0,
side.z, reUp.z,-forward.z,0.0,
0.0,		0.0,		0.0,		  1.0
};

glMultMatrixf(M);
glTranslatef (-eye.x, -eye.y, -eye.z); 
 
}
void ChrisGluLookAt(float eyeX, float eyeY, float eyeZ, float lookAtX, float lookAtY, float lookAtZ, float upX, float upY, float upZ) 
{ 
// not using here proper implementation for vectors. 
// if you want, you can replace the arrays with your own 
// vector types 
float f[3];

// calculating the viewing vector 
f[0] = lookAtX - eyeX; 
f[1] = lookAtY - eyeY; 
f[2] = lookAtZ - eyeZ; 

float fMag, upMag; 
fMag = static_cast<float>(sqrt(f[0]*f[0] + f[1]*f[1] + f[2]*f[2])); 
upMag = static_cast<float>(sqrt(upX*upX + upY*upY + upZ*upZ)); 

//CIwGameError::LogError("!!! fMag = ",CIwGameString(fMag).c_str());
//CIwGameError::LogError("!!! upMag = ",CIwGameString(upMag).c_str());

// normalizing the viewing vector 
if( fMag != 0) 
{ 
f[0] = f[0]/fMag; 
f[1] = f[1]/fMag; 
f[2] = f[2]/fMag; 
} 

// normalising the up vector. no need for this here if you have your 
// up vector already normalised, which is mostly the case. 
if( upMag != 0 ) 
{ 
upX = upX/upMag; 
upY = upY/upMag; 
upZ = upZ/upMag; 
} 

float s[3], u[3]; 

CrossProd(f[0], f[1], f[2], upX, upY, upZ, s); 
CrossProd(s[0], s[1], s[2], f[0], f[1], f[2], u); 

/*float M[]= 
{ 
s[0], u[0], f[0], 0, 
s[1], u[1], f[1], 0, 
s[2], u[2], f[2], 0, 
0, 0, 0, 1 
};*/
float M[]= 
{ 
s[0], s[1], s[2], 0, 
u[0], u[1], u[2], 0, 
-f[0], -f[1], -f[2], 0, 
0, 0, 0, 1 
};

glMultMatrixf(&M[0]);
glTranslatef (-eyeX, -eyeY, -eyeZ); 
 
}

struct VertexPoint{
	float x,y,z,time;
};

CzString vertexCoordToString(CIwFVec3 * point){
	
	CzString x;// = CIwGameString(point->x) + CIwGameString(point->y) + CIwGameString(point->z);
	return x;
}


//has the camera reached a checkpoint
//do the check_point coords. lie between camera +- delta?
bool checkPointReached(CIwFVec3 * delta, CIwFVec3 * camera, CIwFVec3 * check_point){
	static float min_x, max_x, min_y, max_y, min_z, max_z;
	//static CIwFVec3 * delta;
	//delta = &CIwFVec3(*delta_in);
	//delta = & (*delta * 2.0);
    *delta = *delta * 2.0;
	
	
	min_x = camera->x + delta->x > camera->x - delta->x ?camera->x - delta->x :camera->x + delta->x;
	max_x = camera->x + delta->x < camera->x - delta->x ?camera->x - delta->x :camera->x + delta->x;
	if (min_x > check_point->x || max_x < check_point->x) {
        *delta  = *delta / 2.0;
        return false;   
    }
	
	

	min_y = camera->y + delta->y > camera->y - delta->y ?camera->y - delta->y :camera->y + delta->y;
	max_y = camera->y + delta->y < camera->y - delta->y ?camera->y - delta->y :camera->y + delta->y;
	if (min_y > check_point->y || max_y < check_point->y){
        *delta = *delta / 2.0;
        return false;
    }
	

	min_z = camera->z + delta->z > camera->z - delta->z ?camera->z - delta->z :camera->z + delta->z;
	max_z = camera->z + delta->z < camera->z - delta->z ?camera->z - delta->z :camera->z + delta->z;
    if (min_z > check_point->z || max_z < check_point->z){
        *delta = *delta / 2.0;
        return false;   
    }
	/*
	if (min_x < check_point->x && max_x > check_point->x){ 
		if (min_y < check_point->y && max_y > check_point->y){ 
			if (min_z < check_point->z && max_z > check_point->z) return true;
		}
	}
	*/
	camera->x = check_point->x;
	camera->y = check_point->y;
	camera->z = check_point->z;
    *delta = *delta / 2.0;
	//return false;
	return true;

}