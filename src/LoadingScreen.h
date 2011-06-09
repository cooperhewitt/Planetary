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
#include "Orientation.h"
#include "OrientationEvent.h"

class LoadingScreen {  
public:
    void setup( ci::app::AppCocoaTouch *app, const ci::app::Orientation &orientation );
    void setEnabled( bool enabled );
    void draw( ci::gl::Texture starGlowTex );
    void setInterfaceOrientation( const ci::app::Orientation &orientation );
private:
    
    bool onTouchEvent(ci::app::TouchEvent event) { return mEnabled; };
    
    ci::app::AppCocoaTouch mApp;
    bool mEnabled;
    
	ci::gl::Texture	mPlanetaryTex;
	ci::gl::Texture mPlanetTex;
	ci::gl::Texture mBackgroundTex;
	
    ci::app::AppCocoaTouch *app;
    ci::CallbackId mCbOrientationChanged;
    ci::app::Orientation mInterfaceOrientation;
    ci::Matrix44f mOrientationMatrix;
    ci::Vec2f mInterfaceSize;
};