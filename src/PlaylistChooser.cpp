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

void PlaylistChooser::setup( const Font &font, WheelOverlayRef wheelOverlay, const Vec2f &interfaceSize )
{
    mFont					= font;
    mFontHeight             = mFont.getAscent() + mFont.getDescent();

    mTouchDragId			= 0;
    mTouchDragStartPos		= Vec2i( 0, 0 );
    mTouchDragStartOffset	= 0.0f;
    mTouchDragPlaylistIndex	= -1;
    
	mTouchVel				= 0.0f;
	mTouchPos				= Vec2i( 0, 0 );
	mTouchPrevPos			= Vec2i( 0, 0 );
    
    mOffsetX				= 0.0f;
	mOffsetXLocked			= 0.0f;
	
	mNumPlaylists			= 0;
	mIsDragging				= false;
	
	mPlaylistSize			= Vec2f( 120.f, 30.0f );
	mSpacerWidth			= 30.0f;
	mStartY					= 10.0f;

	mPrevIndex				= -1;
	mCurrentIndex			= 0;
	
	mWheelOverlay           = wheelOverlay;
    
    mInterfaceSize          = interfaceSize;
    
    mFullRect.set( 0, 0, mInterfaceSize.x, mStartY + mFontHeight + mStartY );
}

bool PlaylistChooser::touchBegan( ci::app::TouchEvent::Touch touch )
{
    if( mData == NULL || mTouchDragPlaylistIndex >= 0 ) return false;
	
    mIsDragging		= false;
	mTouchPrevPos	= mTouchPos;
	mTouchPos		= globalToLocal( touch.getPos() );
	mTouchVel		= 0.0f;
	
    Vec2f padding	= Vec2f( 10.0f, 10.0f );
    
    // see if we're touching a specific button
    mTouchDragPlaylistIndex = -1;
	for( int i = 0; i < mPlaylistRects.size(); i++ ){
		if( mPlaylistRects[i] && mPlaylistRects[i]->inflated( padding ).contains( mTouchPos ) ){
			mTouchDragPlaylistIndex = i;
            break;
        }
	}
    
    // allow dragging (but not selection) inside the whole panel as well
    if( mTouchDragPlaylistIndex >= 0 || mFullRect.contains(mTouchPos) ) {
        mTouchDragId			= touch.getId();
        mTouchDragStartPos		= mTouchPos;
        mTouchDragStartOffset	= mOffsetX;   
        return true;
    }

    return false;
}

bool PlaylistChooser::touchMoved( ci::app::TouchEvent::Touch touch )
{
    if (mData == NULL) return false;
    
    mIsDragging = true;
	
    mTouchPrevPos	= mTouchPos;
    mTouchPos		= globalToLocal( touch.getPos() );
    mOffsetX		= mTouchDragStartOffset + (mTouchPos.x - mTouchDragStartPos.x);            
    mTouchVel		= mTouchPos.x - mTouchPrevPos.x;
    
    return true;
}

bool PlaylistChooser::touchEnded( ci::app::TouchEvent::Touch touch )
{
    if (mData == NULL) return false;

	mIsDragging		= false;

    mTouchPos		= globalToLocal( touch.getPos() );
    mOffsetX		= mTouchDragStartOffset + (mTouchPos.x - mTouchDragStartPos.x);            
    
    if (mTouchDragPlaylistIndex >= 0) {
        
        float movement	= mTouchDragStartPos.distance( mTouchPos );
        if (movement < 15.0f) {
            // TODO: also measure time and don't allow long selection gaps
            mCurrentIndex = mTouchDragPlaylistIndex;
            mPrevIndex = mCurrentIndex; // set this so that we won't fire the callback twice
            mCbPlaylistSelected.call( mData->mPlaylists[mTouchDragPlaylistIndex] );
            mCbPlaylistTouched.call( mData->mPlaylists[mTouchDragPlaylistIndex] );
        }
    }
    
    mTouchDragId			= 0;
    mTouchDragPlaylistIndex = -1;
    mTouchDragStartPos		= mTouchPos;
    mTouchDragStartOffset	= mOffsetX;
    
    return true;
}


