//
//  Buttons.cpp
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "cinder/gl/gl.h"
#include "Buttons.h"

void ToggleButton::draw()
{
    Rectf textureRect = mOn ? mOnTextureRect : mOffTextureRect;
    
    UIElement::drawTextureRect(textureRect);            
}

void SimpleButton::draw()
{
    Rectf textureRect = mDown ? mDownTextureRect : mUpTextureRect;
    
    UIElement::drawTextureRect(textureRect);      
}

void TwoStateButton::draw()
{
    Rectf textureRect = mOn ? (mDown ? mOnDownTextureRect : mOnUpTextureRect) : (mDown ? mOffDownTextureRect : mOffUpTextureRect) ;
    
    UIElement::drawTextureRect(textureRect);    
}
