/*
 *  Data.h
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once 
#include "cinder/gl/Texture.h"
#include "CinderIPodPlayer.h"

using std::stringstream;
using std::vector;

class Data {
  public:
	Data();
	void initArtists();
	void filterArtistsByAlpha( char c );
	
	std::vector<ci::ipod::PlaylistRef> mArtists;
	std::vector<int> mFilteredArtists;
};
