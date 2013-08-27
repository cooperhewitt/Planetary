//
//  PlanetRing.cpp
//  Kepler
//
//  Created by Tom Carden on 6/14/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#include "PlanetRing.h"
#include "cinder/gl/gl.h"
#include "cinder/Vector.h"

using namespace ci;
using namespace std;

void PlanetRing::setup()
{
    if (mVerts != NULL) {
        delete[] mVerts;
    }
    
	mVerts		= new VertexData[6];
	int i = 0;
	float w	= 1.0f;
	
	mVerts[i].vertex  = Vec3f( -w, 0.0f, -w );
    mVerts[i].texture = Vec2f( 0.0f, 0.0f );
    i++;
	
	mVerts[i].vertex  = Vec3f( w, 0.0f, -w );
    mVerts[i].texture = Vec2f( 1.0f, 0.0f );
    i++;
	
	mVerts[i].vertex  = Vec3f( w, 0.0f, w );	
    mVerts[i].texture = Vec2f( 1.0f, 1.0f );
    i++;
	
	mVerts[i].vertex  = Vec3f( -w, 0.0f, -w );
    mVerts[i].texture = Vec2f( 0.0f, 0.0f );
    i++;
	
	mVerts[i].vertex  = Vec3f( w, 0.0f, w );
    mVerts[i].texture = Vec2f( 1.0f, 1.0f );
    i++;
	
	mVerts[i].vertex  = Vec3f( -w, 0.0f, w );
    mVerts[i].texture = Vec2f( 0.0f, 1.0f );
    i++;
}

void PlanetRing::draw() const
{
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glVertexPointer( 3, GL_FLOAT, sizeof(VertexData), mVerts );
    glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), &mVerts[0].texture );    
    glDrawArrays( GL_TRIANGLES, 0, 6 );
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );    
}
