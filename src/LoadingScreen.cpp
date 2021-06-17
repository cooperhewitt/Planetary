//
//  LoadingScreen.cpp
//  Kepler
//
//  Created by Tom Carden on 3/17/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#include "LoadingScreen.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/ImageIo.h"
#include "BloomScene.h"
#include "cinder/app/AppCocoaTouch.h" // for loadResource
#include "Globals.h"

using namespace ci;
using namespace ci::app;

void LoadingScreen::setup( const ci::gl::Texture &planetaryTex, const ci::gl::Texture &planetTex,
                          const ci::gl::Texture &backgroundTex, const ci::gl::Texture &starGlowTex )
{
	mPlanetaryTex	= planetaryTex;
	mPlanetTex		= planetTex;
	mBackgroundTex	= backgroundTex;
    mStarGlowTex    = starGlowTex;
    mTextureProgress = 0.0;
    mTextureProgressDest = 0.0;
    mArtistProgress = 0.0;
    mArtistProgressDest = 0.0;
    mPlaylistProgress = 0.0;
    mPlaylistProgressDest = 0.0;
}

void LoadingScreen::setTextureProgress( float prop )
{
//    std::cout << "LoadingScreen::setTextureProgress " << prop << std::endl;
    if (prop >= 0) {
        mTextureProgressDest = prop;
    }
    else {
        // reset with negative values
        mTextureProgress = 0.0;
        mTextureProgressDest = 0.0;
    }
}

void LoadingScreen::setArtistProgress( float prop )
{
    if (prop >= 0) {
        mArtistProgressDest = prop;
    }
    else {
        // reset with negative values
        mArtistProgress = 0.0;
        mArtistProgressDest = 0.0;
    }
}

void LoadingScreen::setPlaylistProgress( float prop )
{
    if (prop >= 0) {
        mPlaylistProgressDest = prop;
    }
    else {
        // reset with negative values
        mPlaylistProgress = 0.0;
        mPlaylistProgressDest = 0.0;
    }
}

void LoadingScreen::update()
{
    mTextureProgress += (mTextureProgressDest - mTextureProgress) * 0.2f;
    if ( fabs(mTextureProgress - mTextureProgressDest) < 0.001f ) {
        mTextureProgress = mTextureProgressDest;
    }
    mArtistProgress += (mArtistProgressDest - mArtistProgress) * 0.2f;
    if ( fabs(mArtistProgress - mArtistProgressDest) < 0.001f ) {
        mArtistProgress = mArtistProgressDest;
    }
    mPlaylistProgress += (mPlaylistProgressDest - mPlaylistProgress) * 0.2f;
    if ( fabs(mPlaylistProgress - mPlaylistProgressDest) < 0.001f ) {
        mPlaylistProgress = mPlaylistProgressDest;
    }
    mInterfaceSize = getRoot()->getInterfaceSize();
}

bool LoadingScreen::isComplete() 
{
    bool textureProgressComplete = fabs(mTextureProgress - mTextureProgressDest) < 0.01f;
    bool artistProgressComplete = fabs(mArtistProgress - mArtistProgressDest) < 0.01f;
    bool playlistProgressComplete = fabs(mPlaylistProgress - mPlaylistProgressDest) < 0.01f;
    return textureProgressComplete && artistProgressComplete && playlistProgressComplete;
}

void LoadingScreen::draw()
{
	Vec2f pos;
	float radius;
	
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

	float barHeight = 2.0f;
    gl::color( BLUE );
    gl::drawSolidRect( Rectf( 0, mInterfaceSize.y - barHeight * 3.0f, mInterfaceSize.x * mTextureProgress,  mInterfaceSize.y - barHeight * 2.0f ) );
    gl::color( BRIGHT_BLUE );
    gl::drawSolidRect( Rectf( 0, mInterfaceSize.y - barHeight * 2.0f, mInterfaceSize.x * mArtistProgress,   mInterfaceSize.y - barHeight * 1.0f ) );
    gl::color( BRIGHT_YELLOW );
    gl::drawSolidRect( Rectf( 0, mInterfaceSize.y - barHeight * 1.0f, mInterfaceSize.x * mPlaylistProgress, mInterfaceSize.y - barHeight * 0.0f ) );
}
