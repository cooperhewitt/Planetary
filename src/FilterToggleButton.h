//
//  FilterToggleButton.h
//  Kepler
//
//  Created by Tom Carden on 7/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/Font.h"
#include "cinder/gl/Texture.h"
#include "State.h" // for FilterMode
#include "cinder/app/AppCocoaTouch.h"
#include "BloomNode.h"


class FilterToggleButton : public BloomNode {

public:
    
    FilterToggleButton() { mVisible = false; }
    
    void setup( const State::FilterMode &filterMode, const ci::Font &font );
	void update();
    void setFilterMode(const State::FilterMode &filterMode);

    void draw();
	Rectf getRect(){ return mHitRect; }
    void setVisible( bool visible = true ) { mVisible = visible; }
	bool isVisible(){ return mVisible; }
    
    template<typename T>
	ci::CallbackId registerFilterModeSelected( T *obj, bool ( T::*callback )( State::FilterMode ) ){
		return mCbFilterModeSelected.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
    
private:
    
    bool touchBegan( ci::app::TouchEvent::Touch touch );
    bool touchEnded( ci::app::TouchEvent::Touch touch );    
    bool mVisible;

    Vec2f mInterfaceSize;
    
    Vec2f mAlphaPos;
    Vec2f mPlaylistPos;
    Rectf mAlphaRect;
    Rectf mPlaylistRect;
	Rectf mHitRect;
	Rectf mRect;
    
    State::FilterMode mFilterMode;
    ci::gl::Texture mAlphaTexture, mPlaylistTexture, mTex;
    
	ci::CallbackMgr<bool(State::FilterMode)> mCbFilterModeSelected;    
};