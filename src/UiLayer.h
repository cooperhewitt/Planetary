/*
 *  UiLayer.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 Bloom Studio, Inc. All rights reserved.
 *
 */

#pragma once
#include <vector>
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rect.h"
#include "cinder/Color.h"
#include "cinder/Font.h"
#include "BloomNode.h"

class UiLayer : public BloomNode {
 public:
	
    UiLayer(): BloomNode() {}; // get a default ID
	~UiLayer() {};
    
	void	setup( BloomNodeRef mPlaylistChooser, 
                   BloomNodeRef mAlphaChooser, 
                   BloomNodeRef mPlayControls, 
                   BloomNodeRef mSettingsPanel,
                   const ci::gl::Texture &uiButtonsTex, 
                   const ci::gl::Texture &settingsBgTex, 
                   const bool &showSettings, 
                   const ci::Vec2f interfaceSize );
	
    bool	touchBegan( ci::app::TouchEvent::Touch touch );
	bool	touchMoved( ci::app::TouchEvent::Touch touch );
	bool	touchEnded( ci::app::TouchEvent::Touch touch );

    void    setShowSettings( bool visible );
    
	void    update();
	void    draw();
    
	float   getPanelYPos(){ return mPanelY; }	

	bool	getIsPanelOpen() { return mIsPanelOpen; }
	void	setIsPanelOpen( bool b ){ mIsPanelOpen = b; mHasPanelBeenDragged = false; }
	
    bool    hitTest( ci::Vec2f globalPos );
    
    //// expand/collapse/query panels
    void    setShowAlphaFilter(bool visible);
    bool    isShowingAlphaFilter();
    void    setShowPlaylistFilter(bool visible);
    bool    isShowingPlaylistFilter();
    bool    isShowingFilter();    
    
 private:

    void updateLayout( ci::Vec2f interfaceSize );
    
    ci::Vec2f       mInterfaceSize; // for detecting orientation changes    
    ci::gl::Texture mButtonsTex;
	ci::gl::Texture	mSettingsBgTex;
    
    float           mPanelY;                // used in setTransform
	float			mPanelOpenHeight;       // small height when settings closed
	float			mPanelSettingsHeight;   // full height when settings open
    float           mPanelHeight;           // varies depending on if settings are shown
    float           mPanelOpenY;            // updated in setShowSettings/updateLayout
    float           mPanelClosedY;          // updated in updateLayout
	ci::Rectf		mPanelRect;				// Rect defining the panel width and height
	ci::Rectf		mPanelTabRect;			// Rect defining the panel tab
	ci::Rectf		mPanelUpperRect;		// Rect defining the upper half of the panel (used only when drawing panel)
	ci::Rectf		mPanelLowerRect;		// Rect defining the lower half of the panel (used only when drawing panel)
    
	bool			mIsPanelTabTouched;		// Is the Panel Tab currently being touched
	bool			mIsPanelOpen;			// Is the Panel fully open
	bool			mHasPanelBeenDragged;   // Are we dragging or just animating?
    ci::Vec2f		mPanelTabTouchOffset;	// Remember the touch position value when dragging	
    
    BloomNodeRef    mPlaylistChooser, mAlphaChooser, mPlayControls, mSettingsPanel;
};

typedef std::shared_ptr<UiLayer> UiLayerRef;

