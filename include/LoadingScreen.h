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
    void draw( const ci::app::AppCocoaTouch *app, const ci::gl::Texture &loadingTex, const ci::gl::Texture &starGlowTex, const ci::gl::Texture &starTex );
};