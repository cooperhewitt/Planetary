//
//  Rings.h
//  Kepler
//
//  Created by Tom Carden on 6/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/gl/gl.h"
#include "cinder/Vector.h"

class OrbitRing {

public:
        
    OrbitRing();    
    ~OrbitRing();
    
    void setup();
    void drawLowRes() const;
    void drawHighRes() const;

private:

    struct VertexData {
        ci::Vec2f vertex;
        ci::Vec2f texture;
    };
    
    GLuint mLowResVBO, mHighResVBO;
    
};