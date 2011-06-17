//
//  BloomSphere.h
//  Kepler
//
//  Created by Tom Carden on 6/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/gl/gl.h"

namespace bloom {

    struct VertexData {
        ci::Vec4f vertex;
        // no normal, normal == vertex
        ci::Vec4f texture;
    };    
    
    class BloomSphere
    {
    public:
        BloomSphere(): mInited(false) {}
        ~BloomSphere() {
            if (mInited) {
                delete[] mVerts;
            }
        }
        
        void setup( int segments );
        void draw();
    
    private:
        
        bool mInited;
        GLuint mVBO;
        int mNumVerts;
        VertexData *mVerts; 
        
    };
    
}