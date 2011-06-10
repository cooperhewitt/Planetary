//
//  Galaxy.h
//  Kepler
//
//  Created by Tom Carden on 6/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

class Galaxy {
public:
    
    void setup(float initialCamDist, 
               ci::Color lightMatterColor,
               ci::Color centerColor,
               ci::gl::Texture galaxyDome, 
               ci::gl::Texture galaxyTex, 
               ci::gl::Texture darkMatterTex, 
               ci::gl::Texture starGlowTex);
    
    void update(const ci::Vec3f &eye, 
                const float &fadeInAlphaToArtist, 
                const float &elapsedSeconds,
                const ci::Vec3f &bbRight, 
                const ci::Vec3f &bbUp);
    
    void drawLightMatter();
    void drawSpiralPlanes();
    void drawCenter();
    void drawDarkMatter();
    
private:
    
    // set in update()
	float mZoomOff, mCamGalaxyAlpha, mInvAlpha, mElapsedSeconds;
    ci::Vec3f mBbRight, mBbUp;

    // set in setup()
    ci::gl::Texture mGalaxyDome, mGalaxyTex, mDarkMatterTex, mStarGlowTex;
    ci::Color mCenterColor, mLightMatterColor;
    
    // called in setup()
    void initGalaxyVertexArray();
    void initDarkMatterVertexArray();

    // set in initXXX(), used in drawXXX()
	GLfloat			*mGalaxyVerts;
	GLfloat			*mGalaxyTexCoords;
	GLfloat			*mDarkMatterVerts;
	GLfloat			*mDarkMatterTexCoords;
	int				mDarkMatterCylinderRes;
	float			mLightMatterBaseRadius;
	float			mDarkMatterBaseRadius;     
};
