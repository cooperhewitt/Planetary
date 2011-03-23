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
    mApp->unregisterOrientationChanged( mCbOrientationChanged );
}

bool UiLayer::orientationChanged( OrientationEvent event )
{
    // TODO: OrientationEvent helper for this?
    if (UIDeviceOrientationIsValidInterfaceOrientation(event.getOrientation())) {
        mDeviceOrientation = event.getOrientation();
    }
	return false;
}
 
void UiLayer::setup( AppCocoaTouch *app )
{
	mApp = app;
	
	mCbTouchesBegan       = mApp->registerTouchesBegan( this, &UiLayer::touchesBegan );
	mCbTouchesMoved       = mApp->registerTouchesMoved( this, &UiLayer::touchesMoved );
	mCbTouchesEnded       = mApp->registerTouchesEnded( this, &UiLayer::touchesEnded );
    mCbOrientationChanged = mApp->registerOrientationChanged( this, &UiLayer::orientationChanged );
	
	// Rects
	int x1		= 0.0f;
	int y1		= getWindowHeight() - NAV_H;
	int x2		= getWindowWidth();
	int y2		= getWindowHeight();
	mStripRect	= Rectf( x1, y1, x2, y2 );
	
	// PANEL AND TAB
	mPanelRect				= Rectf( 0.0f, 0.0f, getWindowWidth(), 75.0f );
	mPanelOpenYPos			= getWindowHeight() - mPanelRect.y2;
	mPanelClosedYPos		= getWindowHeight();
	mPanelYPos				= mPanelClosedYPos;
	mPanelYPosDest			= mPanelOpenYPos;
	setPanelPos( mPanelYPos, true );
	mIsPanelTabTouched		= false;
	mIsPanelOpen			= true;
	mHasPanelBeenDragged	= false;
	mCountSinceLastTouch	= 0;
	mLastTouchedType		= NO_BUTTON;
}
 
bool UiLayer::touchesBegan( TouchEvent event )
{
	std::cout << "UiLayer TouchesBegan" << std::endl;
	
	mHasPanelBeenDragged = false;

	mTouchPos = event.getTouches().begin()->getPos();

	if( mPanelTabRect.contains( mTouchPos ) ){
		mPanelTabTouchYOffset = mPanelPos.y - mTouchPos.y;
		mIsPanelTabTouched = true;
		mLastTouchedType = PANEL_BUTTON;
	} else {
		mIsPanelTabTouched = false;
		mLastTouchedType = NO_BUTTON;
	}
	mCountSinceLastTouch = 0;
		
	return false; //mIsPanelTabTouched;
}

bool UiLayer::touchesMoved( TouchEvent event )
{
	mTouchPos = event.getTouches().begin()->getPos();

	if( mIsPanelTabTouched ){
		mHasPanelBeenDragged = true;
		setPanelPos( mTouchPos.y, false );
	}
	mCountSinceLastTouch = 0;

	return false;// mIsPanelTabTouched;
}

bool UiLayer::touchesEnded( TouchEvent event )
{
	if (event.getTouches().size() > 0) {
		mTouchPos = event.getTouches().begin()->getPos();
	}
	
	if( mIsPanelTabTouched ){
        mIsPanelTabTouched = false;
		if( mHasPanelBeenDragged ){
			mHasPanelBeenDragged = false;
			setPanelPos( mTouchPos.y, true );
		} else if( mPanelTabRect.contains( mTouchPos ) ){
			if( mIsPanelOpen ){
				mPanelYPosDest = mPanelClosedYPos;
			} else {
				mPanelYPosDest = mPanelOpenYPos;
			}
		}
	}
	mCountSinceLastTouch = 0;
	
	return false; //mIsPanelTabTouched;
}


void UiLayer::setPanelPos( float y, bool doneDragging )
{
	mPanelYPosDest		= y + mPanelTabTouchYOffset;
	
	// if the mPanel y position is outside of the max and min, clamp it
	if( mPanelYPosDest <= mPanelOpenYPos ){
		mIsPanelOpen = true;
		mPanelYPosDest = mPanelOpenYPos;
	}
    else if( mPanelYPosDest >= mPanelClosedYPos ) {
		mIsPanelOpen = false;
		mPanelYPosDest = mPanelClosedYPos;
	}
	
	if( doneDragging ){
		if( mPanelYPos < mPanelOpenYPos + mPanelRect.y2 * 0.5f ){
			mPanelYPosDest = mPanelOpenYPos;
		}
        else {
			mPanelYPosDest = mPanelClosedYPos;
		}
	}	
}

void UiLayer::update()
{
	if( mIsPanelTabTouched ){
        mPanelYPos  = mPanelYPosDest;
    }
    else {
        mPanelYPos -= ( mPanelYPos - mPanelYPosDest ) * 0.25f;
    }
    mPanelPos = Vec2f( 0.0f, mPanelYPos );
	
	if( mPanelYPos < mPanelOpenYPos + mPanelRect.y2 * 0.5f ){
		mIsPanelOpen = true;
	} else {
		mIsPanelOpen = false;
	}
	
	mPanelTabRect	= Rectf( getWindowWidth() * 0.5f - 25.0f, mPanelPos.y - 50.0f, getWindowWidth() * 0.5f + 25.0f, mPanelPos.y + 0.5f );
	
	mCountSinceLastTouch ++;
	
	if( mCountSinceLastTouch > 6 ){
		mLastTouchedType = NO_BUTTON;
	}
}

void UiLayer::draw( const vector<gl::Texture> &texs )
{	
    float width = app::getWindowWidth();
    float height = app::getWindowHeight();
    
    Matrix44f orientationMtx;
    
    switch ( mDeviceOrientation )
    {
        case PORTRAIT_ORIENTATION:
            break;
        case UPSIDE_DOWN_PORTRAIT_ORIENTATION:
            orientationMtx.translate( Vec3f( width, height, 0 ) );            
            orientationMtx.rotate( Vec3f( 0, 0, M_PI ) );
            break;
        case LANDSCAPE_LEFT_ORIENTATION:
            orientationMtx.translate( Vec3f( width, 0, 0 ) );
            orientationMtx.rotate( Vec3f( 0, 0, M_PI/2.0f ) );
            break;
        case LANDSCAPE_RIGHT_ORIENTATION:
            orientationMtx.translate( Vec3f( 0, height, 0 ) );
            orientationMtx.rotate( Vec3f( 0, 0, -M_PI/2.0f ) );
            break;
        default:
            break;
    }
    
    gl::pushModelView();
    gl::multModelView( orientationMtx );    
    
	gl::color( ColorA( 0.0f, 0.0f, 0.0f, 1.0f ) );
	gl::pushModelView();
	gl::translate( Vec2i( mPanelPos ) );
	gl::drawSolidRect( mPanelRect );
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.1f ) );
	gl::drawLine( Vec2f( 0.0f, 0.0f ), Vec2f( width, 0.0f ) );
	gl::popModelView();
	
	
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );

    int texIndex = -1;
	if( mIsPanelTabTouched ){
		if( mIsPanelOpen ) texIndex = TEX_PANEL_DOWN_ON;
		else texIndex = TEX_PANEL_UP_ON;
	} else {
		if( mIsPanelOpen ) texIndex = TEX_PANEL_DOWN;
		else texIndex = TEX_PANEL_UP;
	}
    
    texs[texIndex].enableAndBind();
	gl::drawSolidRect( mPanelTabRect );
    texs[texIndex].disable();
    
    gl::popModelView();
}
