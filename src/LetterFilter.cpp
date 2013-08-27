//
//  LetterFilter.cpp
//  Kepler
//
//  Created by Tom Carden on 6/4/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#include "LetterFilter.h"
#include <stdio.h> // for toupper(char), isalpha(char), etc.
#include <string>
#include <boost/algorithm/string.hpp> // for boost::to_upper(std::string)

FilterRef LetterFilter::create(char letter)
{
    return FilterRef( new LetterFilter( letter ) );
}

LetterFilter::LetterFilter(char letter)
{
    mLetter = letter;
    if (mLetter != '#' && !isupper(mLetter)) {
        mLetter = static_cast<char> ( toupper( mLetter ) );
    }
}

bool LetterFilter::testArtist(ci::ipod::PlaylistRef artist) const
{
    return testArtistName(artist->getArtistName());
}

bool LetterFilter::testAlbum(ci::ipod::PlaylistRef album) const
{
    return testArtistName(album->getArtistName());
}

bool LetterFilter::testTrack(ci::ipod::TrackRef track) const
{
    return testArtistName(track->getArtist());    
}

bool LetterFilter::testArtistName(const std::string &name) const
{
    if (mLetter != '#') {
        char firstLetter = name[0];
        if (name.length() > 5) {
            std::string the = boost::to_upper_copy(name.substr( 0, 4 ));
            if( the == "THE " ){
                firstLetter = name[4];
            }
        }
        return isalpha(firstLetter) && toupper(firstLetter) == mLetter;
    }
    else {
        return !isalpha( name[0] );
	}    
}
