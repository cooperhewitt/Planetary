/*
 *  AlphaChooser.cpp
 *  Kepler
 *
 *  Created by Tom Carden on 3/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "AlphaChooser.h"
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

void AlphaChooser::setup( const Font &font, UiLayerRef uiLayer, WheelOverlayRef wheelOverlay )
{	
	// Textures
	mAlphaString	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ#";
	mAlphaIndex		= 0;
	mAlphaChar		= 'A';

    mUiLayer = uiLayer;
	mWheelOverlay = wheelOverlay;
	
	for( int i=0; i<mAlphaString.length(); i++ ){
		TextLayout layout;	
		layout.setFont( font );
		layout.setColor( Color::white() );
		layout.addCenteredLine( ci::toString(mAlphaString[i]) );
		mAlphaTextures.push_back( gl::Texture( layout.render( true, false ) ) );
	}
}

void AlphaChooser::setRects()
{
	mAlphaRects.clear();
    float totalWidth = 0.0f;
    float totalHeight = 0.0f;
	for( int i=0; i<mAlphaString.length(); i++ ){
        totalWidth += mAlphaTextures[i].getWidth();
        totalHeight = max( totalHeight, (float)mAlphaTextures[i].getHeight() );
    }    
    const float hPadding = 20.0f;
    const float spacing = (mInterfaceSize.x - totalWidth - hPadding*2.0f) / 26.0f;
    float x = hPadding - (mInterfaceSize.x / 2.0f);
	for( int i=0; i<mAlphaString.length(); i++ ){
		const float w = mAlphaTextures[i].getWidth();
		const float h = mAlphaTextures[i].getHeight();
		mAlphaRects.push_back( Rectf( x, 0.0f, x + w,  h ) );
        x += w + spacing;
	}
    mFullRect.set( hPadding - mInterfaceSize.x / 2.0f, 0.0f, (mInterfaceSize.x / 2.0f) - hPadding, totalHeight );
}

bool AlphaChooser::touchBegan( TouchEvent::Touch touch )
{
    if (!mWheelOverlay->getShowWheel()) return false;    
    Vec2f pos = globalToLocal( touch.getPos() );
    // TODO: make a vertical padded hitrect
    return mFullRect.contains( pos );
}

bool AlphaChooser::touchMoved( TouchEvent::Touch touch )
{	
    if (!mWheelOverlay->getShowWheel()) return false;
    
    Vec2f pos = globalToLocal( touch.getPos() );
    for (int i = 0; i < mAlphaRects.size(); i++) {
        // TODO: make a vertical padded hitrect
        if ( mAlphaRects[i].contains( pos ) ) {
            mAlphaIndex = i;
			if( mAlphaChar != mAlphaString[i] ){            
                mAlphaChar = mAlphaString[i];
                mCallbacksAlphaCharSelected.call( mAlphaChar );
            }
            return true;
        }
    }
    
    return false;
}

bool AlphaChooser::touchEnded( TouchEvent::Touch touch )
{	
    if (!mWheelOverlay->getShowWheel()) return false;

    Vec2f pos = globalToLocal( touch.getPos() );
    for (int i = 0; i < mAlphaRects.size(); i++) {
        // TODO: make a vertical padded hitrect
        if ( mAlphaRects[i].contains( pos ) ) {
            mAlphaIndex = i;
			if( mAlphaChar != mAlphaString[i] ){            
                mAlphaChar = mAlphaString[i];
                mCallbacksAlphaCharSelected.call( mAlphaChar );
                mWheelOverlay->setShowWheel(false);
            }
            return true;
        }
    }

    return false;
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
    
    // wheel is already centered, so we have to subtract half of interface height
    float y = mUiLayer->getPanelYPos() - (mInterfaceSize.y / 2.0f) - 100.0f;        
    Matrix44f mat;
    mat.translate( Vec3f( 0, y, 0) );
    setTransform( mat );
    // FIXME: only do this if ypos changes?
}

void AlphaChooser::draw()
{	
	if( mWheelOverlay->getWheelScale() < 1.95f ){

        float r = BRIGHT_BLUE.r;
        float g = BRIGHT_BLUE.g;
        float b = BRIGHT_BLUE.b;
        float alpha = constrain(2.0f - mWheelOverlay->getWheelScale(), 0.0f, 1.0f);

        // make a rect without hPadding (see setRects) and a bit more vPadding:
        float vPadding = 10.0f;
        Rectf bgRect = mFullRect;
        bgRect.x1 = -mInterfaceSize.x / 2.0f;
        bgRect.x2 = mInterfaceSize.x / 2.0f;
        bgRect.y1 -= vPadding;
        bgRect.y2 += vPadding;
        
        // draw background:
        gl::color( ColorA( 0.0f, 0.0f, 0.0f, alpha ) );
        gl::drawSolidRect( bgRect ); // TODO: slight transparency?
        gl::color( ColorA( r, g, b, alpha * 0.25f ) );
        gl::drawLine( bgRect.getUpperLeft(), bgRect.getUpperRight() );
        gl::drawLine( bgRect.getLowerLeft(), bgRect.getLowerRight() );
                
		for( int i=0; i<27; i++ ){
			float c = mNumberAlphaPerChar[i];
            if ( mAlphaString[i] == mAlphaChar ) {
                gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
            } else if( c > 0.0f ){
				c += 0.3f;
				gl::color( ColorA( r*c, g*c, b*c, alpha ) );
			} else {
				gl::color( ColorA( 0.3f, 0.0f, 0.0f, alpha ) );
			}
			mAlphaTextures[i].enableAndBind();
			gl::drawSolidRect( mAlphaRects[i] );
			mAlphaTextures[i].disable();            
		}
		
	}
}
