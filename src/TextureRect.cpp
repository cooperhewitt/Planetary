//
//  TextureRect.cpp
//  Kepler
//
//  Created by Tom Carden on 7/18/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#include "TextureRect.h"
#include "cinder/gl/gl.h"
#include "BloomGl.h"

using namespace ci;

void TextureRect::draw()
{
    bloom::gl::batchRect( mTexture, mArea, mRect );
}