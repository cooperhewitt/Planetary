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
    mTouchDragId = 0;
    mTouchDragStartPos.set(0,0);
    mTouchDragStartOffset = 0.0f;
    mTouchDragPlaylistIndex = -1;    
    offsetX = 0.0f;
}

bool PlaylistChooser::touchesBegan( ci::app::TouchEvent event )
{
    if (mData == NULL || !mVisible || mTouchDragPlaylistIndex >= 0) return false;
    
    Matrix44f invMtx = mOrientationMatrix.inverted();
    
    std::vector<TouchEvent::Touch> touches = event.getTouches();
    for (int j = 0; j < touches.size(); j++) {
        TouchEvent::Touch touch = touches[j];
        Vec2f pos = (invMtx * Vec3f(touch.getPos(),0)).xy();
        for (int i = 0; i < mPlaylistRects.size(); i++) {
            if (mPlaylistRects[i].contains(pos)) {
                // remember the id and dispatch this event on touchesEnded if it hasn't moved much (otherwise just drag)
                mTouchDragId = touch.getId();
                mTouchDragStartPos = pos;
                mTouchDragStartOffset = offsetX;
                mTouchDragPlaylistIndex = i;
                return true;
            }
        }
    }
    
    return false;
}

bool PlaylistChooser::touchesMoved( ci::app::TouchEvent event )
{
    if (mData == NULL || !mVisible || mTouchDragPlaylistIndex < 0) return false;
    
    std::vector<TouchEvent::Touch> touches = event.getTouches();
    for (int j = 0; j < touches.size(); j++) {
        TouchEvent::Touch touch = touches[j];
        if (touch.getId() == mTouchDragId) {
            Vec2f pos = (mOrientationMatrix.inverted() * Vec3f(touch.getPos(),0)).xy();
            offsetX = mTouchDragStartOffset + (mTouchDragStartPos.x - pos.x);
            return true;
        }
    }
    
    return false;    
}

bool PlaylistChooser::touchesEnded( ci::app::TouchEvent event )
{
    if (mData == NULL || !mVisible || mTouchDragPlaylistIndex < 0) return false;
    
    std::vector<TouchEvent::Touch> touches = event.getTouches();
    for (int j = 0; j < touches.size(); j++) {
        TouchEvent::Touch touch = touches[j];
        if (touch.getId() == mTouchDragId) {
            Vec2f pos = (mOrientationMatrix.inverted() * Vec3f(touch.getPos(),0)).xy();
            float movement = mTouchDragStartPos.distance(pos);
            offsetX = mTouchDragStartOffset + (mTouchDragStartPos.x - pos.x);            
            if (movement < 3.0f) {
                // TODO: also measure time and don't allow long selection gaps
                mCbPlaylistSelected.call( mData->mPlaylists[mTouchDragId] );
            }
            // remember the id and dispatch this event on touchesEnded if it hasn't moved much (otherwise just drag)
            mTouchDragId = 0;
            mTouchDragStartPos = pos;
            mTouchDragStartOffset = offsetX;
            mTouchDragPlaylistIndex = -1;
        }
    }
    
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
    const int numPlaylists = mData->mPlaylists.size();
    
    // FIXME: keep track of scrolling momentum, do proper springy iOS style limits    
//    const float maxOffsetX = (numPlaylists * playlistWidth) + ((numPlaylists-1) * spacing.x) - (endX - startX);
//    if (offsetX < 0.0) offsetX = 0.0;
//    if (offsetX > maxOffsetX) offsetX = maxOffsetX;
    
    mPlaylistRects.clear();
    
    glPushMatrix();
    glMultMatrixf( mOrientationMatrix );
    
    glEnable(GL_SCISSOR_TEST);
//    glScissor( startX-1, startY-1+playlistHeight+2, endX-startX+2, playlistHeight+2 );
    
    Vec2f pos( startX - offsetX, startY );
    for (int i = 0; i < numPlaylists; i++) {
        ipod::PlaylistRef playlist = mData->mPlaylists[i];

        // make a rect for everyone because the indexes are useful
        Rectf listRect(pos, pos+playlistSize);
        mPlaylistRects.push_back(listRect);

        if (pos.x < endX && pos.x + playlistWidth > startX) {
                    
            // scissor rect is from bottom left of window, FIXME: in *untransformed* coords :(
            glScissor( max(startX, min(endX, listRect.x1)), mInterfaceSize.y - listRect.y2, min(endX,listRect.x2) - max(startX, listRect.x1), listRect.getHeight() );        

            gl::color( ColorA(0.0f,0.0f,0.0f,0.25f) );
            gl::drawSolidRect( listRect );        
            
            // FIXME: make mHighlightColor?
            gl::color( (i == mTouchDragPlaylistIndex) ? Color::white() : mLineColor );        
            string name = playlist->getPlaylistName();
            // FIXME: sadly have to dig deeper on the text stuff because TextureFont won't support international characters
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
            
        }
        
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
