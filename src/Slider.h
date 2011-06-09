//
//  Slider.h
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "UIElement.h"
#include "cinder/Area.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

using namespace ci;

class Slider : public UIElement {
public:
    
    Slider() {}
    ~Slider() {}
    
    void setup(int id, 
               const gl::Texture &texture,               
               Area bgTexArea, 
               Area fgTexArea, 
               Area thumbDownTexArea, 
               Area thumbUpTexArea)
    {
        UIElement::setup(id);
        mTexture = texture;
        // texture Areas:
        mBgTexArea = bgTexArea;
        mFgTexArea = fgTexArea;
        mThumbDownTexArea = thumbDownTexArea;
        mThumbUpTexArea = thumbUpTexArea;
        // state:
        mIsDragging = false;
    }

    bool isDragging() { return mIsDragging; }
    void setIsDragging(bool isDragging) { mIsDragging = isDragging; }

    float getValue() { return mValue; }
    void setValue(float value) { mValue = value; }
    
    void draw();
    
protected:
    
    Area mFgTexArea, mBgTexArea, mThumbDownTexArea, mThumbUpTexArea; // texture coords, fixed
    gl::Texture mTexture;
    
    float mValue;
    bool mIsDragging;
    
};