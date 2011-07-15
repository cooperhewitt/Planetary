//
//  FilterToggleButton.cpp
//  Kepler
//
//  Created by Tom Carden on 7/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "FilterToggleButton.h"
#include "Globals.h"
#include "cinder/Text.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

void FilterToggleButton::setup(AppCocoaTouch *app, const State::FilterMode &filterMode, const Font &font, const Orientation &orientation)
{    
    TextLayout layout;
    layout.setFont( font );
    layout.setColor( ColorA( BRIGHT_BLUE, 1.0f ) );
    layout.addCenteredLine( "Alphabetical" );
    mAlphaTexture = gl::Texture( layout.render( true, false ) );

    layout = TextLayout();
    layout.setFont( font );
    layout.setColor( ColorA( BRIGHT_BLUE, 1.0f ) );
    layout.addCenteredLine( "Playlists" );
    mPlaylistTexture = gl::Texture( layout.render( true, false ) );    

    setFilterMode( filterMode );
    setInterfaceOrientation( orientation );
    
    app->registerTouchesBegan( this, &FilterToggleButton::touchesBegan );
    app->registerTouchesEnded( this, &FilterToggleButton::touchesEnded );    
}

void FilterToggleButton::setInterfaceOrientation( const Orientation &orientation )
{
    mInterfaceOrientation = orientation;
    mOrientationMatrix = getOrientationMatrix44(mInterfaceOrientation, getWindowSize());
    
    mInterfaceSize = app::getWindowSize();
    
    if ( isLandscapeOrientation( mInterfaceOrientation ) ) {
        mInterfaceSize = mInterfaceSize.yx(); // swizzle it!
    }
    
    const float padding = 10.0f;
    const Vec2f paddingVec( padding, padding );
    
    mAlphaPos.set( mInterfaceSize.x/2.0f - mAlphaTexture.getWidth() - padding, 60.0f );
    mPlaylistPos.set( mInterfaceSize.x/2.0f + padding, 60.0f );
    
    mAlphaRect = Rectf( mAlphaPos - paddingVec, 
                        mAlphaPos + mAlphaTexture.getSize() + paddingVec );
    mPlaylistRect = Rectf( mPlaylistPos - paddingVec, 
                           mPlaylistPos + mPlaylistTexture.getSize() + paddingVec );
}

bool FilterToggleButton::touchesBegan( TouchEvent event )
{
    if (!mVisible) return false;
    if (event.getTouches().size() == 1) {
        TouchEvent::Touch touch = event.getTouches()[0];
        Vec2f pos = (mOrientationMatrix.inverted() * Vec3f(touch.getPos(),0)).xy();
        if (mPlaylistRect.contains(pos) || mAlphaRect.contains(pos)) {
            return true;
        }
    }
    return false;
}

bool FilterToggleButton::touchesEnded( TouchEvent event )
{
    if (!mVisible) return false;    
    if (event.getTouches().size() == 1) {
        TouchEvent::Touch touch = event.getTouches()[0];
        Vec2f pos = (mOrientationMatrix.inverted() * Vec3f(touch.getPos(),0)).xy();
        if (mAlphaRect.contains(pos)) {
            // TODO: check this touch started in this button?
            mCbFilterModeSelected.call(State::FilterModeAlphaChar);
            return true;
        }
        else if (mPlaylistRect.contains(pos)) {
            // TODO: check this touch started in this button?
            mCbFilterModeSelected.call(State::FilterModePlaylist);          
            return true;
        }
    }
    return false;
}

void FilterToggleButton::setFilterMode(const State::FilterMode &filterMode)
{
    mFilterMode = filterMode;
}

void FilterToggleButton::draw()
{    
    if (!mVisible) return; // FIXME fade in/out
    
    glPushMatrix();
    glMultMatrixf( mOrientationMatrix );
    
    gl::color( mFilterMode == State::FilterModeAlphaChar ? ColorA( BRIGHT_BLUE, 0.45f ) : ColorA( 0.0f, 0.0f, 0.0f, 0.15f) );
    gl::drawSolidRect( mAlphaRect );
    
    gl::color( mFilterMode == State::FilterModePlaylist ? ColorA( BRIGHT_BLUE, 0.45f ) : ColorA( 0.0f, 0.0f, 0.0f, 0.15f) );
    gl::drawSolidRect( mPlaylistRect );    
    
    gl::color( Color::white() );
    gl::draw( mAlphaTexture, mAlphaPos );
    gl::draw( mPlaylistTexture, mPlaylistPos );
    
    // FIXME: whither roundrect? :)
    // at least, fix the corners to be snuggier than this
    gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.5f ) );
    gl::drawStrokedRect( mAlphaRect );
    gl::drawStrokedRect( mPlaylistRect );    
    //gl::drawLine( Vec2f(mInterfaceSize.x/2.0f, 100.0f), Vec2f(mInterfaceSize.x/2.0f, 100.0f+mAlphaTexture.getHeight()) );
    
    glPopMatrix();
}
