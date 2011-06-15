//
//  Constellation.h
//  Kepler
//
//  Created by Tom Carden on 6/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <vector>
#include "cinder/Vector.h"
#include "NodeArtist.h"

class Constellation
{

public:
    
    Constellation()
    {
        mPrevTotalConstellationVertices = -1;
        mConstellationVerts			= NULL;
        mConstellationTexCoords		= NULL;        
    }
    ~Constellation()
    {
		if (mConstellationVerts != NULL) delete[] mConstellationVerts; 
		if (mConstellationTexCoords != NULL) delete[] mConstellationTexCoords;         
    }
    
    void setup(const vector<NodeArtist*> &filteredNodes);
    void draw() const;
    
private:

	std::vector<ci::Vec3f> mConstellation;
	std::vector<float> mConstellationDistances;
	int mTotalConstellationVertices;
	int mPrevTotalConstellationVertices;
	float *mConstellationVerts;
	float *mConstellationTexCoords;
    
};
