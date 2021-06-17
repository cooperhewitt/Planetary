//
//  PlaylistFilter.h
//  Kepler
//
//  Created by Tom Carden on 6/4/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#pragma once
#include <boost/unordered_set.hpp>
#include "Filter.h"
#include "CinderIPod.h"

class PlaylistFilter : public Filter {
  public:
    static FilterRef create(ci::ipod::PlaylistRef playlist);
    bool testArtist( ci::ipod::PlaylistRef artist ) const;
    bool testAlbum( ci::ipod::PlaylistRef album ) const;
    bool testTrack( ci::ipod::TrackRef track ) const;
  private:
    PlaylistFilter(ci::ipod::PlaylistRef playlist);
    boost::unordered_set<uint64_t> mArtistSet;    
    boost::unordered_set<uint64_t> mAlbumSet;    
    boost::unordered_set<uint64_t> mTrackSet;    
};
