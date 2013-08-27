//
//  Rings.cpp
//  Kepler
//
//  Created by Tom Carden on 6/13/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#include "cinder/gl/gl.h"
#include "cinder/Color.h"
#include "OrbitRing.h"
#include "Globals.h"

using namespace ci;

OrbitRing::OrbitRing()
{
    mLowResVBO = 0;
    mHighResVBO = 0;
}

OrbitRing::~OrbitRing()
{
    if (mLowResVBO != 0) {
        glDeleteBuffers(1, &mLowResVBO);
    }
    if (mHighResVBO != 0) {
        glDeleteBuffers(1, &mHighResVBO);
    }
}

void OrbitRing::setup()
{
	VertexData *mVertsLowRes  = new VertexData[ G_RING_LOW_RES ];  // X,Y,U,V
	
	for( int i=0; i<G_RING_LOW_RES; i++ ){
		float per	= (float)i/(float)(G_RING_LOW_RES-1);
		float angle	= per * TWO_PI;
		mVertsLowRes[i].vertex  = Vec2f( cos( angle ), sin( angle ) );
		mVertsLowRes[i].texture = Vec2f( per, 0.5f );
	}

    glGenBuffers(1, &mLowResVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mLowResVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * G_RING_LOW_RES, mVertsLowRes, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Leave no VBO bound.        

    delete[] mVertsLowRes;

	VertexData *mVertsHighRes = new VertexData[ G_RING_HIGH_RES ]; // X,Y,U,V
	
	for( int i=0; i<G_RING_HIGH_RES; i++ ){
		float per	= (float)i/(float)(G_RING_HIGH_RES-1);
		float angle	= per * TWO_PI;
		mVertsHighRes[i].vertex  = Vec2f( cos( angle ), sin( angle ) );
		mVertsHighRes[i].texture = Vec2f( per, 0.5f );
	}    
    
    glGenBuffers(1, &mHighResVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mHighResVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * G_RING_HIGH_RES, mVertsHighRes, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Leave no VBO bound.        

    delete[] mVertsHighRes;    
}

void OrbitRing::drawLowRes() const
{
    glBindBuffer(GL_ARRAY_BUFFER, mLowResVBO);
    glVertexPointer( 2, GL_FLOAT, sizeof(VertexData), 0 ); // last arg becomes an offset instead of an address
    glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), (void*)sizeof(Vec2f) );        
    glBindBuffer(GL_ARRAY_BUFFER,0); // Leave no VBO bound.        

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );    
    glDrawArrays( GL_LINE_STRIP, 0, G_RING_LOW_RES );
    glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );    
}

void OrbitRing::drawHighRes() const
{
    glBindBuffer(GL_ARRAY_BUFFER, mHighResVBO);
    glVertexPointer( 2, GL_FLOAT, sizeof(VertexData), 0 ); // last arg becomes an offset instead of an address
    glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), (void*)sizeof(Vec2f) );        
    glBindBuffer(GL_ARRAY_BUFFER,0); // Leave no VBO bound. 

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );    
	glDrawArrays( GL_LINE_STRIP, 0, G_RING_HIGH_RES );
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );    
}