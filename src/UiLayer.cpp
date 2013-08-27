/*
 *  UiLayer.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2013 Smithsonian Institution. All rights reserved.
 *
 */


#include "cinder/gl/gl.h"
#include "UiLayer.h"
#include "CinderFlurry.h"
#include "Globals.h"
#include "BloomGl.h"
#include "BloomScene.h"

using namespace pollen::flurry;
using namespace ci;
using namespace ci::app;
using namespace std;

void UiLayer::setup( PlaylistChooserRef playlistChooser, 
                     AlphaChooserRef alphaChooser, 
                     PlayControlsRef playControls, 
                     SettingsPanelRef settingsPanel,
                     const gl::Texture &uiButtonsTex, 
                     const bool &showSettings, 
                     const Vec2f interfaceSize )
{
    mPlaylistChooser = playlistChooser;
    mAlphaChooser = alphaChooser;
    mPlayControls = playControls;
    mSettingsPanel = settingsPanel;
    
    mButtonsTex				= uiButtonsTex;
	
    mIsPanelOpen			= false;
	mIsPanelTabTouched		= false;
	mHasPanelBeenDragged	= false;

    // set now, needed in setShowSettings and updateLayout
    mInterfaceSize = interfaceSize;

    // these rectangles are essentially constant (except for width), movement is handled by setTransform
    mPanelTabRect   = Rectf( interfaceSize.x - 200.0f, -42.0f, interfaceSize.x, 2.0f );
    
    // make sure we're showing enough, then update layout
    setShowSettings(showSettings);    
    
    updateLayout( interfaceSize );
}

void UiLayer::setShowSettings( bool visible ) 
{
    mPanelOpenY	= mInterfaceSize.y - getPanelHeight();        
}

void UiLayer::updateLayout( Vec2f interfaceSize )
{    
    mPanelTabRect.x1 = interfaceSize.x - 200.0f;
    mPanelTabRect.x2 = interfaceSize.x;    
    
    mPanelOpenY		= interfaceSize.y - getPanelHeight();
    mPanelClosedY	= interfaceSize.y;
    
    // cancel interactions
    mIsPanelTabTouched   = false;
    mHasPanelBeenDragged = false;
    
    // set dests for sub-panels
    mSettingsDestY = mPlayControls->getHeight();
    mChooserDestY = mPlayControls->getHeight();
    if ( isShowingFilter() ) {
        mSettingsDestY += max(mAlphaChooser->getHeight(), mPlaylistChooser->getHeight());
    }
    
    // jump to end of animation
    if ( mIsPanelOpen ) {
        mPanelY = mPanelOpenY;        
        mSettingsY = mSettingsDestY;
        mChooserY = mChooserDestY;
    }
    else {
        mPanelY = mPanelClosedY;        
    }
    
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
        mPanelY += newPos.y - mPanelTabRect.y1;
		
		const float maxPanelY = mInterfaceSize.y - getPanelHeight();
		mPanelY = constrain( mPanelY, maxPanelY, mPanelClosedY );
	}

	return mIsPanelTabTouched;
}

bool UiLayer::touchEnded( TouchEvent::Touch touch )
{
    // decide if the open state should change:
	if( mIsPanelTabTouched ){
		if( mHasPanelBeenDragged ){
            mIsPanelOpen = fabs(mPanelY - mPanelOpenY) < fabs(mPanelY - mPanelClosedY);
		} 
        else {
            mIsPanelOpen = !mIsPanelOpen;
//            if (mIsPanelOpen) {
//                Flurry::getInstrumentation()->logEvent("UIPanel Opened");
//            } else {
//                Flurry::getInstrumentation()->logEvent("UIPanel Closed");
//            }
		}
	}

    // reset for next time
    mIsPanelTabTouched = false;
    mHasPanelBeenDragged = false;
    
	return false;
}

void UiLayer::update()
{
    Vec2f interfaceSize = getRoot()->getInterfaceSize();
    // check for orientation change
    if( interfaceSize != mInterfaceSize ){
        updateLayout( interfaceSize );
    }

    if ( !mHasPanelBeenDragged ) {
        // if we're not dragging, animate to current state
        if( mIsPanelOpen ){
            mPanelY += (mPanelOpenY - mPanelY) * 0.25f;
        }
        else {
            mPanelY += (mPanelClosedY - mPanelY) * 0.25f;
        }
    }
    
    mChooserY += (mChooserDestY - mChooserY) * 0.25f;
    mSettingsY += (mSettingsDestY - mSettingsY) * 0.25f;
    
    mPlaylistChooser->setTransform( Matrix44f::createTranslation( Vec3f(0, mChooserY, 0) ) );
    mAlphaChooser->setTransform( Matrix44f::createTranslation( Vec3f(0, mChooserY, 0) ) );
    mSettingsPanel->setTransform( Matrix44f::createTranslation( Vec3f(0, mSettingsY, 0) ) );        

    // don't use mPanelOpenY or current height as a constraint here, 
    // use maximum value because we want things to ease closed
    const float maxPanelY = mInterfaceSize.y - getMaxPanelHeight();
    mPanelY = constrain( mPanelY, maxPanelY, mPanelClosedY );
    
    Matrix44f transform;
    transform.translate( Vec3f( 0, ceil( mPanelY ), 0 ) );
    setTransform( transform );
}