void PlaylistChooser::update()
{    
    if (mData == NULL) return;
    
    mInterfaceSize  = getRoot()->getInterfaceSize();
    
    // update full rect for layout, hit testing etc.
    mFullRect.set( 0, 0, mInterfaceSize.x, mStartY + mFontHeight + mStartY );

    // total size of all playlist labels + spaces
    float totalWidth = (mPlaylistSize.x * mNumPlaylists) + (mSpacerWidth * (mNumPlaylists-1));
    
    // min/max drag positions
    float maxOffsetX = (mInterfaceSize.x / 2.0f) - (mPlaylistSize.x / 2.0f);              // when dragging right
    float minOffsetX = (mPlaylistSize.x / 2.0f) + (mInterfaceSize.x / 2.0f) - totalWidth; // when dragging left
    
    /////////////
    
	if( !mIsDragging ){
        // carry on with inertia/momentum scrolling
		mOffsetX += mTouchVel;
		
        // spring back if we've gone too far
		if( mOffsetX < minOffsetX ){
			mTouchVel = 0.0f;
			mOffsetX += ( minOffsetX - mOffsetX ) * 0.2f;
		} 
        else if( mOffsetX > maxOffsetX ){
			mTouchVel = 0.0f;
			mOffsetX += ( maxOffsetX - mOffsetX ) * 0.2f;
		}
		        
        if( abs( mTouchVel ) > 1.0f ) {
            // slow down if we're moving fast:
            mTouchVel *= 0.95f;
        }
        else {
            // otherwise we're slow enough to think again
            // so cancel momentum completely:
			mTouchVel		 = 0.0f;

            // find the rect that covers the centerRect
/*            Rectf centerRect( (mInterfaceSize.x - mPlaylistSize.x) / 2.0f, 0.0f, 
                              (mInterfaceSize.x + mPlaylistSize.x) / 2.0f, mFullRect.getHeight() );
            RectRef match;
            int chosenIndex = -1;
            for( int i = 0; i < mPlaylistRects.size(); i++ ){
                RectRef rect = mPlaylistRects[i];
                if (rect && rect->x1 >= centerRect.x1 && rect->x2 <= centerRect.x2) {
                    match = rect;
                    chosenIndex = i;
                    break;
                }
            }
            
            if (match) {
                // ease to exact position (centered)
                float matchCenter = (match->x1 + match->x2) / 2.0f;                
                mOffsetX = mOffsetX + (matchCenter - mInterfaceSize.x);
            }
            
			mOffsetXLocked -= ( mOffsetXLocked - mOffsetX ) * 0.2f;
						
            // if we're done easing, check if we settled on a new selection
			if( chosenIndex >= 0 && abs( mOffsetXLocked - mOffsetX ) < 1.0f ){
				mPrevIndex			= mCurrentIndex;
				mCurrentIndex		= chosenIndex;
//				if( mPrevIndex != mCurrentIndex ){
//					mCbPlaylistSelected.call( mData->mPlaylists[mCurrentIndex] );
//				}	
			} */
			
		}
	}
    
    ////////// update placement rects
    
    mPlaylistRects.clear();
    
//    Vec2f pos( -mOffsetXLocked, mStartY );
    Vec2f pos( mOffsetX, mStartY );
    
    for( int i = 0; i < mNumPlaylists; i++ )
	{	
        if( pos.x < mInterfaceSize.x && pos.x + mPlaylistSize.x > 0.0f )
		{    
			if (!mTextures[i]) {
				makeTexture( i, mData->mPlaylists[i] );
            }            
            // put texture rect at center of playlist rect
			float x = pos.x + mPlaylistSize.x * 0.5f;
			float w	= mTextures[i].getWidth() * 0.5f;
			mPlaylistRects.push_back( RectRef( new Rectf( x - w, mStartY, x + w, mStartY + mFontHeight ) ) );
        } 
        else {
			mPlaylistRects.push_back( RectRef() );
		}
		
        pos.x += mSpacerWidth + mPlaylistSize.x;
    }    
}


void PlaylistChooser::draw()
{
	if( mData == NULL ) return;
	
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::enableAlphaBlending();
	gl::enableAdditiveBlending();

    float r = BRIGHT_BLUE.r;
    float g = BRIGHT_BLUE.g;
    float b = BRIGHT_BLUE.b;
    // opacity is supplied by UiLayer::draw
    float alpha = mOpacity * constrain(2.0f - mWheelOverlay->getWheelScale(), 0.0f, 1.0f);
    
    gl::color( ColorA( r, g, b, alpha * 0.125f ) );
    gl::drawLine( Vec2f(0,0), Vec2f(mInterfaceSize.x,0) );

    for( int i = 0; i < mNumPlaylists; i++ )
	{	
        RectRef rect = mPlaylistRects[i];
        if( rect && rect->x1 < mInterfaceSize.x && rect->x2 > 0.0f )
        {
            float textAlpha = mOpacity; // opacity is set by UiLayer
            
            if ( i == mCurrentIndex ) {
                gl::color( ColorA( 1, 1, 1, textAlpha ) );
            } 
            else if ( i == mTouchDragPlaylistIndex ) {
                gl::color( ColorA( BRIGHT_BLUE, textAlpha ) );
            } 
            else {
                gl::color( ColorA( BLUE, textAlpha ) );
            }
                        
            gl::draw( mTextures[i], rect->getUpperLeft() );
        }
    }
    
//    Rectf center( (mInterfaceSize.x - mPlaylistSize.x) / 2.0f, 0.0f, 
//                  (mInterfaceSize.x + mPlaylistSize.x) / 2.0f, mFullRect.getHeight() );
//
//    gl::color( ColorA( BRIGHT_BLUE, alpha * 0.15f ) );
//    gl::drawSolidRect( center );
//    
//    gl::color( ColorA( BRIGHT_BLUE, alpha * 0.5f ) );
//    gl::drawSolidRect( Rectf( center.x1, center.y1 + 0.0f, center.x2, center.y1 + 3.0f ) );
//    gl::drawSolidRect( Rectf( center.x1, center.y2 - 3.0f, center.x2, center.y2 - 0.0f ) );
    
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::enableAlphaBlending();
}

void PlaylistChooser::makeTexture( int index, ipod::PlaylistRef playlist )
{
    // FIXME: measure the texture and shorten the name if needed
	string name = playlist->getPlaylistName();
	if( name.length() > 20 ){
		name = name.substr( 0, 20 );
		name += "...";
	}
	TextLayout layout;
	layout.setFont( mFont );
	layout.setColor( Color::white() );
	layout.addLine( name );
	mTextures[index] = gl::Texture( layout.render( true, false ) );
}

float PlaylistChooser::getAlpha( float x )
{
	float per		= x / mInterfaceSize.x;
	float invCos	= ( 1.0f - (float)cos( per * TWO_PI ) ) * 0.5f;
	float cosPer	= pow( invCos, 0.5f );
	return cosPer;
}

void PlaylistChooser::setDataWorldCam( Data *data, World *world, CameraPersp *cam )
{
    mData			= data;
    mWorld			= world;
    mCam			= cam;
	mNumPlaylists	= mData->mPlaylists.size();
    mTextures.resize(mNumPlaylists);
}

float PlaylistChooser::getHeight()
{
    return mFullRect.getHeight();
}


