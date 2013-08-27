//
//  Vignette.h
//  Kepler
//
//  Created by Robert Hodgin on 7/20/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#pragma once

#include "cinder/app/AppCocoaTouch.h"
#include "cinder/Function.h"
#include "cinder/Vector.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rect.h"
#include "cinder/Color.h"
#include "BloomNode.h"

class Vignette : public BloomNode {
public:
	Vignette() {}
	~Vignette() 
    { 	
        delete[] mVerts; 
    }
	
	void	setup( const ci::gl::Texture &tex );
	void	update();
	void	draw();

	void	setShowing( bool b );
	bool	isShowing(){ return mShowing; }
    
	float   getScale() { return mScale; }
    
	template<typename T>
	ci::CallbackId registerToggled( T *obj, bool ( T::*callback )( bool ) ){
		return mCallbacksToggled.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
    
private:

	struct VertexData {
        ci::Vec2f vertex;
        ci::Vec2f texture;
    };
	    
    void updateVerts();
    
	ci::gl::Texture	mTex;

	float mScale;
    
    bool mShowing;
    
	int mTotalVertices;
	VertexData *mVerts;
    
    ci::Vec2f mInterfaceSize, mInterfaceCenter;
    
	ci::CallbackMgr<bool(bool)> mCallbacksToggled;    
};
