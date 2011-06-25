//
//  Shadow.cpp
//  Kepler
//
//  Created by Tom Carden on 6/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Shadow.h"

using namespace ci;

Shadow::Shadow()
{
	mShadowVerts		= NULL;
	mShadowTexCoords	= NULL;    
}

Shadow::~Shadow()
{
    if( mShadowVerts != NULL )		delete[] mShadowVerts;
    if( mShadowTexCoords != NULL )  delete[] mShadowTexCoords;
}

void Shadow::setup( Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4 )
{
    if( mShadowVerts != NULL )		delete[] mShadowVerts;
    if( mShadowTexCoords != NULL )  delete[] mShadowTexCoords;
    
	int numVerts		= 12;			// dont forget to change the vert count in draw vvv
	mShadowVerts		= new float[ numVerts * 3 ]; // x, y
	mShadowTexCoords	= new float[ numVerts * 2 ]; // u, v
	int i = 0;
	int t = 0;
	
	Vec3f v1 = ( p1 + p2 ) * 0.5f;	// midpoint between base vertices
	Vec3f v2 = ( p3 + p4 ) * 0.5f;	// midpoint between end vertices
	
	mShadowVerts[i++]	= p1.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p1.y;		mShadowTexCoords[t++]	= 0.2f;
	mShadowVerts[i++]	= p1.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;
	mShadowVerts[i++]	= p3.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p3.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= p3.z;
    
	// umbra 
	mShadowVerts[i++]	= p1.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= p1.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p1.z;
	mShadowVerts[i++]	= v1.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v1.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= v1.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;
	
	// umbra 
	mShadowVerts[i++]	= v1.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v1.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= v1.z;
	mShadowVerts[i++]	= p2.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= p2.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p2.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;
	
	mShadowVerts[i++]	= p2.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p2.y;		mShadowTexCoords[t++]	= 0.2f;
	mShadowVerts[i++]	= p2.z;
	mShadowVerts[i++]	= p4.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p4.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= p4.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;	
	
}

void Shadow::draw()
{
    glVertexPointer( 3, GL_FLOAT, 0, mShadowVerts );
    glTexCoordPointer( 2, GL_FLOAT, 0, mShadowTexCoords );
    
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glDrawArrays( GL_TRIANGLES, 0, 12 ); // dont forget to change the vert count in setup ^^^
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );    
}
