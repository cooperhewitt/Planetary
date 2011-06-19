//
//  Slider.cpp
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Slider.h"

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

    gl::draw(mTexture, mBgTexArea, mRect);    
	//gl::enableAdditiveBlending();    
    gl::draw(mTexture, mFgTexArea, fgRect);
	//gl::enableAlphaBlending();    
    gl::draw(mTexture, thumbTexArea, thumbRect);
}