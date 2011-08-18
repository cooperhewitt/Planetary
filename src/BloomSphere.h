//
//  BloomSphere.h
//  Kepler
//
//  Created by Tom Carden on 6/12/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once

#include "cinder/gl/gl.h"

namespace bloom {

    class BloomSphere
    {
      public:
        
        BloomSphere(): mInited(false) {}
        ~BloomSphere() {
            if (mInited) {
                glDeleteBuffers(1, &mVBO);
            }
        }
        
        void setup( int segments );
        void draw();

      private:
        
        struct VertexData {
            ci::Vec3f vertex;
            // no normal, normal == vertex
            ci::Vec2f texture;
        };            
        
        bool mInited;
        GLuint mVBO;
        int mNumVerts;
        VertexData *mVerts; 
        
    };
    
}