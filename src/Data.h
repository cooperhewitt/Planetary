/*
 *  Data.h
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once 
#include <Foundation/NSAutoReleasePool.h>
#include <map>
#include "CinderIPod.h" // for PlaylistRef
#include "Filter.h"

class Data {
  public:
    
    enum LoadState { LoadStateDefault, LoadStateLoading, LoadStatePending, LoadStateComplete };
    
	Data() { mState = LoadStateDefault; };
    ~Data() {};
    
    void setup();
	void update();
    
	std::vector<ci::ipod::PlaylistRef> mArtists;
	std::vector<ci::ipod::PlaylistRef> mPlaylists;
    
	std::map< char, float > mNumArtistsPerChar;
	float mNormalizedArtistsPerChar[27];
    
    LoadState getState() { return mState; }
    
  private:
	    
	void backgroundInit();	
    
    LoadState mState;
	std::vector<ci::ipod::PlaylistRef> mPendingArtists;
	std::vector<ci::ipod::PlaylistRef> mPendingPlaylists;	
	
};