void UiLayer::draw()
{	
    gl::color( Color::white() );    
    gl::draw( mButtonsTex, Area( 0, 456, 200, 500 ), mPanelTabRect);

    // draw background for all sub-panels
    gl::color( Color::black() );    
    gl::drawSolidRect( Rectf(0.0f, 0.0f, mInterfaceSize.x, getMaxPanelHeight()) );
    
    // fuck maths, just figure out which is bigger and smaller and stop pretending to know...
    const float minY = min(mPanelClosedY, mPanelOpenY);
    const float maxY = max(mPanelClosedY, mPanelOpenY);
    const float per = ( mPanelY - minY ) / ( maxY - minY );
    // invert (1-per) because higher Y should be darker and lower Y should be lighter
	const float dragAlphaPer = min( pow( 1.0f - per, 2.0f ), 1.0f ); 

	// top highlight stroke
    gl::color( ColorA( BRIGHT_BLUE, 0.1f * dragAlphaPer + 0.1f ) );
	gl::drawLine( Vec2f( 0.0f, 0.0f ), Vec2f( mPanelTabRect.x1 + 22.0f, 0.0f ) );
	
    // apply alpha to children
    mPlayControls->setOpacity( dragAlphaPer );
    mPlaylistChooser->setOpacity( dragAlphaPer );
    mAlphaChooser->setOpacity( dragAlphaPer );
    mSettingsPanel->setOpacity( dragAlphaPer );    
}

bool UiLayer::hitTest( Vec2f globalPos ) 
{
    if (mVisible) {
        Vec2f pos = globalToLocal( globalPos );
        return mPanelTabRect.contains(pos) || pos.y > mPanelTabRect.y2;
    }
    return false;
}

void UiLayer::setShowAlphaFilter(bool visible)
{
    mAlphaChooser->setVisible(visible);
    if (visible) {
        mPlaylistChooser->setVisible(false);
    }
    // set dests for sub-panels
    mSettingsDestY = mPlayControls->getHeight();
    mChooserDestY = mPlayControls->getHeight();
    if ( isShowingFilter() ) {
        mSettingsDestY += max(mAlphaChooser->getHeight(), mPlaylistChooser->getHeight());
    }     
    mPanelOpenY	= mInterfaceSize.y - getPanelHeight();        
}

bool UiLayer::isShowingAlphaFilter()
{
    return mAlphaChooser->isVisible();
}

void UiLayer::setShowPlaylistFilter(bool visible)
{
    mPlaylistChooser->setVisible(visible);
    if (visible) {
        mAlphaChooser->setVisible(false);
    }
    // set dests for sub-panels
    mSettingsDestY = mPlayControls->getHeight();
    mChooserDestY = mPlayControls->getHeight();
    if ( isShowingFilter() ) {
        mSettingsDestY += max(mAlphaChooser->getHeight(), mPlaylistChooser->getHeight());
    }    
    mPanelOpenY	= mInterfaceSize.y - getPanelHeight();    
}

bool UiLayer::isShowingPlaylistFilter()
{
    return mPlaylistChooser->isVisible();
}

bool UiLayer::isShowingFilter()
{
    return isShowingPlaylistFilter() || isShowingAlphaFilter();
}

float UiLayer::getPanelHeight()
{
    float panelHeight = mPlayControls->getHeight();
    if ( isShowingFilter() ) {
        panelHeight += max( mPlaylistChooser->getHeight(), mAlphaChooser->getHeight() );
    }
    if (mSettingsPanel->isVisible()) {
        panelHeight += mSettingsPanel->getHeight();
    }
    return panelHeight;
}

float UiLayer::getMaxPanelHeight()
{
    float panelHeight = mPlayControls->getHeight();
    panelHeight += max( mPlaylistChooser->getHeight(), mAlphaChooser->getHeight() );
    panelHeight += mSettingsPanel->getHeight();
    return panelHeight;
}
