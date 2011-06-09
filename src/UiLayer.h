/*
 *  UiLayer.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/Vector.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rect.h"
#include "cinder/Color.h"
#include "cinder/Font.h"
#include "Orientation.h"
#include "OrientationEvent.h"
#include <vector>

class UiLayer {
 public:
	
    UiLayer();
	~UiLayer();
    
	enum	ButtonTexId { TEX_PANEL_UP, TEX_PANEL_UP_ON, TEX_PANEL_DOWN, TEX_PANEL_DOWN_ON };
	
	void	setup( ci::app::AppCocoaTouch *app, const ci::app::Orientation &orientation, const bool &showSettings );
	
    bool	touchesBegan( ci::app::TouchEvent event );
	bool	touchesMoved( ci::app::TouchEvent event );
	bool	touchesEnded( ci::app::TouchEvent event );

    void    setInterfaceOrientation( const ci::app::Orientation &orientation );
    void    setShowSettings( bool visible );
    
	void	update();
	void	draw( const ci::gl::Texture &uiButtonsTex );
    
	float	getPanelYPos(){ return mPanelRect.y1; }	
    ci::Rectf getPanelTabRect() { return mPanelTabRect; }
	bool	getIsPanelOpen() { return mIsPanelOpen; }
	void	setIsPanelOpen( bool b ){ mIsPanelOpen = b; mHasPanelBeenDragged = false; }
	
 private:
    
	ci::app::AppCocoaTouch *mApp;
	ci::CallbackId	mCbTouchesBegan, mCbTouchesMoved, mCbTouchesEnded, mCbOrientationChanged;
	
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
	
    // TODO: use a Matrix32f (after we write one or someone adds one to Cinder - look at the Cairo one first)
    ci::Matrix44f        mOrientationMatrix;     // For adjusting ui drawing and hitrects    
    ci::app::Orientation mInterfaceOrientation;
    ci::Vec2f            mInterfaceSize;

    ci::Rectf   transformRect( const ci::Rectf &worldRect, const ci::Matrix44f &matrix );
};

