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
    ~Slider() {
        // FIXME: clear VBOs and verts
    }
    
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

    virtual void setRect(const Rectf &rect);
    virtual void setRect(const float &x1, const float &y1, const float &x2, const float &y2);
    
    void draw();
    
protected:

    struct VertexData {
        ci::Vec2f vertex;
        ci::Vec2f texture;
    };    
    
    Area mFgTexArea, mBgTexArea, mThumbDownTexArea, mThumbUpTexArea; // texture coords, fixed
    gl::Texture mTexture;
    
    float mValue;
    bool mIsDragging;
    
    void updateVerts();
    
    GLuint mNumVerts;
    VertexData *mVerts;
    
};