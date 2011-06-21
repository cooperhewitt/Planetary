//
//  LoadingScreen.cpp
//  Kepler
//
//  Created by Tom Carden on 3/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "LoadingScreen.h"
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

void LoadingScreen::setup( AppCocoaTouch *app, const Orientation &orientation )
{
    mEnabled = true;
    app->registerTouchesBegan(this, &LoadingScreen::onTouchEvent);
    app->registerTouchesMoved(this, &LoadingScreen::onTouchEvent);
    app->registerTouchesEnded(this, &LoadingScreen::onTouchEvent);            
    setInterfaceOrientation( orientation );
    gl::Texture::Format fmt;
    fmt.enableMipmapping( true );
    fmt.setMinFilter( GL_LINEAR_MIPMAP_LINEAR );        
    fmt.setMagFilter( GL_LINEAR );        
	mPlanetaryTex	= gl::Texture( loadImage( loadResource( "planetary.png" ) )/*, fmt*/ );
	mPlanetTex		= gl::Texture( loadImage( loadResource( "planet.png" ) ), fmt );
	mBackgroundTex	= gl::Texture( loadImage( loadResource( "background.jpg" ) )/*, fmt*/ );
}

void LoadingScreen::setEnabled( bool enabled )
{
    mEnabled = enabled;
}

void LoadingScreen::setInterfaceOrientation( const Orientation &orientation )
{
    mInterfaceOrientation = orientation;    
    
    mOrientationMatrix = getOrientationMatrix44(mInterfaceOrientation, getWindowSize());

    mInterfaceSize = getWindowSize();
    
    // TODO: isLandscape()/isPortrait() conveniences on event?
    if ( isLandscapeOrientation(mInterfaceOrientation) ) {
        mInterfaceSize = mInterfaceSize.yx(); // swizzle it!
    }
}

void LoadingScreen::draw( gl::Texture mStarGlowTex )
{
	Vec2f pos;
	float radius;
	
    gl::setMatricesWindow( app::getWindowSize() );    

    glPushMatrix();
    glMultMatrixf( mOrientationMatrix );
    Vec2f center = mInterfaceSize * 0.5f;
    gl::color( Color::white() );
	
	//float fadeInAlpha = constrain( app::getElapsedFrames()/30.0f - 1.0f, 0.0f, 1.0f );
	
// BACKGROUND	
	mBackgroundTex.enableAndBind();
	Vec2f v1( center - mBackgroundTex.getSize() * 0.5f );
	Vec2f v2( v1 + mBackgroundTex.getSize() );
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	gl::drawSolidRect( Rectf( v1, v2 ) );
	mBackgroundTex.disable();
	

	gl::enableAdditiveBlending();
	
	
// PLANETARY TEXT
    mPlanetaryTex.enableAndBind();
	float h		= mPlanetaryTex.getHeight();
	v1			= Vec2f( center.x + 60.0f, center.y-h*0.5f );
	v2			= v1 + mPlanetaryTex.getSize();
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
    gl::drawSolidRect( Rectf( v1, v2 ) );
    mPlanetaryTex.disable();
	
	
// STARGLOW
	mStarGlowTex.enableAndBind();
	Vec2f starSize = mStarGlowTex.getSize() * Rand::randFloat( 0.75f, 0.85f );
	v1			= center - starSize;
	v2			= v1 + starSize * 2.0f;
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.2f ) );
	gl::drawSolidRect( Rectf( v1, v2 ) );
	mStarGlowTex.disable();
	

	gl::enableAlphaBlending();

	mPlanetTex.enableAndBind();

// TINY PLANET	
	float speed		= app::getElapsedFrames() * 0.014f - M_PI_2;
	float sinAmt	= sin( speed );
	float cosAmt	= cos( speed );
	if( cosAmt > 0.0f ){
		pos			= Vec2f( sinAmt * 500.0f, 0.0f );
		radius		= 6.0f;
		v1			= center + pos - Vec2f( radius, radius );
		v2			= v1 + Vec2f( radius, radius ) * 2.0f;
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
		gl::drawSolidRect( Rectf( v1, v2 ) );
	}
	
// SMALL PLANET		
	speed			= app::getElapsedFrames() * 0.01f - M_PI_2;
	sinAmt			= sin( speed );
	cosAmt			= cos( speed );
	if( cosAmt > 0.0f ){
		pos				= Vec2f( sinAmt * 700.0f, 0.0f );
		radius			= 25.0f;
		v1				= center + pos - Vec2f( radius, radius );
		v2				= v1 + Vec2f( radius, radius ) * 2.0f;
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
		gl::drawSolidRect( Rectf( v1, v2 ) );
	}

// MEDIUM PLANET		
	speed			= app::getElapsedFrames() * 0.005f - M_PI_4;
	sinAmt			= sin( speed );
	cosAmt			= cos( speed );
	if( cosAmt > 0.0f ){
		pos				= Vec2f( sinAmt * 700.0f, 0.0f );
		radius			= 50.0f;
		v1				= center + pos - Vec2f( radius, radius );
		v2				= v1 + Vec2f( radius, radius ) * 2.0f;
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
		gl::drawSolidRect( Rectf( v1, v2 ) );
	}
	
// LARGE PLANET		
	speed			= app::getElapsedFrames() * 0.004f - M_PI_4;
	sinAmt			= sin( speed );
	cosAmt			= cos( speed );
	if( cosAmt > 0.0f ){
		pos				= Vec2f( sinAmt * 1500.0f, 0.0f );
		radius			= 300.0f;
		v1				= center + pos - Vec2f( radius, radius );
		v2				= v1 + Vec2f( radius, radius ) * 2.0f;
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
		gl::drawSolidRect( Rectf( v1, v2 ) );
	}
	
	/*
// LARGE PLANET
	v1			= Vec2f( center - Vec2f( mPlanetTex.getWidth(), mPlanetTex.getHeight() * 0.5f ) + Vec2f( app::getElapsedFrames() * 0.25f - 50.0f, 0.0f ) );
	v2			= v1 + mPlanetTex.getSize();
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	gl::drawSolidRect( Rectf( v1, v2 ) );
	*/
	mPlanetTex.disable();
    glPopMatrix();
}
