//
//  ScrollingLabel.cpp
//  Kepler
//
//  Created by Tom Carden on 5/25/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#include "ScrollingLabel.h"
#include "cinder/app/AppCocoaTouch.h" // app::getElapsedSeconds()
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "BloomGl.h"

using namespace std;
using namespace ci;
using namespace ci::app;

void ScrollingLabel::setText(string text)
{ 
    if (mText != text) {
		mIsScrolling = false;
        mText = text; 
        updateTexture(); 
        mLastChangeTime = app::getElapsedSeconds();
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

void ScrollingLabel::update()
{    
    if (mTexture) {
    
        float ctSpaceWidth = mRect.x2 - mRect.x1;
        float ctTexWidth = mTexture.getWidth();
        
        if( ctTexWidth < ctSpaceWidth ){ 
            mIsScrolling = false;
        }
        else {
            // if the texture is too wide, animate the u coords...        
            float elapsedSeconds = ci::app::getElapsedSeconds();
            float x1;
            if( elapsedSeconds - mLastChangeTime > 5.0f ) { 
                // but wait 5 seconds first
                mIsScrolling = true;
                x1 = fmodf( ( elapsedSeconds - ( mLastChangeTime + 5.0f ) ) * 20.0f, ctTexWidth + 10.0f ) - ctTexWidth-10.0f;
            } else {
                x1 = -ctTexWidth-10.0f;
            }
            
            mFirstArea = Area( x1, 0.0f, x1 + ctSpaceWidth, mTexture.getHeight() );
            mSecondArea = Area( x1 + ctTexWidth + 10.0f, 0.0f, x1 + ctTexWidth + 10.0f + ctSpaceWidth, mTexture.getHeight() );
        }    
    }
}

void ScrollingLabel::draw()
{    
    if (mTexture) {
        
        // force texture height to be correct:
        float restoreY2 = mRect.y2;
        mRect.y2 = mRect.y1 + mTexture.getHeight();
        
        float ctSpaceWidth = mRect.x2 - mRect.x1;
        float ctTexWidth = mTexture.getWidth();
        
        if( ctTexWidth < ctSpaceWidth ){ 
            // if the texture width is less than the rect to fit it in...
            bloom::gl::batchRect( mTexture, mRect.getUpperLeft() );
        }
        else {
            bloom::gl::batchRect( mTexture, mFirstArea, mRect );
            bloom::gl::batchRect( mTexture, mSecondArea, mRect );
        }    
        
        // restore mRect
        // (this is a hack because mRect is also used for hit testing)
        mRect.y2 = restoreY2;
    }
}

bool ScrollingLabel::touchBegan( TouchEvent::Touch touch )
{
    return mRect.contains( globalToLocal( touch.getPos() ) );
}

bool ScrollingLabel::touchEnded( TouchEvent::Touch touch )
{
    return mRect.contains( globalToLocal( touch.getPos() ) );
}
