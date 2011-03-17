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
#include <vector>

class UiLayer {
 public:
	UiLayer();
	~UiLayer();
	enum	PlayButton { NO_BUTTON, PANEL_BUTTON };
	enum	ButtonTexId { TEX_PANEL_UP, TEX_PANEL_UP_ON, TEX_PANEL_DOWN, TEX_PANEL_DOWN_ON };
	
	void	setup( ci::app::AppCocoaTouch *app );
	bool	touchesBegan( ci::app::TouchEvent event );
	bool	touchesMoved( ci::app::TouchEvent event );
	bool	touchesEnded( ci::app::TouchEvent event );
	void	setPanelPos( float y, bool doneDragging );
	void	update();
	void	draw( const std::vector<ci::gl::Texture> &texs );
	float	getPanelYPos(){ return mPanelPos.y; }	
	
 private:
	ci::app::AppCocoaTouch *mApp;
	ci::CallbackId	mCbTouchesBegan, mCbTouchesMoved, mCbTouchesEnded;
	ci::Vec2f		mTouchPos;
	float			mPanelYPos, mPanelYPosDest;
	float			mPanelOpenYPos, mPanelClosedYPos;
	
	ci::Vec2i		mPanelPos;				// XY position of the panel upper left corner
	ci::Rectf		mPanelRect;				// Rect defining the panel width and height
	ci::Rectf		mPanelTabRect;			// Rect defining the panel tab
	bool			mIsPanelTabTouched;		// Is the Panel Tab currently being touched
	bool			mIsPanelOpen;			// Is the Panel fully open
	bool			mHasPanelBeenDragged;
	float			mPanelTabTouchYOffset;	// Accommodate the touch position y value
	int				mCountSinceLastTouch;
	PlayButton		mLastTouchedType;
	
	ci::Rectf		mStripRect;
};

