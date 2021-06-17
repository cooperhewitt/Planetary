/*
 *  AlphaChooser.h
 *  Kepler
 *
 *  Created by Tom Carden on 3/14/11.
 *  Copyright 2013 Smithsonian Institution. All rights reserved.
 *
 */

#pragma once
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rect.h"
#include "cinder/Color.h"
#include "cinder/Font.h"
#include "cinder/Function.h"

#include "BloomNode.h"

class AlphaChooser;
typedef std::shared_ptr<AlphaChooser> AlphaChooserRef;

class AlphaChooser : public BloomNode {

public:
    
	AlphaChooser(): mOpacity(1.0f) {};
	~AlphaChooser() {};
		
	void	setup( const ci::Font &font, const ci::Vec2f &interfaceSize );    
	void	update();
	void	draw();
    
	bool	touchBegan( ci::app::TouchEvent::Touch touch );
	bool	touchMoved( ci::app::TouchEvent::Touch touch );
	bool	touchEnded( ci::app::TouchEvent::Touch touch );
    
    void    setNumberAlphaPerChar( float *numAlphaPerChar );
        	
    void	setAlphaChar( char c ){ mAlphaChar = c; }
	char	getAlphaChar(){ return mAlphaChar; }
	
	template<typename T>
	ci::CallbackId registerAlphaCharSelected( T *obj, bool ( T::*callback )( char ) ){
		return mCallbacksAlphaCharSelected.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
    
    bool hitTest( ci::Vec2f globalPos ) { return mVisible && mFullRect.contains( globalToLocal( globalPos ) ); }

    // used in UiLayer layout...
    float getHeight();

    void setOpacity( float opacity ) { mOpacity = opacity; }
    
private:
    
	void	setRects();    
    
    float           *mNumberAlphaPerChar;

	std::string		mAlphaString;
	int				mAlphaIndex;
	char			mAlphaChar;
	
    float           mOpacity;

    ci::Vec2f       mInterfaceSize;
    
	std::vector<ci::gl::Texture> mAlphaTextures;
	std::vector<ci::Rectf>       mAlphaRects;
	std::vector<ci::Rectf>       mAlphaHitRects;
    ci::Rectf mFullRect;
    
	ci::CallbackMgr<bool(char)> mCallbacksAlphaCharSelected;    
};

