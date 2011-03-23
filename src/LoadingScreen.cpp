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

using namespace ci;
using namespace ci::app;

void LoadingScreen::draw( const AppCocoaTouch *app, const ci::gl::Texture &mLoadingTex, const ci::gl::Texture &mStarGlowTex, const ci::gl::Texture &mStarTex)
{
    gl::color( Color::white() );
    mLoadingTex.enableAndBind();
    gl::setMatricesWindow( app->getWindowSize() );
    gl::drawSolidRect( app->getWindowBounds() );
    mLoadingTex.disable();
    
    gl::enableAdditiveBlending();
    float xCenter = app->getWindowWidth() * 0.5f;
    float yCenter = app->getWindowHeight() * 0.5f;
    
    Rectf bigRect = Rectf( xCenter - 50, yCenter - 50, xCenter + 50, yCenter + 50 );
    mStarGlowTex.enableAndBind();
    gl::drawSolidRect( bigRect );
    mStarGlowTex.disable();
    
    Rectf rect = Rectf( xCenter - 28, yCenter - 28, xCenter + 28, yCenter + 28 );
    mStarTex.enableAndBind();
    gl::drawSolidRect( rect );
    
    float smallOffset	= cos( app->getElapsedSeconds() * 0.3f + 2.0f ) * 30.0f;
    Rectf smallRect		= Rectf( xCenter - 4.0f + smallOffset, yCenter - 4.0f, xCenter + 4.0f + smallOffset, yCenter + 4.0f );
    //float mediumOffset	= ( getElapsedSeconds() - 3.0f ) * 10.0f;	
    //Rectf mediumRect	= Rectf( xCenter - 25.0f + mediumOffset * 2.5f, yCenter - 25.0f, xCenter + 25.0f + mediumOffset * 2.5f, yCenter + 25.0f );
    gl::color( Color::black() );
    gl::disableAlphaBlending();
    gl::enableAlphaBlending();
    gl::drawSolidRect( smallRect );
    //gl::drawSolidRect( mediumRect );
    mStarTex.disable();
    
    gl::disableAlphaBlending();    
}
