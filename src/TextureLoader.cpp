//
//  TextureLoader.cpp
//  Kepler
//
//  Created by Tom Carden on 7/25/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#include "TextureLoader.h"
#include "cinder/app/AppCocoaTouch.h" // for loadResource
#include "cinder/ImageIo.h"
#include "TaskQueue.h"

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

//void TextureLoader::addRequest( int texId, std::string compressedFileName, ci::Vec2i size )
//{
//    // FIXME
////    mTotalRequests++;
//}

//gl::Texture KeplerApp::loadCompressedTexture(const std::string &dataPath, const Vec2i &imageSize)
//{
//    // NB:- compressed textures *must* be square
//    //      also, file sizes are actually larger than jpg 
//    //      ... but it stays compressed on the GPU for more awesome
//    gl::Texture::Format compressedFormat;
//    compressedFormat.setInternalFormat(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG);
//    compressedFormat.enableMipmapping(false); // TODO: talk to Ryan about mipmapped compressed textures
//    compressedFormat.setMagFilter( GL_LINEAR );
//    compressedFormat.setMinFilter( GL_LINEAR );
//    
//    DataSourceRef dataSource = loadResource(dataPath);
//    const size_t dataSize = dataSource->getBuffer().getDataSize();
//    const uint8_t *data = static_cast<uint8_t*>(dataSource->getBuffer().getData());
//    return gl::Texture::withCompressedData(data, imageSize.x, imageSize.y, dataSize, compressedFormat);    
//}

void TextureLoader::start()
{
    TaskQueue::pushTask( std::bind( std::mem_fun( &TextureLoader::loadSurfaces ), this ) );    
}

void TextureLoader::loadSurfaces()
{
    for (int i = 0; i < mRequests.size(); i++) {
        mRequestsMutex.lock();
        std::string path = mRequests[i].mFileName;
        mRequestsMutex.unlock();
        // no lock for the long running part...
        Surface surface = loadImage( loadResource( path ) );
        mRequestsMutex.lock();
        mRequests[i].mSurface = surface;
        mRequestsMutex.unlock();
        mRequestsComplete++;
        // called on the UI thread...
        UiTaskQueue::pushTask( std::bind( std::mem_fun( &TextureLoader::surfaceLoaded ), this ) );
    }    
}

void TextureLoader::surfaceLoaded()
{
//    std::cout << "TextureLoader::surfaceLoaded()" << std::endl;
    
    if (mTextures.size() == mTotalRequests) {
        return;
    }
    
    // one texture per frame
    if (mTextures.size() < mRequestsComplete) {
//        std::cout << "TextureLoader::surfaceLoaded checking lock... ";

        // if mRequestsMutex isn't already locked (don't block, we're on the UI thread)
        if (mRequestsMutex.try_lock()) {
//            std::cout << " got lock!" << std::endl;
            int index = mTextures.size();
            mTextures[ mRequests[index].mTexId ] = gl::Texture( mRequests[index].mSurface, mRequests[index].mFormat );
            mRequests[index].mSurface.reset();
            mRequestsMutex.unlock();
        }
        else {
//            std::cout << " try again next time!" << std::endl;
            // try again
            UiTaskQueue::pushTask( std::bind( std::mem_fun( &TextureLoader::surfaceLoaded ), this ) );            
        }
    }
    
    if (mTextures.size() == mTotalRequests) {
//        std::cout << "TextureLoader::surfaceLoaded complete!" << std::endl;        
        mRequests.clear();
        mCbComplete.call( this );
    }
}
