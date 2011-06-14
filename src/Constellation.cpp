//
//  Constellation.cpp
//  Kepler
//
//  Created by Tom Carden on 6/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Constellation.h"
#include "cinder/gl/gl.h"
#include "Node.h"
#include "Globals.h"

using std::vector;
using namespace ci;

void Constellation::setup(const vector<Node*> &nodes, const vector<int> &filteredArtists)
{
	mConstellation.clear();
	//mConstellationColors.clear();
	
	// CREATE DATA FOR CONSTELLATION
	vector<float> distances;	// used for tex coords of the dotted line
	for( vector<int>::const_iterator it1 = filteredArtists.begin(); it1 != filteredArtists.end(); ++it1 ){

		Node *child1 = nodes[*it1];
		float shortestDist = 5000.0f;
		Node *nearestChild;
		
		vector<int>::const_iterator it2 = it1;
		for( ++it2; it2 != filteredArtists.end(); ++it2 ) {
			Node *child2 = nodes[*it2];
			
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
		
		//mConstellationColors.push_back( ColorA( child1->mGlowColor, 0.15f ) );
		//mConstellationColors.push_back( ColorA( nearestChild->mGlowColor, 0.15f ) );
	}
	
	/*
     // CONSTELLATION IN ALPHABETICAL ORDER
     Node *child1;
     Node *child2;
     int index = 0;
     vector<float> distances;	// used for tex coords of the dotted line
     for( vector<int>::iterator it = mData->mFilteredArtists.begin(); it != mData->mFilteredArtists.end(); ++it ){
     if( index > 0 ){
     child2 = mNodes[*it];
     
     Vec3f dirBetweenChildren = child1->mPosDest - child2->mPosDest;
     float distBetweenChildren = dirBetweenChildren.length();
     
     distances.push_back( distBetweenChildren );
     mConstellation.push_back( child1->mPosDest );
     mConstellation.push_back( child2->mPosDest );
     }
     child1 = mNodes[*it];
     index ++;
     }
     */
    
	mTotalConstellationVertices	= mConstellation.size();
	if (mTotalConstellationVertices != mPrevTotalConstellationVertices) {
		if (mConstellationVerts != NULL) delete[] mConstellationVerts; 
		if (mConstellationTexCoords != NULL) delete[] mConstellationTexCoords; 
		
		mConstellationVerts			= new float[mTotalConstellationVertices*3];
		mConstellationTexCoords		= new float[mTotalConstellationVertices*2];
		mPrevTotalConstellationVertices = mTotalConstellationVertices;
	}
	
	int vIndex = 0;
	int tIndex = 0;
	int distancesIndex = 0;
	for( int i=0; i<mTotalConstellationVertices; i++ ){
		Vec3f pos = mConstellation[i];
		mConstellationVerts[vIndex++]	= pos.x;
		mConstellationVerts[vIndex++]	= pos.y;
		mConstellationVerts[vIndex++]	= pos.z;
		
		if( i%2 == 0 ){
			mConstellationTexCoords[tIndex++]	= 0.0f;
			mConstellationTexCoords[tIndex++]	= 0.5f;
		} else {
			mConstellationTexCoords[tIndex++]	= distances[distancesIndex];// * 0.4f;
			mConstellationTexCoords[tIndex++]	= 0.5f;
			distancesIndex ++;
		}
	}    
}

void Constellation::draw() const
{
    if( mTotalConstellationVertices > 2 ){
        
        // FIXME: pass this in as a function argument (to remove dependency on global)
        float zoomPer = ( 1.0f - (G_ZOOM-1.0f) ) * 0.4f;
        
        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        //glEnableClientState( GL_COLOR_ARRAY );
        glVertexPointer( 3, GL_FLOAT, 0, mConstellationVerts );
        glTexCoordPointer( 2, GL_FLOAT, 0, mConstellationTexCoords );
        //glColorPointer( 4, GL_FLOAT, 0, mColors );
        
        gl::color( ColorA( 0.12f, 0.25f, 0.85f, zoomPer ) );
        glDrawArrays( GL_LINES, 0, mTotalConstellationVertices );
        
        glDisableClientState( GL_VERTEX_ARRAY );
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
        //glDisableClientState( GL_COLOR_ARRAY );
    }
}