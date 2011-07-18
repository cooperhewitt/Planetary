/*
 *  UiLayer.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <vector>
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rect.h"
#include "cinder/Color.h"
#include "cinder/Font.h"
#include "UINode.h"

class UiLayer : public UINode {
 public:
	
    UiLayer(): UINode() {}; // get a default ID
	~UiLayer() {};
    
	enum	ButtonTexId { TEX_PANEL_UP, TEX_PANEL_UP_ON, TEX_PANEL_DOWN, TEX_PANEL_DOWN_ON };
	
	void	setup( const ci::gl::Texture &uiButtonsTex, const bool &showSettings, const ci::Vec2f interfaceSize );
	
    bool	touchBegan( ci::app::TouchEvent::Touch touch );
	bool	touchMoved( ci::app::TouchEvent::Touch touch );
	bool	touchEnded( ci::app::TouchEvent::Touch touch );

    void    setShowSettings( bool visible );
    
	void	update();
	virtual void draw();
    
	float	getPanelYPos(){ return mPanelRect.y1; }	
    ci::Rectf getPanelTabRect() { return mPanelTabRect; }
	bool	getIsPanelOpen() { return mIsPanelOpen; }
	void	setIsPanelOpen( bool b ){ mIsPanelOpen = b; mHasPanelBeenDragged = false; }
	
 private:

    void updateLayout();
    
    ci::Vec2f       mInterfaceSize; // for detecting orientation changes    
    ci::gl::Texture mUiButtonsTex;
    
	float			mPanelOpenHeight;
	float			mPanelSettingsHeight;
    float           mPanelHeight;           // varies depending on if settings are shown
    float           mPanelOpenY;            // updated in orientationChanged, interfaceHeight-mPanelHeight
    float           mPanelClosedY;          // updated in orientationChanged, interfaceHeight
	ci::Rectf		mPanelRect;				// Rect defining the panel width and height
	ci::Rectf		mPanelTabRect;			// Rect defining the panel tab
	ci::Rectf		mPanelUpperRect;		// Rect defining the upper half of the panel (used only when drawing panel)
	ci::Rectf		mPanelLowerRect;		// Rect defining the lower half of the panel (used only when drawing panel)
    
	bool			mIsPanelTabTouched;		// Is the Panel Tab currently being touched
	bool			mIsPanelOpen;			// Is the Panel fully open
	bool			mHasPanelBeenDragged;   // Are we dragging or just animating?
    ci::Vec2f		mPanelTabTouchOffset;	// Remember the touch position value when dragging	
};

