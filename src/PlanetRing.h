//
//  PlanetRing.h
//  Kepler
//
//  Created by Tom Carden on 6/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

class PlanetRing {

public:
    
    PlanetRing()
    {
        mVerts = NULL;
        mTexCoords = NULL;
    }
    
    ~PlanetRing()
    {
        if (mVerts != NULL) delete[] mVerts;
        if (mTexCoords != NULL) delete[] mTexCoords;        
    }
    
    void setup();
    void draw() const;
    
private:

	float *mVerts;
	float *mTexCoords;
    
};