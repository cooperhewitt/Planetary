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
#include "cinder/Text.h"
#include "cinder/Rect.h"
#include "cinder/Utilities.h"
#include "UIElement.h"

using namespace ci;
using namespace std;

class TimeLabel : public UIElement {
    
public:
    
    TimeLabel() { mSeconds = -100000; }
    ~TimeLabel() {}
    
    void setup(const int &id, const Font &font, const Color &color)
    {
        UIElement::setup(id);
        mFont = font;
        mColor = color;
    }
    
    void draw();

    void setSeconds(int seconds);

private:
    
    void updateTexture();
    
    Font mFont;
    Color mColor;
    int mSeconds;

    gl::Texture mTexture, mHyphenTexture;
    
};