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
		
	void	setup( const ci::Font &font, WheelOverlayRef wheelOverlay );    
	void	update();
	void	draw();
    
	bool	touchBegan( ci::app::TouchEvent::Touch touch );
	bool	touchMoved( ci::app::TouchEvent::Touch touch );
	bool	touchEnded( ci::app::TouchEvent::Touch touch );
    
    void    setNumberAlphaPerChar( float *numAlphaPerChar );
	void	setTimePinchEnded( float timePinchEnded );
        	
    void	setAlphaChar( char c ){ mAlphaChar = c; }
	char	getAlphaChar(){ return mAlphaChar; }
	
	template<typename T>
	ci::CallbackId registerAlphaCharSelected( T *obj, bool ( T::*callback )( AlphaWheel* ) ){
		return mCallbacksAlphaCharSelected.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
			
private:
    
	void	setRects();    
	void	initAlphaTextures( const ci::Font &font );
    
	bool	selectWheelItem( const ci::Vec2f &pos, bool closeWheel );
    
    uint32_t        mActiveTouchId;
	ci::Vec2f		mLastTouchPos;

    WheelOverlayRef mWheelOverlay;
	
	float			mTimePinchEnded;
    float           *mNumberAlphaPerChar;

	std::string		mAlphaString;
	int				mAlphaIndex;
	char			mAlphaChar, mPrevAlphaChar;
	
    ci::Vec2f       mInterfaceSize;
    
	std::vector<ci::gl::Texture> mAlphaTextures;
	std::vector<ci::Rectf>       mAlphaRects;
	
	ci::CallbackMgr<bool(AlphaWheel*)> mCallbacksAlphaCharSelected;    
};

