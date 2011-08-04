//
//  TextureLoader.h
//  Kepler
//
//  Created by Tom Carden on 7/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <vector>
#include <string>
#include "cinder/Surface.h"

class TextureLoader {
public:

    enum State { WAITING, RUNNING, COMPLETE };
    
    struct Request {
        int mId;
        std::string mFileName;
        ci::Surface mSurface;
        
        Request( int reqId, std::string fileName ): mId(reqId), mFileName(fileName) {}
    };        
    
    TextureLoader( std::vector<Request> requests ): mRequests(requests), mState(WAITING), mProgress(0) {} 
    
    int getProgress() { return mProgress; }
    
    int getCount() { return mRequests.size(); }
    
    // check for COMPLETE before making textures
    State getState() { return mState; }
    
    // begin the thread
    void start();
        
private:
    
    void loadSurfaces();
        
    std::vector<Request> mRequests;
    State mState;
    int mProgress;

};