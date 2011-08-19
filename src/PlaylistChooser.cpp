//
//  PlaylistChooser.cpp
//  Kepler
//
//  Created by Tom Carden on 7/10/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include <string>
#include "cinder/Vector.h"
#include "cinder/PolyLine.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Path2d.h"
#include "PlaylistChooser.h"
#include "NodeArtist.h"
#include "BloomScene.h"
#include "Globals.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void PlaylistChooser::setup( const Font &font, UiLayerRef uiLayer, WheelOverlayRef wheelOverlay )
{
    mFont					= font;

    mTouchDragId			= 0;
    mTouchDragStartPos		= Vec2i( 0, 0 );
    mTouchDragStartOffset	= 0.0f;
    mTouchDragPlaylistIndex	= -1;
    
	mTouchVel				= 0.0f;
	mTouchPos				= Vec2i( 0, 0 );
	mTouchPrevPos			= Vec2i( 0, 0 );
    mOffsetX				= -200.0f;
	mOffsetXLocked			= -200.0f;
	
	mNumPlaylists			= 0;
	mIsDragging				= false;
	
	mPlaylistSize			= Vec2f( 120.f, 30.0f );
	mSpacerWidth			= 30.0f;
	mStartY					= 350.0f;

	mPrevIndex				= -1;
	mCurrentIndex			= 0;
	
	mWheelOverlay = wheelOverlay;
    mUiLayer = uiLayer;
}

bool PlaylistChooser::touchBegan( ci::app::TouchEvent::Touch touch )
{
    if( mData == NULL || mTouchDragPlaylistIndex >= 0 ) return false;
	
    mIsDragging		= false;
	mTouchPrevPos	= mTouchPos;
	mTouchPos		= globalToLocal( touch.getPos() );
	mTouchVel		= 0.0f;
	
	for( int i = 0; i < mPlaylistRects.size(); i++ ){
		if( mPlaylistRects[i].contains( mTouchPos ) ){
			// remember the id and dispatch this event on touchesEnded if it hasn't moved much (otherwise just drag)
			mTouchDragId			= touch.getId();
			mTouchDragStartPos		= mTouchPos;
			mTouchDragStartOffset	= mOffsetX;
			mTouchDragPlaylistIndex = i;
			return true;
		}
	}

    return false;
}

bool PlaylistChooser::touchMoved( ci::app::TouchEvent::Touch touch )
{
    if (mData == NULL || mTouchDragPlaylistIndex < 0) return false;
    mIsDragging = true;
	
    mTouchPrevPos	= mTouchPos;
    mTouchPos		= globalToLocal( touch.getPos() );
    mOffsetX		= mTouchDragStartOffset + ( mTouchDragStartPos.x - mTouchPos.x );
    mTouchVel		= mTouchPos.x - mTouchPrevPos.x;
    
    return true;
}

bool PlaylistChooser::touchEnded( ci::app::TouchEvent::Touch touch )
{
    if (mData == NULL) return false;

	mIsDragging		= false;

    mTouchPos		= globalToLocal( touch.getPos() );
    
    if (mTouchDragPlaylistIndex >= 0) {
        float movement	= mTouchDragStartPos.distance( mTouchPos );
        mOffsetX		= mTouchDragStartOffset + (mTouchDragStartPos.x - mTouchPos.x);            
        if (movement < 15.0f) {
            // TODO: also measure time and don't allow long selection gaps
            mCurrentIndex = mTouchDragPlaylistIndex;
            mPrevIndex = mCurrentIndex; // set this so that we won't fire the callback twice
            mCbPlaylistSelected.call( mData->mPlaylists[mTouchDragPlaylistIndex] );
            mCbPlaylistTouched.call( mData->mPlaylists[mTouchDragPlaylistIndex] );
            mOffsetX = mTouchDragPlaylistIndex * ( mPlaylistSize.x + mSpacerWidth );
            mTouchDragId = 0;
            mTouchDragPlaylistIndex = -1;
            mWheelOverlay->setShowWheel(false);
            return true;                
        }
        
        mTouchDragId			= 0;
        mTouchDragPlaylistIndex = -1;
        mTouchDragStartPos		= mTouchPos;
        mTouchDragStartOffset	= mOffsetX;
    }
    
    // if we didn't already return...
    // we can dismiss the wheel since we must have tapped inside the world
//    mWheelOverlay->setShowWheel(false);
    
    return false;
}


