//
//  FilterToggleButton.cpp
//  Kepler
//
//  Created by Tom Carden on 7/10/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "FilterToggleButton.h"
#include "Globals.h"
#include "cinder/Text.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "BloomScene.h"
#include "BloomGl.h"

using namespace ci;
using namespace ci::app;

void FilterToggleButton::setup( const State::FilterMode &filterMode, const Font &font, const gl::Texture &tex, UiLayerRef uiLayer, WheelOverlayRef wheelOverlay )
{    
    mTex = tex;
    mUiLayer = uiLayer;
    mWheelOverlay = wheelOverlay;

	mRect	= Rectf( 0.0f, 0.0f, mTex.getWidth(), mTex.getHeight()/2 );    
	mAlphaRect		= Rectf( 0.0f, 0.0f, 127.0f, mRect.getHeight() ); 
	mPlaylistRect	= Rectf( 127.0f, 0.0f, mRect.getWidth(), mRect.getHeight() ); 
		
    setFilterMode( filterMode );
}

bool FilterToggleButton::touchBegan( TouchEvent::Touch touch )
{	
	Vec2f pos = globalToLocal( touch.getPos() );
	return mPlaylistRect.contains(pos) || mAlphaRect.contains(pos);
}

bool FilterToggleButton::touchEnded( TouchEvent::Touch touch )
{
    Vec2f pos = globalToLocal( touch.getPos() );
	if (mAlphaRect.contains(pos)) {
		mCbFilterModeSelected.call(State::FilterModeAlphaChar);
		return true;
	}
	else if (mPlaylistRect.contains(pos)) {
		mCbFilterModeSelected.call(State::FilterModePlaylist);          
		return true;
	}
    return false;
}


void FilterToggleButton::setFilterMode(const State::FilterMode &filterMode)
{
    mFilterMode = filterMode;
}


void FilterToggleButton::update()
{
	mInterfaceSize = getRoot()->getInterfaceSize();
    
    // we're a child of WheelOverlay, which is centered already...
    float hPadding = 0.0f;
    float x = hPadding - (mInterfaceSize.x / 2.0f);
    float y = mUiLayer->getPanelYPos() - (mInterfaceSize.y / 2.0f) - 50.0f;
    
    Matrix44f mat;
    mat.translate( Vec3f( x, y, 0.0f ) );
    setTransform( mat );    
}

void FilterToggleButton::draw()
{   
	if( mWheelOverlay->getWheelScale() < 1.95f ){
        
        float alpha = constrain(2.0f - mWheelOverlay->getWheelScale(), 0.0f, 1.0f);
    
        bloom::gl::beginBatch();
        if( mFilterMode == State::FilterModeAlphaChar ){
            bloom::gl::batchRect( mTex, Rectf(0.0f, 0.0f, 1.0f, 0.5f), mRect );
        } else {
            bloom::gl::batchRect( mTex, Rectf(0.0f, 0.5f, 1.0f, 1.0f), mRect );
        }
        gl::color( ColorA( alpha, alpha, alpha, alpha ) );    
        bloom::gl::endBatch();   
    }
}
