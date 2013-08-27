//
//  Filter.h
//  Kepler
//
//  Created by Tom Carden on 6/4/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#pragma once
#include "CinderIPod.h"

// pure virtual "interface" class, Filters must implement tests for PlaylistRefs and TrackRefs
class Filter {
public:
    Filter() {}
    virtual ~Filter() {}
    virtual bool testArtist(ci::ipod::PlaylistRef artist) const = 0;
    virtual bool testAlbum(ci::ipod::PlaylistRef album) const = 0;
    virtual bool testTrack(ci::ipod::TrackRef track) const = 0;
};

typedef std::shared_ptr<Filter> FilterRef;
