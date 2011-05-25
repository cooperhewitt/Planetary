//
//  Button.cpp
//  Kepler
//
//  Created by Robert Hodgin on 5/24/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Button.h"
#include "BloomGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

Button::Button(){}
			   
Button::Button( int type, Vec2f pos, Rectf drawRect, Rectf texCoordsRect, Vec2f texCoordsOffset, Rectf hitRect )
{
	mType				= type;
	mPos				= pos;
	mDrawRect			= drawRect;
	mTexCoordsRect		= texCoordsRect;
	mTexCoordsOffset	= texCoordsOffset;
	mHitRect			= hitRect;
	mIsPressed			= false;
}

void Button::setup()
{
	
}

void Button::update()
{
	
}

void Button::draw()
{
	if( mIsPressed )
		drawButton( mDrawRect + mPos, mTexCoordsRect + mTexCoordsOffset );
	else
		drawButton( mDrawRect + mPos, mTexCoordsRect );
}
