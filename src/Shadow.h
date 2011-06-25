//
//  Shadow.h
//  Kepler
//
//  Created by Tom Carden on 6/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "cinder/Vector.h"
#include "cinder/gl/gl.h"

class Shadow
{
public:
    
    void setup( ci::Vec3f p1, ci::Vec3f p2, ci::Vec3f p3, ci::Vec3f p4 );
    void draw();

private:
    
	GLfloat		*mShadowVerts;
	GLfloat		*mShadowTexCoords;
    
};