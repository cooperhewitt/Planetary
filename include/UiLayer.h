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
    
	enum	ButtonTexId { TEX_PANEL_UP, TEX_PANEL_UP_ON, TEX_PANEL_DOWN, TEX_PANEL_DOWN_ON };
	
	void	setup( ci::app::AppCocoaTouch *app );
	
    bool	touchesBegan( ci::app::TouchEvent event );
	bool	touchesMoved( ci::app::TouchEvent event );
	bool	touchesEnded( ci::app::TouchEvent event );
    bool    orientationChanged( ci::app::OrientationEvent event );
    
	void	update();
	void	draw( const std::vector<ci::gl::Texture> &texs );
    
	float	getPanelYPos(){ return mPanelRect.y1; }	
    ci::Rectf getPanelTabRect() { return mPanelTabRect; }
	
 private:

    void        setPanelPos( float y, bool doneDragging );
    ci::Rectf   transformRect( const ci::Rectf &worldRect, const ci::Matrix44f &matrix );
    
	ci::app::AppCocoaTouch *mApp;
	ci::CallbackId	mCbTouchesBegan, mCbTouchesMoved, mCbTouchesEnded, mCbOrientationChanged;
	
    float           mPanelHeight;           // TODO: const?
    float           mPanelOpenY;            // updated in orientationChanged, interfaceHeight-mPanelHeight
    float           mPanelClosedY;          // updated in orientationChanged, interfaceHeight
	ci::Rectf		mPanelRect;				// Rect defining the panel width and height
	ci::Rectf		mPanelTabRect;			// Rect defining the panel tab
    
	bool			mIsPanelTabTouched;		// Is the Panel Tab currently being touched
	bool			mIsPanelOpen;			// Is the Panel fully open
	bool			mHasPanelBeenDragged;   // Are we dragging or just animating?
    ci::Vec2f		mPanelTabTouchOffset;	// Remember the touch position value when dragging
	
    ci::Matrix44f   mOrientationMatrix;     // For adjusting ui drawing and hitrects
    
    ci::app::DeviceOrientation mDeviceOrientation;
};

