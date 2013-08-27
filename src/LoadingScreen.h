//
//  LoadingScreen.h
//  Kepler
//
//  Created by Tom Carden on 3/17/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#pragma once

#include "cinder/gl/Texture.h"
#include "BloomNode.h"

class LoadingScreen : public BloomNode {  
  public:
    void setup( const ci::gl::Texture &planetaryTex, const ci::gl::Texture &planetTex,
               const ci::gl::Texture &backgroundTex, const ci::gl::Texture &starGlowTex );
    void draw();
    void update();
    bool touchBegan( ci::app::TouchEvent::Touch touch ) { return isVisible(); };
    bool touchMoved( ci::app::TouchEvent::Touch touch ) { return isVisible(); };
    bool touchEnded( ci::app::TouchEvent::Touch touch ) { return isVisible(); };
    void setTextureProgress( float prop );
    void setArtistProgress( float prop );
    void setPlaylistProgress( float prop );
    bool isComplete(); // returns true if all the progress bars are done animating to their dests
  private:
    float mTextureProgress, mTextureProgressDest;
    float mArtistProgress, mArtistProgressDest;
    float mPlaylistProgress, mPlaylistProgressDest;
    ci::gl::Texture mStarGlowTex;
	ci::gl::Texture	mPlanetaryTex;
	ci::gl::Texture mPlanetTex;
	ci::gl::Texture mBackgroundTex;	
    ci::Vec2f mInterfaceSize;
};