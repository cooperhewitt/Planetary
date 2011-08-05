/*
 *  AlphaWheel.cpp
 *  Kepler
 *
 *  Created by Tom Carden on 3/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "AlphaWheel.h"
#include "cinder/gl/gl.h"
#include "cinder/Font.h"
#include "cinder/Text.h"
#include "cinder/ImageIo.h"
#include "Globals.h"
#include "BloomGl.h"
#include "BloomScene.h"
#include "cinder/app/AppCocoaTouch.h" // for loadResource, getElapsedSeconds
#include "cinder/Utilities.h" // for toString
#include <sstream>

using namespace ci;
using namespace ci::app;
using namespace std;

void AlphaWheel::setup( const Font &font, WheelOverlayRef wheelOverlay )
{	
	// Textures
	mAlphaString	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ#";
	mAlphaIndex		= 0;
	mAlphaChar		= ' ';
	mPrevAlphaChar	= ' ';

	mWheelOverlay = wheelOverlay;
	
	initAlphaTextures( font );
}

void AlphaWheel::initAlphaTextures( const Font &font )
{
	for( int i=0; i<mAlphaString.length(); i++ ){
		TextLayout layout;	
		layout.setFont( font );
		layout.setColor( ColorA( BRIGHT_BLUE, 1.0f ) );
		layout.addCenteredLine( ci::toString(mAlphaString[i]) );
		mAlphaTextures.push_back( gl::Texture( layout.render( true, false ) ) );
	}
}

void AlphaWheel::setRects()
{
	mAlphaRects.clear();
	for( int i=0; i<mAlphaString.length(); i++ ){
		const float per = (float)i/27.0f;
		const float angle = per * TWO_PI - M_PI_2;
		const float w = mAlphaTextures[i].getWidth()/2.0f;
		const float h = mAlphaTextures[i].getHeight()/2.0f;
        const float scale = ( mWheelOverlay->getRadius() - 10.0f );
		Vec2f pos( cos( angle ) * scale, sin( angle ) * scale );
		Rectf r = Rectf( pos.x - w, pos.y - h, pos.x + w, pos.y + h );
		mAlphaRects.push_back( r );
	}
}

bool AlphaWheel::touchBegan( TouchEvent::Touch touch )
{
    if (!mWheelOverlay->getShowWheel()) return false;
    
    if (selectWheelItem( touch.getPos(), false )) {
        // this means we'll follow the last touch that starts on a wheel item
        mLastTouchPos = touch.getPos();
        mActiveTouchId = touch.getId();
        return true;
    }
    else {
        // capture all touches inside wheel so we can dismiss a tap inside the world
        Vec2f dir = globalToLocal( touch.getPos() );
		float distToCenter = dir.length();
        float maxDiam = mWheelOverlay->getRadius() + 25.0f;        
		if( distToCenter < maxDiam ){
            return true;
        }
    }
	
	return false;
}

bool AlphaWheel::touchMoved( TouchEvent::Touch touch )
{
    if (mWheelOverlay->getShowWheel()) {
        // only follow the last valid touch we received
        if (touch.getId() == mActiveTouchId) {
            mLastTouchPos = touch.getPos();
            return selectWheelItem( mLastTouchPos, false );
        }
    }
	return false;
}

bool AlphaWheel::touchEnded( TouchEvent::Touch touch )
{	
	std::cout << "AlphaWheel touchEnded" << std::endl;
	
    if (!mWheelOverlay->getShowWheel()) return false;
    
    mLastTouchPos = touch.getPos();
    return selectWheelItem( mLastTouchPos, true );
}


bool AlphaWheel::selectWheelItem( const Vec2f &pos, bool closeWheel )
{
    if (!mWheelOverlay->getShowWheel()) return false;

	float minDiam = mWheelOverlay->getRadius() - 25.0f;
	float maxDiam = minDiam + 50.0f;
	
	float timeSincePinchEnded = getElapsedSeconds() - mTimePinchEnded;
	if( timeSincePinchEnded > 0.5f ){ 
        Vec2f dir = globalToLocal(pos);
		float distToCenter = dir.length();
		if( distToCenter > minDiam && distToCenter < maxDiam ){
			float touchAngle	= atan2( dir.y, dir.x ) + M_PI;				// RANGE 0 -> TWO_PI
			float anglePer		= ( touchAngle + 0.11365f + M_PI*1.5f )/TWO_PI;
			mAlphaIndex			= (int)( anglePer * 27 )%27;
			mPrevAlphaChar		= mAlphaChar;
			mAlphaChar			= mAlphaString.at( mAlphaIndex % mAlphaString.size() );
			if( mPrevAlphaChar != mAlphaChar || closeWheel ){
				mCallbacksAlphaCharSelected.call( mAlphaChar );
			}
		}
		
		if( closeWheel ){ // && distToCenter < maxDiam ){
            mWheelOverlay->setShowWheel(false);
		}

        return distToCenter > minDiam && distToCenter < maxDiam;
	}
    return false;
}

void AlphaWheel::setTimePinchEnded( float timePinchEnded )
{
	mTimePinchEnded = timePinchEnded;	
}

void AlphaWheel::setNumberAlphaPerChar( float *numberAlphaPerChar )
{
    mNumberAlphaPerChar = numberAlphaPerChar;
}

void AlphaWheel::update( )
{    
    Vec2f interfaceSize = getRoot()->getInterfaceSize();    
    if (mInterfaceSize != interfaceSize) {
        mInterfaceSize = interfaceSize;
        setRects();
    }
}

void AlphaWheel::draw()
{	
	if( mWheelOverlay->getWheelScale() < 1.95f ){

        float alpha = constrain(2.0f - mWheelOverlay->getWheelScale(), 0.0f, 1.0f);
        
		for( int i=0; i<27; i++ ){
			float c = mNumberAlphaPerChar[i];
			if( c > 0.0f ){
				c += 0.3f;
				gl::color( ColorA( c, c, c, alpha ) );
			} else {
				gl::color( ColorA( 0.5f, 0, 0, alpha ) );
			}
			mAlphaTextures[i].enableAndBind();
            // TODO: batch these rects (adapt bloom::gl::batchRect for color tints?)
			gl::drawSolidRect( mAlphaRects[i] );
			mAlphaTextures[i].disable();            
		}
		
	}
}
