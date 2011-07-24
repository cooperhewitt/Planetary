//
//  TextureRect.cpp
//  Kepler
//
//  Created by Tom Carden on 7/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "TextureRect.h"
#include "cinder/gl/gl.h"

using namespace ci;

void TextureRect::draw()
{
    gl::draw( mTexture, mArea, mRect );
}