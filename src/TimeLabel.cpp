//
//  TimeLabel.cpp
//  Kepler
//
//  Created by Tom Carden on 5/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "TimeLabel.h"

void TimeLabel::setTime(float time)
{ 
    if (mTime != time) { 
        mTime = time; 
        updateTexture(); 
    }
}

void TimeLabel::updateTexture()
{
    TextLayout layout;
    layout.setFont( mFont );
    layout.setColor( mColor );			
    layout.addLine( "-00:00" ); // FIXME
    bool PREMULT = false;
    mTexture = gl::Texture( layout.render( true, PREMULT ) );        
}

void TimeLabel::draw()
{
    if (mTexture) {
        gl::draw(mTexture, mRect.getUpperLeft());
    }
}
