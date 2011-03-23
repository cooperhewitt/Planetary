/*
 *  AlphaWheel.h
 *  Kepler
 *
 *  Created by Tom Carden on 3/14/11.
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


class AlphaWheel {
public:
	AlphaWheel();
	~AlphaWheel();
	void	setup( ci::app::AppCocoaTouch *app );
	void	initAlphaTextures( const ci::Font &font );
	bool	touchesBegan( ci::app::TouchEvent event );
	bool	touchesMoved( ci::app::TouchEvent event );
	bool	touchesEnded( ci::app::TouchEvent event );
    bool    orientationChanged( ci::app::OrientationEvent event );
	void	update( float fov );
	void	setTimePinchEnded( float timePinchEnded );
	void	draw();
    // TODO: if we're resetting prev alpha char here should we fire a callback?
	void	setShowWheel( bool b ){ mShowWheel = b; if ( mShowWheel ) mPrevAlphaChar = ' '; }
	bool	getShowWheel(){ return mShowWheel; }
	void	setAlphaChar( char c ){ mAlphaChar = c; }
	char	getAlphaChar(){ return mAlphaChar; }
	
	template<typename T>
	ci::CallbackId registerAlphaCharSelected( T *obj, bool ( T::*callback )( AlphaWheel* ) ){
		return mCallbacksAlphaCharSelected.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
	
	template<typename T>
	ci::CallbackId registerWheelClosed( T *obj, bool ( T::*callback )( AlphaWheel* ) ){
		return mCallbacksWheelClosed.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
	
	
private:
	void	drawWheel();
	void	drawAlphaChar();    
	bool	selectWheelItem( const ci::Vec2f &pos, bool closeWheel );
    
	ci::app::AppCocoaTouch *mApp;
	ci::CallbackId	mCbTouchesBegan, mCbTouchesMoved, mCbTouchesEnded, mCbOrientationChanged;
	ci::Vec2f		mTouchPos;

	float			mTimePinchEnded;

	bool			mShowWheel;
	std::string		mAlphaString;
	int				mAlphaIndex;
	char			mAlphaChar, mPrevAlphaChar;
	float			mWheelScale;
	
	ci::gl::Texture	mWheelTex;
	std::vector<ci::gl::Texture> mAlphaTextures;
	
	ci::CallbackMgr<bool(AlphaWheel*)> mCallbacksAlphaCharSelected;
	ci::CallbackMgr<bool(AlphaWheel*)> mCallbacksWheelClosed;
    
    ci::app::DeviceOrientation mDeviceOrientation;
    ci::Matrix44f              mOrientationMatrix;
    ci::Vec2f                  mInterfaceCenter;
};

