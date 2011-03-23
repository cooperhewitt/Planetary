/*
 *  Globals.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/20/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once 

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