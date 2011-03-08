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

const static int NAV_H = 35;
const static int NAV_MARGIN = 2;

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
	mStripRect			 = Rectf( x1, y1, x2, y2 );
	
	// Textures
	mWheelTex		= gl::Texture( loadImage( loadResource( "wheel.png" ) ) );
	
	mAlphaString	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ#";
	mAlphaIndex		= 0;
	mAlphaChar		= ' ';
	mPrevAlphaChar	= ' ';
	mShowWheel		= false;
	
	// PANEL AND TAB
	mPanelRect			= Rectf( 0.0f, 0.0f, getWindowWidth(), 75.0f );
	mPanelOpenYPos		= getWindowHeight() - mPanelRect.y2;
	mPanelClosedYPos	= getWindowHeight();
	mPanelYPos			= mPanelClosedYPos;
	mPanelYPosDest		= mPanelOpenYPos;
	setPanelPos( mPanelYPos, true );
	mIsPanelTabTouched	= false;
	mIsPanelOpen		= true;
	mHasPanelBeenDragged = false;
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
	
	mHasPanelBeenDragged = false;
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		mTouchPos = touchIt->getPos();
	}
	
	if( mIsPanelOpen ){
		selectWheelItem( mTouchPos, false );
	}
	
	if( mPanelTabRect.contains( mTouchPos ) ){
		mPanelTabTouchYOffset = mPanelPos.y - mTouchPos.y;
		mIsPanelTabTouched = true;
	} else {
		mIsPanelTabTouched = false;
	}
		
	return false;
}

bool UiLayer::touchesMoved( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		mTouchPos = touchIt->getPos();
	}
	
	if( mIsPanelOpen ){
		selectWheelItem( mTouchPos, false );
	}
	if( mIsPanelTabTouched ){
		mHasPanelBeenDragged = true;
		setPanelPos( mTouchPos.y, false );
	}

	return false;
}

bool UiLayer::touchesEnded( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		mTouchPos = touchIt->getPos();
	}
	
	if( mTouchPos.x < 50.0f && mTouchPos.y < 50.0f ){
		G_DEBUG = !G_DEBUG;
	}
	
	if( mIsPanelOpen ){
		selectWheelItem( mTouchPos, true );
	}
	
	if( mIsPanelTabTouched ){
		if( mHasPanelBeenDragged ){
			setPanelPos( mTouchPos.y, true );
			mIsPanelTabTouched		= false;
			mHasPanelBeenDragged	= false;
		} else {
			if( mIsPanelOpen ){
				mPanelYPosDest = mPanelClosedYPos;
			} else {
				mPanelYPosDest = mPanelOpenYPos;
			}
		}
		
		
	}
	
	return false;
}


void UiLayer::setPanelPos( float y, bool doneDragging )
{
	mPanelYPosDest		= y + mPanelTabTouchYOffset;
	
	// if the mPanel y position is outside of the max and min, clamp it
	if( mPanelYPosDest <= mPanelOpenYPos ){
		mIsPanelOpen	= true;
		mPanelYPosDest		= mPanelOpenYPos;
		
	} else if( mPanelYPosDest >= mPanelClosedYPos ) {
		mIsPanelOpen	= false;
		mPanelYPosDest		= mPanelClosedYPos;
	}
	
	
	if( doneDragging ){
		if( mPanelYPos < mPanelOpenYPos + mPanelRect.y2 * 0.5f ){
			mPanelYPosDest = mPanelOpenYPos;
		} else {
			mPanelYPosDest = mPanelClosedYPos;
		}

	}
	
}

void UiLayer::selectWheelItem( const Vec2f &pos, bool closeWheel )
{
	if( mShowWheel ){ 
		if( ! mStripRect.contains( pos ) ){
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
				}
			}
		}
	}
}

void UiLayer::update()
{
	mPanelYPos -= ( mPanelYPos - mPanelYPosDest ) * 0.25f;
	mPanelPos	= Vec2f( 0.0f, mPanelYPos );
	
	if( mPanelYPos < mPanelOpenYPos + mPanelRect.y2 * 0.5f ){
		mIsPanelOpen = true;
	} else {
		mIsPanelOpen = false;
	}
	
	mPanelTabRect	= Rectf( getWindowWidth() * 0.5f - 25.0f, mPanelPos.y - 50.0f, getWindowWidth() * 0.5f + 25.0f, mPanelPos.y + 0.5f );
}

void UiLayer::draw( const gl::Texture &upTex, const gl::Texture &downTex )
{
	drawWheel();
	drawPanel( upTex, downTex );
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

void UiLayer::drawPanel( const gl::Texture &upTex, const gl::Texture &downTex )
{
	
	gl::color( ColorA( 0.0f, 0.0f, 0.0f, 1.0f ) );
	gl::pushModelView();
	gl::translate( mPanelPos );
	gl::drawSolidRect( mPanelRect );
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.1f ) );
	gl::drawLine( Vec2f::zero(), Vec2f( getWindowWidth(), 0.0f ) );
	gl::popModelView();
	
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.05f ) );
	gl::drawLine( Vec2f( 1.0f, 1.0f ), Vec2f( getWindowWidth(), 1.0f ) );
	gl::drawLine( Vec2f( 1.0f, 1.0f ), Vec2f( 1.0f, mPanelPos.y ) );
	gl::drawLine( Vec2f( getWindowWidth(), 0.0f ), Vec2f( getWindowWidth(), mPanelPos.y ) );
	
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	if( mIsPanelOpen )
		downTex.enableAndBind();
	else
		upTex.enableAndBind();
	
	gl::drawSolidRect( mPanelTabRect );
	
	if( mIsPanelOpen )
		downTex.disable();
	else
		upTex.disable();

}

