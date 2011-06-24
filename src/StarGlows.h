//
//  StarGlows.h
//  Kepler
//
//  Created by Tom Carden on 6/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <vector>
#include "cinder/Vector.h"

class NodeArtist;

class StarGlows {
public:
    
    StarGlows()
    {
        mVerts = NULL;
		mTexCoords = NULL;
		mColors = NULL;
    }
    
    ~StarGlows()
    {
        if (mVerts != NULL)		delete[] mVerts; 
		if (mTexCoords != NULL) delete[] mTexCoords; 
		if (mColors != NULL)	delete[] mColors;        
    }
    
    void setup( const std::vector<NodeArtist*> &filteredNodes,
                const ci::Vec3f &bbRight, const ci::Vec3f &bbUp, 
                const float &zoomAlpha );
    void draw();
    
private:

    // FIXME: use a "DYNAMIC" VBO, or a VAO, or at least interleave these verts
    // consider using GL_POINT_SPRITE too (if we can figure out distance attenuation)
	int mTotalVertices;
    int mPrevTotalVertices; // so we only recreate frames
	float *mVerts;
	float *mTexCoords;
	float *mColors;
	
};