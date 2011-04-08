//
//  GlExtras.cpp
//  Kepler
//
//  Created by Robert Hodgin on 4/7/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "BloomGl.h"
#include "cinder/gl/gl.h"

void drawButton( const ci::Rectf &rect, float u1, float v1, float u2, float v2 )
{
	glEnableClientState( GL_VERTEX_ARRAY );
	GLfloat verts[8];
	glVertexPointer( 2, GL_FLOAT, 0, verts );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	GLfloat texCoords[8];
	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
	int vi = 0;
	int ti = 0;
	verts[vi++] = rect.getX2(); texCoords[ti++] = u2;
	verts[vi++] = rect.getY1(); texCoords[ti++] = v1;
	verts[vi++] = rect.getX1(); texCoords[ti++] = u1;
	verts[vi++] = rect.getY1(); texCoords[ti++] = v1;
	verts[vi++] = rect.getX2(); texCoords[ti++] = u2;
	verts[vi++] = rect.getY2(); texCoords[ti++] = v2;
	verts[vi++] = rect.getX1(); texCoords[ti++] = u1;
	verts[vi++] = rect.getY2(); texCoords[ti++] = v2;
	
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
}