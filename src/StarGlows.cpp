//
//  StarGlows.cpp
//  Kepler
//
//  Created by Tom Carden on 6/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "StarGlows.h"
#include "NodeArtist.h"

using namespace ci;
using namespace std;

StarGlows::StarGlows()
{
    mVerts = NULL;
    mPrevTotalVertices = -1;
}

StarGlows::~StarGlows()
{
    if (mVerts != NULL)	{
        delete[] mVerts; 
        mVerts = NULL;
    }
}

void StarGlows::setup( const vector<NodeArtist*> &filteredNodes, const Vec3f &bbRight, const Vec3f &bbUp, const float &zoomAlpha )
{
	mTotalVertices	= filteredNodes.size() * 6;	// 6 = 2 triangles per quad
	
    if (mTotalVertices != mPrevTotalVertices) {
        if (mVerts != NULL) {
            delete[] mVerts; 
        }
        mVerts = new VertexData[mTotalVertices];
        mPrevTotalVertices = mTotalVertices;
    }
	
	int vIndex = 0;
	
	for( vector<NodeArtist*>::const_iterator it = filteredNodes.begin(); it != filteredNodes.end(); ++it ){

        Vec3f pos			= (*it)->mPos;
        float r				= (*it)->mRadius * ( (*it)->mEclipseStrength * 2.0f + 1.5f ); // HERE IS WHERE YOU CAN MAKE THE GLOW HUGER/BIGGER/AWESOMER
        //if( !(*it)->mIsSelected )
        //	r				-= zoomAlpha;
        
        float alpha			= (*it)->mDistFromCamZAxisPer * ( 1.0f - (*it)->mEclipseStrength );
        //if( !(*it)->mIsSelected && !(*it)->mIsPlaying )
        //	alpha			= 1.0f - zoomAlpha;
        
        Color c             = (*it)->mGlowColor;
        Vec4f col			= Vec4f( c.r, c.g, c.b, alpha );
        
        Vec3f right			= bbRight * r;
        Vec3f up			= bbUp * r;
        
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

void StarGlows::draw()
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	
    // TODO: could we use a DYNAMIC VBO or a "VAO" (as Apple recommends) to manage this?
    // NB:- we don't use POINT_SPRITE because we need to draw BIG points sometimes
    
	glVertexPointer( 3, GL_FLOAT, sizeof(VertexData), mVerts );
	glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), &mVerts[0].texture );
	glColorPointer( 4, GL_FLOAT, sizeof(VertexData), &mVerts[0].color );
	
	glDrawArrays( GL_TRIANGLES, 0, mTotalVertices );
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
}

