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

void LoadingScreen::setup( AppCocoaTouch *app )
{
    app->registerOrientationChanged(this, &LoadingScreen::orientationChanged);
    mDeviceOrientation = PORTRAIT_ORIENTATION;
    mInterfaceSize = getWindowSize();
}

bool LoadingScreen::orientationChanged( OrientationEvent event )
{
    if ( UIDeviceOrientationIsValidInterfaceOrientation( event.getOrientation() ) ){
        mDeviceOrientation = event.getOrientation();
    }
    else {
        return false;
    }
    
    Vec2f windowSize = getWindowSize();    
    
    mOrientationMatrix.setToIdentity();
    
    if ( mDeviceOrientation == UPSIDE_DOWN_PORTRAIT_ORIENTATION )
    {
        mOrientationMatrix.translate( Vec3f( windowSize.x, windowSize.y, 0 ) );            
        mOrientationMatrix.rotate( Vec3f( 0, 0, M_PI ) );
    }
    else if ( mDeviceOrientation == LANDSCAPE_LEFT_ORIENTATION )
    {
        mOrientationMatrix.translate( Vec3f( windowSize.x, 0, 0 ) );
        mOrientationMatrix.rotate( Vec3f( 0, 0, M_PI/2.0f ) );
    }
    else if ( mDeviceOrientation == LANDSCAPE_RIGHT_ORIENTATION )
    {
        mOrientationMatrix.translate( Vec3f( 0, windowSize.y, 0 ) );
        mOrientationMatrix.rotate( Vec3f( 0, 0, -M_PI/2.0f ) );
    }
    
    mInterfaceSize = windowSize;
    
    // TODO: isLandscape()/isPortrait() conveniences on event?
    if ( UIInterfaceOrientationIsLandscape(mDeviceOrientation) ) {
        mInterfaceSize = mInterfaceSize.yx(); // swizzle it!
    }
    
    return false;
}

void LoadingScreen::draw( gl::Texture mLoadingTex, gl::Texture mStarGlowTex, gl::Texture mStarTex)
{
    gl::setMatricesWindow( app::getWindowSize() );    

    gl::pushModelView();
    gl::multModelView( mOrientationMatrix );

    Vec2f center = mInterfaceSize * 0.5f;
    
    gl::color( Color::white() );
    mLoadingTex.enableAndBind();
    Vec2f offset = mLoadingTex.getSize() * 0.5f;
    gl::drawSolidRect( Rectf(center - offset, center + offset) );
    mLoadingTex.disable();
    
    gl::enableAdditiveBlending();
    
    Rectf bigRect( center - Vec2f(50,50), center + Vec2f(50,50) );
    mStarGlowTex.enableAndBind();
    gl::drawSolidRect( bigRect );
    mStarGlowTex.disable();

    Rectf rect( center - Vec2f(28,28), center + Vec2f(28,28) );
    mStarTex.enableAndBind();
    gl::drawSolidRect( rect );

    float smallOffset	= cos( app::getElapsedSeconds() * 0.3f + 2.0f ) * 30.0f;
    Rectf smallRect( center - Vec2f(4.0f-smallOffset,4.0f), center + Vec2f(4.0f + smallOffset,4.0f) );
    
    //float mediumOffset	= ( getElapsedSeconds() - 3.0f ) * 10.0f;	
    //Rectf mediumRect	= Rectf( xCenter - 25.0f + mediumOffset * 2.5f, yCenter - 25.0f, xCenter + 25.0f + mediumOffset * 2.5f, yCenter + 25.0f );
    gl::color( Color::black() );
    gl::disableAlphaBlending();
    gl::enableAlphaBlending();
    gl::drawSolidRect( smallRect );
    //gl::drawSolidRect( mediumRect );
    mStarTex.disable();
    
    gl::disableAlphaBlending();
    
    gl::popModelView();
}
