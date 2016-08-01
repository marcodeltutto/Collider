//
//  TMath.cpp
//  GameSceneGL
//
//  Created by Tom McLaughlan on 23/05/2013.
//
//

#include <vector>
#include <math.h>

float deltaR(float eta0, float phi0, float eta1, float phi1) {
    
    float dEta = fabsf(eta0 - eta1);
    float dPhi = fabsf(phi0 - phi1);
    
    float dR2 = (dEta*dEta) + (dPhi*dPhi);
    
    return sqrtf(dR2);
    
}

int getTrackIndex(std::vector<float> trk_pt, std::vector<float> trk_eta, std::vector<float> trk_phi, float el_pt, float el_eta, float el_phi) {
    
    int index = -1;
    
    float dRsmall = 10.0;
    for (int i = 0; i<trk_pt.size(); ++i) {
        
        if(deltaR(el_eta, el_phi, trk_eta[i], trk_phi[i]) < dRsmall && fabsf(trk_pt[i] - el_pt) < 20.0 && fabsf(trk_pt[i]) > 10.0) {
            dRsmall = deltaR(el_eta, el_phi, trk_eta[i], trk_phi[i]);
            index = i;
        }
        
    }
    
    return index;
}

float transverseMass(float m0, float pt0, float phi0, float met_x, float met_y) {

    float mu_x = pt0*cos(phi0);
    float mu_y = pt0*sin(phi0);
    
    float mwt2 = ((met_x*met_x) + (met_y*met_y)) + ((m0*m0)+((mu_x*mu_x)+(mu_y*mu_y)))
    - ((met_x+mu_x)*(met_x+mu_x) + (met_y+mu_y)*(met_y+mu_y));
    
    return sqrt(fabsf(mwt2));
}

float invMass(float m0, float pt0, float eta0, float phi0, float m1, float pt1, float eta1, float phi1) {

    float x0 = pt0*cos(phi0);
    float y0 = pt0*sin(phi0);
    float z0 = pt0*sinh(eta0);
    
    float x1 = pt1*cos(phi1);
    float y1 = pt1*sin(phi1);
    float z1 = pt1*sinh(eta1);
    
    // E2 = m2 + p2
    // m2 = E2 - p2
    
    float E0 = sqrtf((m0*m0) + (x0*x0) + (y0*y0) + (z0*z0));
    float E1 = sqrtf((m1*m1) + (x1*x1) + (y1*y1) + (z1*z1));
    
    float sumE2 = (E0+E1)*(E0+E1);
    float sumpx2 = (x0+x1)*(x0+x1);
    float sumpy2 = (y0+y1)*(y0+y1);
    float sumpz2 = (z0+z1)*(z0+z1);
    
    float m2 = sumE2 - sumpx2 - sumpy2 - sumpz2;
    
    return sqrt(fabsf(m2));
}