void PlaylistChooser::update()
{    
    if (mData == NULL) return;
    
    mInterfaceSize  = getRoot()->getInterfaceSize();

    // wheel is already centered, so we have to subtract half of interface height    
    mStartY	= mUiLayer->getPanelYPos() - (mInterfaceSize.y / 2.0f) - 100.0f;
        
    float maxOffsetX = (mPlaylistSize.x * (mNumPlaylists+0.5f)) + (mSpacerWidth * (mNumPlaylists-1));
    float minOffsetX = -mPlaylistSize.x * 0.5f;
    
    /////////////
    
	if( !mIsDragging ){
        
        // carry on with inertia/momentum scrolling...
		mOffsetX		-= mTouchVel;
		
        // spring back if we've gone too far...
		if( mOffsetX < minOffsetX ){
			mTouchVel = 0.0f;
			mOffsetX -= ( mOffsetX - minOffsetX ) * 0.2f;
		} 
        else if( mOffsetX > maxOffsetX ){
			mTouchVel = 0.0f;
			mOffsetX -= ( mOffsetX - maxOffsetX ) * 0.2f;
		}
		
		if( abs( mTouchVel ) < 10.0f ){
            
            // how far through are we?
			float offsetPer	 = (mOffsetX-mPlaylistSize.x/2.0) / (mPlaylistSize.x + mSpacerWidth);
            // round that for index in mData->mPlaylists
			int chosenIndex  = constrain( (int)round( offsetPer ), 0, mNumPlaylists-1 );
            // ease to exact position (centered)
			float lockOffset = chosenIndex * (mPlaylistSize.x + mSpacerWidth) + mPlaylistSize.x/2.0;            
			mOffsetXLocked	 -= ( mOffsetXLocked - lockOffset ) * 0.2f;
			mOffsetX		 = lockOffset;
			
            // cancel momentum
			mTouchVel			= 0.0f;
			
            // if we're done easing, check if we settled on a new selection...
			if( abs( mOffsetXLocked - lockOffset ) < 1.0f ){
				mPrevIndex			= mCurrentIndex;
				mCurrentIndex		= chosenIndex;
				if( mPrevIndex != mCurrentIndex ){
					mCbPlaylistSelected.call( mData->mPlaylists[mCurrentIndex] );
				}	
			}
			
		} else {
			mOffsetXLocked		= mOffsetX;
			mTouchVel			*= 0.95f; // slow down
		}
	} else {
		mOffsetXLocked = mOffsetX;
	}
    
    // calculate full rect for ignoring touches in main app
    // this will be one frame out of date but that should be OK
    if (mPlaylistRects.size() > 0) {
        mFullRect = mPlaylistRects[0];
        for (int i = 0; i < mPlaylistRects.size(); i++) {
            mFullRect.include(mPlaylistRects[i]);
        }
    }
}


