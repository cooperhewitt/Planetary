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
    bool testArtist( ci::ipod::PlaylistRef artist ) const;
    bool testAlbum( ci::ipod::PlaylistRef album ) const;
    bool testTrack( ci::ipod::TrackRef track ) const;
  private:
    std::set<uint64_t> mArtistSet;    
    std::set<uint64_t> mAlbumSet;    
    std::set<uint64_t> mTrackSet;    
};
