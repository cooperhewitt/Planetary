//
//  TextureLoader.cpp
//  Kepler
//
//  Created by Tom Carden on 7/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "TextureLoader.h"
#include "cinder/app/AppCocoaTouch.h" // for loadResource
#include "cinder/Thread.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

void TextureLoader::start()
{
    mState = RUNNING;
    std::thread loaderThread( &TextureLoader::loadSurfaces, this );  
}

void TextureLoader::loadSurfaces()
{
    for (int i = 0; i < mRequests.size(); i++) {
        mRequests[i].mSurface = loadImage( loadResource( mRequests[i].mFileName ) );
        mProgress = i;
    }
    mState = COMPLETE;
}