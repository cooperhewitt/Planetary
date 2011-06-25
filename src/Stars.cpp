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

Stars::Stars()
{
    mVerts = NULL;
    mPrevTotalVertices = -1;
}

Stars::~Stars()
{
    if (mVerts != NULL)	{
        delete[] mVerts;
        glDeleteBuffers(1, &vboId);        
    }
}

void Stars::setup( const vector<NodeArtist*> &nodes, const float &zoomAlpha )
{
	mTotalVertices = nodes.size();//; // * 6; // 6 = 2 triangles per quad
        
    if (mTotalVertices != mPrevTotalVertices) {
        if (mVerts != NULL) {
            delete[] mVerts; 
            mVerts = NULL;
        }
        if ( mPrevTotalVertices != -1 ) {
            glDeleteBuffers(1, &vboId);      
        }
        if (mTotalVertices > 0) {
            glGenBuffers(1, &vboId);        
            mVerts = new VertexData[mTotalVertices];
            mPrevTotalVertices = mTotalVertices;
        }
    }
	
	int vIndex	= 0;
	const float scaleOffset	= 0.5f - constrain( G_ARTIST_LEVEL - G_ZOOM, 0.0f, 1.0f ) * 0.25f; // 0.25 -> 0.5
	const float zoomOffset	= zoomAlpha * 1.5f;
	
	for( vector<NodeArtist*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it ){
		
        Vec3f pos = (*it)->mPos;
        Color c = (*it)->mColor;
        
		uint col = (uint)(c.r*255.0f) << 24 | (uint)(c.g*255.0f) << 16 | (uint)(c.b*255.0f) << 8 | 0xff;

//		float r = (*it)->mRadius * scaleOffset * 0.85f + ( 0.5f - scaleOffset );
		float radius = (*it)->mRadius * 3.5f;
		
//		if( (*it)->mIsSelected ){
//			radius *= 65.0f;
//		} else 
		if( (*it)->mIsHighlighted ){
			radius *= 2.0f;
		}
        
		mVerts[vIndex].vertex = pos;
        mVerts[vIndex].color = col;
        mVerts[vIndex].size = radius;
        vIndex++;
	}
    
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData)*mTotalVertices, mVerts, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);            
}

void Stars::draw( )
{
    glEnable(GL_POINT_SPRITE_OES);
    glTexEnvi(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE);
    
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
    glEnableClientState( GL_POINT_SIZE_ARRAY_OES );
	
	glVertexPointer( 3, GL_FLOAT, sizeof(VertexData), 0 );
	glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof(VertexData), (GLvoid*)sizeof(Vec3f) );
    glPointSizePointerOES( GL_FLOAT, sizeof(VertexData), (GLvoid*)(sizeof(Vec3f) + sizeof(uint)) );
	
	glDrawArrays( GL_POINTS, 0, mTotalVertices );

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_POINT_SIZE_ARRAY_OES );
    
    glDisable(GL_POINT_SPRITE_OES);
    glTexEnvi(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE);    
}
