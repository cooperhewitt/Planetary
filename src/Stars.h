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

class Stars {
public:
    
    Stars();
    
    ~Stars();
    
    void setup( const std::vector<NodeArtist*> &nodes,
               const ci::Vec3f &bbRight, const ci::Vec3f &bbUp, 
               const float &zoomAlpha );
    void draw();
    
private:

    struct VertexData {
        ci::Vec3f vertex;
        ci::Vec2f texture;
        ci::Vec4f color;
    };
    
	int mTotalVertices, mPrevTotalVertices;
	VertexData *mVerts;
	
};