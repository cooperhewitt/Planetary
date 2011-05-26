//
//  Slider.h
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "cinder/Rect.h"
#include "cinder/gl/gl.h"

using namespace ci;

class Slider {
public:
    
    Slider() {}
    ~Slider() {}
    
    void setup(int sliderId, 
               Rectf rect, 
               Rectf bgTexRect, 
               Rectf fgTexRect, 
               Rectf thumbDownTexRect, 
               Rectf thumbUpTexRect)
    {
        mSliderId = sliderId;
        // position:
        mRect = rect;
        // textures:
        mBgTexRect = bgTexRect;
        mFgTexRect = fgTexRect;
        mThumbDownTexRect = thumbDownTexRect;
        mThumbUpTexRect = thumbUpTexRect;
        // state:
        mIsDragging = false;
    }
    
    int getId() { return mSliderId; }
    
    Rectf getRect() { return mRect; }
    void setRect(Rectf rect) { mRect = rect; }
    
    float getValue() { return mValue; }
    void setValue(float value) { mValue = value; }
    
    void draw();
    
protected:
    
    void drawTextureRect(Rectf rect, Rectf textureRect);
    
    int mSliderId;    
    Rectf mRect; // position, settable
    
    Rectf mFgTexRect, mBgTexRect, mThumbDownTexRect, mThumbUpTexRect; // texture coords, fixed
    
    float mValue;
    bool mIsDragging;
    
};