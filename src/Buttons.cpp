//
//  Buttons.cpp
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "cinder/gl/gl.h"
#include "Buttons.h"
#include "BloomGl.h"

void ToggleButton::draw()
{
    Area textureArea = mOn ? mOnTextureArea : mOffTextureArea;
    //gl::draw(mTexture, textureArea, mRect);
    bloom::gl::batchRect(mTexture, textureArea, mRect);
}

void SimpleButton::draw()
{
    Area textureArea = mDown ? mDownTextureArea : mUpTextureArea;
    //gl::draw(mTexture, textureArea, mRect);
    bloom::gl::batchRect(mTexture, textureArea, mRect);
}

void TwoStateButton::draw()
{
    Area textureArea = mOn ? (mDown ? mOnDownTextureArea : mOnUpTextureArea) : (mDown ? mOffDownTextureArea : mOffUpTextureArea) ;    
    //gl::draw(mTexture, textureArea, mRect);
    bloom::gl::batchRect(mTexture, textureArea, mRect);
}
