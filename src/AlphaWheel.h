/*
 *  AlphaWheel.h
 *  Kepler
 *
 *  Created by Tom Carden on 3/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "cinder/Vector.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rect.h"
#include "cinder/Color.h"
#include "cinder/Font.h"
#include "Orientation.h"
#include "OrientationEvent.h"
#include "WheelOverlay.h"
#include "cinder/Function.h"
#include "BloomNode.h"

class AlphaWheel : public BloomNode {
public:
	AlphaWheel() {};
	~AlphaWheel() {};
	
	struct VertexData {
        ci::Vec2f vertex;
        ci::Vec2f texture;
    };
	
	void	setup( const ci::Font &font );
	void	initAlphaTextures( const ci::Font &font );
	void	setRects();
	bool	touchBegan( ci::app::TouchEvent::Touch touch );
	bool	touchMoved( ci::app::TouchEvent::Touch touch );
	bool	touchEnded( ci::app::TouchEvent::Touch touch );
    
    void    setNumberAlphaPerChar( float *numAlphaPerChar ); // FIXME: call this in main app
	void	setTimePinchEnded( float timePinchEnded );
    
	void	update();
	void	draw();
    
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
	void	drawAlphaChar();    
	bool	selectWheelItem( const ci::Vec2f &pos, bool closeWheel );
    
    uint32_t        mActiveTouchId;
	ci::Vec2f		mLastTouchPos;    

	WheelOverlay	mWheelOverlay;
	
	float			mTimePinchEnded;
    
    float           *mNumberAlphaPerChar;

	bool			mShowWheel;
	std::string		mAlphaString;
	int				mAlphaIndex;
	char			mAlphaChar, mPrevAlphaChar;
	float			mWheelScale;
	
	std::vector<ci::gl::Texture> mAlphaTextures;
	std::vector<ci::Rectf> mAlphaRects;
	
	ci::CallbackMgr<bool(AlphaWheel*)> mCallbacksAlphaCharSelected;
	ci::CallbackMgr<bool(AlphaWheel*)> mCallbacksWheelToggled;
    
    ci::Vec2f                  mInterfaceSize, mInterfaceCenter;
};

