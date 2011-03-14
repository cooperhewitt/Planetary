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
 
bool UiLayer::touchesBegan( TouchEvent event )
{
	std::cout << "UiLayer TouchesBegan" << std::endl;
	
	mHasPanelBeenDragged = false;

	mTouchPos = event.getTouches().begin()->getPos();

	if( mPanelTabRect.contains( mTouchPos ) ){
		mPanelTabTouchYOffset = mPanelPos.y - mTouchPos.y;
		mIsPanelTabTouched = true;
	} else {
		mIsPanelTabTouched = false;
	}
		
	return mIsPanelTabTouched;
}

bool UiLayer::touchesMoved( TouchEvent event )
{
	mTouchPos = event.getTouches().begin()->getPos();

	if( mIsPanelTabTouched ){
		mHasPanelBeenDragged = true;
		setPanelPos( mTouchPos.y, false );
	}

	return mIsPanelTabTouched;
}

bool UiLayer::touchesEnded( TouchEvent event )
{
	if (event.getTouches().size() > 0) {
		mTouchPos = event.getTouches().begin()->getPos();
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
	
	return mIsPanelTabTouched;
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
	gl::color( ColorA( 0.0f, 0.0f, 0.0f, 1.0f ) );
	gl::pushModelView();
	gl::translate( Vec2i( mPanelPos ) );
	gl::drawSolidRect( mPanelRect );
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.1f ) );
	gl::drawLine( Vec2f( 0.0f, 0.0f ), Vec2f( getWindowWidth(), 0.0f ) );
	gl::popModelView();
	
	
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

