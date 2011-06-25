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

void Stars::setup( const vector<NodeArtist*> &nodes,
                   const Vec3f &bbRight, const Vec3f &bbUp, 
                   const float &zoomAlpha )
{
	mTotalVertices = nodes.size() * 6; // 6 = 2 triangles per quad
    
    if (mTotalVertices != mPrevTotalVertices) {
        if (mVerts != NULL) delete[] mVerts; 
        mVerts = new VertexData[mTotalVertices];
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
        
		Vec3f right	= bbRight * r;
		Vec3f up	= bbUp * r;
		
		Vec3f p1	= pos - right - up;
		Vec3f p2	= pos + right - up;
		Vec3f p3	= pos - right + up;
		Vec3f p4	= pos + right + up;
		
		mVerts[vIndex].vertex = p1;
        mVerts[vIndex].texture = Vec2f(0.0f,0.0f);
        mVerts[vIndex].color = col;
        vIndex++;

		mVerts[vIndex].vertex = p2;
        mVerts[vIndex].texture = Vec2f(1.0f,0.0f);
        mVerts[vIndex].color = col;
        vIndex++;

        mVerts[vIndex].vertex = p3;
        mVerts[vIndex].texture = Vec2f(0.0f,1.0f);
        mVerts[vIndex].color = col;
        vIndex++;

        mVerts[vIndex].vertex = p2;
        mVerts[vIndex].texture = Vec2f(1.0f,0.0f);
        mVerts[vIndex].color = col;
        vIndex++;

        mVerts[vIndex].vertex = p3;
        mVerts[vIndex].texture = Vec2f(0.0f,1.0f);
        mVerts[vIndex].color = col;
        vIndex++;
		
        mVerts[vIndex].vertex = p4;
        mVerts[vIndex].texture = Vec2f(1.0f,1.0f);
        mVerts[vIndex].color = col;
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
	glColorPointer( 4, GL_FLOAT, sizeof(VertexData), &mVerts[0].color );
	
	glDrawArrays( GL_TRIANGLES, 0, mTotalVertices );
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
}
