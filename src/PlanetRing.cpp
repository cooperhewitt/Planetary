//
//  PlanetRing.cpp
//  Kepler
//
//  Created by Tom Carden on 6/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "PlanetRing.h"
#include "cinder/gl/gl.h"
#include "cinder/Vector.h"

using namespace ci;
using namespace std;

void PlanetRing::setup()
{
    if (mVerts != NULL) delete[] mVerts;
    if (mTexCoords != NULL) delete[] mTexCoords;
    
	mVerts		= new float[18];
	mTexCoords	= new float[12];
	int i = 0;
	int t = 0;
	Vec3f corner;
	float w	= 1.0f;
	
	corner			= Vec3f( -w, 0.0f, -w );
	mVerts[i++]		= corner.x;
	mVerts[i++]		= corner.y;
	mVerts[i++]		= corner.z;
	mTexCoords[t++]	= 0.0f;
	mTexCoords[t++]	= 0.0f;
	
	corner			= Vec3f( w, 0.0f, -w );
	mVerts[i++]		= corner.x;
	mVerts[i++]		= corner.y;
	mVerts[i++]		= corner.z;
	mTexCoords[t++]	= 1.0f;
	mTexCoords[t++]	= 0.0f;
	
	corner			= Vec3f( w, 0.0f, w );	
	mVerts[i++]		= corner.x;
	mVerts[i++]		= corner.y;
	mVerts[i++]		= corner.z;
	mTexCoords[t++]	= 1.0f;
	mTexCoords[t++]	= 1.0f;
	
	corner			= Vec3f( -w, 0.0f, -w );
	mVerts[i++]		= corner.x;
	mVerts[i++]		= corner.y;
	mVerts[i++]		= corner.z;
	mTexCoords[t++]	= 0.0f;
	mTexCoords[t++]	= 0.0f;
	
	corner			= Vec3f( w, 0.0f, w );
	mVerts[i++]		= corner.x;
	mVerts[i++]		= corner.y;
	mVerts[i++]		= corner.z;
	mTexCoords[t++]	= 1.0f;
	mTexCoords[t++]	= 1.0f;
	
	corner			= Vec3f( -w, 0.0f, w );
	mVerts[i++]		= corner.x;
	mVerts[i++]		= corner.y;
	mVerts[i++]		= corner.z;
	mTexCoords[t++]	= 0.0f;
	mTexCoords[t++]	= 1.0f;
}

void PlanetRing::draw() const
{
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, mVerts );
    glTexCoordPointer( 2, GL_FLOAT, 0, mTexCoords );    
    glDrawArrays( GL_TRIANGLES, 0, 6 );
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );    
}
