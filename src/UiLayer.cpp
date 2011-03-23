/*
 *  UiLayer.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "UiLayer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

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
    else {
        return false;
    }

    Vec2f windowSize = getWindowSize();    

    mOrientationMatrix.setToIdentity();
    
    if ( mDeviceOrientation == UPSIDE_DOWN_PORTRAIT_ORIENTATION )
    {
        mOrientationMatrix.translate( Vec3f( windowSize.x, windowSize.y, 0 ) );            
        mOrientationMatrix.rotate( Vec3f( 0, 0, M_PI ) );
    }
    else if ( mDeviceOrientation == LANDSCAPE_LEFT_ORIENTATION )
    {
        mOrientationMatrix.translate( Vec3f( windowSize.x, 0, 0 ) );
        mOrientationMatrix.rotate( Vec3f( 0, 0, M_PI/2.0f ) );
    }
    else if ( mDeviceOrientation == LANDSCAPE_RIGHT_ORIENTATION )
    {
        mOrientationMatrix.translate( Vec3f( 0, windowSize.y, 0 ) );
        mOrientationMatrix.rotate( Vec3f( 0, 0, -M_PI/2.0f ) );
    }
    
    Vec2f interfaceSize = windowSize;

     // TODO: isLandscape()/isPortrait() conveniences on event?
    if ( UIInterfaceOrientationIsLandscape(mDeviceOrientation) ) {
        // swizzle it!
        interfaceSize = interfaceSize.yx();
    }
    
    mPanelOpenY = interfaceSize.y - mPanelHeight;
    mPanelClosedY = interfaceSize.y;
    mPanelRect.x1 = 0;
    mPanelRect.x2 = interfaceSize.x;

    // cancel interactions
    mIsPanelTabTouched   = false;
    mHasPanelBeenDragged = false;

    // jump to end of animation
    if ( mIsPanelOpen ) {
        mPanelRect.y1 = mPanelOpenY;        
    }
    else {
        mPanelRect.y1 = mPanelClosedY;        
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
	
	// PANEL AND TAB
	mPanelHeight			= 75.0f;
	mPanelRect				= Rectf( 0.0f, getWindowHeight(), getWindowWidth(), getWindowHeight()+mPanelHeight );

    mPanelClosedY           = getWindowHeight();
    mPanelOpenY             = getWindowHeight() - mPanelHeight;

	mIsPanelOpen			= false;
	mIsPanelTabTouched		= false;
	mHasPanelBeenDragged	= false;
}
 
bool UiLayer::touchesBegan( TouchEvent event )
{
	mHasPanelBeenDragged = false;

	Vec2f touchPos = event.getTouches().begin()->getPos();
    touchPos = mOrientationMatrix * touchPos;

    mIsPanelTabTouched = mPanelTabRect.contains( touchPos );
    
	if( mIsPanelTabTouched ){
        // remember touch offset for accurate dragging
		mPanelTabTouchYOffset = mPanelTabRect.y2 - touchPos.y;
	}
		
	return mIsPanelTabTouched;
}

bool UiLayer::touchesMoved( TouchEvent event )
{
	Vec2f touchPos = event.getTouches().begin()->getPos();
    touchPos = mOrientationMatrix * touchPos;

	if( mIsPanelTabTouched ){
		mHasPanelBeenDragged = true;
        mPanelRect.y1 = touchPos.y + mPanelTabTouchYOffset;
        mPanelRect.y2 = mPanelRect.y1 + mPanelHeight;
	}

	return mIsPanelTabTouched;
}

bool UiLayer::touchesEnded( TouchEvent event )
{
    // TODO: these touch handlers might need some refinement for multi-touch
    // ... perhaps store the first touch ID in touchesBegan and reject other touches?
    
	if( mIsPanelTabTouched ){
		if( mHasPanelBeenDragged ){
            mIsPanelOpen = (mPanelOpenY - mPanelRect.y1) < mPanelHeight/2.0f;
		} 
        else {
            mIsPanelOpen = !mIsPanelOpen;
		}
	}

    mIsPanelTabTouched = false;
    mHasPanelBeenDragged = false;
    
	return false;
}

void UiLayer::update()
{
    // if we're not dragging, animate to current state
    if ( !mHasPanelBeenDragged ) {
        if( mIsPanelOpen ){
            mPanelRect.y1 += (mPanelOpenY - mPanelRect.y1) * 0.25f;
        }
        else {
            mPanelRect.y1 += (mPanelClosedY - mPanelRect.y1) * 0.25f;
        }
    }
    
    // make sure we're not over the limits
    if (mPanelRect.y1 < mPanelOpenY) {
        mPanelRect.y1 = mPanelOpenY;
    }
    else if (mPanelRect.y1 > mPanelClosedY) {
        mPanelRect.y1 = mPanelClosedY;
    }
    
    // keep up y2!
    mPanelRect.y2 = mPanelRect.y1 + mPanelHeight;
	
    // adjust tab rect:
    mPanelTabRect = Rectf( (mPanelRect.x1 + mPanelRect.x2) * 0.5f - 25.0f, mPanelRect.y1 - 50.0f,
                           (mPanelRect.x1 + mPanelRect.x2) * 0.5f + 25.0f, mPanelRect.y1 + 0.5f );
}

void UiLayer::draw( const vector<gl::Texture> &texs )
{	
    gl::pushModelView();
    gl::multModelView( mOrientationMatrix );
    
	gl::color( ColorA( 0.0f, 0.0f, 0.0f, 1.0f ) );
	gl::drawSolidRect( mPanelRect );
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.1f ) );
	gl::drawLine( Vec2f( mPanelRect.x1, mPanelRect.y1 ), Vec2f( mPanelRect.x2, mPanelRect.y1 ) );
	
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );

    int texIndex = -1;
	if( mIsPanelTabTouched ){
		texIndex = mIsPanelOpen ? TEX_PANEL_DOWN_ON : TEX_PANEL_UP_ON;
	}
    else {
		texIndex = mIsPanelOpen ? TEX_PANEL_DOWN : TEX_PANEL_UP;
	}
    
    texs[texIndex].enableAndBind();
	gl::drawSolidRect( mPanelTabRect );
    texs[texIndex].disable();
    
    gl::popModelView();
}
