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
               Area thumbUpTexArea);

    bool isDragging();
    void setIsDragging(bool isDragging);

    float getValue();
    void setValue(float value);

    void draw();
    
protected:

    Area mFgTexArea, mBgTexArea, mThumbDownTexArea, mThumbUpTexArea; // texture coords, fixed
    gl::Texture mTexture;
    
    float mValue;
    bool mIsDragging;
    
    void updateVerts();    
};