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
	mWheelTex		= gl::Texture( loadImage( loadResource( "alphaWheelMask.png" ) ) );
	
	mAlphaString	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ#";
	mAlphaIndex		= 0;
	mAlphaChar		= ' ';
	mPrevAlphaChar	= ' ';
	mShowWheel		= false;
	mWheelScale		= 1.0f;	
	
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
//		float per = (float)i/27.0f;
//		float angle = per * TWO_PI - M_PI_2;
		TextLayout layout;	
		layout.setFont( font );
		layout.setColor( ColorA( BRIGHT_BLUE, 1.0f ) );
		layout.addCenteredLine( ci::toString(mAlphaString[i]) );
		mAlphaTextures.push_back( gl::Texture( layout.render( true, false ) ) );
		
        // setRects()
//		float w = mAlphaTextures[i].getWidth()/2.0f;
//		float h = mAlphaTextures[i].getHeight()/2.0f;
//		Vec2f pos = Vec2f( cos( angle ), sin( angle ) ) * mAlphaRadius;
//		Rectf r = Rectf( pos.x - w, pos.y - h, pos.x + w, pos.y + h );
//		mAlphaRects.push_back( r );
	}
}

void AlphaWheel::setRects()
{
	mAlphaRects.clear();
	for( int i=0; i<mAlphaString.length(); i++ ){
		float per = (float)i/27.0f;
		float angle = per * TWO_PI - M_PI_2;
		float w = mAlphaTextures[i].getWidth()/2.0f;
		float h = mAlphaTextures[i].getHeight()/2.0f;
		Vec2f pos = Vec2f( cos( angle ), sin( angle ) ) * mAlphaRadius;
		Rectf r = Rectf( pos.x - w, pos.y - h, pos.x + w, pos.y + h );
		mAlphaRects.push_back( r );
	}
}

void AlphaWheel::setVerts()
{
	mTotalVertices = 54;
	delete[] mVerts; 
	mVerts = NULL;
	mVerts = new VertexData[mTotalVertices];
	
	float W	= mInterfaceSize.x;
	float H = mInterfaceSize.y;
	float CW = W/2;
	float CH = H/2;
	float L = ( W - mAlphaRadius * 2.0f )/2;
	float T = ( H - mAlphaRadius * 2.0f )/2;
	float R = L + mAlphaRadius * 2.0f;
	float B = T + mAlphaRadius * 2.0f;
		
	vector<Vec2i> positions;
	positions.push_back( Vec2i( 0 - CW, 0 - CH ) );
	positions.push_back( Vec2i( L - CW, 0 - CH ) );
	positions.push_back( Vec2i( R - CW, 0 - CH ) );
	positions.push_back( Vec2i( W - CW, 0 - CH ) );
	
	positions.push_back( Vec2i( 0 - CW, T - CH ) );
	positions.push_back( Vec2i( L - CW, T - CH ) );
	positions.push_back( Vec2i( R - CW, T - CH ) );
	positions.push_back( Vec2i( W - CW, T - CH ) );
	
	positions.push_back( Vec2i( 0 - CW, B - CH ) );
	positions.push_back( Vec2i( L - CW, B - CH ) );
	positions.push_back( Vec2i( R - CW, B - CH ) );
	positions.push_back( Vec2i( W - CW, B - CH ) );
	
	positions.push_back( Vec2i( 0 - CW, H - CH ) );
	positions.push_back( Vec2i( L - CW, H - CH ) );
	positions.push_back( Vec2i( R - CW, H - CH ) );
	positions.push_back( Vec2i( W - CW, H - CH ) );
	
	vector<Vec2f> textures;
	textures.push_back( Vec2f( 0, 0 ) );
	textures.push_back( Vec2f( 0, 0 ) );
	textures.push_back( Vec2f( 1, 0 ) );
	textures.push_back( Vec2f( 1, 0 ) );
	
	textures.push_back( Vec2f( 0, 0 ) );
	textures.push_back( Vec2f( 0, 0 ) );
	textures.push_back( Vec2f( 1, 0 ) );
	textures.push_back( Vec2f( 1, 0 ) );
	
	textures.push_back( Vec2f( 0, 1 ) );
	textures.push_back( Vec2f( 0, 1 ) );
	textures.push_back( Vec2f( 1, 1 ) );
	textures.push_back( Vec2f( 1, 1 ) );
	
	textures.push_back( Vec2f( 0, 1 ) );
	textures.push_back( Vec2f( 0, 1 ) );
	textures.push_back( Vec2f( 1, 1 ) );
	textures.push_back( Vec2f( 1, 1 ) );
	
	int indices[54] = { 0, 1, 4,
					  1, 5, 4,
					  1, 2, 5,
					  2, 6, 5,
					  2, 3, 6,
					  3, 7, 6,
					  4, 5, 8, 
					  5, 9, 8,
					  5, 6, 9, 
					  6, 10, 9, 
					  6, 7, 10, 
					  7, 11, 10,
					  8, 9, 12, 
					  9, 13, 12, 
					  9, 10, 13, 
					  10, 14, 13, 
					  10, 11, 14, 
					  11, 15, 14 };
	
	int vIndex = 0;
	for( int i=0; i<54; i++ ){
		mVerts[vIndex].vertex	= positions[indices[i]];
		mVerts[vIndex].texture	= textures[indices[i]];
		
		vIndex ++;
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
    if (!mShowWheel) return false;
    
        // only follow the last valid touch we received
//        if (touch.getId() == mActiveTouchId) {
//            mLastTouchPos = touch.getPos();
//            return selectWheelItem( mLastTouchPos, false );
//        }
	
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
	float minDiam = mAlphaRadius - 25.0f;
	float maxDiam = mAlphaRadius + 25.0f;
	
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
		mWheelScale -= ( mWheelScale - 0.0f ) * 0.2f;
	} else {
		mWheelScale -= ( mWheelScale - 1.15f ) * 0.2f;	
	}	
    
    Vec2f interfaceSize = mRoot->getInterfaceSize();
    
    if (mInterfaceSize != interfaceSize) {
        mInterfaceSize = interfaceSize;
        mInterfaceCenter = mInterfaceSize * 0.5f;        
    
        float prevRadius = mAlphaRadius;
        
        mAlphaRadius = 300.0f; // portrait
        
        if ( mInterfaceSize.x > mInterfaceSize.y ) {
            float amount = (mInterfaceSize.x - mInterfaceSize.y) / (1024-768);

            Matrix44f mat;
            mat.translate( Vec3f(0, -12.0f * amount, 0) );
            setTransform(mat);
            
            mAlphaRadius -= 30.0f * amount; // smaller for landscape to accomodate PlayControls/Settings
        }
        
        if (prevRadius != mAlphaRadius) {
            setRects();
            setVerts();
        }
    }
}

