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
	
	// PANEL AND TAB
	mPanelHeight			= 75.0f;
	mPanelRect				= Rectf( 0.0f, getWindowHeight(), 
                                     getWindowWidth(), getWindowHeight()+mPanelHeight );

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
    mPanelTabRect = Rectf( (mPanelRect.x1 + mPanelRect.x2) * 0.5f - 25.0f, mPanelRect.y1 - 50.0f,
                           (mPanelRect.x1 + mPanelRect.x2) * 0.5f + 25.0f, mPanelRect.y1 );
}

void UiLayer::draw( const vector<gl::Texture> &texs )
{	
    gl::pushModelView();
    gl::multModelView( mOrientationMatrix );
    
	gl::color( ColorA( 0.0f, 0.0f, 0.0f, 1.0f ) );
	gl::drawSolidRect( mPanelRect );
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.1f ) );
	gl::drawLine( Vec2f( mPanelRect.x1, mPanelRect.y1-0.5f ), Vec2f( mPanelRect.x2, mPanelRect.y1-0.5f ) );
	
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
