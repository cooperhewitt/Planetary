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
    
    struct VertexData {
        ci::Vec3f vertex;
        ci::Vec2f texture;
        uint      color;
    };
    
    StarGlows();    
    ~StarGlows();
    
    void setup( const std::vector<NodeArtist*> &filteredNodes,
                const ci::Vec3f &bbRight, const ci::Vec3f &bbUp, 
                const float &zoomAlpha );
    void draw();
    
private:

    // FIXME: use a "DYNAMIC" VBO, or a VAO
    // don't use POINT_SPRITE because we need to draw BIG
	int mTotalVertices;
    int mPrevTotalVertices; // so we only recreate frames
	VertexData *mVerts;
	
};