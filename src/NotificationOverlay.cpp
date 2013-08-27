//
//  NotificationOverlay.cpp
//  Kepler
//
//  Created by Tom Carden on 6/2/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
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
    mat.translate(Vec3f( interfaceSize.x * 0.5f, interfaceSize.y * 0.5f + 184.0f - mMessageTexture.getHeight(), 0.0f ));
    setTransform(mat);
}

void NotificationOverlay::draw()
{
    if (!mActive) return;

    gl::enableAdditiveBlending(); // as long as this is the last thing.
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, mAlpha ) );
	gl::draw( mMessageTexture, mMessageRect );
    
    bool hasOverGraphic = mCurrentSecondSrcArea.getWidth() != 0;
    if (hasOverGraphic) {
        // under graphic should be dimmer
        gl::color( ColorA( 1.0f, 1.0f, 1.0f, mAlpha * 0.25f ) );
    }
    // TODO: batch these calls if using the same texture, avoid cinder::gl::draw() overhead?
    gl::draw( mCurrentTexture, mCurrentSrcArea, mIconRect );
    
    if (hasOverGraphic) {
        // overgraphic should be same alpha as text?
        gl::color( ColorA( 1.0f, 1.0f, 1.0f, mAlpha ) );
        gl::draw( mCurrentTexture, mCurrentSecondSrcArea, mIconRect );        
    }
}

void NotificationOverlay::show(const gl::Texture &texture, const Area &srcArea, const string &message)
{
    show(texture, srcArea, Area(0,0,0,0), message);
}

void NotificationOverlay::show( const ci::gl::Texture &texture, const ci::Area &srcArea1, const ci::Area &srcArea2, const std::string &message )
{
    if (!mSetup) return;
    
    mCurrentTexture = texture;
    mCurrentSrcArea = srcArea1;
    mCurrentSecondSrcArea = srcArea2;
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
	
    gl::Texture texture = gl::Texture( charLayout.render( true, true ) );
    
    show( texture, 
          Area( 0, 0, texture.getWidth(), texture.getHeight() + 15.0f ), 
          Area( 0, 0, 0, 0 ),
          message );
}

void NotificationOverlay::hide()
{
    mActive = false;
    mCurrentTexture.reset();
}
