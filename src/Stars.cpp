//
//  Stars.cpp
//  Kepler
//
//  Created by Tom Carden on 6/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Stars.h"
#include "cinder/gl/gl.h"
#include "Globals.h" // mumble
#include "NodeArtist.h"

using namespace ci;
using namespace std;

void Stars::setup( const vector<NodeArtist*> &nodes, const float &zoomAlpha )
{
	mTotalVertices = nodes.size();//; // * 6; // 6 = 2 triangles per quad
    
    if (mTotalVertices != mPrevTotalVertices) {
        if (mVerts != NULL) delete[] mVerts; 
        if (mSizes != NULL) delete[] mSizes; 
        mVerts = new VertexData[mTotalVertices];
        mSizes = new float[mTotalVertices];
        mPrevTotalVertices = mTotalVertices;
    }
	
	int vIndex	= 0;
	const float scaleOffset	= 0.5f - constrain( G_ARTIST_LEVEL - G_ZOOM, 0.0f, 1.0f ) * 0.25f; // 0.25 -> 0.5
	const float zoomOffset	= zoomAlpha * 1.5f;
	
	for( vector<NodeArtist*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it ){
		
        Vec3f pos = (*it)->mPos;
        Color c = (*it)->mColor;
		Vec4f col = Vec4f( c.r, c.g, c.b, 1.0f );

		float r = (*it)->mRadius * scaleOffset * 0.85f + ( 0.5f - scaleOffset );
        if( !(*it)->mIsHighlighted ){
			r -= zoomOffset;
		}
        
		mVerts[vIndex].vertex = pos; // p1;
        mVerts[vIndex].color = col;
        mSizes[vIndex] = r * 0.5f;
        vIndex++;
	}
}

void Stars::draw( )
{
    glEnable(GL_POINT_SPRITE_OES);
    glTexEnvi(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE);
    
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
    glEnableClientState( GL_POINT_SIZE_ARRAY_OES );
	
	glVertexPointer( 3, GL_FLOAT, sizeof(VertexData), mVerts );
	glColorPointer( 4, GL_FLOAT, sizeof(VertexData), &mVerts[0].color );
    glPointSizePointerOES( GL_FLOAT, 4, mSizes );
	
	glDrawArrays( GL_POINTS, 0, mTotalVertices );
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_POINT_SIZE_ARRAY_OES );
    
    glDisable(GL_POINT_SPRITE_OES);
    glTexEnvi(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE);    
}
