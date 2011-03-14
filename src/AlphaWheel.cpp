/*
 *  AlphaWheel.cpp
 *  Kepler
 *
 *  Created by Tom Carden on 3/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "AlphaWheel.h"
#include "cinder/gl/gl.h"
#include "cinder/Font.h"
#include "cinder/Text.h"
#include "cinder/ImageIo.h"
#include "Globals.h"
#include <sstream>

using std::stringstream;
using namespace ci;
using namespace ci::app;
using namespace std;

AlphaWheel::AlphaWheel()
{
}

AlphaWheel::~AlphaWheel()
{
	mApp->unregisterTouchesBegan( mCbTouchesBegan );
	mApp->unregisterTouchesMoved( mCbTouchesMoved );
	mApp->unregisterTouchesEnded( mCbTouchesEnded );
}


void AlphaWheel::setup( AppCocoaTouch *app )
{
	mApp = app;
	
	mCbTouchesBegan = mApp->registerTouchesBegan( this, &AlphaWheel::touchesBegan );
	mCbTouchesMoved = mApp->registerTouchesMoved( this, &AlphaWheel::touchesMoved );
	mCbTouchesEnded = mApp->registerTouchesEnded( this, &AlphaWheel::touchesEnded );
	
	// Textures
	mWheelTex		= gl::Texture( loadImage( loadResource( "wheel.png" ) ) );
	
	mAlphaString	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ#";
	mAlphaIndex		= 0;
	mAlphaChar		= ' ';
	mPrevAlphaChar	= ' ';
	mShowWheel		= false;
	mWheelScale		= 1.0f;	
}

void AlphaWheel::initAlphaTextures( const Font &font )
{
	for( int i=0; i<mAlphaString.length(); i++ ){
		TextLayout layout;	
		layout.setFont( font );
		layout.setColor( ColorA( 1.0f, 1.0f, 1.0f, 0.3f ) );
		stringstream s;
		s.str("");
		s << mAlphaString[i];
		layout.addCenteredLine( s.str() );
		mAlphaTextures.push_back( gl::Texture( layout.render( true, false ) ) );
	}
}

bool AlphaWheel::touchesBegan( TouchEvent event )
{
	std::cout << "AlphaWheel TouchesBegan" << std::endl;
	
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		mTouchPos = touchIt->getPos();
	}
	
	selectWheelItem( mTouchPos, false );
	
	return false;
}

bool AlphaWheel::touchesMoved( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		mTouchPos = touchIt->getPos();
	}
	
	selectWheelItem( mTouchPos, false );
	
	return false;
}

bool AlphaWheel::touchesEnded( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		mTouchPos = touchIt->getPos();
	}
	
	selectWheelItem( mTouchPos, true );
	
	return false;
}


void AlphaWheel::selectWheelItem( const Vec2f &pos, bool closeWheel )
{
	if( mShowWheel && mTimeSincePinchEnded > 0.5f){ 
		Vec2f dir				= pos - getWindowCenter();
		float distToCenter		= dir.length();
		if( distToCenter > 250 && distToCenter < 350 ){
			float touchAngle	= atan2( dir.y, dir.x ) + M_PI;				// RANGE 0 -> TWO_PI
			float anglePer		= ( touchAngle + 0.11365f + M_PI )/(M_PI * 2.0f);
			mAlphaIndex			= (int)( anglePer * 27 )%27;
			mPrevAlphaChar		= mAlphaChar;
			mAlphaChar			= mAlphaString.at( mAlphaIndex % mAlphaString.size() );
			if( mPrevAlphaChar != mAlphaChar || closeWheel ){
				mCallbacksAlphaCharSelected.call( this );
			}
			if( closeWheel ){
				mShowWheel = false;
				mCallbacksWheelClosed.call( this );
			}
		}
	}
}

void AlphaWheel::update( float fov, float timeSincePinchEnded )
{
	mTimeSincePinchEnded = timeSincePinchEnded;
	
	//mWheelScale = ( ( 130.0f - fov ) / 30.0f );
	if( getShowWheel() ){
		mWheelScale -= ( mWheelScale - 0.0f ) * 0.2f;
	} else {
		mWheelScale -= ( mWheelScale - 1.0f ) * 0.2f;	
	}	
}

void AlphaWheel::draw( )
{
	if( mWheelScale < 1.0f ){
		mWheelTex.enableAndBind();
		gl::pushModelView();
		gl::translate( getWindowCenter() );
		
		gl::scale( Vec3f( mWheelScale + 1.0f, mWheelScale + 1.0f, 1.0f ) );
		Rectf r = Rectf( -getWindowWidth() * 0.5f, -getWindowHeight() * 0.5f, getWindowWidth() * 0.5f, getWindowHeight() * 0.5f );
		float c = 1.0f - mWheelScale;
		gl::color( ColorA( c, c, c, c ) );
		gl::drawSolidRect( r );
		gl::popModelView();
		mWheelTex.disable();
		
		if( mAlphaChar != ' ' )
			drawAlphaChar();
	}
}

void AlphaWheel::drawAlphaChar()
{
	float w = mAlphaTextures[mAlphaIndex].getWidth() * 0.5f;
	float h = mAlphaTextures[mAlphaIndex].getHeight() * 0.5f;
	float x = getWindowWidth() * 0.5f;
	float y = getWindowHeight() * 0.5f;
	
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f - mWheelScale ) );
	mAlphaTextures[mAlphaIndex].enableAndBind();
	gl::drawSolidRect( Rectf( x - w, y - h, x + w, y + h ) );
	mAlphaTextures[mAlphaIndex].disable();
}
