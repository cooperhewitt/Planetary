//
//  NotificationOverlay.cpp
//  Kepler
//
//  Created by Tom Carden on 6/2/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "NotificationOverlay.h"
#include "cinder/Font.h"
#include "cinder/Text.h"
#include "Globals.h"
#include <sstream>

using std::stringstream;

NotificationOverlay::NotificationOverlay() {
    mActive			= false;
    mFadeDelay		= 2.0f;
    mFadeDuration	= 1.0f;
}

NotificationOverlay::~NotificationOverlay() {
    hide();
}

void NotificationOverlay::setup( AppCocoaTouch *app, const Orientation &orientation, const Font &font )
{
    mApp = app;
    setInterfaceOrientation(orientation);
	mFont = font;
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
    
	Vec2f center = Vec2f( mInterfaceSize.x * 0.5f, mInterfaceSize.y - 250.0f );
	
    Vec2f iconTopLeft(			center.x - mCurrentSrcArea.getWidth()/2.0f, center.y - mCurrentSrcArea.getHeight()/2.0f );
    Vec2f iconBottomRight(		center.x + mCurrentSrcArea.getWidth()/2.0f, center.y + mCurrentSrcArea.getHeight()/2.0f );
    Rectf iconRect( iconTopLeft, iconBottomRight );
    
	float halfWidth = mMessageTexture.getWidth() * 0.5f;
	Vec2f messageTopLeft(		center.x - halfWidth, iconBottomRight.y - 10.0f );
	Vec2f messageBottomRight(	center.x + halfWidth, iconBottomRight.y + mMessageTexture.getHeight() - 10.0f );
	Rectf messageRect( messageTopLeft, messageBottomRight );
	
	float border = 20.0f;
	Vec2f blackBgTopLeft(		messageTopLeft.x - border, iconTopLeft.y - 5.0f );
	Vec2f blackBgBottomRight(	messageBottomRight.x + border, messageBottomRight.y + border );
	Rectf blackBgRect( blackBgTopLeft, blackBgBottomRight );
	
	glPushMatrix();
    glMultMatrixf( mOrientationMatrix );

	gl::color( ColorA( 1.0f, 1.0f, 1.0f, alpha ) );
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
    mCurrentTexture = texture;
    mCurrentSrcArea = srcArea;
    mCurrentMessage = message;
	
	TextLayout layout;	
	layout.setFont( mFont );
	layout.setColor( ColorA( BRIGHT_BLUE, 0.5f ) );
	stringstream s;
	s.str("");
	s << message;
	layout.addCenteredLine( s.str() );
	mMessageTexture = gl::Texture( layout.render( true, true ) );
	
	
    mActive = true;
    mLastShowTime = mApp->getElapsedSeconds();
}

void NotificationOverlay::hide()
{
    mActive = false;
    mCurrentTexture.reset();
}
