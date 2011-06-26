//
//  NotificationOverlay.h
//  Kepler
//
//  Created by Tom Carden on 6/2/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <string>
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Area.h"
#include "OrientationHelper.h"

using namespace std;
using namespace ci;
using namespace ci::app;

class NotificationOverlay {
  
public:
    
    NotificationOverlay();    
    ~NotificationOverlay();
    
    void setup( AppCocoaTouch *app, const Orientation &orientation, const Font &font );
    void update();
    void draw();
    
    void setInterfaceOrientation( const Orientation &orientation );
    
    void show(const gl::Texture &texture, const Area &srcRect, const string &message);
    void hide();
    
private:
    AppCocoaTouch *mApp;

    Orientation mInterfaceOrientation;
    Matrix44f mOrientationMatrix;
    Vec2f mInterfaceSize;
    
    bool mSetup;
    bool mActive;
    float mFadeDelay, mFadeDuration, mLastShowTime;
    gl::Texture mCurrentTexture;
    Area mCurrentSrcArea;
    string mCurrentMessage;
	
	Font mFont;
	gl::Texture mMessageTexture;
    
};
