/*
 *  UiLayer.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "cinder/gl/gl.h"
#include "UiLayer.h"
#include "CinderFlurry.h"
#include "Globals.h"
#include "BloomGl.h"
#include "UIController.h"

using namespace pollen::flurry;
using namespace ci;
using namespace ci::app;
using namespace std;

void UiLayer::setup( const gl::Texture &uiButtonsTex, const bool &showSettings, const Vec2f interfaceSize )
{
    mUiButtonsTex = uiButtonsTex;
    
	mPanelOpenHeight		= 65.0f;
	mPanelSettingsHeight	= 105.0f;    
    
    mIsPanelOpen			= false;
	mIsPanelTabTouched		= false;
	mHasPanelBeenDragged	= false;

    // set now, needed in setShowSettings and updateLayout
    mInterfaceSize = interfaceSize;

    // set this to something sensible, *temporary*:
    mPanelRect = Rectf(0, interfaceSize.y, interfaceSize.x, interfaceSize.y + mPanelSettingsHeight);
	mPanelUpperRect = Rectf(0, interfaceSize.y, interfaceSize.x, interfaceSize.y + mPanelSettingsHeight);
	mPanelLowerRect = Rectf(0, interfaceSize.y, interfaceSize.x, interfaceSize.y + mPanelSettingsHeight);
    
    // make sure we're showing enough, then update layout
    setShowSettings(showSettings);    
    
    updateLayout( interfaceSize );
}

void UiLayer::setShowSettings( bool visible ) 
{
	if( visible ){
		mPanelHeight = mPanelSettingsHeight;
	} else {
		mPanelHeight = mPanelOpenHeight;
	}    
    mPanelOpenY	= mInterfaceSize.y - mPanelHeight;        
}

void UiLayer::updateLayout( Vec2f interfaceSize )
{
    mPanelRect.x2 = interfaceSize.x;
    
    mPanelOpenY		= interfaceSize.y - mPanelHeight;
    mPanelClosedY	= interfaceSize.y;
    
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
    mPanelRect.y2 = mPanelRect.y1 + mPanelHeight;    
    
    mInterfaceSize = interfaceSize;
}

bool UiLayer::touchBegan( TouchEvent::Touch touch )
{
	mHasPanelBeenDragged = false;

	Vec2f touchPos = globalToLocal( touch.getPos() );

    mIsPanelTabTouched = mPanelTabRect.contains( touchPos );
    
	if( mIsPanelTabTouched ){
        // remember touch offset for accurate dragging
		mPanelTabTouchOffset = mPanelTabRect.getUpperLeft() - touchPos;
	}
		
	return mIsPanelTabTouched;
}

bool UiLayer::touchMoved( TouchEvent::Touch touch )
{
	Vec2f touchPos = globalToLocal( touch.getPos() );
    
	if( mIsPanelTabTouched ){
		mHasPanelBeenDragged = true;

        // apply the touch pos and offset
        Vec2f newPos = touchPos + mPanelTabTouchOffset;
        newPos.x = mPanelTabRect.x1; // only translate y
        mPanelTabRect.offset(newPos - mPanelTabRect.getUpperLeft());
        
        // set the panel position based on the mPanelTabRect
        mPanelRect.y1 = mPanelTabRect.y2;
        mPanelRect.y2 = mPanelRect.y1 + mPanelHeight;
	}

	return mIsPanelTabTouched;
}

bool UiLayer::touchEnded( TouchEvent::Touch touch )
{
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
    Vec2f interfaceSize = mRoot->getInterfaceSize();
    std::cout << "UiLayer interfaceSize " << interfaceSize << std::endl;
    // check for orientation change
    if (interfaceSize != mInterfaceSize) {
        std::cout << "UiLayer updateLayout()" << std::endl;
        updateLayout( interfaceSize );
    }    
    
    if ( !mHasPanelBeenDragged ) {
        // if we're not dragging, animate to current state
        if( mIsPanelOpen ){
            mPanelRect.y1 += (mPanelOpenY - mPanelRect.y1) * 0.25f;
        }
        else {
            mPanelRect.y1 += (mPanelClosedY - mPanelRect.y1) * 0.25f;
        }
    } 
    
    // make sure the drag/ease hasn't messed anything up
    // always use the tallest size for maxPanelY so we'll ease when closing settings...
    const float maxPanelY = mInterfaceSize.y - mPanelSettingsHeight;
    mPanelRect.y1 = constrain( mPanelRect.y1, maxPanelY, mPanelClosedY );
    
    // keep up y2!
    mPanelRect.y2 = mPanelRect.y1 + mPanelSettingsHeight;
	
	mPanelUpperRect = Rectf( 0.0f, mPanelRect.y1, mInterfaceSize.x, mPanelRect.y1 + mPanelOpenHeight );
	mPanelLowerRect = Rectf( 0.0f, mPanelRect.y1 + mPanelOpenHeight, mInterfaceSize.x, mPanelRect.y1 + mPanelSettingsHeight );
	
    // adjust tab rect:
    mPanelTabRect = Rectf( mPanelRect.x2 - 200.0f, mPanelRect.y1 - 38.0f,
                           mPanelRect.x2, mPanelRect.y1 + 2.0f );
	
}

void UiLayer::draw()
{	
    bloom::gl::beginBatch();
    bloom::gl::batchRect(mUiButtonsTex, Rectf(0.0, 0.71f, 0.09f, 0.79f), mPanelUpperRect);
    bloom::gl::batchRect(mUiButtonsTex, Rectf(0.0, 0.91f, 0.09f, 0.99f), mPanelLowerRect);
    bloom::gl::batchRect(mUiButtonsTex, Rectf(0.5f, 0.5f, 1.0f, 0.7f), mPanelTabRect);
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );    
    bloom::gl::endBatch();

    gl::color( ColorA( BRIGHT_BLUE, 0.2f ) );
	gl::drawLine( Vec2f( mPanelRect.x1, round(mPanelRect.y1) ), Vec2f( mPanelTabRect.x1+23, round(mPanelRect.y1) ) );
	
	gl::color( ColorA( BRIGHT_BLUE, 0.1f ) );
	gl::drawLine( Vec2f( mPanelRect.x1, mPanelRect.y1 + mPanelOpenHeight + 1.0f ), Vec2f( mPanelRect.x2, mPanelRect.y1 + mPanelOpenHeight + 1.0f ) );    
}
