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
    
    FilterToggleButton() { setVisible(false); }
    
    void setup( const State::FilterMode &filterMode, const ci::Font &font );
	void update();
    void setFilterMode(const State::FilterMode &filterMode);

    void draw();
	Rectf getRect(){ return mRect; }
    
    template<typename T>
	ci::CallbackId registerFilterModeSelected( T *obj, bool ( T::*callback )( State::FilterMode ) ){
		return mCbFilterModeSelected.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
    
private:
    
    bool touchBegan( ci::app::TouchEvent::Touch touch );
    bool touchEnded( ci::app::TouchEvent::Touch touch );    

    Vec2f mInterfaceSize;
    
    Rectf mAlphaRect;
    Rectf mPlaylistRect;
	Rectf mRect;
    
    State::FilterMode mFilterMode;
    ci::gl::Texture mTex;
    
	ci::CallbackMgr<bool(State::FilterMode)> mCbFilterModeSelected;    
};