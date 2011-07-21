//
//  LoadingScreen.h
//  Kepler
//
//  Created by Tom Carden on 3/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/gl/Texture.h"
#include "UINode.h"

class LoadingScreen : public UINode {  
  public:
    void setup( const ci::gl::Texture &starGlowTex );
    void draw();
    void update();
    bool touchBegan( ci::app::TouchEvent::Touch touch ) { return isVisible(); };
    bool touchMoved( ci::app::TouchEvent::Touch touch ) { return isVisible(); };
    bool touchEnded( ci::app::TouchEvent::Touch touch ) { return isVisible(); };

  private:
    ci::gl::Texture mStarGlowTex;
	ci::gl::Texture	mPlanetaryTex;
	ci::gl::Texture mPlanetTex;
	ci::gl::Texture mBackgroundTex;	
    ci::Vec2f mInterfaceSize;
};