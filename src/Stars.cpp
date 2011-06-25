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
        mVerts = NULL;
    }
}

void Stars::setup( const vector<NodeArtist*> &nodes, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp, const float &zoomAlpha )
{
	mTotalVertices = nodes.size() * 6;
        
    if (mTotalVertices != mPrevTotalVertices) {
        if (mVerts != NULL) {
            delete[] mVerts; 
            mVerts = NULL;
        }
        if (mTotalVertices > 0) {
            mVerts = new VertexData[mTotalVertices];
            mPrevTotalVertices = mTotalVertices;
        }
    }
	
	int vIndex	= 0;
//	const float scaleOffset	= 0.5f - constrain( G_ARTIST_LEVEL - G_ZOOM, 0.0f, 1.0f ) * 0.25f; // 0.25 -> 0.5
//	const float zoomOffset	= zoomAlpha * 1.5f;
	
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
        
        Vec3f right			= bbRight * radius;
        Vec3f up			= bbUp * radius;
        
        Vec3f p1			= pos - right - up;
        Vec3f p2			= pos + right - up;
        Vec3f p3			= pos - right + up;
        Vec3f p4			= pos + right + up;
        
        mVerts[vIndex].vertex  = p1;
        mVerts[vIndex].texture = Vec2f(0.0f,0.0f);
        mVerts[vIndex].color   = col;
        vIndex++;
        
        mVerts[vIndex].vertex  = p2;
        mVerts[vIndex].texture = Vec2f(1.0f,0.0f);
        mVerts[vIndex].color   = col;
        vIndex++;
        
        mVerts[vIndex].vertex  = p3;
        mVerts[vIndex].texture = Vec2f(0.0f,1.0f);
        mVerts[vIndex].color   = col;
        vIndex++;
        
        mVerts[vIndex].vertex  = p2;
        mVerts[vIndex].texture = Vec2f(1.0f,0.0f);
        mVerts[vIndex].color   = col;
        vIndex++;
        
        mVerts[vIndex].vertex  = p3;
        mVerts[vIndex].texture = Vec2f(0.0f,1.0f);
        mVerts[vIndex].color   = col;
        vIndex++;
        
        mVerts[vIndex].vertex  = p4;
        mVerts[vIndex].texture = Vec2f(1.0f,1.0f);
        mVerts[vIndex].color   = col;
        vIndex++;        
	}    
}

void Stars::draw( )
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	
	glVertexPointer( 3, GL_FLOAT, sizeof(VertexData), mVerts );
	glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), &mVerts[0].texture );
	glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof(VertexData), &mVerts[0].color );
	
	glDrawArrays( GL_TRIANGLES, 0, mTotalVertices );
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
}
