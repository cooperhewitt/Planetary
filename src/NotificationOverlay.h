//
//  NotificationOverlay.h
//  Kepler
//
//  Created by Tom Carden on 6/2/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <string>
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Area.h"
#include "BloomNode.h"

using namespace std;
using namespace ci;
using namespace ci::app;

class NotificationOverlay : public BloomNode {
  
public:
    
    NotificationOverlay();    
    ~NotificationOverlay();
    
    void setup( const Font &font );
    void update();
    void draw();
    
    void show(const gl::Texture &texture, const Area &srcRect, const string &message);
	void showLetter( const char c, const string &message, const Font &hugeFont );
    void hide();
    
private:
    
    bool mSetup;
    bool mActive;
    float mFadeDelay, mFadeDuration, mLastShowTime;
    gl::Texture mCurrentTexture;
    Area mCurrentSrcArea;
    string mCurrentMessage;
	Rectf mMessageRect, mIconRect;
    float mAlpha;
	Font mFont;
	gl::Texture mMessageTexture;
    
};
