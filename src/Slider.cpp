//
//  Slider.cpp
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Slider.h"
#include "BloomGl.h"

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

void Slider::setup(int id, 
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
    mValue = 0.0f;
    mIsDragging = false;
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
    
    bloom::gl::batchRect(mTexture, mBgTexArea, mRect);            
    bloom::gl::batchRect(mTexture, mFgTexArea, fgRect);
    bloom::gl::batchRect(mTexture, thumbTexArea, thumbRect);        
}
