//
//  Button.h
//  Kepler
//
//  Created by Robert Hodgin on 5/24/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/Vector.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rect.h"

class Button {
  public:
	Button();
	Button( int type, ci::Vec2f pos, ci::Rectf drawRect, ci::Rectf texCoordsRect, ci::Vec2f texCoordsOffset, ci::Rectf hitRect );
	void setup();
	void update();
	void draw();
	
	ci::Rectf getHitRect(){ return mHitRect; }
	int getType(){ return mType; }
	
  private:
	ci::Vec2f	mPos;
	ci::Rectf	mDrawRect;
	ci::Rectf	mTexCoordsRect;
	ci::Rectf	mHitRect;
	ci::Vec2f	mTexCoordsOffset;
	int			mType;
	bool		mIsPressed;
};