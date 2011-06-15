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

void StarGlows::setup( const vector<NodeArtist*> &filteredNodes, const Vec3f &bbRight, const Vec3f &bbUp, const float &zoomAlpha )
{
	mTotalVertices	= filteredNodes.size() * 6;	// 6 = 2 triangles per quad
	
    if (mTotalVertices != mPrevTotalVertices) {
        if (mVerts != NULL)		delete[] mVerts; 
		if (mTexCoords != NULL) delete[] mTexCoords; 
		if (mColors != NULL)	delete[] mColors;
		
        mVerts			= new float[mTotalVertices*3];
        mTexCoords		= new float[mTotalVertices*2];
        mColors			= new float[mTotalVertices*4];
		
        mPrevTotalVertices = mTotalVertices;
    }
	
	int vIndex = 0;
	int tIndex = 0;
	int cIndex = 0;
	
	for( vector<NodeArtist*>::const_iterator it = filteredNodes.begin(); it != filteredNodes.end(); ++it ){

        Vec3f pos			= (*it)->mPos;
        float r				= (*it)->mRadius * ( (*it)->mEclipseStrength * 2.0f + 1.5f ); // HERE IS WHERE YOU CAN MAKE THE GLOW HUGER/BIGGER/AWESOMER
        //if( !(*it)->mIsSelected )
        //	r				-= zoomAlpha;
        
        float alpha			= (*it)->mDistFromCamZAxisPer * ( 1.0f - (*it)->mEclipseStrength );
        //if( !(*it)->mIsSelected && !(*it)->mIsPlaying )
        //	alpha			= 1.0f - zoomAlpha;
        
        ColorA col			= ColorA( (*it)->mGlowColor, alpha );
        
        Vec3f right			= bbRight * r;
        Vec3f up			= bbUp * r;
        
        Vec3f p1			= pos - right - up;
        Vec3f p2			= pos + right - up;
        Vec3f p3			= pos - right + up;
        Vec3f p4			= pos + right + up;
        
        mVerts[vIndex++]		= p1.x;
        mVerts[vIndex++]		= p1.y;
        mVerts[vIndex++]		= p1.z;
        mTexCoords[tIndex++]	= 0.0f;
        mTexCoords[tIndex++]	= 0.0f;
        mColors[cIndex++]		= col.r;
        mColors[cIndex++]		= col.g;
        mColors[cIndex++]		= col.b;
        mColors[cIndex++]		= col.a;
        
        mVerts[vIndex++]		= p2.x;
        mVerts[vIndex++]		= p2.y;
        mVerts[vIndex++]		= p2.z;
        mTexCoords[tIndex++]	= 1.0f;
        mTexCoords[tIndex++]	= 0.0f;
        mColors[cIndex++]		= col.r;
        mColors[cIndex++]		= col.g;
        mColors[cIndex++]		= col.b;
        mColors[cIndex++]		= col.a;
        
        mVerts[vIndex++]		= p3.x;
        mVerts[vIndex++]		= p3.y;
        mVerts[vIndex++]		= p3.z;
        mTexCoords[tIndex++]	= 0.0f;
        mTexCoords[tIndex++]	= 1.0f;
        mColors[cIndex++]		= col.r;
        mColors[cIndex++]		= col.g;
        mColors[cIndex++]		= col.b;
        mColors[cIndex++]		= col.a;
        
        mVerts[vIndex++]		= p2.x;
        mVerts[vIndex++]		= p2.y;
        mVerts[vIndex++]		= p2.z;
        mTexCoords[tIndex++]	= 1.0f;
        mTexCoords[tIndex++]	= 0.0f;
        mColors[cIndex++]		= col.r;
        mColors[cIndex++]		= col.g;
        mColors[cIndex++]		= col.b;
        mColors[cIndex++]		= col.a;
        
        mVerts[vIndex++]		= p3.x;
        mVerts[vIndex++]		= p3.y;
        mVerts[vIndex++]		= p3.z;
        mTexCoords[tIndex++]	= 0.0f;
        mTexCoords[tIndex++]	= 1.0f;
        mColors[cIndex++]		= col.r;
        mColors[cIndex++]		= col.g;
        mColors[cIndex++]		= col.b;
        mColors[cIndex++]		= col.a;
        
        mVerts[vIndex++]		= p4.x;
        mVerts[vIndex++]		= p4.y;
        mVerts[vIndex++]		= p4.z;
        mTexCoords[tIndex++]	= 1.0f;
        mTexCoords[tIndex++]	= 1.0f;
        mColors[cIndex++]		= col.r;
        mColors[cIndex++]		= col.g;
        mColors[cIndex++]		= col.b;
        mColors[cIndex++]		= col.a;
	}
}

void StarGlows::draw()
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	
	glVertexPointer( 3, GL_FLOAT, 0, mVerts );
	glTexCoordPointer( 2, GL_FLOAT, 0, mTexCoords );
	glColorPointer( 4, GL_FLOAT, 0, mColors );
	
	glDrawArrays( GL_TRIANGLES, 0, mTotalVertices );
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
}

