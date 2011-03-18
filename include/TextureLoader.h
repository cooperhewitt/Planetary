//
//  TextureLoader.h
//  Kepler
//
//  Created by Tom Carden on 3/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <string>
#include <map>
#include "cinder/Thread.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"

class TextureLoader
{
    
  public:
    
    void update();
    void requestTexture(const string &name, ci::gl::Texture &texture);
    
  private:
    
    std::map<string, ci::gl::Texture*> textures;
    std::map<string, ci::Surface>     images;
    std::mutex                        imageMutex;
    
    void threadedResourceLoad(const string &name);
    
};

void TextureLoader::update()
{
    // only transfer one thing at a time
    imageMutex.lock();
    if (images.size() > 0) {
        std::pair<string, ci::Surface> p = *images.begin();
        string name = p.first;
        ci::Surface img = p.second;
        *(textures[name]) = ci::gl::Texture(img);
        images.erase(images.begin());
    }
    imageMutex.unlock();    
}

void TextureLoader::requestTexture(const string &name, ci::gl::Texture &texture)
{
    textures[name] = &texture;
    // never do quite believe that it's this easy to thread something, but whatever
    std::thread requestThread(&TextureLoader::threadedResourceLoad, this, name);
}

void TextureLoader::threadedResourceLoad(const string &name)
{
	NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
    // this part ties up the thread...
    ci::Surface img = loadImage( loadResource ( name ) );
    // this part should be very quick...
    imageMutex.lock();
    images[name] = img;
    imageMutex.unlock();
    [autoreleasepool release];    
}


