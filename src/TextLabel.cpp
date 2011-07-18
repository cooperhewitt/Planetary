//
//  TextLabel.cpp
//  Kepler
//
//  Created by Tom Carden on 5/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "TextLabel.h"
#include "BloomGl.h"

using namespace std;
using namespace ci;

void TextLabel::setText(string text)
{ 
    if (mText != text) { 
        mText = text; 
        updateTexture(); 
    }
}

void TextLabel::updateTexture()
{
    TextLayout layout;
    layout.setFont( mFont );
    layout.setColor( mColor );
    layout.addLine( mText );
    mTexture = layout.render( true, false );
}

void TextLabel::draw()
{
    if (mTexture) {
        // use this even though the texture is unique because it minimizes state changes
//        bloom::gl::batchRect(mTexture, mRect.getUpperLeft());
        gl::draw(mTexture, mRect.getUpperLeft());
    }
    // draw children:
    UINode::draw();
}


bool TextLabel::touchBegan(ci::app::TouchEvent::Touch touch)
{
    return mRect.contains( globalToLocal( touch.getPos() ) );
}
bool TextLabel::touchEnded(ci::app::TouchEvent::Touch touch)
{
    return mRect.contains( globalToLocal( touch.getPos() ) );
}
