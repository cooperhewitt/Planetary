//
//  Slider.cpp
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Slider.h"
#include "BloomGl.h"
#include "cinder/gl/gl.h"

using namespace ci;

bool Slider::isDragging() 
{ 
    return mIsDragging; 
}
void Slider::setIsDragging(bool isDragging) 
{
    mIsDragging = isDragging; 
}

float Slider::getValue()
{ 
    return mValue; 
}
void Slider::setValue(float value)
{
    mValue = value;
}

void Slider::draw()
{   
    const float thumbProgress = (mRect.x2-mRect.x1) * mValue;
    const float midY = (mRect.y1 + mRect.y2) / 2.0f;
    
    const float thumbWidth = 28.0f;
    const float thumbHeight = 28.0f;
    
    Rectf fgRect(mRect.x1, mRect.y1, mRect.x1 + thumbProgress, mRect.y2);
    Rectf thumbRect(mRect.x1 + thumbProgress - thumbWidth/2.0f, 
                    midY - thumbHeight/2.0, 
                    mRect.x1 + thumbProgress + thumbWidth/2.0f, 
                    midY + thumbHeight/2.0);
    
    Area thumbTexArea = mIsDragging ? mThumbDownTexArea : mThumbUpTexArea; 
    
//    bloom::gl::batchRect(mTexture, mBgTexArea, mRect);            
//    bloom::gl::batchRect(mTexture, mFgTexArea, fgRect);
//    bloom::gl::batchRect(mTexture, thumbTexArea, thumbRect);        
    gl::draw(mTexture, mBgTexArea, mRect);            
    gl::draw(mTexture, mFgTexArea, fgRect);
    gl::draw(mTexture, thumbTexArea, thumbRect);        
}

bool Slider::touchBegan(ci::app::TouchEvent::Touch touch)
{
    if (mIsDragging) {
        // slider can only handle one touch
        return false;
    }
    Vec2f touchPos = globalToLocal( touch.getPos() );
    bool inside = mRect.contains( touchPos );
    setIsDragging(inside);
    return inside;
}
bool Slider::touchMoved(ci::app::TouchEvent::Touch touch)
{
    // adjust for orientation and offset
    Vec2f pos = globalToLocal( touch.getPos() );
    
    // FIXME: assumes slider is horizontal :)
    float sliderPer = (pos.x - mRect.x1) / (mRect.x2 - mRect.x1);
    if (sliderPer < 0.0f) 
        sliderPer = 0.0f;
    else if (sliderPer > 1.0f) 
        sliderPer = 1.0f;
    
    setValue( sliderPer ); 
    
    return true; // always consume drags for slider
}
bool Slider::touchEnded(ci::app::TouchEvent::Touch touch)
{
    touchMoved(touch);
    setIsDragging(false);
    return true;
}
