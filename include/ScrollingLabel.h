//
//  ScrollingLabel.h
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
#include "cinder/Text.h"
#include "cinder/Rect.h"

using namespace std;
using namespace ci;

class ScrollingLabel {

public:

    ScrollingLabel() {}
    ~ScrollingLabel() {}
    
    void setup(Font font, Color color) {
        mFont = font;
        mColor = color;
    }
    
    void update() {}
    void draw();

    void setText(string text);
    void setLastTrackChangeTime(float lastTrackChangeTime) { mLastTrackChangeTime = lastTrackChangeTime; }

    void setRect(Rectf rect) { mRect = rect; }
    void setRect(const float &x1, const float &y1, const float &x2, const float &y2) { mRect.set(x1,y1,x2,y2); }
    Rectf getRect() { return mRect; }
	bool isScrollingText() { return mIsScrolling; }
	

private:
    
    void updateTexture();
    
    Rectf mRect;
    Font mFont;
    Color mColor;

    string mText;
    float mLastTrackChangeTime;
    
    gl::Texture mTexture;
	
	bool mIsScrolling;
    
};