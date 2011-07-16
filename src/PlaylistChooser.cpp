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
            if (movement < 5.0f) {
                // TODO: also measure time and don't allow long selection gaps
                mCbPlaylistSelected.call( mData->mPlaylists[mTouchDragPlaylistIndex] );
                mTouchDragId = 0;
                mTouchDragPlaylistIndex = -1;
                return true;                
            }
            mTouchDragId = 0;
            mTouchDragPlaylistIndex = -1;
            mTouchDragStartPos = pos;
            mTouchDragStartOffset = offsetX;
            return false;
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
    
//    std::vector<ScissorRect> scissorRects;
    
    mPlaylistRects.clear();
    
    glPushMatrix();
    glMultMatrixf( mOrientationMatrix );
    
    glEnable(GL_SCISSOR_TEST); // NB:- simple clipping for window-space rectangles only (no rotations)
    
    Vec2f pos( startX - offsetX, startY );
    for (int i = 0; i < numPlaylists; i++) {
        ipod::PlaylistRef playlist = mData->mPlaylists[i];

        // make a rect for everyone because the indexes are useful
        Rectf listRect(pos, pos+playlistSize);
        mPlaylistRects.push_back(listRect);

        if (pos.x < endX && pos.x + playlistWidth > startX) {
            
            ScissorRect sr; // make a rect in the current screen space
            sr.x = max(startX, min(endX, listRect.x1));
            sr.y = listRect.y1;
            sr.w = min(endX,listRect.x2) - sr.x;
            sr.h = listRect.getHeight();
            getWindowSpaceRect( sr.x, sr.y, sr.w, sr.h ); // transform rect to window coords
//            scissorRects.push_back( sr );
            glScissor( sr.x, sr.y, sr.w, sr.h ); // used for GL_SCISSOR_TEST
            
            gl::color( ColorA(0.0f,0.0f,0.0f,0.25f) );
            gl::drawSolidRect( listRect );        
            
            bool highlight = (i == mTouchDragPlaylistIndex) || (i == mCurrentPlaylistIndex);
            gl::color( highlight ? Color::white() : mLineColor ); // FIXME: make mHighlightColor?
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

//    gl::color( ColorA(1.0f,0.0f,0.0f,1.0f) );
//    for (int i = 0; i < scissorRects.size(); i++) {
//        ScissorRect sr = scissorRects[i];
//        std::cout << i << " " << sr.x << " " << sr.y << " " << sr.w << " " << sr.h << std::endl;
//        gl::drawStrokedRect( Rectf(sr.x, app::getWindowHeight() - sr.y - sr.h, sr.x + sr.w, app::getWindowHeight() - sr.y) );
//    }

}

// scissor rect is from *bottom left* of window in *untransformed* coords
// x,y,w,h to this function are in rotated screen space, from top left of screen
void PlaylistChooser::getWindowSpaceRect( float &x, float &y, float &w, float &h )
{
    Vec3f topLeft(x,y,0);
    Vec3f bottomRight(x+w,y+h,0);
    Vec2f tl = (mOrientationMatrix * topLeft).xy();
    Vec2f br = (mOrientationMatrix * bottomRight).xy();
    // use min max and fabs to canonicalize the scissor rect...
    x = min(br.x, tl.x);
    y = app::getWindowHeight() - max(br.y, tl.y); // flip y
    w = fabs(br.x - tl.x);
    h = fabs(br.y - tl.y);
}

void PlaylistChooser::setDataWorldCam( Data *data, World *world, CameraPersp *cam )
{
    mData = data;
    mWorld = world;
    mCam = cam;
}
