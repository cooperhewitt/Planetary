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
    
    struct VertexData {
        ci::Vec3f vertex;
        ci::Vec4f color;
    };        
    
    Stars()
    {
        mVerts = NULL;
        mSizes = NULL;
        mPrevTotalVertices = -1;
    }
    
    ~Stars()
    {
        if (mVerts != NULL)	delete[] mVerts;
        if (mSizes != NULL)	delete[] mSizes;
    }
    
    void setup( const std::vector<NodeArtist*> &nodes, const float &zoomAlpha );
    void draw();

private:

    int mTotalVertices;
    int mPrevTotalVertices; // so we only recreate frames
	VertexData *mVerts;
    float *mSizes;
    
};