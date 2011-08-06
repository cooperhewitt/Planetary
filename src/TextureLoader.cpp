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

void TextureLoader::addRequest( int texId, std::string fileName )
{
    addRequest( texId, fileName, ci::gl::Texture::Format() );
}

void TextureLoader::addRequest( int texId, std::string fileName, ci::gl::Texture::Format format )
{
    mRequestsMutex.lock();
    mRequests.push_back( Request(texId, fileName, format) );
    mRequestsMutex.unlock();
    mTotalRequests++;
}

void TextureLoader::addRequest( int texId, std::string compressedFileName, ci::Vec2i size )
{
    // FIXME
//    mTotalRequests++;
}

void TextureLoader::start()
{
    std::thread loaderThread( &TextureLoader::loadSurfaces, this );  
//    loadSurfaces();
}

void TextureLoader::loadSurfaces()
{
	NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
    
    for (int i = 0; i < mRequests.size(); i++) {
        Surface surface = loadImage( loadResource( mRequests[i].mFileName ) );
        mRequestsMutex.lock();
        mRequests[i].mSurface = loadImage( loadResource( mRequests[i].mFileName ) );
        mRequestsMutex.unlock();
        mRequestsComplete++;
    }
    
    [autoreleasepool release];
}

void TextureLoader::update()
{
    if (mTextures.size() == mTotalRequests) {
        return;
    }
    
    // one texture per frame
    if (mTextures.size() < mRequestsComplete) {
        // if mRequestsMutex isn't already locked (don't block)
        if (mRequestsMutex.try_lock()) {
            int index = mTextures.size();
            mTextures[ mRequests[index].mTexId ] = gl::Texture( mRequests[index].mSurface );
            mRequests[index].mSurface.reset();
            mRequestsMutex.unlock();
        }
    }
    
    if (mTextures.size() == mTotalRequests) {
        mRequests.clear();
        mCbComplete.call( this );
    }
}
