/*
 *  State.h
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

using namespace ci;

class State {
 public:
	State();
	
	// Alpha char for filtering artist name
	char getAlphaChar(){ return mAlphaChar; }
	void setAlphaChar( char c );
	template<typename T>
	CallbackId registerAlphaCharStateChanged( T *obj, bool ( T::*callback )( State* ) ){
		return mCallbacksAlphaCharStateChanged.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
	
 private:
	CallbackMgr<bool(State*)> mCallbacksAlphaCharStateChanged;	
	
	char mAlphaChar;
};

