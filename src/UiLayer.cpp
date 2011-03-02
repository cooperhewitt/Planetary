/*
 *  UiLayer.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "UiLayer.h"
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

const static int NAV_H = 50;
const static int NAV_MARGIN = 2;
const static int SHOW_WHEEL_BTN_W = 50;

UiLayer::UiLayer()
{
}

UiLayer::~UiLayer()
{
	mApp->unregisterTouchesBegan( mCbTouchesBegan );
	mApp->unregisterTouchesMoved( mCbTouchesMoved );
	mApp->unregisterTouchesEnded( mCbTouchesEnded );
}
 
 
void UiLayer::setup( AppCocoaTouch *app )
{
	mApp = app;
	
	mCbTouchesBegan = mApp->registerTouchesBegan( this, &UiLayer::touchesBegan );
	mCbTouchesMoved = mApp->registerTouchesMoved( this, &UiLayer::touchesMoved );
	mCbTouchesEnded = mApp->registerTouchesEnded( this, &UiLayer::touchesEnded );
	
	// Rects
	int x1 = 0.0f;
	int y1 = getWindowHeight() - NAV_H;
	int x2 = getWindowWidth();
	int y2 = getWindowHeight();
	mNavRect			 = Rectf( x1, y1, x2, y2 );
	
	// Textures
	mWheelTex		= gl::Texture( loadImage( loadResource( "wheel.png" ) ) );
	
	mAlphaString	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ#";
	mAlphaIndex		= 0;
	mAlphaChar		= ' ';
	mPrevAlphaChar	= ' ';
	mShowWheel		= false;
}

void UiLayer::initAlphaTextures( const Font &font )
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
 
bool UiLayer::touchesBegan( TouchEvent event )
{
	std::cout << "UiLayer TouchesBegan" << std::endl;
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		mTouchPos = touchIt->getPos();
		
		selectWheelItem( mTouchPos, false );
	}
	
	return false;
}

bool UiLayer::touchesMoved( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		mTouchPos = touchIt->getPos();
		
		selectWheelItem( mTouchPos, false );
	}

	return false;
}

bool UiLayer::touchesEnded( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		mTouchPos = touchIt->getPos();
	}
	
	if( mTouchPos.x < 50.0f && mTouchPos.y < 50.0f ){
		G_LOCK_TO_LEFT_SIDE = !G_LOCK_TO_LEFT_SIDE;
	}
	
	selectWheelItem( mTouchPos, true );
	
	return false;
}


void UiLayer::selectWheelItem( const Vec2f &pos, bool closeWheel )
{
	if( mShowWheel && ! mNavRect.contains( pos ) ){
		Vec2f dir				= pos - getWindowCenter();
		float distToCenter		= dir.length();
		if( distToCenter > 250 && distToCenter < 350 ){
			float touchAngle	= atan2( dir.y, dir.x ) + M_PI;				// RANGE 0 -> TWO_PI
			float anglePer		= ( touchAngle + 0.11365f + M_PI )/(M_PI * 2.0f);
			mAlphaIndex			= (int)( anglePer * 27 )%27;
			mPrevAlphaChar		= mAlphaChar;
			mAlphaChar			= mAlphaString.at( mAlphaIndex % mAlphaString.size() );
			if( mPrevAlphaChar != mAlphaChar ){
				mCallbacksAlphaCharSelected.call( this );
			}
			if( closeWheel ){
				mShowWheel = false;
			}
		}
	}
}

void UiLayer::draw()
{
	drawWheel();
	drawNav();
}

void UiLayer::drawWheel()
{
	if( mShowWheel ){
		gl::color( Color( 1.0f, 1.0f, 1.0f ) );
		mWheelTex.enableAndBind();
		gl::drawSolidRect( getWindowBounds() );
		mWheelTex.disable();
		
		if( mAlphaChar != ' ' )
			drawAlphaChar();
	}
}

void UiLayer::drawAlphaChar()
{
	float w = mAlphaTextures[mAlphaIndex].getWidth() * 0.5f;
	float h = mAlphaTextures[mAlphaIndex].getHeight() * 0.5f;
	float x = getWindowWidth() * 0.5f;
	float y = getWindowHeight() * 0.5f;
		
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	mAlphaTextures[mAlphaIndex].enableAndBind();
	gl::drawSolidRect( Rectf( x - w, y - h, x + w, y + h ) );
	mAlphaTextures[mAlphaIndex].disable();
}

void UiLayer::drawNav()
{
	gl::color( Color::black() );
	gl::drawSolidRect( mNavRect );
}

