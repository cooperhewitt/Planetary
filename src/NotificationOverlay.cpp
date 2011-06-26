//
//  NotificationOverlay.cpp
//  Kepler
//
//  Created by Tom Carden on 6/2/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "NotificationOverlay.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <sstream>
#include "cinder/Font.h"
#include "cinder/Text.h"
#include "Globals.h"

using std::stringstream;

NotificationOverlay::NotificationOverlay() 
{
    mActive			= false;
    mSetup          = false;
    mFadeDelay		= 2.0f;
    mFadeDuration	= 1.0f;
}

NotificationOverlay::~NotificationOverlay() 
{
    hide();
}

void NotificationOverlay::setup( AppCocoaTouch *app, const Orientation &orientation, const Font &font )
{
    mApp = app;
    setInterfaceOrientation(orientation);
	mFont = font;
    mSetup = true;
}

void NotificationOverlay::update()
{
    if (!mActive) return;
    
    // if enough time has passed, hide the overlay (cleanup happens in hide())
    float elapsedSince = mApp->getElapsedSeconds() - mLastShowTime;
    if (elapsedSince > mFadeDelay + mFadeDuration) {
        hide();
    }
}

void NotificationOverlay::draw()
{
    if (!mActive) return;
    
    float elapsedSince = mApp->getElapsedSeconds() - mLastShowTime;
    
    float alpha = 1.0f;
        
    if (elapsedSince > mFadeDelay) {
        alpha = 1.0f - (elapsedSince - mFadeDelay) / mFadeDuration;
    }
    
	Vec2f pos = Vec2f( mInterfaceSize.x * 0.5f, mInterfaceSize.y - 250.0f - mMessageTexture.getHeight() );
	Vec2f iconSize = mCurrentSrcArea.getSize();
    
    Rectf iconRect( pos - iconSize/2.0f, pos + iconSize/2.0f );
    
	float halfWidth = mMessageTexture.getWidth() * 0.5f;
	Vec2f messageTopLeft(	  pos.x - halfWidth, iconRect.y2 - 10.0f );
	Vec2f messageBottomRight( pos.x + halfWidth, iconRect.y2 + mMessageTexture.getHeight() - 10.0f );
	Rectf messageRect( messageTopLeft, messageBottomRight );
		
	glPushMatrix();
    glMultMatrixf( mOrientationMatrix );
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, alpha ) );
    // TODO: batch these calls, avoid cinder::gl::draw()
	gl::draw( mMessageTexture, messageRect );
    gl::draw( mCurrentTexture, mCurrentSrcArea, iconRect );
	glPopMatrix(); 
}

void NotificationOverlay::setInterfaceOrientation( const Orientation &orientation )
{
    mInterfaceOrientation = orientation;
    mOrientationMatrix = getOrientationMatrix44(mInterfaceOrientation, getWindowSize());
    
    mInterfaceSize = getWindowSize();
    
    if ( isLandscapeOrientation( mInterfaceOrientation ) ) {
        mInterfaceSize = mInterfaceSize.yx(); // swizzle it!
    }
}

void NotificationOverlay::show(const gl::Texture &texture, const Area &srcArea, const string &message)
{
    if (!mSetup) return;
    
    mCurrentTexture = texture;
    mCurrentSrcArea = srcArea;
    mCurrentMessage = message;
	
	TextLayout layout;	
	layout.setFont( mFont );
	layout.setColor( ColorA( BRIGHT_BLUE, 0.5f ) );
    vector<string> results;
    boost::split(results, message, boost::is_any_of("\n"));     
    for (int i = 0; i < results.size(); i++) {
        layout.addCenteredLine( results[i] );
    }
	mMessageTexture = gl::Texture( layout.render( true, true ) );
	
    mActive = true;
    mLastShowTime = mApp->getElapsedSeconds();
}

void NotificationOverlay::hide()
{
    mActive = false;
    mCurrentTexture.reset();
}
