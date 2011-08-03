//
//  WheelOverlay.h
//  Kepler
//
//  Created by Robert Hodgin on 7/20/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
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

class WheelOverlay : public BloomNode {
public:
	WheelOverlay() {}
	~WheelOverlay() {}
	
	struct VertexData {
        ci::Vec2f vertex;
        ci::Vec2f texture;
    };
	
	void	setup();
	void	update();
	void	draw();

	void	setShowWheel( bool b );
	bool	getShowWheel(){ return mShowWheel; }
    
	float getRadius() { return mRadius; }
	float getWheelScale() { return mWheelScale; }
    
    bool    hitTest( const ci::Vec2f &point );
    
	template<typename T>
	ci::CallbackId registerWheelToggled( T *obj, bool ( T::*callback )( bool ) ){
		return mCallbacksWheelToggled.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}
    
private:
    
    void updateVerts();
    
	ci::gl::Texture	mTex;

	float mRadius;
	float mWheelScale;
    
    bool mShowWheel;
    
	int mTotalVertices;
	VertexData *mVerts;
    
    ci::Vec2f mInterfaceSize, mInterfaceCenter;
    
	ci::CallbackMgr<bool(bool)> mCallbacksWheelToggled;    
};

typedef std::shared_ptr<WheelOverlay> WheelOverlayRef;

