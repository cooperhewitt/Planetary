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
#include "BloomScene.h"
#include "cinder/app/AppCocoaTouch.h" // FIXME: just for time

using namespace std;
using namespace ci;
using namespace ci::app;

NotificationOverlay::NotificationOverlay() 
{
    mActive			= false;
    mSetup          = false;
    mFadeDelay		= 1.0f;
    mFadeDuration	= 0.5f;
}

NotificationOverlay::~NotificationOverlay() 
{
    hide();
}

void NotificationOverlay::setup( const Font &font )
{
	mFont = font;
    mSetup = true;
}

void NotificationOverlay::update()
{
    if (!mActive) return;
    
    // if enough time has passed, hide the overlay (cleanup happens in hide())
    float elapsedSince = app::getElapsedSeconds() - mLastShowTime;
    if (elapsedSince > mFadeDelay + mFadeDuration) {
        hide();
    }
    
    mAlpha = 1.0f;
    if (elapsedSince > mFadeDelay) {
        mAlpha = 1.0f - (elapsedSince - mFadeDelay) / mFadeDuration;
    }    
    
    Vec2f interfaceSize = getRoot()->getInterfaceSize();
    
    Matrix44f mat;
    mat.translate(Vec3f( interfaceSize.x * 0.5f, interfaceSize.y * 0.5f + 134.0f - mMessageTexture.getHeight(), 0.0f ));
    setTransform(mat);
}

void NotificationOverlay::draw()
{
    if (!mActive) return;
    gl::enableAdditiveBlending(); // as long as this is the last thing.
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, mAlpha ) );
    // TODO: batch these calls, avoid cinder::gl::draw()
	gl::draw( mMessageTexture, mMessageRect );
    gl::draw( mCurrentTexture, mCurrentSrcArea, mIconRect );
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

	Vec2f iconSize = mCurrentSrcArea.getSize();
    mIconRect = Rectf( -iconSize/2.0f, iconSize/2.0f );
    
	float halfWidth = mMessageTexture.getWidth() * 0.5f;
	Vec2f messageTopLeft( -halfWidth, mIconRect.y2 - 10.0f );
	Vec2f messageBottomRight( halfWidth, mIconRect.y2 + mMessageTexture.getHeight() - 10.0f );
	mMessageRect = Rectf( messageTopLeft, messageBottomRight );
	
    mActive = true;
    mLastShowTime = app::getElapsedSeconds();
}

void NotificationOverlay::showLetter( const char &c, const string &message, const Font &hugeFont )
{
    if (!mSetup) return;

	TextLayout charLayout;
	charLayout.setFont( hugeFont );
	charLayout.setColor( ColorA( BRIGHT_BLUE, 0.5f ) );
	string s = "";
	s += c;
	charLayout.addCenteredLine( s );
	mCurrentTexture = gl::Texture( charLayout.render( true, true ) );    
	mCurrentSrcArea = Area( 0, 0, mCurrentTexture.getWidth(), mCurrentTexture.getHeight() + 15.0f );
    mCurrentMessage = message;
	
    // FIXME: at this point, just call show(...) with the right params and avoid mismatching functionality
    
	TextLayout layout;
	layout.setFont( mFont );
	layout.setColor( ColorA( BRIGHT_BLUE, 0.5f ) );
    vector<string> results;
    boost::split(results, message, boost::is_any_of("\n"));     
    for (int i = 0; i < results.size(); i++) {
        layout.addCenteredLine( results[i] );
    }
	mMessageTexture = gl::Texture( layout.render( true, true ) );

	Vec2f iconSize = mCurrentSrcArea.getSize();
    mIconRect = Rectf( -iconSize/2.0f, iconSize/2.0f );
    
	float halfWidth = mMessageTexture.getWidth() * 0.5f;
	Vec2f messageTopLeft( -halfWidth, mIconRect.y2 - 10.0f );
	Vec2f messageBottomRight( halfWidth, mIconRect.y2 + mMessageTexture.getHeight() - 10.0f );
	mMessageRect = Rectf( messageTopLeft, messageBottomRight );	
    
    mActive = true;
    mLastShowTime = app::getElapsedSeconds();
}

void NotificationOverlay::hide()
{
    mActive = false;
    mCurrentTexture.reset();
}
