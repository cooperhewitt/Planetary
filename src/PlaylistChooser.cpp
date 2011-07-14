//
//  PlaylistChooser.cpp
//  Kepler
//
//  Created by Tom Carden on 7/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <string>
#include "cinder/Vector.h"
#include "cinder/PolyLine.h"
#include "PlaylistChooser.h"
#include "NodeArtist.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void PlaylistChooser::setup( AppCocoaTouch *app, const Orientation &orientation, const Font &font, const Color &lineColor )
{
    mLineColor = lineColor;
    mFont = font;
    mTextureFont = gl::TextureFont::create( font );
    setInterfaceOrientation(orientation);
    app->registerTouchesBegan( this, &PlaylistChooser::touchesBegan );
    app->registerTouchesMoved( this, &PlaylistChooser::touchesMoved );
    app->registerTouchesEnded( this, &PlaylistChooser::touchesEnded );
}

bool PlaylistChooser::touchesBegan( ci::app::TouchEvent event )
{
    if (mData == NULL || !mVisible) return false;
    
    Matrix44f invMtx = mOrientationMatrix.inverted();
    
    std::vector<TouchEvent::Touch> touches = event.getTouches();
    for (int j = 0; j < touches.size(); j++) {
        TouchEvent::Touch touch = touches[j];
        Vec2f pos = (mOrientationMatrix.inverted() * Vec3f(touch.getPos(),0)).xy();
        for (int i = 0; i < mPlaylistRects.size(); i++) {
            if (mPlaylistRects[i].contains(pos)) {
                // FIXME: just remember the id and dispatch this event on touchesEnded instead?
                mCbPlaylistSelected.call( mData->mPlaylists[i] );
                break;
            }
        }
    }
    
    return false;
}

bool PlaylistChooser::touchesMoved( ci::app::TouchEvent event )
{
    if (mData == NULL || !mVisible) return false;
    return false;    
}

bool PlaylistChooser::touchesEnded( ci::app::TouchEvent event )
{
    if (mData == NULL || !mVisible) return false;
    return false;    
}

void PlaylistChooser::setInterfaceOrientation( const Orientation &orientation )
{
    mInterfaceOrientation = orientation;
    mOrientationMatrix = getOrientationMatrix44(mInterfaceOrientation, getWindowSize());
    
    mInterfaceSize = app::getWindowSize();
    
    if ( isLandscapeOrientation( mInterfaceOrientation ) ) {
        mInterfaceSize = mInterfaceSize.yx(); // swizzle it!
    }
}

void PlaylistChooser::draw()
{
    if (mData == NULL || !mVisible) return;

    // show three-ish playlists, allow swipey navigation
    const float playlistWidth = 200.0f;
    const float playlistHeight = 200.0f;
    const Vec2f playlistSize( playlistWidth, playlistHeight );
    const Vec2f spacing( playlistWidth + 10.0f, 0 );
    const Vec2f textPadding( 5.0f, 5.0f + mFont.getAscent() );
    const float startX = 50.0f;
    const float startY = 150.0f;
    const float endX = mInterfaceSize.x - 50.0f;
    
    mPlaylistRects.clear();
    
    glPushMatrix();
    glMultMatrixf( mOrientationMatrix );
    
    glEnable(GL_SCISSOR_TEST);
//    glScissor( startX-1, startY-1+playlistHeight+2, endX-startX+2, playlistHeight+2 );
    
    int numPlaylists = mData->mPlaylists.size();
    Vec2f pos( startX, startY );
    for (int i = 0; i < numPlaylists; i++) {
        ipod::PlaylistRef playlist = mData->mPlaylists[i];
        
        Rectf listRect(pos, pos+playlistSize);
        mPlaylistRects.push_back(listRect);
        
        // scissor rect is from bottom left of window, FIXME: in *untransformed* coords :(
        glScissor( listRect.x1, mInterfaceSize.y - listRect.y2, listRect.getWidth(), listRect.getHeight() );        

        gl::color( ColorA(0.0f,0.0f,0.0f,0.25f) );
        gl::drawSolidRect( listRect );        
        
        gl::color( mLineColor );        
        string name = playlist->getPlaylistName();
        // FIXME: sadly have to deeper on the text stuff because TextureFont won't support international characters
        mTextureFont->drawStringWrapped( name, listRect, textPadding );
        gl::drawStrokedRect( Rectf(pos+Vec2f(1,1), pos+playlistSize) );

        // FIXME: use constellation logic
        // FIXME: probably don't draw this on the fly, cache things instead?
        vector<Vec2f> lines(playlist->size());
        for (int j = 0; j < playlist->size(); j++) {            
            ipod::TrackRef track = (*playlist)[j];
            NodeArtist* nodeArtist = mWorld->getArtistNodeById( track->getArtistId() );        
            // can't use mScreenPos here because we only calculate it for highlighted (labeled) nodes
            lines[j] = pos + mCam->worldToScreen(nodeArtist->mPos, playlistWidth, playlistHeight); // pretend screen is small
        }
        gl::draw(PolyLine2f(lines));
        
        pos += spacing;
        if (pos.x > endX) {
            break;
        }
    }

    glScissor( 0, mInterfaceSize.y, mInterfaceSize.x, mInterfaceSize.y );
    glDisable(GL_SCISSOR_TEST);
    
    glPopMatrix();
}

void PlaylistChooser::setDataWorldCam( Data *data, World *world, CameraPersp *cam )
{
    mData = data;
    mWorld = world;
    mCam = cam;
}
