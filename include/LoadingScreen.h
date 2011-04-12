//
//  LoadingScreen.h
//  Kepler
//
//  Created by Tom Carden on 3/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/AppCocoaTouch.h"

class LoadingScreen {  
public:
    void setup( ci::app::AppCocoaTouch *app );
    void draw( ci::gl::Texture starGlowTex );
    bool orientationChanged( ci::app::OrientationEvent event );
    void setInterfaceOrientation( const ci::app::Orientation &orientation );
private:
	ci::gl::Texture	mPlanetaryTex;
	ci::gl::Texture mPlanetTex;
	ci::gl::Texture mBackgroundTex;
	
    ci::app::AppCocoaTouch *app;
    ci::CallbackId mCbOrientationChanged;
    ci::app::Orientation mInterfaceOrientation;
    ci::Matrix44f mOrientationMatrix;
    ci::Vec2f mInterfaceSize;
};