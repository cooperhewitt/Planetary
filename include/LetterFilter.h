//
//  LetterFilter.h
//  Kepler
//
//  Created by Tom Carden on 6/4/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "Filter.h"
#include "CinderIPod.h"

class LetterFilter : public Filter {
  public:
    LetterFilter( char letter );
    bool test( ci::ipod::PlaylistRef artist ) const;
    bool test( ci::ipod::TrackRef track ) const;
  private:
    char mLetter; // always uppercase, unless '#'
    bool testArtistName(const std::string &name) const;
};