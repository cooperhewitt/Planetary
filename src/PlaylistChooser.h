//
//  PlaylistChooser.h
//  Kepler
//
//  Created by Tom Carden on 7/10/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#pragma once

#include <vector>
#include <map>

#include "cinder/app/AppCocoaTouch.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Font.h"
#include "cinder/Color.h"
#include "cinder/Camera.h"

#include "CinderIPod.h"

#include "Data.h"
#include "World.h"
#include "BloomNode.h"

class PlaylistChooser;
typedef std::shared_ptr<PlaylistChooser> PlaylistChooserRef;

typedef std::shared_ptr<ci::Rectf> RectRef;

class PlaylistChooser : public BloomNode {

public:    
    
    PlaylistChooser(): mData(NULL), mOffsetX(0.0f), mOpacity(1.0f) {}
    
    void setup( const ci::Font &font, const ci::Vec2f &interfaceSize );
	void update();
    void draw();

    bool touchBegan( ci::app::TouchEvent::Touch touch );
    bool touchMoved( ci::app::TouchEvent::Touch touch );
    bool touchEnded( ci::app::TouchEvent::Touch touch );
    
    void setDataWorldCam(Data *data, World *world, ci::CameraPersp *cam);

    template<typename T>
	ci::CallbackId registerPlaylistSelected( T *obj, bool ( T::*callback )( ci::ipod::PlaylistRef ) ){
		return mCbPlaylistSelected.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}

    template<typename T>
	ci::CallbackId registerPlaylistTouched( T *obj, bool ( T::*callback )( ci::ipod::PlaylistRef ) ){
		return mCbPlaylistTouched.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
    
    bool hitTest( ci::Vec2f globalPos ) { return mVisible && mFullRect.contains( globalToLocal( globalPos ) ); }

    // used in UiLayer layout...
    float getHeight();
    
    void setOpacity( float opacity ) { mOpacity = opacity; }

    void clearTextures() { mTextures.clear(); }
    
private:
    
	float			getAlpha( float x );
	void            makeTexture( int index, ci::ipod::PlaylistRef playlist );
	
	int				mNumPlaylists;	
	int				mCurrentIndex;
	int				mPrevIndex;
	
	ci::Vec2i		mTouchPos, mTouchPrevPos;
	float			mTouchVel;
	uint64_t		mTouchDragId;
    ci::Vec2f		mTouchDragStartPos;
    float			mTouchDragStartOffset;
    int				mTouchDragPlaylistIndex;
    bool			mIsDragging;
	
	ci::Vec2f		mPlaylistSize;
	float			mSpacerWidth;
	float			mOffsetX;		// for scrolling
	float			mStartY;
    
    float           mOpacity;

    ci::Rectf       mFullRect;
    
    std::vector<RectRef> mPlaylistRects;
	
	std::vector<ci::gl::Texture> mTextures;
        
    Data			*mData;  // for playlists
    World			*mWorld; // for nodes
    ci::CameraPersp *mCam;   // for projecting to screen

    ci::Font		mFont;
    float           mFontHeight;
    
    ci::Vec2f		mInterfaceSize;
			
	ci::CallbackMgr<bool(ci::ipod::PlaylistRef)> mCbPlaylistSelected, mCbPlaylistTouched;        
};