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
    void draw( ci::gl::Texture loadingTex, ci::gl::Texture starGlowTex, ci::gl::Texture starTex );
    bool orientationChanged( ci::app::OrientationEvent event );
private:
    ci::app::AppCocoaTouch *app;
    ci::CallbackId mCbOrientationChanged;
    ci::app::DeviceOrientation mDeviceOrientation;
    ci::Matrix44f mOrientationMatrix;
    ci::Vec2f mInterfaceSize;
};