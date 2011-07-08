//
//  Constellation.cpp
//  Kepler
//
//  Created by Tom Carden on 6/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Constellation.h"
#include "cinder/gl/gl.h"
#include "Globals.h"

using std::vector;
using namespace ci;

void Constellation::setup(const vector<NodeArtist*> &filteredNodes)
{
	mConstellation.clear();
	//mConstellationColors.clear();
	
	// CREATE DATA FOR CONSTELLATION
	vector<float> distances;	// used for tex coords of the dotted line
	for( vector<NodeArtist*>::const_iterator it1 = filteredNodes.begin(); it1 != filteredNodes.end(); ++it1 ){

		NodeArtist *child1 = *it1;
		float shortestDist = 5000.0f;
		NodeArtist *nearestChild;
		
		vector<NodeArtist*>::const_iterator it2 = it1;
		for( ++it2; it2 != filteredNodes.end(); ++it2 ) {
			NodeArtist *child2 = *it2;
			
			Vec3f dirBetweenChildren = child1->mPosDest - child2->mPosDest;
			float distBetweenChildren = dirBetweenChildren.length();
			if( distBetweenChildren < shortestDist ){
				shortestDist = distBetweenChildren;
				nearestChild = child2;
			}
		}
		
		distances.push_back( shortestDist );
		mConstellation.push_back( child1->mPosDest );
		mConstellation.push_back( nearestChild->mPosDest );		
	}
    
	mTotalConstellationVertices	= mConstellation.size();
	if (mTotalConstellationVertices != mPrevTotalConstellationVertices) {
		if (mConstellationVerts != NULL) 
            delete[] mConstellationVerts; 
		mConstellationVerts	= new VertexData[mTotalConstellationVertices];
		mPrevTotalConstellationVertices = mTotalConstellationVertices;
	}
	
	int vIndex = 0;
	int distancesIndex = 0;
	for( int i=0; i<mTotalConstellationVertices; i++ ){
		Vec3f pos = mConstellation[i];
		mConstellationVerts[vIndex].vertex = mConstellation[i];
		if( i%2 == 0 ){
			mConstellationVerts[vIndex].texture	= Vec2f(0.0f, 0.5f);
		} else {
			mConstellationVerts[vIndex].texture	= Vec2f(distances[distancesIndex], 0.5f);
			distancesIndex++;
		}
        vIndex++;
	}    
}

void Constellation::draw( const float &alpha ) const
{
    if( mTotalConstellationVertices > 2 ){
        
        gl::color( ColorA( 0.12f, 0.25f, 0.85f, alpha ) );
        
        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );

        glVertexPointer( 3, GL_FLOAT, sizeof(VertexData), mConstellationVerts );
        glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), &mConstellationVerts[0].texture );
        
        glDrawArrays( GL_LINES, 0, mTotalConstellationVertices );
        
        glDisableClientState( GL_VERTEX_ARRAY );
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
    }
}