//
//  TimeLabel.cpp
//  Kepler
//
//  Created by Tom Carden on 5/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "TimeLabel.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"

void TimeLabel::setSeconds(int seconds)
{ 
    if (mSeconds != seconds) { 
        mSeconds = seconds; 
        updateTexture(); 
    }
}

void TimeLabel::updateTexture()
{
	int hours	= floor(abs(mSeconds)/3600.0f);
    int minutes = floor(abs(mSeconds)/60.0f);
	minutes = minutes - hours * 60;
    int seconds = (int)abs(mSeconds)%60;
    
	string hourStr = ci::toString( hours );
    string minsStr = ci::toString( minutes );
    string secsStr = ci::toString( seconds );
    if( minsStr.length() == 1 ) minsStr = "0" + minsStr;
    if( secsStr.length() == 1 ) secsStr = "0" + secsStr;		
	
    stringstream ss;
	if( hours > 0 )
		ss << hourStr << ":";
    ss << minsStr << ":" << secsStr << endl;
        
    TextLayout layout;
    layout.setFont( mFont );
    layout.setColor( mColor );
    layout.addLine( ss.str() );
    mTexture = layout.render( true, false );

    if (mSeconds <= 0) {
        TextLayout hyphenLayout;
        hyphenLayout.setFont( mFont );
        hyphenLayout.setColor( mColor );
        hyphenLayout.addLine( "-" );
        mHyphenTexture = hyphenLayout.render( true, false );
    }
    else {
        mHyphenTexture.reset();
    }
}

void TimeLabel::draw()
{
    // to keep the digit left-aligned in the box, squeeze the minus sign in before it
    if (mHyphenTexture) {
        gl::draw(mHyphenTexture, mRect.getUpperLeft() - Vec2f(mHyphenTexture.getWidth()-1.0f,0));
    }    
    if (mTexture) {
        gl::draw(mTexture, mRect.getUpperLeft());
    }
}

bool TimeLabel::touchBegan(ci::app::TouchEvent::Touch touch)
{
    return mRect.contains( globalToLocal( touch.getPos() ) );
}
bool TimeLabel::touchEnded(ci::app::TouchEvent::Touch touch)
{
    return mRect.contains( globalToLocal( touch.getPos() ) );
}

