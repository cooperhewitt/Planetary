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

using namespace ci;

class UiLayer {
 public:
	UiLayer();
	~UiLayer();
	void	setup( app::AppCocoaTouch *app );
	void	initAlphaTextures( const ci::Font &font );
	bool	touchesBegan( app::TouchEvent event );
	bool	touchesMoved( app::TouchEvent event );
	bool	touchesEnded( app::TouchEvent event );
	void	selectWheelItem( const Vec2f &pos, bool closeWheel );
	void	draw();
	void	drawWheel();
	void	drawAlphaChar();
	void	drawNav();
	void	setShowWheel( bool b ){ mShowWheel = b; }
	bool	getShowWheel(){ return mShowWheel; }
	void	setAlphaChar( char c ){ mAlphaChar = c; }
	char	getAlphaChar(){ return mAlphaChar; }

	template<typename T>
	CallbackId registerAlphaCharSelected( T *obj, bool ( T::*callback )( UiLayer* ) ){
		return mCallbacksAlphaCharSelected.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
	
	
 private:
	app::AppCocoaTouch *mApp;
	CallbackId		mCbTouchesBegan, mCbTouchesMoved, mCbTouchesEnded;
	Vec2f			mTouchPos;
	
	Rectf			mNavRect;
	bool			mShowWheel;
	std::string		mAlphaString;
	int				mAlphaIndex;
	char			mAlphaChar, mPrevAlphaChar;
	
	gl::Texture		mWheelTex;
	std::vector<ci::gl::Texture> mAlphaTextures;
	
	CallbackMgr<bool(UiLayer*)> mCallbacksAlphaCharSelected;
};

