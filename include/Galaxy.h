//
//  Galaxy.h
//  Kepler
//
//  Created by Tom Carden on 6/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

class Galaxy {
public:
    
    void setup(float initialCamDist, ci::gl::Texture galaxyDome, ci::gl::Texture galaxyTex, ci::gl::Texture darkMatterTex, ci::gl::Texture starGlowTex);
    
    // TODO: move invAlpha, lightMatterColor, elapsedSeconds, camGalaxyAlpha, zoomOff to members
    void update() {};
    
    void drawLightMatter(float invAlpha, ci::Color color /* BRIGHT_BLUE */, float elapsedSeconds);
    void drawSpiralPlanes(float camGalaxyAlpha, float zoomOff, float elapsedSeconds);
    void drawCenter(float invAlpha, ci::Color color /* BLUE */, float elapsedSeconds, ci::Vec3f bbRight, ci::Vec3f bbUp);
    void drawDarkMatter(float invAlpha, float camGalaxyAlpha, float zoomOff, float elapsedSeconds);
    
private:
    
    void initGalaxyVertexArray();
    void initDarkMatterVertexArray();

	GLfloat			*mGalaxyVerts;
	GLfloat			*mGalaxyTexCoords;
	GLfloat			*mDarkMatterVerts;
	GLfloat			*mDarkMatterTexCoords;
	int				mDarkMatterCylinderRes;
	float			mLightMatterBaseRadius;
	float			mDarkMatterBaseRadius; 
    
    ci::gl::Texture mGalaxyDome, mGalaxyTex, mDarkMatterTex, mStarGlowTex;
};
