//
//  Slider.h
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "UIElement.h"
#include "cinder/Rect.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

using namespace ci;

class Slider : public UIElement {
public:
    
    Slider() {}
    ~Slider() {}
    
    void setup(int id, 
               const gl::Texture &texture,               
               Rectf bgTexRect, 
               Rectf fgTexRect, 
               Rectf thumbDownTexRect, 
               Rectf thumbUpTexRect)
    {
        UIElement::setup(id);
        mTexture = texture;
        // texture rects:
        mBgTexRect = bgTexRect;
        mFgTexRect = fgTexRect;
        mThumbDownTexRect = thumbDownTexRect;
        mThumbUpTexRect = thumbUpTexRect;
        // state:
        mIsDragging = false;
    }

    bool isDragging() { return mIsDragging; }
    void setIsDragging(bool isDragging) { mIsDragging = isDragging; }

    float getValue() { return mValue; }
    void setValue(float value) { mValue = value; }
    
    void draw();
    
protected:
    
    void drawTextureRect(Rectf rect, Rectf textureRect);

    Rectf mFgTexRect, mBgTexRect, mThumbDownTexRect, mThumbUpTexRect; // texture coords, fixed
    gl::Texture mTexture;
    
    float mValue;
    bool mIsDragging;
    
};