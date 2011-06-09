//
//  PlaylistFilter.h
//  Kepler
//
//  Created by Tom Carden on 6/4/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include <set>
#include "Filter.h"
#include "CinderIPod.h"

class PlaylistFilter : public Filter {
  public:
    PlaylistFilter(ci::ipod::PlaylistRef playlist);
    bool test(ci::ipod::PlaylistRef artist) const;
    bool test(ci::ipod::TrackRef track) const;
  private:
    std::set<uint64_t> mArtistSet;    
};
