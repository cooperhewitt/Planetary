//
//  BloomSphere.h
//  Kepler
//
//  Created by Tom Carden on 6/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

namespace bloom {

    class BloomSphere
    {
    public:
        BloomSphere(): mInited(false) {}
        ~BloomSphere() {
            if (mInited) {
                delete[] mVerts;
                delete[] mNormals;
                delete[] mTexCoords;
            }
        }
        
        void setup( int segments );
        void draw();
    
    private:
        
        bool mInited;
        int mNumVerts;
        float *mVerts; 
        float *mNormals;
        float *mTexCoords;
        
    };
    
}