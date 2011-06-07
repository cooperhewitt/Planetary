/*
 *  Data.cpp
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Data.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Thread.h"
#include "cinder/Text.h"
#include "CinderFlurry.h"

using namespace ci;
using namespace ci::ipod;
using namespace std;
using namespace pollen::flurry;

void Data::setup()
{
	mArtists.clear();
	mPlaylists.clear();
	mFilteredArtists.clear();    
    mNumArtistsPerChar.clear();
	
    if (!isIniting) {
        isIniting = true;
        std::thread artistLoaderThread( &Data::backgroundInit, this );	
    }
}

void Data::backgroundInit()
{
	NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
	
	Flurry::getInstrumentation()->startTimeEvent("Music Loading");
	pendingArtists = getArtists();
	std::cout << "got " << pendingArtists.size() << " artists" << std::endl;
	
	
// QUICK FIX FOR GETTING MORE DATA ONTO THE ALPHAWHEEL
	string alphaString	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ#";
	for( int i=0; i<27; i++ ){
		mNumArtistsPerChar[alphaString[i]] = 0;
	}
	float maxCount = 0.0001f;
	for( vector<ci::ipod::PlaylistRef>::iterator it = pendingArtists.begin(); it != pendingArtists.end(); ++it ){
		string name		= (*it)->getArtistName();
		string the		= name.substr( 0, 4 );
		char firstLetter;
		
		if( the == "The " || the == "the " ){
			firstLetter = name[4];
		} else {
			firstLetter = name[0];
		}
		
		if( isdigit(firstLetter) ){
			firstLetter = '#';
		} else {
			firstLetter = static_cast<char> ( toupper( firstLetter ) );
		}
		
		mNumArtistsPerChar[firstLetter] += 1.0f;
		
		if( mNumArtistsPerChar[firstLetter] > maxCount ){
			maxCount = mNumArtistsPerChar[firstLetter];
		}
	}
	
	for( int i=0; i<27; i++ ){
		mNormalizedArtistsPerChar[i] = mNumArtistsPerChar[alphaString[i]]/maxCount;
	}

// END ALPHAWHEEL QUICK FIX
	
    std::map<string, string> params;
    params["NumArtists"] = i_to_string(pendingArtists.size());
    Flurry::getInstrumentation()->logEvent("Artists loaded", params);
	
	Flurry::getInstrumentation()->startTimeEvent("Playlists Loading");
	pendingPlaylists = getPlaylists();
	//std::cout << "got " << pendingPlaylists.size() << " playlists" << std::endl;
    params["NumPlaylists"] = i_to_string( pendingPlaylists.size());
    Flurry::getInstrumentation()->logEvent("Playlists loaded", params);	

	Flurry::getInstrumentation()->stopTimeEvent("Music Loading");
	
	[autoreleasepool release];
	    
	isIniting = false;
}


bool Data::update()
{
	if (!isIniting && wasIniting) {
		// TODO: time this, is it OK in one frame?
		// TODO: switch state to enum. potential cause of freeze-on-load-screen bug
		mArtists.insert( mArtists.end(), pendingArtists.begin(), pendingArtists.end() );
		pendingArtists.clear();
		
		mPlaylists.insert( mPlaylists.end(), pendingPlaylists.begin(), pendingPlaylists.end() );
		pendingPlaylists.clear();
		
        wasIniting = isIniting;
		return true;
	}
    wasIniting = isIniting;
	return false;
}

void Data::setFilter(const Filter &filter)
{
	mFilteredArtists.clear();
	for( int i = 0; i < mArtists.size(); i++ ){
        if ( filter.test( mArtists[i] ) ){
            mFilteredArtists.push_back( i );
        }
    }    
}