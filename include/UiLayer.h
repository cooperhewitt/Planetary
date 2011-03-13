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


class UiLayer {
 public:
	UiLayer();
	~UiLayer();
	void	setup( ci::app::AppCocoaTouch *app );
	void	initAlphaTextures( const ci::Font &font );
	bool	touchesBegan( ci::app::TouchEvent event );
	bool	touchesMoved( ci::app::TouchEvent event );
	bool	touchesEnded( ci::app::TouchEvent event );
	void	setPanelPos( float y, bool doneDragging );
	void	selectWheelItem( const ci::Vec2f &pos, bool closeWheel );
	void	update( float fov, float timeSincePinchEnded );
	void	draw( const ci::gl::Texture &upTex, const ci::gl::Texture &downTex );
	void	drawWheel();
	void	drawAlphaChar();
	void	drawPanel( const ci::gl::Texture &upTex, const ci::gl::Texture &downTex );
	void	setShowWheel( bool b ){ mShowWheel = b; if ( mShowWheel ) mPrevAlphaChar = ' '; }
	bool	getShowWheel(){ return mShowWheel; }
	float	getPanelYPos(){ return mPanelPos.y; }
	void	setAlphaChar( char c ){ mAlphaChar = c; }
	char	getAlphaChar(){ return mAlphaChar; }

	template<typename T>
	ci::CallbackId registerAlphaCharSelected( T *obj, bool ( T::*callback )( UiLayer* ) ){
		return mCallbacksAlphaCharSelected.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
	
	template<typename T>
	ci::CallbackId registerWheelClosed( T *obj, bool ( T::*callback )( UiLayer* ) ){
		return mCallbacksWheelClosed.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
	
	
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
	float			mTimeSincePinchEnded;
	
	ci::Rectf		mStripRect;
	bool			mShowWheel;
	std::string		mAlphaString;
	int				mAlphaIndex;
	char			mAlphaChar, mPrevAlphaChar;
	float			mWheelScale;
	
	ci::gl::Texture	mWheelTex;
	std::vector<ci::gl::Texture> mAlphaTextures;
	
	ci::CallbackMgr<bool(UiLayer*)> mCallbacksAlphaCharSelected;
	ci::CallbackMgr<bool(UiLayer*)> mCallbacksWheelClosed;
};