void PlaylistChooser::draw()
{
	if( mData == NULL ) return;
	
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::enableAlphaBlending();

	gl::color( Color::white() );
		
    mPlaylistRects.clear();

    float border = mPlaylistSize.x * 0.5f;
    float startX = -mInterfaceSize.x / 2.0 + border;
    float endX = mInterfaceSize.x / 2.0 - border;    
	
    Vec2f pos( -mOffsetXLocked, mStartY );
	    
	gl::enableAdditiveBlending();

    for( int i = 0; i < mNumPlaylists; i++ )
	{	
		ipod::PlaylistRef playlist = mData->mPlaylists[i];
		
        if( pos.x < endX && pos.x + mPlaylistSize.x > startX )
		{
			float x			= pos.x + mPlaylistSize.x * 0.5f; // x center of the rect
			float alpha		= getAlpha( x );

			if (!mTextures[i]) {
				makeTexture( i, playlist );
            }
			
            if ( i == mCurrentIndex ) {
                gl::color( ColorA( 1, 1, 1, alpha ) );
			} 
            else if ( i == mTouchDragPlaylistIndex ) {
                gl::color( ColorA( BRIGHT_BLUE, alpha ) );
			} 
            else {
                gl::color( ColorA( BLUE, alpha ) );
            }
			
			float w			= mTextures[i].getWidth() * 0.5f;
			float h			= mFont.getAscent() + mFont.getDescent();
			float padding	= 10.0f;
			Rectf rect		= Rectf( x - w - padding, mStartY - padding, 
                                     x + w + padding, mStartY + h + padding );
			mPlaylistRects.push_back( rect );
			gl::draw( mTextures[i], Vec2f( x - w, mStartY ) );

            // debuggenrectankles
//            gl::drawStrokedRect( rect );
//            gl::drawStrokedRect( Rectf( x - w, mStartY, x + w, mStartY + h ) );
//            gl::drawStrokedRect( Rectf( x - mPlaylistSize.x/2.0, mStartY, x + mPlaylistSize.x/2.0, mStartY + h ) );

        } else {
			// STUPID FIX:
			// Making sure all rects are made, even ones that are offscreen.
			mPlaylistRects.push_back( Rectf( Vec2f( -500.0f, 0.0f ), Vec2f( -400.0f, 0.0f ) ) );
		}
		
        pos.x += mSpacerWidth + mPlaylistSize.x;
        
        if( pos.x > endX ){
            break;
        }
    }

    // highlight the region things will settle into...
    float w = mPlaylistSize.x/2.0;
    Path2d path;
    path.moveTo( -w, mStartY - 8.0f);
    path.curveTo( Vec2f( -w * 0.8f, mStartY - 14.0f), 
                  Vec2f( -w * 0.1f, mStartY - 16.0f), 
                  Vec2f( 0.0f, mStartY - 20.0f) );
    path.curveTo( Vec2f( w * 0.1f, mStartY - 16.0f), 
                  Vec2f( w * 0.8f, mStartY - 14.0f), 
                  Vec2f( w, mStartY - 8.0f) );
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.2f ) );
    gl::draw(path);
//    gl::drawStrokedRect( Rectf( -w, mStartY, w, mStartY + h ) );    
    
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::enableAlphaBlending();
    
}

void PlaylistChooser::makeTexture( int index, ipod::PlaylistRef playlist )
{
	string name = playlist->getPlaylistName();
	if( name.length() > 20 ){
		name = name.substr( 0, 20 );
		name += "...";
	}
	TextLayout layout;
	layout.setFont( mFont );
	layout.setColor( BRIGHT_BLUE );
	layout.addLine( name );
	mTextures[index] = gl::Texture( layout.render( true, false ) );
}

float PlaylistChooser::getAlpha( float x )
{
	float per		= (x + mInterfaceSize.x/2.0f) / mInterfaceSize.x;
	float invCos	= ( 1.0f - (float)cos( per * M_PI * 2.0f ) ) * 0.5f;
	float cosPer	= pow( invCos, 0.5f );
	return cosPer;
}

float PlaylistChooser::getScale( float x )
{
	float per		= (x + mInterfaceSize.x/2.0f) / mInterfaceSize.x;
	float invCos	= ( 1.0f - (float)cos( per * M_PI * 2.0f ) ) * 0.5f;
	float cosPer	= max( pow( invCos, 3.5f ) + 0.4f, 0.5f );
	return cosPer;
}

float PlaylistChooser::getNewX( float x )
{
	float per		= (x + mInterfaceSize.x/2.0f) / mInterfaceSize.x;
    per *= 0.7f;
    per += 0.15f;
	float cosPer	= ( 1.0f - cos( per * M_PI ) ) * 0.5f;
	return cosPer * mInterfaceSize.x;
}

float PlaylistChooser::getNewY( float x )
{
	float per		= (x + mInterfaceSize.x/2.0f) / mInterfaceSize.x;
	float sinPer	= sin( per * M_PI );
	return sinPer;
}

void PlaylistChooser::setDataWorldCam( Data *data, World *world, CameraPersp *cam )
{
    mData			= data;
    mWorld			= world;
    mCam			= cam;
	mNumPlaylists	= mData->mPlaylists.size();
    mTextures.resize(mNumPlaylists);
}


