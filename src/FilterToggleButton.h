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
#include "OrientationHelper.h"
#include "cinder/app/AppCocoaTouch.h"

class FilterToggleButton {

public:
    
    FilterToggleButton() { mVisible = false; }
    
    void setup(ci::app::AppCocoaTouch *app, const State::FilterMode &filterMode, const ci::Font &font, const ci::app::Orientation &orientation );
    void setFilterMode(const State::FilterMode &filterMode);
    void draw();
    void setInterfaceOrientation( const ci::app::Orientation &orientation );    
    
    void setVisible( bool visible = true ) { mVisible = visible; }
    
    template<typename T>
	ci::CallbackId registerFilterModeSelected( T *obj, bool ( T::*callback )( State::FilterMode ) ){
		return mCbFilterModeSelected.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
    
private:
    
    bool touchesBegan( ci::app::TouchEvent event );
    bool touchesEnded( ci::app::TouchEvent event );
    
    bool mVisible;
    
    ci::app::Orientation mInterfaceOrientation;
    Matrix44f mOrientationMatrix;
    Vec2f mInterfaceSize;
    
    Vec2f mAlphaPos;
    Vec2f mPlaylistPos;
    Rectf mAlphaRect;
    Rectf mPlaylistRect;
    
    State::FilterMode mFilterMode;
    ci::gl::Texture mAlphaTexture, mPlaylistTexture;
    
	ci::CallbackMgr<bool(State::FilterMode)> mCbFilterModeSelected;    
};