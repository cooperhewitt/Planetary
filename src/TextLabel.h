//
//  TextLabel.h
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

class TextLabel : public UIElement {
    
public:
    
    TextLabel() {}
    ~TextLabel() {}
    
    void setup(const int &id, const Font &font, const Color &color)
    {
        UIElement::setup(id);
        mFont = font;
        mColor = color;
    }
    
    void draw();

    void setText(string text);


private:
    
    void updateTexture();
    
    Font mFont;
    Color mColor;
    string mText;

    gl::Texture mTexture;
    
};