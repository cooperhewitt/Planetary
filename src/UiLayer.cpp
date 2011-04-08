/*
 *  UiLayer.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "UiLayer.h"
#include "CinderFlurry.h"
#include "BloomGl.h"
#include "Globals.h"

using namespace pollen::flurry;
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
    if ( event.isValidInterfaceOrientation() ) {
        mDeviceOrientation = event.getOrientation();
    }
    else {
        return false;
    }
    
    mOrientationMatrix = event.getOrientationMatrix();
    
    Vec2f interfaceSize = getWindowSize();

    if ( event.isLandscape() ) {
        interfaceSize = interfaceSize.yx(); // swizzle it!
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

// TODO: move this to an operator in Cinder's Matrix class?
Rectf UiLayer::transformRect( const Rectf &rect, const Matrix44f &matrix )
{
    Vec2f topLeft = (matrix * Vec3f(rect.x1,rect.y1,0)).xy();
    Vec2f bottomRight = (matrix * Vec3f(rect.x2,rect.y2,0)).xy();
    Rectf newRect(topLeft, bottomRight);
    newRect.canonicalize();    
    return newRect;
}

void UiLayer::setup( AppCocoaTouch *app )
{
	mApp = app;
	
	mCbTouchesBegan       = mApp->registerTouchesBegan( this, &UiLayer::touchesBegan );
	mCbTouchesMoved       = mApp->registerTouchesMoved( this, &UiLayer::touchesMoved );
	mCbTouchesEnded       = mApp->registerTouchesEnded( this, &UiLayer::touchesEnded );
    mCbOrientationChanged = mApp->registerOrientationChanged( this, &UiLayer::orientationChanged );

    mIsPanelOpen			= false;
	mIsPanelTabTouched		= false;
	mHasPanelBeenDragged	= false;

	// PANEL AND TAB
	mPanelHeight			= 65.0f;
	mPanelRect				= Rectf( 0.0f, getWindowHeight(), 
                                     getWindowWidth(), getWindowHeight()+mPanelHeight );

    mPanelClosedY           = getWindowHeight();
    mPanelOpenY             = getWindowHeight() - mPanelHeight;

    // just do orientation stuff in here:
    orientationChanged(OrientationEvent(mApp->getDeviceOrientation(),mApp->getDeviceOrientation()));
}
 
bool UiLayer::touchesBegan( TouchEvent event )
{
	mHasPanelBeenDragged = false;

	Vec2f touchPos = event.getTouches().begin()->getPos();

    // find where mPanelTabRect is being drawn in screen space (i.e. touchPos space)
    Rectf screenTabRect = transformRect(mPanelTabRect, mOrientationMatrix);

    mIsPanelTabTouched = screenTabRect.contains( touchPos );
    
	if( mIsPanelTabTouched ){
        // remember touch offset for accurate dragging
		mPanelTabTouchOffset = Vec2f(screenTabRect.x1, screenTabRect.y1) - touchPos;
	}
		
	return mIsPanelTabTouched;
}

bool UiLayer::touchesMoved( TouchEvent event )
{
	Vec2f touchPos = event.getTouches().begin()->getPos();
    
	if( mIsPanelTabTouched ){
		mHasPanelBeenDragged = true;

        // find where mPanelTabRect is being drawn in screen space (i.e. touchPos space)
        Rectf screenTabRect = transformRect(mPanelTabRect, mOrientationMatrix);
        
        // apply the touch pos and offset in screen space
        Vec2f newPos = touchPos + mPanelTabTouchOffset;
        screenTabRect.offset(newPos - screenTabRect.getUpperLeft());

        // pull the screen-space rect back into mPanelTabRect space
        Rectf tabRect = transformRect( screenTabRect, mOrientationMatrix.inverted() );

        // set the panel position based on the new tabRect (mPanelTabRect will follow in update())
        mPanelRect.y1 = tabRect.y2;
        mPanelRect.y2 = mPanelRect.y1 + mPanelHeight;
	}

	return mIsPanelTabTouched;
}

bool UiLayer::touchesEnded( TouchEvent event )
{
    // TODO: these touch handlers might need some refinement for multi-touch
    // ... perhaps store the first touch ID in touchesBegan and reject other touches?
    
    // decide if the open state should change:
	if( mIsPanelTabTouched ){
		if( mHasPanelBeenDragged ){
            mIsPanelOpen = (mPanelRect.y1 - mPanelOpenY) < mPanelHeight/2.0f;
		} 
        else {
            mIsPanelOpen = !mIsPanelOpen;
            if (mIsPanelOpen) {
                Flurry::getInstrumentation()->logEvent("UIPanel Opened");
            } else {
                Flurry::getInstrumentation()->logEvent("UIPanel Closed");
            }
		}
	}

    // reset for next time
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
    mPanelTabRect = Rectf( mPanelRect.x2 - 200.0f, mPanelRect.y1 - 49.0f,
                           mPanelRect.x2, mPanelRect.y1 + 2.0f );
}

void UiLayer::draw( const gl::Texture &uiButtonsTex )
{	
    gl::pushModelView();
    gl::multModelView( mOrientationMatrix );
    
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	uiButtonsTex.enableAndBind();
    drawButton( mPanelRect, 0.41f, 0.9f, 0.49f, 0.99f );
	uiButtonsTex.disable();
	
	gl::color( ColorA( COLOR_BRIGHT_BLUE, 0.125f ) );
	gl::drawLine( Vec2f( mPanelRect.x1, mPanelRect.y1 ), Vec2f( mPanelRect.x2, mPanelRect.y1 ) );
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	
	
	float u1 = 0.5f;
	float u2 = 1.0f;
	float v1, v2;
	
    if( mIsPanelTabTouched ){
		v1 = 0.5f;
		v2 = 0.69f;	// HA MGUNK!!!
    } else {
		v1 = 0.5f;
		v2 = 0.69f;
	}
	uiButtonsTex.enableAndBind();
	drawButton( mPanelTabRect, u1, v1, u2, v2 );
	
    uiButtonsTex.disable();
        
	
    gl::popModelView();    
}