void AlphaWheel::setNumberAlphaPerChar( float *numberAlphaPerChar )
{
    mNumberAlphaPerChar = numberAlphaPerChar;
}

void AlphaWheel::draw()
{	
	if( mWheelScale < 0.95f ){

		glPushMatrix();
        
		gl::color( Color::white() );
		gl::translate( mInterfaceCenter );
		gl::scale( Vec3f( mWheelScale + 1.0f, mWheelScale + 1.0f, 1.0f ) );
		drawWheel();
		
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

void AlphaWheel::drawWheel()
{
	mWheelTex.enableAndBind();
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	
	glVertexPointer( 2, GL_FLOAT, sizeof(VertexData), mVerts );
	glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), &mVerts[0].texture );
	
	glDrawArrays( GL_TRIANGLES, 0, mTotalVertices );
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	mWheelTex.disable();
	
//    float wMask = mWheelTex.getWidth() * 0.5f;
//    float hMask = mWheelTex.getHeight() * 0.5f;
//
//    float c = 1.0f - mWheelScale;
//    gl::color( ColorA( c, c, c, c ) );
//    
//    mWheelTex.enableAndBind();
//    gl::drawSolidRect( Rectf( -wMask, -hMask, wMask, hMask ) );
//    mWheelTex.disable();    
//	
//    // TODO: batch this geometry, adapt bloom::gl::batchRect() for solid fills    
//    if ( isLandscapeOrientation(mInterfaceOrientation) ) {
//        Vec2f interfaceSize = getWindowSize().yx(); // SWIZ!
//        gl::color( Color::black() );
//        // left bar, relative to center:
//        gl::drawSolidRect( Rectf( -interfaceSize.x/2, -interfaceSize.y/2, -wMask, hMask ) );
//        // right bar, relative to center:
//        gl::drawSolidRect( Rectf( wMask, -interfaceSize.y/2, interfaceSize.x/2, hMask ) );
//    } else {
//		Vec2f interfaceSize = getWindowSize().xy();
//		gl::color( Color::black() );
//        // top bar, relative to center:
//        gl::drawSolidRect( Rectf( -interfaceSize.x/2, -interfaceSize.y/2, interfaceSize.x/2, -hMask ) );
//        // bottom bar, relative to center:
//        gl::drawSolidRect( Rectf( -interfaceSize.x/2, hMask, interfaceSize.x/2, interfaceSize.y/2 ) );
//	}
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

