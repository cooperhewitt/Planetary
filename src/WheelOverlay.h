//
//  WheelOverlay.h
//  Kepler
//
//  Created by Robert Hodgin on 7/20/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/Vector.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rect.h"
#include "cinder/Color.h"

class WheelOverlay {
public:
	WheelOverlay();
	~WheelOverlay();
	
	struct VertexData {
        ci::Vec2f vertex;
        ci::Vec2f texture;
    };
	
	void	setup();
	void	setVerts();
	void	update( const ci::Vec2f &interfaceSize );
	void	draw();
	
	float	mRadius;
	
private:
	ci::gl::Texture	mTex;

	int mTotalVertices;
	VertexData *mVerts;
};
