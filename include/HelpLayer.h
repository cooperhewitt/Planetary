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

class HelpLayer {
 public:
	
    HelpLayer();
	~HelpLayer();
	void	setup( ci::app::AppCocoaTouch *app );
	
    bool	touchesBegan( ci::app::TouchEvent event );
	bool	touchesMoved( ci::app::TouchEvent event );
	bool	touchesEnded( ci::app::TouchEvent event );
    bool    orientationChanged( ci::app::OrientationEvent event );
    void    setInterfaceOrientation( const ci::app::Orientation &orientation );
	void	update();
	void	draw( const ci::gl::Texture &uiButtonsTex );
	template<typename T>
	ci::CallbackId registerHelpButtonPressed( T *obj, bool (T::*callback)( HelpLayer* ) )
	{
		return mCallbacksHelpButtonPressed.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}
	
	
 private:
	ci::CallbackMgr<bool(HelpLayer*)> mCallbacksHelpButtonPressed;
	
	
    ci::Rectf   transformRect( const ci::Rectf &worldRect, const ci::Matrix44f &matrix );
    
	ci::app::AppCocoaTouch *mApp;
	ci::CallbackId	mCbTouchesBegan, mCbTouchesMoved, mCbTouchesEnded, mCbOrientationChanged;
	
	ci::gl::Texture mHelpPanelTex;
	
    float           mPanelHeight;           // TODO: const?
	ci::Rectf		mPanelRect;				// Rect defining the panel width and height
	ci::Rectf		mCloseRect;				// close button
	bool			mIsCloseTouched;
	
	ci::app::Orientation mInterfaceOrientation;
    ci::Matrix44f   mOrientationMtx;
    ci::Vec2f       mInterfaceSize;
};

