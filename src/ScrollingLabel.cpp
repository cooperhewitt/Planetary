//
//  ScrollingLabel.cpp
//  Kepler
//
//  Created by Tom Carden on 5/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ScrollingLabel.h"
#include "cinder/app/AppCocoaTouch.h" // app::getElapsedSeconds()
#include "cinder/gl/gl.h"
#include "cinder/Text.h"

using namespace std;
using namespace ci;

void ScrollingLabel::setText(string text)
{ 
    if (mText != text) {
		mIsScrolling = false;
        mText = text; 
        updateTexture(); 
        mLastTrackChangeTime = app::getElapsedSeconds();
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
        
        float restoreY2 = mRect.y2;
        
        // force texture height to be correct:
        mRect.y2 = mRect.y1 + mTexture.getHeight();
                
        float ctSpaceWidth = mRect.x2 - mRect.x1;
        float ctTexWidth = mTexture.getWidth();
        
        if( ctTexWidth < ctSpaceWidth ){ 
			mIsScrolling = false;
            // if the texture width is less than the rect to fit it in...
            gl::draw(mTexture, mRect.getUpperLeft());            
        }
        else {
            // if the texture is too wide, animate the u coords...        
            float elapsedSeconds = ci::app::getElapsedSeconds();
            float x1;
            if( elapsedSeconds - mLastTrackChangeTime > 5.0f ) { 
                // but wait 5 seconds first
				mIsScrolling = true;
                x1 = fmodf( ( elapsedSeconds - ( mLastTrackChangeTime + 5.0f ) ) * 20.0f, ctTexWidth + 10.0f ) - ctTexWidth-10.0f;
            } else {
                x1 = -ctTexWidth-10.0f;
            }
            
            Area a1( x1, 0.0f, 
                     x1 + ctSpaceWidth, mTexture.getHeight() );
            gl::draw( mTexture, a1, mRect );
            
            Area a2( x1 + ctTexWidth + 10.0f, 0.0f, 
                     x1 + ctTexWidth + 10.0f + ctSpaceWidth, mTexture.getHeight() );
            gl::draw( mTexture, a2, mRect );			
        }    
    
        mRect.y2 = restoreY2; // this is a hack because mRect is also used for hit testing
    }
}

bool ScrollingLabel::touchBegan(ci::app::TouchEvent::Touch touch)
{
    return mRect.contains( globalToLocal( touch.getPos() ) );
}
bool ScrollingLabel::touchEnded(ci::app::TouchEvent::Touch touch)
{
    return mRect.contains( globalToLocal( touch.getPos() ) );
}
