/*
 *  UiLayer.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2013 Smithsonian Institution. All rights reserved.
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
#include "PlaylistChooser.h"
#include "AlphaChooser.h"
#include "PlayControls.h"
#include "SettingsPanel.h"

class UiLayer : public BloomNode {
 public:
	
    UiLayer(): 
        BloomNode(), // get a default ID
        mChooserY(0.0f), 
        mSettingsY(0.0f), 
        mChooserDestY(0.0f), 
        mSettingsDestY(0.0f) 
    { }; 
    
	~UiLayer() {};
    
	void	setup( PlaylistChooserRef playlistChooser, 
                   AlphaChooserRef alphaChooser, 
                   PlayControlsRef playControls, 
                   SettingsPanelRef settingsPanel,
                   const ci::gl::Texture &uiButtonsTex, 
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
    
    float           mPanelY;                // used in setTransform
    float           mPanelOpenY;            // updated in setShowSettings/updateLayout
    float           mPanelClosedY;          // updated in updateLayout
	ci::Rectf		mPanelTabRect;			// Rect defining the panel tab
    
	bool			mIsPanelTabTouched;		// Is the Panel Tab currently being touched
	bool			mIsPanelOpen;			// Is the Panel fully open
	bool			mHasPanelBeenDragged;   // Are we dragging or just animating?
    ci::Vec2f		mPanelTabTouchOffset;	// Remember the touch position value when dragging	
    
    float           getPanelHeight();
    float           getMaxPanelHeight();
    
    float           mChooserY, mSettingsY;
    float           mChooserDestY, mSettingsDestY;
    
    PlaylistChooserRef mPlaylistChooser;
    AlphaChooserRef    mAlphaChooser;
    PlayControlsRef    mPlayControls;
    SettingsPanelRef   mSettingsPanel;
};

typedef std::shared_ptr<UiLayer> UiLayerRef;

