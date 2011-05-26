//
//  ScrollingLabel.cpp
//  Kepler
//
//  Created by Tom Carden on 5/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ScrollingLabel.h"

void ScrollingLabel::setText(string text)
{ 
    if (mText != text) { 
        mText = text; 
        updateTexture(); 
    }
}

void ScrollingLabel::updateTexture()
{
    if (mText == "") {
        mTexture.reset();
    }
    else {
		TextLayout layout;
		layout.setFont( mFont );
		layout.setColor( mColor );			
		layout.addLine( mText );
		bool PREMULT = false;
		mTexture = gl::Texture( layout.render( true, PREMULT ) );        
    }
}

void ScrollingLabel::draw()
{
    if (mTexture) {
        gl::draw(mTexture, mRect.getUpperLeft());    
    }
}
