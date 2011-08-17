//
//  Buttons.cpp
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Buttons.h"
#include "cinder/gl/gl.h"

using namespace ci;

void ToggleButton::draw()
{
    Area textureArea = mOn ? mOnTextureArea : mOffTextureArea;
    gl::draw(mTexture, textureArea, mRect);
}

bool ToggleButton::touchBegan(ci::app::TouchEvent::Touch touch)
{
    return mRect.contains( globalToLocal( touch.getPos() ) );
}
bool ToggleButton::touchEnded(ci::app::TouchEvent::Touch touch)
{
    return mRect.contains( globalToLocal( touch.getPos() ) );
}

void SimpleButton::draw()
{
    Area textureArea = mDownCount ? mDownTextureArea : mUpTextureArea;
    gl::draw(mTexture, textureArea, mRect);    
}
bool SimpleButton::touchBegan(ci::app::TouchEvent::Touch touch)
{
    bool inside = mRect.contains( globalToLocal( touch.getPos() ) );
    if (inside) mDownCount++;
    return inside;
}
bool SimpleButton::touchEnded(ci::app::TouchEvent::Touch touch)
{
    mDownCount--;
    return mRect.contains( globalToLocal( touch.getPos() ) );
}

void TwoStateButton::draw()
{
    Area textureArea = mOn ? (mDownCount > 0 ? mOnDownTextureArea : mOnUpTextureArea) : (mDownCount > 0 ? mOffDownTextureArea : mOffUpTextureArea) ;    
    gl::draw(mTexture, textureArea, mRect);    
}
bool TwoStateButton::touchBegan(ci::app::TouchEvent::Touch touch)
{
    bool inside = mRect.contains( globalToLocal( touch.getPos() ) );
    if (inside) mDownCount++;
    return inside;    
}
bool TwoStateButton::touchEnded(ci::app::TouchEvent::Touch touch)
{
    mDownCount--;    
    return mRect.contains( globalToLocal( touch.getPos() ) );
}


void ThreeStateButton::draw()
{
    Area textureArea = mState == 0 ? mFirstTextureArea : mState == 1 ? mSecondTextureArea : mThirdTextureArea;
    gl::draw(mTexture, textureArea, mRect);    
}
bool ThreeStateButton::touchBegan(ci::app::TouchEvent::Touch touch)
{
    return mRect.contains( globalToLocal( touch.getPos() ) );
}
bool ThreeStateButton::touchEnded(ci::app::TouchEvent::Touch touch)
{
    return mRect.contains( globalToLocal( touch.getPos() ) );
}
