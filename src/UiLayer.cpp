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
#include "Globals.h"
#include "BloomGl.h"

using namespace pollen::flurry;
using namespace ci;
using namespace ci::app;
using namespace std;

UiLayer::UiLayer()
{
	mPanelOpenHeight		= 65.0f;
	mPanelSettingsHeight	= 150.0f;    
}

UiLayer::~UiLayer()
{
	mApp->unregisterTouchesBegan( mCbTouchesBegan );
	mApp->unregisterTouchesMoved( mCbTouchesMoved );
	mApp->unregisterTouchesEnded( mCbTouchesEnded );
}

void UiLayer::setup( AppCocoaTouch *app, const Orientation &orientation, const bool &showSettings )
{
	mApp = app;
	
	mCbTouchesBegan       = mApp->registerTouchesBegan( this, &UiLayer::touchesBegan );
	mCbTouchesMoved       = mApp->registerTouchesMoved( this, &UiLayer::touchesMoved );
	mCbTouchesEnded       = mApp->registerTouchesEnded( this, &UiLayer::touchesEnded );

    mIsPanelOpen			= false;
	mIsPanelTabTouched		= false;
	mHasPanelBeenDragged	= false;

    // set this to something sensible, *temporary*:
    mPanelRect = Rectf(0, getWindowHeight(), getWindowWidth(), getWindowHeight() + mPanelSettingsHeight);
	mPanelUpperRect = Rectf(0, getWindowHeight(), getWindowWidth(), getWindowHeight() + mPanelSettingsHeight);
	mPanelLowerRect = Rectf(0, getWindowHeight(), getWindowWidth(), getWindowHeight() + mPanelSettingsHeight);
    
    // make sure we've got everything the right way around
    setInterfaceOrientation(orientation);

    // and make sure we're showing enough
    setShowSettings(showSettings);    
}

void UiLayer::setShowSettings( bool visible ) 
{
	if( visible ){
		mPanelHeight = mPanelSettingsHeight;
	} else {
		mPanelHeight = mPanelOpenHeight;
	}
    
    mPanelOpenY		= mInterfaceSize.y - mPanelHeight;
    mPanelClosedY	= mInterfaceSize.y;
}

void UiLayer::setInterfaceOrientation( const Orientation &orientation )
{
    mInterfaceOrientation = orientation;
    
    mOrientationMatrix = getOrientationMatrix44(mInterfaceOrientation, getWindowSize());
    
    mInterfaceSize = getWindowSize();
    
    if ( isLandscapeOrientation( mInterfaceOrientation ) ) {
        mInterfaceSize = mInterfaceSize.yx(); // swizzle it!
    }
    
    mPanelRect.x2 = mInterfaceSize.x;
    
    mPanelOpenY		= mInterfaceSize.y - mPanelHeight;
    mPanelClosedY	= mInterfaceSize.y;
    
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
		G_HELP = false;
	}

    // reset for next time
    mIsPanelTabTouched = false;
    mHasPanelBeenDragged = false;
    
	return false;
}

void UiLayer::update()
{
    if ( !mHasPanelBeenDragged ) {
        // if we're not dragging, animate to current state
        if( mIsPanelOpen ){
            mPanelRect.y1 += (mPanelOpenY - mPanelRect.y1) * 0.25f;
        }
        else {
            mPanelRect.y1 += (mPanelClosedY - mPanelRect.y1) * 0.25f;
        }
    } else {
        // otherwise, just make sure the drag hasn't messed anything up
		mPanelRect.y1 = constrain( mPanelRect.y1, mPanelOpenY, mPanelClosedY );
	}
	
    // keep up y2!
    mPanelRect.y2 = mPanelRect.y1 + mPanelSettingsHeight;
	
	mPanelUpperRect = Rectf( 0.0f, mPanelRect.y1,							mInterfaceSize.x, mPanelRect.y1 + mPanelOpenHeight );
	mPanelLowerRect = Rectf( 0.0f, mPanelRect.y1 + mPanelOpenHeight, mInterfaceSize.x, mPanelRect.y1 + mPanelSettingsHeight );
	
    // adjust tab rect:
    mPanelTabRect = Rectf( mPanelRect.x2 - 200.0f, mPanelRect.y1 - 38.0f,
                           mPanelRect.x2, mPanelRect.y1 + 2.0f );
	
}

void UiLayer::draw( const gl::Texture &uiButtonsTex )
{	
    glPushMatrix();
    glMultMatrixf( mOrientationMatrix );

    bloom::gl::beginBatch();
    bloom::gl::batchRect(uiButtonsTex, Rectf(0.0, 0.71f, 0.09f, 0.79f), mPanelUpperRect);
    bloom::gl::batchRect(uiButtonsTex, Rectf(0.0, 0.91f, 0.09f, 0.99f), mPanelLowerRect);
    bloom::gl::batchRect(uiButtonsTex, Rectf(0.5f, 0.5f, 1.0f, 0.7f), mPanelTabRect);
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );    
    bloom::gl::endBatch();

    gl::color( ColorA( BRIGHT_BLUE, 0.2f ) );
	gl::drawLine( Vec2f( mPanelRect.x1, round(mPanelRect.y1) ), Vec2f( mPanelTabRect.x1+23, round(mPanelRect.y1) ) );
	
	gl::color( ColorA( BRIGHT_BLUE, 0.1f ) );
	gl::drawLine( Vec2f( mPanelRect.x1, mPanelRect.y1 + mPanelOpenHeight + 1.0f ), Vec2f( mPanelRect.x2, mPanelRect.y1 + mPanelOpenHeight + 1.0f ) );
    
    glPopMatrix();    
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
