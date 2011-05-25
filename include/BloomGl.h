//
//  GlExtras.h
//  Kepler
//
//  Created by Robert Hodgin on 4/7/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/Rect.h"

void drawButton( const ci::Rectf &rect, float u1, float v1, float u2, float v2 );
void drawButton( const ci::Rectf &rect, const ci::Rectf&texRect );
void inflateRect( ci::Rectf &rect, float amount );

class Triangle {
public:
	Triangle() {}
	Triangle( ci::Vec3f pos1, ci::Vec3f pos2, ci::Vec3f pos3 )
	: p1( pos1 ), p2( pos2 ), p3( pos3 )
	{
		
	}
	
	ci::Vec3f p1;
	ci::Vec3f p2;
	ci::Vec3f p3;
};