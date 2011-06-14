//
//  Rings.cpp
//  Kepler
//
//  Created by Tom Carden on 6/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "cinder/gl/gl.h"
#include "cinder/Color.h"
#include "OrbitRing.h"
#include "Globals.h"

using namespace ci;

void OrbitRing::setup()
{
	if( mVertsLowRes	!= NULL ) delete[] mVertsLowRes;
	if( mTexLowRes		!= NULL ) delete[] mTexLowRes;
	if( mVertsHighRes	!= NULL ) delete[] mVertsHighRes;
	if( mTexHighRes		!= NULL ) delete[] mTexHighRes;
	
	mVertsLowRes	= new float[ G_RING_LOW_RES*2 ];	// X,Y
	mTexLowRes		= new float[ G_RING_LOW_RES*2 ];	// U,V
	mVertsHighRes	= new float[ G_RING_HIGH_RES*2 ];	// X,Y
	mTexHighRes		= new float[ G_RING_HIGH_RES*2 ];	// U,V
	
	Color c				= BRIGHT_BLUE;
	
	for( int i=0; i<G_RING_LOW_RES; i++ ){
		float per				 = (float)i/(float)(G_RING_LOW_RES-1);
		float angle				 = per * TWO_PI;
		mVertsLowRes[i*2+0]	 = cos( angle );
		mVertsLowRes[i*2+1]	 = sin( angle );
		mTexLowRes[i*2+0] = per;
		mTexLowRes[i*2+1] = 0.5f;
	}
	
	for( int i=0; i<G_RING_HIGH_RES; i++ ){
		float per				 = (float)i/(float)(G_RING_HIGH_RES-1);
		float angle				 = per * TWO_PI;
		mVertsHighRes[i*2+0] = cos( angle );
		mVertsHighRes[i*2+1] = sin( angle );
		mTexHighRes[i*2+0] = per;
		mTexHighRes[i*2+1] = 0.5f;
	}    
}

void OrbitRing::drawLowRes() const
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, mVertsLowRes );
	glTexCoordPointer( 2, GL_FLOAT, 0, mTexLowRes );
	glDrawArrays( GL_LINE_STRIP, 0, G_RING_LOW_RES );
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );    
}

void OrbitRing::drawHighRes() const
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, mVertsHighRes );
	glTexCoordPointer( 2, GL_FLOAT, 0, mTexHighRes );
	glDrawArrays( GL_LINE_STRIP, 0, G_RING_HIGH_RES );
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );    
}