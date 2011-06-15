//
//  Stars.h
//  Kepler
//
//  Created by Tom Carden on 6/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <vector>
#include "cinder/Vector.h"

class NodeArtist;

class Stars {
public:
    
    Stars()
    {
        mVerts = NULL;
		mTexCoords = NULL;
		mColors = NULL;
        mPrevTotalVertices = -1;
    }
    
    ~Stars()
    {
        if (mVerts != NULL)		delete[] mVerts; 
		if (mTexCoords != NULL) delete[] mTexCoords; 
		if (mColors != NULL)	delete[] mColors;        
    }
    
    void setup( const std::vector<NodeArtist*> &nodes,
                const ci::Vec3f &bbRight, const ci::Vec3f &bbUp, 
                const float &zoomAlpha );
    void draw();

private:

    int mTotalVertices;
    int mPrevTotalVertices; // so we only recreate frames
	float *mVerts;
	float *mTexCoords;
	float *mColors;
    
};