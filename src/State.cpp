/*
 *  State.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "State.h"
#include "cinder/gl/gl.h"
#include "cinder/Rect.h"

using std::stringstream;
using namespace ci;
using namespace ci::app;
using namespace std;

State::State()
{
	mAlphaChar = ' ';
}

void State::setAlphaChar( char c )
{
	std::cout << "State::setAlphaChar " << c << std::endl;
	mAlphaChar = c;
	mCallbacksAlphaCharStateChanged.call( this );
}
