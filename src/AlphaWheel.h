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
#include "Orientation.h"
#include "OrientationEvent.h"

class AlphaWheel {
public:
	AlphaWheel();
	~AlphaWheel();
	void	setup( ci::app::AppCocoaTouch *app, const ci::app::Orientation &orientation, float radius );
	void	initAlphaTextures( const ci::Font &font );
	bool	touchesBegan( ci::app::TouchEvent event );
	bool	touchesMoved( ci::app::TouchEvent event );
	bool	touchesEnded( ci::app::TouchEvent event );
    void    setInterfaceOrientation( const ci::app::Orientation &orientation );
	void	update( float fov );
	void	setTimePinchEnded( float timePinchEnded );
	void	draw( float *numAlphaPerChar );
	void	setShowWheel( bool b );
	bool	getShowWheel(){ return mShowWheel; }
	float	getWheelScale(){ return mWheelScale; }
	void	setAlphaChar( char c ){ mAlphaChar = c; }
	char	getAlphaChar(){ return mAlphaChar; }
	
	template<typename T>
	ci::CallbackId registerAlphaCharSelected( T *obj, bool ( T::*callback )( AlphaWheel* ) ){
		return mCallbacksAlphaCharSelected.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
	
	template<typename T>
	ci::CallbackId registerWheelToggled( T *obj, bool ( T::*callback )( AlphaWheel* ) ){
		return mCallbacksWheelToggled.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
		
private:
	void	drawWheel();
	void	drawAlphaChar();    
	bool	selectWheelItem( const ci::Vec2f &pos, bool closeWheel );
    
	ci::app::AppCocoaTouch *mApp;
	ci::CallbackId	mCbTouchesBegan, mCbTouchesMoved, mCbTouchesEnded, mCbOrientationChanged;
    uint32_t        mActiveTouchId;
	ci::Vec2f		mLastTouchPos;    

	float			mTimePinchEnded;

	bool			mShowWheel;
	std::string		mAlphaString;
	int				mAlphaIndex;
	char			mAlphaChar, mPrevAlphaChar;
	float			mAlphaRadius;
	float			mWheelScale;
	
	ci::gl::Texture	mWheelTex;
	std::vector<ci::gl::Texture> mAlphaTextures;
	std::vector<ci::Rectf> mAlphaRects;
	
	ci::CallbackMgr<bool(AlphaWheel*)> mCallbacksAlphaCharSelected;
	ci::CallbackMgr<bool(AlphaWheel*)> mCallbacksWheelToggled;
    
    ci::app::Orientation       mInterfaceOrientation;
    ci::Matrix44f              mOrientationMatrix;
    ci::Vec2f                  mInterfaceCenter;
};

