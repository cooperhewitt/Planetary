//
//  PlaylistFilter.cpp
//  Kepler
//
//  Created by Tom Carden on 6/4/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "PlaylistFilter.h"

PlaylistFilter::PlaylistFilter(ci::ipod::PlaylistRef playlist)
{
    for( ci::ipod::Playlist::Iter i = playlist->begin(); i != playlist->end(); i++ ){
        mArtistSet.insert( (*i)->getArtistId() );
    }        
}

bool PlaylistFilter::test(ci::ipod::PlaylistRef artist) const
{
    return mArtistSet.find( artist->getArtistId() ) != mArtistSet.end();
}

bool PlaylistFilter::test(ci::ipod::TrackRef track) const
{
    return mArtistSet.find( track->getArtistId() ) != mArtistSet.end();
}
