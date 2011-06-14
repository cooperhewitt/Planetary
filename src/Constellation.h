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

class Node;

class Constellation
{

public:
    void setup(const std::vector<Node*> &nodes, const std::vector<int> &filteredArtists);
    void draw() const;
    
private:

	std::vector<ci::Vec3f> mConstellation;
	std::vector<float> mConstellationDistances;
	int mTotalConstellationVertices;
	int mPrevTotalConstellationVertices;
	float *mConstellationVerts;
	float *mConstellationTexCoords;
    
};
