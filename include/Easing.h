/*
 *  Easing.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/4/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
 
 // Equations as interpreted by Robert Penner.
 
#include "cinder/Vector.h"

float easeInOutQuad( double t, float b, float c, double d )
{
	if ((t/=d/2.0) < 1.0)
		return c/2.0 * pow(t, 3.0) + b;
	return c/2.0 * (pow(t-2.0, 3.0) + 2.0) + b;
}

ci::Vec3f easeInOutQuad( double t, ci::Vec3f b, ci::Vec3f c, double d )
{
	if ((t/=d/2.0) < 1.0)
		return c/2.0 * pow(t, 3.0) + b;
	return c/2.0 * (pow(t-2.0, 3.0) + 2.0) + b;
}