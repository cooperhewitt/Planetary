//
//  NotificationOverlay.h
//  Kepler
//
//  Created by Tom Carden on 6/2/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#pragma once

#include <string>
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Area.h"
#include "BloomNode.h"

class NotificationOverlay : public BloomNode {
  
public:
    
    NotificationOverlay();    
    ~NotificationOverlay();
    
    void setup( const ci::Font &font );
    void update();
    void draw();
    
    void show( const ci::gl::Texture &texture, const ci::Area &srcRect, const std::string &message );    
    void show( const ci::gl::Texture &texture1, const ci::Area &srcRect1, const ci::Area &srcRect2, const std::string &message );
	void showLetter( const char &c, const std::string &message, const ci::Font &hugeFont );
    void hide();
    
private:
    
    bool mSetup;
    bool mActive;
    float mFadeDelay, mFadeDuration, mLastShowTime;
    ci::gl::Texture mCurrentTexture;
    ci::Area mCurrentSrcArea;
    ci::Area mCurrentSecondSrcArea;
    std::string mCurrentMessage;
    ci::Rectf mMessageRect, mIconRect;
    float mAlpha;
    ci::Font mFont;
    ci::gl::Texture mMessageTexture;
    
};
