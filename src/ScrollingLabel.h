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
#include "UIElement.h"

using namespace std;
using namespace ci;

class ScrollingLabel : public UIElement {

public:

    ScrollingLabel() {}
    ~ScrollingLabel() {}
    
    void setup(const int &id, const Font &font, const Color &color)
    {
        UIElement::setup(id);
        mFont = font;
        mColor = color;
    }
    
    void draw();

    void setText(string text);
    void setLastTrackChangeTime(float lastTrackChangeTime) { mLastTrackChangeTime = lastTrackChangeTime; }

	bool isScrollingText() { return mIsScrolling; }

    void setColor(Color color) { mColor = color; updateTexture(); }
    
private:
    
    void updateTexture();
    
    Font mFont;
    Color mColor;

    string mText;
    float mLastTrackChangeTime;
    
    gl::Texture mTexture;
	
	bool mIsScrolling;
    
};