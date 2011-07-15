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
#include "cinder/Font.h"
#include "cinder/Color.h"
#include "cinder/Camera.h"
#include "CinderIPod.h"
#include "OrientationHelper.h"
#include "Data.h"
#include "World.h"

class PlaylistChooser {

public:    
    
    PlaylistChooser(): mData(NULL), mVisible(false), offsetX(0.0f), mCurrentPlaylistIndex(-1) {}
    
    void setup( ci::app::AppCocoaTouch *app, const ci::app::Orientation &orientation, const ci::Font &font, const ci::Color &lineColor );
    void draw();

    void setVisible( bool visible = true ) { mVisible = visible; }
    bool isVisible() { return mVisible; }
    
    void setInterfaceOrientation( const ci::app::Orientation &orientation );    
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

    bool mVisible;
    float offsetX; // for scrolling
    
    std::vector<ci::Rectf> mPlaylistRects;
    
    uint64_t mTouchDragId;
    ci::Vec2f mTouchDragStartPos;
    float mTouchDragStartOffset;
    int mTouchDragPlaylistIndex;
    
    int mCurrentPlaylistIndex; // set from main app, passive
    
    bool touchesBegan( ci::app::TouchEvent event );
    bool touchesMoved( ci::app::TouchEvent event );
    bool touchesEnded( ci::app::TouchEvent event );
    
    Data *mData; // for playlists
    World *mWorld; // for nodes
    ci::CameraPersp *mCam; // for projecting to screen
    
    ci::gl::TextureFontRef mTextureFont;
    ci::Color mLineColor;
    ci::Font mFont;
    
    ci::app::Orientation mInterfaceOrientation;
    ci::Matrix44f mOrientationMatrix;
    ci::Vec2f mInterfaceSize; 
    
	ci::CallbackMgr<bool(ci::ipod::PlaylistRef)> mCbPlaylistSelected;        
    
    void getWindowSpaceRect( float &x, float &y, float &w, float &h );
};