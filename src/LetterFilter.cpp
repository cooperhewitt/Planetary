//
//  LetterFilter.cpp
//  Kepler
//
//  Created by Tom Carden on 6/4/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "LetterFilter.h"
#include <string>
#include <boost/algorithm/string.hpp> // for boost::to_upper(std::string)
#include <stdio.h> // for toupper(char)

bool LetterFilter::test(ci::ipod::PlaylistRef artist) const
{
    if (mLetter != '#') {
        std::string name = artist->getArtistName();
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
        return !isalpha( artist->getArtistName()[0] );
	}
}

bool LetterFilter::test(ci::ipod::TrackRef track) const
{
    return true;    
}
