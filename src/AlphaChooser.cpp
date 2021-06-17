/*
 *  AlphaChooser.cpp
 *  Kepler
 *
 *  Created by Tom Carden on 3/14/11.
 *  Copyright 2013 Smithsonian Institution. All rights reserved.
 *
 */

#include "AlphaChooser.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "Globals.h"
#include "BloomGl.h"
#include "BloomScene.h"
#include "cinder/Utilities.h" // for toString
#include <sstream>

using namespace ci;
using namespace ci::app;
using namespace std;

void AlphaChooser::setup( const Font &font, const Vec2f &interfaceSize )
{	
	// Textures
	mAlphaString	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ#";
	mAlphaIndex		= 0;
	mAlphaChar		= 'A';

	for( int i=0; i<mAlphaString.length(); i++ ){
		TextLayout layout;	
		layout.setFont( font );
		layout.setColor( Color::white() );
		layout.addCenteredLine( ci::toString(mAlphaString[i]) );
		mAlphaTextures.push_back( gl::Texture( layout.render( true, false ) ) );
	}
    
    mInterfaceSize = interfaceSize;
    
    setRects();
}

void AlphaChooser::setRects()
{
	mAlphaRects.clear();
    mAlphaHitRects.clear();
    float totalWidth = 0.0f;
    float maxHeight = 0.0f;
	for( int i=0; i<mAlphaString.length(); i++ ){
        totalWidth += mAlphaTextures[i].getWidth();
        maxHeight = max( maxHeight, (float)mAlphaTextures[i].getHeight() );
    }    
    const float hPadding = 20.0f;
    const float vTopPadding = 11.0f;
    const float vBottomPadding = 9.0f;
    const float spacing = (mInterfaceSize.x - totalWidth - (hPadding * 2.0f)) / 26.0f;
    float x = hPadding;
	for( int i = 0; i < mAlphaString.length(); i++ ){
		const float w = mAlphaTextures[i].getWidth();
		const float h = mAlphaTextures[i].getHeight();
		mAlphaRects.push_back( Rectf( x, vTopPadding, x + w, h + vTopPadding ) );
		mAlphaHitRects.push_back( Rectf( x - spacing/2.0f, 0.0f, x + w + spacing/2.0f, h + vTopPadding + vBottomPadding ) );
        x += w + spacing;
	}
    mFullRect.set( 0.0f, 0.0f, mInterfaceSize.x, maxHeight + vTopPadding + vBottomPadding );
}

bool AlphaChooser::touchBegan( TouchEvent::Touch touch )
{
    if (!isVisible()) return false;    
    Vec2f pos = globalToLocal( touch.getPos() );
    return mFullRect.contains( pos );
}

bool AlphaChooser::touchMoved( TouchEvent::Touch touch )
{	
    if (!isVisible()) return false;
    
    Vec2f pos = globalToLocal( touch.getPos() );
    for (int i = 0; i < mAlphaRects.size(); i++) {
        if ( mNumberAlphaPerChar[i] ) {                        
            if ( mAlphaHitRects[i].contains( pos ) ) {
                mAlphaIndex = i;
                if( mAlphaChar != mAlphaString[i] ){            
                    mAlphaChar = mAlphaString[i];
                    mCallbacksAlphaCharSelected.call( mAlphaChar );
                }
                return true;
            }
        }
    }
    
    return mFullRect.contains( pos );
}

bool AlphaChooser::touchEnded( TouchEvent::Touch touch )
{	
    if (!isVisible()) return false;

    Vec2f pos = globalToLocal( touch.getPos() );
    for (int i = 0; i < mAlphaRects.size(); i++) {
        if ( mNumberAlphaPerChar[i] ) {            
            if ( mAlphaHitRects[i].contains( pos ) ) {
                mAlphaIndex = i;
                mAlphaChar = mAlphaString[i];
                mCallbacksAlphaCharSelected.call( mAlphaChar );
                return true;
            }
        }
    }

    return mFullRect.contains( pos );
}

void AlphaChooser::setNumberAlphaPerChar( float *numberAlphaPerChar )
{
    mNumberAlphaPerChar = numberAlphaPerChar;
}

void AlphaChooser::update( )
{    
    Vec2f interfaceSize = getRoot()->getInterfaceSize();    
    if (mInterfaceSize != interfaceSize) {
        mInterfaceSize = interfaceSize;
        setRects();        
    }
}

void AlphaChooser::draw()
{	        
    // we'll use the bright blue components to draw by frequency
    float r = BRIGHT_BLUE.r;
    float g = BRIGHT_BLUE.g;
    float b = BRIGHT_BLUE.b;

    gl::color( ColorA( r, g, b, mOpacity * 0.125f ) );
    gl::drawLine( mFullRect.getUpperLeft(), mFullRect.getUpperRight() );
            
    for( int i=0; i<27; i++ ){
        float c = mNumberAlphaPerChar[i];
        if ( mAlphaString[i] == mAlphaChar ) {
            gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f * mOpacity ) );
        } else if( c > 0.0f ){
            c += 0.3f;
            gl::color( ColorA( r*c, g*c, b*c, mOpacity ) );
        } else {
            gl::color( ColorA( 0.1f, 0.1f, 0.15f, mOpacity ) );
        }
        mAlphaTextures[i].enableAndBind();
        gl::drawSolidRect( mAlphaRects[i] );
        mAlphaTextures[i].disable();            
    }
}

float AlphaChooser::getHeight()
{
    return mFullRect.getHeight();
}

