//
//  Stats.cpp
//  Kepler
//
//  Created by Tom Carden on 6/10/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#include "Stats.h"
#include <sstream>
#include "cinder/gl/gl.h"
#include "cinder/Text.h"

using namespace std;
using namespace ci;

void Stats::setup(const Font &font, const Color &fpsColor, const Color &color)
{
    mFont = font;
    mFpsColor = fpsColor;
    mColor = color;
}

void Stats::update(const float &fps, 
                   const float &playheadTime, 
                   const float &fov,
				   const float &camDist,
				   const float &pinchPer,
                   const float &currentLevel, 
                   const float &zoom)
{
    stringstream s;
	TextLayout layout;	
	layout.setFont( mFont );
	layout.setColor( mColor );

	s.str("");
	s << "FPS: " << fps;
	layout.addLine( s.str() );
	
//	s.str("");
//	s << "FOV: " << fov;
//	layout.addLine( s.str() );
//	
//	s.str("");
//	s << "mCamDist: " << camDist;
//	layout.addLine( s.str() );
//	
//	s.str("");
//	s << "mPinchTotalDest: " << pinchPer;
//	layout.addLine( s.str() );
	
	mParamsTex = gl::Texture( layout.render( true, false ) );    
}

void Stats::draw(const Matrix44f &mtx)
{
    if (mParamsTex) {
        glPushMatrix();
        glMultMatrixf( mtx );
        gl::color( ColorA( Color::white(), 0.1f ) );
        gl::draw( mParamsTex, Vec2f( 23.0f, 15.0f ) );
        glPopMatrix();    
    }
}
