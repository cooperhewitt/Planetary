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
#include "UIController.h"
#include "cinder/app/AppCocoaTouch.h" // for loadResource, getElapsedSeconds
#include "cinder/Utilities.h" // for toString
#include <sstream>

using namespace ci;
using namespace ci::app;
using namespace std;

void AlphaWheel::setup( const Font &font )
{	
	// Textures
	mAlphaString	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ#";
	mAlphaIndex		= 0;
	mAlphaChar		= ' ';
	mPrevAlphaChar	= ' ';
	mShowWheel		= false;
	mWheelScale		= 1.0f;
	
	mWheelOverlay.setup();
	
	initAlphaTextures( font );

//    mInterfaceSize = mRoot->getInterfaceSize();
//
//    mInterfaceCenter = mInterfaceSize * 0.5f;        
//
//	setRects();
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
	float yOff = 0.0f;
	if( mInterfaceSize.x > mInterfaceSize.y ){
		yOff = -12.0f;
	}
	
	mAlphaRects.clear();
	for( int i=0; i<mAlphaString.length(); i++ ){
		float per = (float)i/27.0f;
		float angle = per * TWO_PI - M_PI_2;
		float w = mAlphaTextures[i].getWidth()/2.0f;
		float h = mAlphaTextures[i].getHeight()/2.0f;
		Vec2f pos = Vec2f( cos( angle ), sin( angle ) ) * ( mWheelOverlay.mRadius - 10.0f );
		pos.y += yOff;
		Rectf r = Rectf( pos.x - w, pos.y - h, pos.x + w, pos.y + h );
		mAlphaRects.push_back( r );
	}
}

bool AlphaWheel::touchBegan( TouchEvent::Touch touch )
{
    if (!mShowWheel) return false;
    
    if (selectWheelItem( touch.getPos(), false )) {
        // this means we'll follow the last touch that starts on a wheel item
        mLastTouchPos = touch.getPos();
        mActiveTouchId = touch.getId();
        return true;
    }
	
	return false;
}

bool AlphaWheel::touchMoved( TouchEvent::Touch touch )
{
    if (mShowWheel) {    
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
    if (!mShowWheel) return false;
    
    mLastTouchPos = touch.getPos();
    return selectWheelItem( mLastTouchPos, true );
}


bool AlphaWheel::selectWheelItem( const Vec2f &pos, bool closeWheel )
{
	if( !mShowWheel ){
		return false;
	}
	float minDiam = mWheelOverlay.mRadius - 25.0f;
	float maxDiam = mWheelOverlay.mRadius + 25.0f;
	
	float timeSincePinchEnded = getElapsedSeconds() - mTimePinchEnded;
	if( mShowWheel && timeSincePinchEnded > 0.5f ){ 
        Vec2f dir = globalToLocal(pos) - mInterfaceCenter;
		float distToCenter = dir.length();
		if( distToCenter > minDiam && distToCenter < maxDiam ){
			float touchAngle	= atan2( dir.y, dir.x ) + M_PI;				// RANGE 0 -> TWO_PI
			float anglePer		= ( touchAngle + 0.11365f + M_PI*1.5f )/TWO_PI;
			mAlphaIndex			= (int)( anglePer * 27 )%27;
			mPrevAlphaChar		= mAlphaChar;
			mAlphaChar			= mAlphaString.at( mAlphaIndex % mAlphaString.size() );
			if( mPrevAlphaChar != mAlphaChar || closeWheel ){
				mCallbacksAlphaCharSelected.call( this );
			}
		}
		
		if( closeWheel && distToCenter < maxDiam ){
            setShowWheel(false);
		}

        return distToCenter > minDiam && distToCenter < maxDiam;
	}
    return false;
}

void AlphaWheel::setTimePinchEnded( float timePinchEnded )
{
	mTimePinchEnded = timePinchEnded;	
}

void AlphaWheel::update( )
{
	if( getShowWheel() ){
		mWheelScale -= ( mWheelScale - 1.0f ) * 0.2f;
	} else {
		mWheelScale -= ( mWheelScale - 2.15f ) * 0.2f;	
	}	
    
    Vec2f interfaceSize = mRoot->getInterfaceSize();
    
    if (mInterfaceSize != interfaceSize) {
        mInterfaceSize = interfaceSize;
        mInterfaceCenter = mInterfaceSize * 0.5f;        
    
		mWheelOverlay.update( mInterfaceSize );
		setRects();
		
        if ( mInterfaceSize.x > mInterfaceSize.y ) {
            float amount = (mInterfaceSize.x - mInterfaceSize.y) / (1024-768);

            Matrix44f mat;
            mat.translate( Vec3f(0, -12.0f * amount, 0) );
            setTransform(mat);
        }
    }
}

void AlphaWheel::setNumberAlphaPerChar( float *numberAlphaPerChar )
{
    mNumberAlphaPerChar = numberAlphaPerChar;
}

void AlphaWheel::draw()
{	
	if( mWheelScale < 1.95f ){

		glPushMatrix();
        
		gl::color( Color::white() );
		gl::translate( mInterfaceCenter );
		gl::scale( Vec3f( mWheelScale, mWheelScale, 1.0f ) );
		mWheelOverlay.draw();
		
		for( int i=0; i<27; i++ ){
			float c = mNumberAlphaPerChar[i];
			if( c > 0.0f ){
				c += 0.3f;
				gl::color( Color( c, c, c ) );
			} else {
				gl::color( Color( 0.5f, 0, 0 ) );
			}
			mAlphaTextures[i].enableAndBind();
            // TODO: batch these rects (adapt bloom::gl::batchRect for color tints?)
			gl::drawSolidRect( mAlphaRects[i] );
			mAlphaTextures[i].disable();            
		}
		
		glPopMatrix();
	}
}

void AlphaWheel::drawAlphaChar()
{
	//std::cout << "AlphaWheel::drawAlphaChar mAlphaIndex = " << mAlphaIndex << std::endl;
	float w = mAlphaTextures[mAlphaIndex].getWidth() * 0.5f;
	float h = mAlphaTextures[mAlphaIndex].getHeight() * 0.5f;
	
	gl::color( ColorA( BLUE, 1.0f - mWheelScale ) );
	
	gl::enableAdditiveBlending();
    mAlphaTextures[mAlphaIndex].enableAndBind();
	gl::drawSolidRect( Rectf( -w, -h, w, h ) );
	mAlphaTextures[mAlphaIndex].disable();
	gl::enableAlphaBlending();
}

void AlphaWheel::setShowWheel( bool b ){ 
    mShowWheel = b; 
    mCallbacksWheelToggled.call(this);
}

