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
	Data() {};
    ~Data() {};
    
    void setup();
	bool update(); // TODO: split into bool isInited() and void update() ?

    void setFilter(const Filter &filter);
    
	std::vector<ci::ipod::PlaylistRef> mArtists;
	std::vector<ci::ipod::PlaylistRef> mPlaylists;
	std::vector<int> mFilteredArtists;
    
	std::map< char, float > mNumArtistsPerChar;
	float mNormalizedArtistsPerChar[27];
	
  private:
	
	void backgroundInit();	
	bool isIniting;
    bool wasIniting;
	std::vector<ci::ipod::PlaylistRef> pendingArtists;
	std::vector<ci::ipod::PlaylistRef> pendingPlaylists;	
	
};
