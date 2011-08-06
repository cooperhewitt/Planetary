//
//  LoadingScreen.h
//  Kepler
//
//  Created by Tom Carden on 3/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/gl/Texture.h"
#include "BloomNode.h"

class LoadingScreen : public BloomNode {  
  public:
    void setup( const ci::gl::Texture &starGlowTex );
    void draw();
    void update();
    bool touchBegan( ci::app::TouchEvent::Touch touch ) { return isVisible(); };
    bool touchMoved( ci::app::TouchEvent::Touch touch ) { return isVisible(); };
    bool touchEnded( ci::app::TouchEvent::Touch touch ) { return isVisible(); };
    void setProgress( float prop );
  private:
    float mProgress, mProgressDest;
    ci::gl::Texture mStarGlowTex;
	ci::gl::Texture	mPlanetaryTex;
	ci::gl::Texture mPlanetTex;
	ci::gl::Texture mBackgroundTex;	
    ci::Vec2f mInterfaceSize;
};