//
//  Rings.h
//  Kepler
//
//  Created by Tom Carden on 6/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

class OrbitRing {

public:
    
    OrbitRing()
    {
        mVertsLowRes  = NULL;
        mTexLowRes	  = NULL;
        mVertsHighRes = NULL;
        mTexHighRes	  = NULL;
    }
    
    ~OrbitRing()
    {
        if( mVertsLowRes  != NULL ) delete[] mVertsLowRes;
        if( mTexLowRes	  != NULL ) delete[] mTexLowRes;
        if( mVertsHighRes != NULL ) delete[] mVertsHighRes;
        if( mTexHighRes	  != NULL ) delete[] mTexHighRes;        
    }
    
    void setup();
    void drawLowRes() const;
    void drawHighRes() const;

private:
    
	float *mVertsLowRes;
	float *mTexLowRes;
	float *mVertsHighRes;
	float *mTexHighRes;
    
};