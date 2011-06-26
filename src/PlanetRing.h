//
//  PlanetRing.h
//  Kepler
//
//  Created by Tom Carden on 6/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/Vector.h"

class PlanetRing {

public:
    
    struct VertexData {
        ci::Vec3f vertex;
        ci::Vec2f texture;
    };
    
    PlanetRing()
    {
        mVerts = NULL;
    }
    
    ~PlanetRing()
    {
        if (mVerts != NULL) {
            delete[] mVerts;
            mVerts = NULL;
        }
    }
    
    void setup();
    void draw() const;
    
private:

    VertexData *mVerts;
    
};