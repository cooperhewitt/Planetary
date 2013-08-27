//
//  Shadow.h
//  Kepler
//
//  Created by Tom Carden on 6/25/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#include "cinder/Vector.h"
#include "cinder/gl/gl.h"

class Node;

class Shadow
{
public:
    
    Shadow();
    ~Shadow();
    
    void setup( Node* node, Node* mParentNode, float camAlpha );
    void draw();

private:

    void buildVerts( ci::Vec3f p1, ci::Vec3f p2, ci::Vec3f p3, ci::Vec3f p4 );
    
	GLfloat		*mShadowVerts;
	GLfloat		*mShadowTexCoords;
    
};