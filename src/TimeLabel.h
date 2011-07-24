//
//  TimeLabel.h
//  Kepler
//
//  Created by Tom Carden on 5/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include <string>
#include "cinder/Font.h"
#include "cinder/Color.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rect.h"
#include "cinder/Utilities.h"
#include "BloomNode.h"

using namespace ci;
using namespace std;

class TimeLabel : public BloomNode {
    
public:
    
    TimeLabel(const int &id, const ci::Font &font, const ci::Color &color): BloomNode(id), mFont(font), mColor(color), mSeconds(-1000000) {}
    ~TimeLabel() {}
    
    virtual bool touchBegan(ci::app::TouchEvent::Touch touch);
    virtual bool touchEnded(ci::app::TouchEvent::Touch touch);        
    virtual void draw();

    void setSeconds(int seconds);

    void setRect(const ci::Rectf &rect) { mRect = rect; }
    void setRect(const float &x1, const float &y1, const float &x2, const float &y2) { mRect.set(x1,y1,x2,y2); }
    const ci::Rectf& getRect() const { return mRect; }

private:
    
    void updateTexture();
    
    ci::Font mFont;
    ci::Color mColor;
    ci::Rectf mRect;
    
    int mSeconds;

    ci::gl::Texture mTexture, mHyphenTexture;
    
};