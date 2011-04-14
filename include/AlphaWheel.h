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
    void    setInterfaceOrientation( const ci::app::Orientation &orientation );
	void	update( float fov );
	void	setTimePinchEnded( float timePinchEnded );
	void	draw( GLfloat *verts, GLfloat *texCoords, GLfloat *colors );
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
		
	ci::gl::Texture	mWheelTex;
private:
	void	drawWheel();
	void	drawWheelMask();
	void	drawWheelData( GLfloat *verts, GLfloat *texCoords, GLfloat *colors );
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
	
	ci::gl::Texture	mWheelMaskTex;
	ci::gl::Texture mBlurRectTex;
	std::vector<ci::gl::Texture> mAlphaTextures;
	
	ci::CallbackMgr<bool(AlphaWheel*)> mCallbacksAlphaCharSelected;
	ci::CallbackMgr<bool(AlphaWheel*)> mCallbacksWheelToggled;
    
    ci::app::Orientation       mInterfaceOrientation;
    ci::Matrix44f              mOrientationMatrix;
    ci::Vec2f                  mInterfaceCenter;
};

