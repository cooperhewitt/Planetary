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
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "PlaylistChooser.h"
#include "NodeArtist.h"
#include "BloomScene.h"
#include "Globals.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void PlaylistChooser::setup( const Font &font, WheelOverlayRef wheelOverlay )
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
	mXCenter				= 0.0f;
	
	mNumPlaylists			= 0;
	mIsDragging				= false;
	
	mPlaylistWidth			= 120.0f;
	mPlaylistHeight			= 30.0f;
	mPlaylistSize			= Vec2f( mPlaylistWidth, mPlaylistHeight );
	mSpacerWidth			= 30.0f;
	mBorder					= mPlaylistWidth * 0.5f;
	mStartX					= mBorder;
	mStartY					= 350.0f;
    
    // FIXME: load these in main app and pass in to setup(...)
	mTex					= gl::Texture( loadImage( loadResource( "playlist.png" ) ) );
	mBgTex					= gl::Texture( loadImage( loadResource( "playlistBg.png" ) ) );	
	
	mPrevIndex				= -1;
	mCurrentIndex			= 0;
	
	mWheelOverlay = wheelOverlay;
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
	
	if (touch.getId() == mTouchDragId) {
		mTouchPrevPos	= mTouchPos;
		mTouchPos		= globalToLocal( touch.getPos() );
		mOffsetX		= mTouchDragStartOffset + ( mTouchDragStartPos.x - mTouchPos.x );
		mTouchVel		= mTouchPos.x - mTouchPrevPos.x;
		
		return true;
	}
    
    return false;    
}

bool PlaylistChooser::touchEnded( ci::app::TouchEvent::Touch touch )
{
    if (mData == NULL || mTouchDragPlaylistIndex < 0) return false;

	mIsDragging		= false;
	if (touch.getId() == mTouchDragId) {
		mTouchPos		= globalToLocal( touch.getPos() );
		float movement	= mTouchDragStartPos.distance( mTouchPos );
		mOffsetX		= mTouchDragStartOffset + (mTouchDragStartPos.x - mTouchPos.x);            
		if (movement < 15.0f) {
			// TODO: also measure time and don't allow long selection gaps
//			mCbPlaylistSelected.call( mData->mPlaylists[mTouchDragPlaylistIndex] );
			float lockOffset	= mTouchDragPlaylistIndex * ( mPlaylistWidth + mSpacerWidth ) - mLeftLimit;
			
			mOffsetX			= lockOffset;
			mTouchDragId = 0;
			mTouchDragPlaylistIndex = -1;
			return true;                
		}
		mTouchDragId			= 0;
		mTouchDragPlaylistIndex = -1;
		mTouchDragStartPos		= mTouchPos;
		mTouchDragStartOffset	= mOffsetX;

		return false;
	}
    
    return false;    
}


void PlaylistChooser::update()
{    
    if (mData == NULL) return;
    
    mInterfaceSize = getRoot()->getInterfaceSize();

    mStartY			= mWheelOverlay->getRadius() - 20.0f;
        
    mStartX         = -mInterfaceSize.x/2.0 + mBorder;
    mEndX			= mInterfaceSize.x/2.0 - mBorder;
    mXCenter		= 0.0f;
    
    mLeftLimit		= mXCenter - mPlaylistWidth;
    mMaxOffsetX		= ( mNumPlaylists * mPlaylistWidth) + ( (mNumPlaylists-1) * mSpacerWidth ) - (mEndX - mStartX) + mLeftLimit;
    mMinOffsetX		= -mLeftLimit;    
    
    /////////////
    
	if( !mIsDragging ){
		mOffsetX		-= mTouchVel;
		
		if( mOffsetX < mMinOffsetX ){
			mTouchVel = 0.0f;
			mOffsetX -= ( mOffsetX - mMinOffsetX ) * 0.2f;
			
		} else if( mOffsetX > mMaxOffsetX ){
			mTouchVel = 0.0f;
			mOffsetX -= ( mOffsetX - mMaxOffsetX ) * 0.2f;
		}
		
		if( abs( mTouchVel ) < 10.0f ){
			float newOffset		= mOffsetX + mLeftLimit;
			float chosenIndex	= newOffset/(mPlaylistWidth + mSpacerWidth);
			int chosenId		= constrain( (int)round( chosenIndex ), 0, mNumPlaylists-1 );
			float lockOffset	= chosenId * (mPlaylistWidth + mSpacerWidth) - mLeftLimit;
			mOffsetXLocked		-= ( mOffsetXLocked - lockOffset ) * 0.2f;
			mOffsetX			= lockOffset;
			
			mTouchVel			= 0.0f;
			
			
			if( abs( mOffsetXLocked - lockOffset ) < 1.0f ){
				mPrevIndex			= mCurrentIndex;
				mCurrentIndex		= chosenId;
				if( mPrevIndex != mCurrentIndex ){
					mCbPlaylistSelected.call( mData->mPlaylists[mCurrentIndex] );
				}	
			}
			
		} else {
			mOffsetXLocked		= mOffsetX;
			mTouchVel			*= 0.95f;
		}
	} else {
		mOffsetXLocked = mOffsetX;
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
	
    Vec2f pos( mStartX - mOffsetXLocked, mStartY );
	
	gl::enableAdditiveBlending();

    for( int i = 0; i < mNumPlaylists; i++ )
	{	
		ipod::PlaylistRef playlist = mData->mPlaylists[i];
		
        if( pos.x < mEndX && pos.x + mPlaylistWidth > mStartX )
		{
			float x			= pos.x + mPlaylistWidth * 0.5f; // x center of the rect
			float alpha		= getAlpha( x );
			Vec2f p			= Vec2f( x, mStartY );

			map<int,gl::Texture>::iterator iter = mTextureMap.begin();
			iter = mTextureMap.find( i );
			if (iter == mTextureMap.end() ) 
				makeTexture( i, playlist );
			
			bool highlight = (i == mTouchDragPlaylistIndex) || (i == mCurrentPlaylistIndex);
			if( highlight ) gl::color( ColorA( BRIGHT_BLUE, alpha ) );
			else			gl::color( ColorA( BLUE, alpha ) );
			
			gl::Texture &tex = mTextureMap.find(i)->second;
			float w			= tex.getWidth() * 0.5f;
			float h			= 10.0f;
			float padding	= 10.0f;
			Rectf rect		= Rectf( p.x - w - padding, p.y - h - padding, p.x + w + padding, p.y + h + padding );
			mPlaylistRects.push_back( rect );
			gl::draw( tex, p - Vec2f( w, h ) );
//            std::cout << playlist->getPlaylistName() << " : " << (p - Vec2f( w, h )) << std::endl;
			
        } else {
			// STUPID FIX:
			// Making sure all rects are made, even ones that are offscreen.
			mPlaylistRects.push_back( Rectf( Vec2f( -500.0f, 0.0f ), Vec2f( -400.0f, 0.0f ) ) );
		}
		
        pos.x += mSpacerWidth + mPlaylistWidth;
        if( pos.x > mEndX ){
            break;
        }
    }
    
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
	gl::Texture textTexture = gl::Texture( layout.render( true, false ) );
	mTextureMap.insert( std::make_pair( index, textTexture ) );
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
}


