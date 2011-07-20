//
//  PlaylistChooser.h
//  Kepler
//
//  Created by Tom Carden on 7/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <vector>
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Font.h"
#include "cinder/Color.h"
#include "cinder/Camera.h"
#include "CinderIPod.h"
#include "OrientationHelper.h"
#include "Data.h"
#include "World.h"
#include "UINode.h"
#include <map>

class PlaylistChooser : public UINode {

public:    
    
    PlaylistChooser(): mData(NULL), mVisible(false), mOffsetX(0.0f), mCurrentPlaylistIndex(-1) {}
    
    void setup( const ci::Font &font, const ci::Color &lineColor );
	void update();
    void draw();
	void makeFbo( int index, ci::ipod::PlaylistRef playlist );
	
	ci::Rectf getRect(){ return mHitRect; }
    void setVisible( bool visible = true ) { mVisible = visible; }
    bool isVisible() { return mVisible; }
       
    void setCurrentPlaylistIndex( int index ) { mCurrentPlaylistIndex = index; } // -1 is "none"
    void setDataWorldCam(Data *data, World *world, ci::CameraPersp *cam);

    template<typename T>
	ci::CallbackId registerPlaylistSelected( T *obj, bool ( T::*callback )( ci::ipod::PlaylistRef ) ){
		return mCbPlaylistSelected.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
    
private:
    
    struct ScissorRect {
        float x, y, w, h;  
    };
	
	struct VertexData {
        ci::Vec3f vertex;
        ci::Vec2f texture;
		ci::Vec4f color;
    };
	
	float			getNewX( float x );
	float			getNewY( float x );
	float			getScale( float x );
	float			getAlpha( float x );
	
	
	int				mNumPlaylists;
	int				mCurrentPlaylistIndex; // set from main app, passive
    bool			mVisible;
	
	ci::Vec2i		mTouchPos, mTouchPrevPos;
	float			mTouchVel;
	uint64_t		mTouchDragId;
    ci::Vec2f		mTouchDragStartPos;
    float			mTouchDragStartOffset;
    int				mTouchDragPlaylistIndex;
    bool			mIsDragging;
	
	float			mPlaylistWidth, mPlaylistHeight;
	ci::Vec2f		mPlaylistSize;
	float			mSpacerWidth;
	float			mOffsetX;		// for scrolling
	float			mOffsetXLocked; // for snap-to effect
	float			mMinOffsetX, mMaxOffsetX;
	float			mBorder;
	float			mStartX, mStartY;
	float			mEndX;
	float			mLeftLimit;
	float			mXCenter;
    
	ci::Rectf		mHitRect;
    std::vector<ci::Rectf> mPlaylistRects;
	
	std::map<int, ci::gl::Fbo> mFboMap;
    
    ci::gl::Texture	mTex;
    
    bool			touchBegan( ci::app::TouchEvent::Touch touch );
    bool			touchMoved( ci::app::TouchEvent::Touch touch );
    bool			touchEnded( ci::app::TouchEvent::Touch touch );
    
    Data			*mData; // for playlists
    World			*mWorld; // for nodes
    ci::CameraPersp *mCam; // for projecting to screen

    ci::Color		mLineColor;
    ci::Font		mFont;
    
    ci::Vec2f		mInterfaceSize;
	
	int mTotalVertices;
    int mPrevTotalVertices; // so we only recreate frames
	VertexData *mVerts;
    
	ci::CallbackMgr<bool(ci::ipod::PlaylistRef)> mCbPlaylistSelected;        
    
    void getWindowSpaceRect( float &x, float &y, float &w, float &h );
};