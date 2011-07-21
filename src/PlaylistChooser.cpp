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
#include "UIController.h"
#include "Globals.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void PlaylistChooser::setup( const Font &font )
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
	mHitRect				= Rectf( 0.0f, 0.0f, 10.0f, 10.0f ); // paranoid if i didn't init it
	mTex					= gl::Texture( loadImage( loadResource( "playlist.png" ) ) );
	mBgTex					= gl::Texture( loadImage( loadResource( "playlistBg.png" ) ) );	
	
	mPrevIndex				= -1;
	mCurrentIndex			= 0;
	
	mWheelOverlay.setup();
}

bool PlaylistChooser::touchBegan( ci::app::TouchEvent::Touch touch )
{
    if (mData == NULL || !mVisible || mTouchDragPlaylistIndex >= 0) return false;
	
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
    if (mData == NULL || !mVisible || mTouchDragPlaylistIndex < 0) return false;
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
    if (mData == NULL || !mVisible || mTouchDragPlaylistIndex < 0) return false;
    
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
	Vec2f interfaceSize = mRoot->getInterfaceSize();
	if( mInterfaceSize != interfaceSize ){
		mInterfaceSize	= interfaceSize;
		
		mWheelOverlay.update( mInterfaceSize );
		
		mStartY			= mInterfaceSize.y * 0.5f + mWheelOverlay.mRadius - 20.0f;
		mHitRect		= Rectf( 0.0f, mStartY - 25.0f, mInterfaceSize.x, mStartY + 25.0f );
		mXCenter		= mInterfaceSize.x * 0.5f;
		mEndX			= mInterfaceSize.x - mBorder;
		
		mLeftLimit		= mXCenter - mPlaylistWidth;
		mMaxOffsetX		= ( mNumPlaylists * mPlaylistWidth) + ( (mNumPlaylists-1) * mSpacerWidth ) - (mEndX - mStartX) + mLeftLimit;
		mMinOffsetX		= -mLeftLimit;
		
		
	}
}


void PlaylistChooser::draw()
{
	if( mData == NULL || !mVisible ) return;
	
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
				mCurrentIndex		= chosenIndex;
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
	
	
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::enableAlphaBlending();
	
	glPushMatrix();
	gl::color( Color::white() );
	gl::translate( mInterfaceSize * 0.5f );
	mWheelOverlay.draw();
	glPopMatrix();
	
	
	
	
	int maxTotalVisiblePlaylists = mInterfaceSize.x / mPlaylistWidth + 1;
	mTotalVertices = maxTotalVisiblePlaylists * 6;
	
	delete[] mVerts; 
	mVerts = NULL;
	mVerts = new VertexData[mTotalVertices];
	mPrevTotalVertices = mTotalVertices;
	
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

			//gl::drawStrokedRect( rect );
			
//			map<int,gl::Fbo>::iterator iter = mFboMap.begin();
//			iter = mFboMap.find( i );
//			if (iter == mFboMap.end() ) 
//				makeFbo( i, playlist );
//			
//			gl::color( ColorA( 1.0f, 1.0f, 1.0f, alpha ) );
//			glPushMatrix();
//			glTranslatef( 0.0f, 0.0f, depth );
//			gl::draw( mFboMap.find(i)->second.getTexture(0), Rectf( rect.x1, rect.y2, rect.x2, rect.y1 ) );
//			glPopMatrix();
			
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

void PlaylistChooser::makeFbo( int index, ipod::PlaylistRef playlist )
{
	const int FBO_WIDTH		= 400;
	const int FBO_HEIGHT	= 50;
	
	
	
	gl::SaveFramebufferBinding bindingSaver;
	gl::Fbo::Format format;
	gl::Fbo fbo = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
	fbo.bindFramebuffer();
	
	gl::setViewport( fbo.getBounds() );
	gl::setMatricesWindowPersp( FBO_WIDTH, FBO_HEIGHT );
	gl::enableAlphaBlending();
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	
	string name = playlist->getPlaylistName();
	TextLayout layout;
	layout.setFont( mFont );
	layout.setColor( BRIGHT_BLUE );
	layout.addLine( name );
	gl::Texture textTexture = gl::Texture( layout.render( true, false ) );
	

//	gl::color( ColorA( BRIGHT_BLUE, 0.2f ) );
//	gl::drawSolidRect( Rectf( 4.0f, 4.0f, FBO_WIDTH-5.0f, FBO_HEIGHT-5.0f ) );
	
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	
	gl::draw( mBgTex );
	gl::draw( textTexture, Vec2f( FBO_WIDTH/2 - textTexture.getWidth()/2, 10.0f ) );
	
//	float size = 35.0f;
//	gl::enableAdditiveBlending();
//	for (int j = 0; j < playlist->size(); j++) {            
//		ipod::TrackRef track = (*playlist)[j];
//		NodeArtist* nodeArtist = mWorld->getArtistNodeById( track->getArtistId() );
//		gl::color( nodeArtist->mColor );
//		Vec2f pos = nodeArtist->mPos.xz() * 1.75f + Vec2f( FBO_WIDTH/2.0f, FBO_HEIGHT/2.0f );
//		pos += Rand::randVec2f() * Rand::randFloat( 3.0f );
//		gl::draw( mTex, Rectf( pos.x - size, pos.y - size, pos.x + size, pos.y + size ) );
//	}


	mFboMap.insert( std::make_pair( index, fbo ) );
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
	float per		= x/mInterfaceSize.x;
	float invCos	= ( 1.0f - (float)cos( per * M_PI * 2.0f ) ) * 0.5f;
	float cosPer	= pow( invCos, 0.5f );
	return cosPer;
}

float PlaylistChooser::getScale( float x )
{
	float per		= x/mInterfaceSize.x;
	float invCos	= ( 1.0f - (float)cos( per * M_PI * 2.0f ) ) * 0.5f;
	float cosPer	= max( pow( invCos, 3.5f ) + 0.4f, 0.5f );
	return cosPer;
}

float PlaylistChooser::getNewX( float x )
{
	float per		= ( x/mInterfaceSize.x ) * 0.7f + 0.15f;
	float cosPer	= ( 1.0f - cos( per * M_PI ) ) * 0.5f;
	return cosPer * mInterfaceSize.x;
}

float PlaylistChooser::getNewY( float x )
{
	float per		= x/mInterfaceSize.x;
	float sinPer	= sin( per * M_PI );
	return sinPer;
}


// scissor rect is from *bottom left* of window in *untransformed* coords
// x,y,w,h to this function are in rotated screen space, from top left of screen
void PlaylistChooser::getWindowSpaceRect( float &x, float &y, float &w, float &h )
{
    Vec3f topLeft(x,y,0);
    Vec3f bottomRight(x+w,y+h,0);
    Vec2f tl = ( getConcatenatedTransform() * topLeft ).xy();
    Vec2f br = ( getConcatenatedTransform() * bottomRight ).xy();
    // use min max and fabs to canonicalize the scissor rect...
    x = min(br.x, tl.x);
    y = app::getWindowHeight() - max(br.y, tl.y); // flip y
    w = fabs(br.x - tl.x);
    h = fabs(br.y - tl.y);
}

void PlaylistChooser::setDataWorldCam( Data *data, World *world, CameraPersp *cam )
{
    mData			= data;
    mWorld			= world;
    mCam			= cam;
	mNumPlaylists	= mData->mPlaylists.size();
}
